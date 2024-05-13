from flask import Flask, render_template, redirect, url_for, request
import firebase_admin
from firebase_admin import credentials, db

# Initialize Flask app
app = Flask(__name__)

# Initialize Firebase
cred = credentials.Certificate("mpr-project-e9dd2-firebase-adminsdk-x04fy-4c465a0181.json")
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://mpr-project-e9dd2-default-rtdb.firebaseio.com/'
})

# Dictionary to store user credentials
users = {
    'john': 'password123',
    'jane': 'abc123',
    'aisha': 'pass456'
}

# Route for login page
@app.route('/', methods=['GET', 'POST'])
def login():
    error = None
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        if username in users and users[username] == password:
            # Redirect to main page if login successful
            return redirect(url_for('index', username=username))
        else:
            # Display error message if login fails
            error = 'Invalid username or password. Please try again.'
    return render_template('login_page.html', error=error)

# Route for sign up page
@app.route('/signup', methods=['POST'])
def signup():
    new_username = request.form['new_username']
    new_password = request.form['new_password']
    if new_username not in users:
        users[new_username] = new_password
        # Redirect to login page after successful sign up
        return redirect(url_for('login'))
    else:
        # Display error message if username already exists
        error = 'Username already exists. Please choose a different one.'
        return render_template('login_page.html', error=error)

# Route to display real-time data
@app.route('/index')
def index():
    # Fetch real-time data from Firebase
    data_ref = db.reference('/')
    data = data_ref.get()

    # Pass data to the template
    return render_template('index.html', 
                           car_no=data.get('Car No', ''),
                           warehouse_status=data.get('Ware house status', ''),
                           latitude=data.get('latitude', ''),
                           longitude=data.get('longitude', ''),
                           company_name=data.get('Company Name', ''),
                           driver_contact_number=data.get('Driver Contact Number', ''),
                           driver_name=data.get('Driver Name', ''),
                           firm_contact_number=data.get('Firm Contact Number', ''),
                           day_of_month=data.get('Day Of the Month', ''),
                           day_of_week=data.get('Day Of the Week', ''),
                           hours=data.get('Hours', ''),
                           minutes=data.get('Minutes', ''),
                           month=data.get('Month', ''),
                           seconds=data.get('Seconds', ''),
                           year=data.get('Year', ''))

# Route for logout
@app.route('/logout', methods=['POST'])
def logout():
    # Redirect to login page after logout
    return redirect(url_for('login'))

if __name__ == '__main__':
    app.run(debug=True)

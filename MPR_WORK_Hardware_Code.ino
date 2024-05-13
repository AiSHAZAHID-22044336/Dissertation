//GPs
#include <TinyGPS++.h>
#include <HardwareSerial.h> // Instead of SoftwareSerial
TinyGPSPlus gps;
HardwareSerial mygps(2); // Use HardwareSerial for ESP32, change pins accordingly (e.g., 16 for TX, 17 for RX)
//Time
#include "time.h"
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
// Define variables to store time information
char timeWeekDay[10];
char month[10];
char dayOfMonth[3];
char year[5];
char hour[3];
char hour12[3];
char minute[3];
char second[3];

//Firebase
#include <FirebaseESP32.h>
#include  <WiFi.h>
FirebaseData firebaseData;
FirebaseJson json;

#include <MFRC522.h>

#define SS_PIN 5
#define RST_PIN 2
#define buzzer 15
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
#define FIREBASE_HOST "mpr-project-e9dd2-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "xcD09VQLeCjbVaCzPDdZSJrJoM96eVifj4RnBQHc"
#define WIFI_SSID       "mywifi"
#define WIFI_PASSWORD   "12345678"


void setup() {
  Serial.begin(9600);
  mygps.begin(9600, SERIAL_8N1, 21, 22); // Set baud rate and pins
  delay(3000);
  pinMode(buzzer,OUTPUT);
  digitalWrite(buzzer,LOW);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  Serial.println(F("RFID Card Reader Initialized"));
  // Connect to wifi.
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
      digitalWrite(buzzer,HIGH);
    delay(700);
    digitalWrite(buzzer,LOW);
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    //  printLocalTime();

}

void loop() {
   
   
  while (mygps.available() > 0){
    gps.encode(mygps.read());
    if (gps.location.isUpdated()){
      Serial.print("Latitude= "); 
      Serial.print(gps.location.lat(), 6);
      Serial.print(" Longitude= "); 
      Serial.println(gps.location.lng(), 6);
       float lat = gps.location.lat();
       float lng = gps.location.lng();
    Firebase.setFloat(firebaseData, "/latitude", lat);
    Firebase.setFloat(firebaseData, "/longitude", lng);
    
  }
  }
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been read
  if (!rfid.PICC_ReadCardSerial())
    return;

  Serial.println(F("RFID Tag Detected!"));

  // Print UID
  Serial.print(F("UID Size: "));
  Serial.print(rfid.uid.size);
  Serial.println(" bytes");
  Serial.print(F("UID Value: "));
  printHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();
  
  // Convert UID to a string for comparison
  String uidString = byteArrayToHexString(rfid.uid.uidByte, rfid.uid.size);
  Serial.println(uidString);
 int uidInt = uidString.toInt();
Firebase.setInt(firebaseData, "/Car No", uidInt);

  // Check if the detected tag UID matches the target UID
  if (uidString == "531c7013") {
    digitalWrite(buzzer,HIGH);
    delay(700);
    digitalWrite(buzzer,LOW);
     Firebase.setString(firebaseData,"/Ware house status", "Authorized Vehicle");
      Firebase.setString(firebaseData,"/Company Name", "SKFood");
     Firebase.setString(firebaseData,"/Driver Name", "Abd");
      Firebase.setString(firebaseData,"/Driver Contact Number", "+447928289801");
       Firebase.setString(firebaseData,"/Firm  Contact Number", "+447928289801");
       printLocalTime();
  }
  if (uidString != "531c7013") {
    digitalWrite(buzzer,HIGH);
    delay(700);
    digitalWrite(buzzer,LOW);
    delay(700);
    digitalWrite(buzzer,HIGH);
    delay(700);
    digitalWrite(buzzer,LOW);
        delay(700);
    digitalWrite(buzzer,HIGH);
    delay(700);
    digitalWrite(buzzer,LOW);
        delay(700);
    digitalWrite(buzzer,HIGH);
    delay(700);
    digitalWrite(buzzer,LOW);
        delay(700);
    digitalWrite(buzzer,HIGH);
    delay(700);
    digitalWrite(buzzer,LOW);
        delay(700);
    Firebase.setString(firebaseData,"/Ware house status", "Unauthorized Vehicle");
    Firebase.setString(firebaseData,"/Company Name", "Unknown");
     Firebase.setString(firebaseData,"/Driver Name", "Unknown");
      Firebase.setString(firebaseData,"/Driver Contact Number", "UnKnown");
       Firebase.setString(firebaseData,"/Firm  Contact Number", "+92300987654321");
       printLocalTime();
    
  }
  
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  //Time
 
   
}

/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Convert a byte array to a hexadecimal string.
 */
String byteArrayToHexString(byte *byteArray, byte arraySize) {
  String hexString = "";
  for (byte i = 0; i < arraySize; i++) {
    // Add leading zero if needed
    if (byteArray[i] < 0x10) {
      hexString += "0";
    }
    // Convert byte to hex string
    hexString += String(byteArray[i], HEX);
  }
  return hexString;
}
 void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  // Store time information into variables
  strftime(timeWeekDay, 10, "%A", &timeinfo);
  strftime(month, 10, "%B", &timeinfo);
  strftime(dayOfMonth, 3, "%d", &timeinfo);
  strftime(year, 5, "%Y", &timeinfo);
  strftime(hour, 3, "%H", &timeinfo);
  strftime(hour12, 3, "%I", &timeinfo);
  strftime(minute, 3, "%M", &timeinfo);
  strftime(second, 3, "%S", &timeinfo);

  // Send day of the week and year to Firebase
//  Firebase.setString(firebaseData, "/Checking", "Wroking Firebase"); // Send day of the week to Firebase
 

  // Print other time information
  Serial.print("Day of week: ");
  Serial.println(timeWeekDay);
  Firebase.setString(firebaseData, "/Time/Day Of the Week", timeWeekDay); // Send year to Firebase
  Serial.print("Month: ");
  Serial.println(month);
  Firebase.setString(firebaseData, "/Time/Month", month); // Send year to Firebase
  Serial.print("Day of Month: ");
  Serial.println(dayOfMonth);
  Firebase.setString(firebaseData, "/Time/Day Of the Month", dayOfMonth); // Send year to Firebase
  Serial.print("Year: ");
  Serial.println(year);
    Firebase.setString(firebaseData, "/Time/Year", year); // Send year to Firebase
  Serial.print("Hour: ");
  Serial.println(hour);
  Firebase.setString(firebaseData, "/Time/Hours", hour); // Send year to Firebase
  Serial.print("Hour (12 hour format): ");
  Serial.println(hour12);
    Firebase.setString(firebaseData, "/Time/12 Hour", hour); // Send year to Firebase
  Serial.print("Minute: ");
  Serial.println(minute);
  Firebase.setString(firebaseData, "/Time/Minutes", minute); // Send year to Firebase
  Serial.print("Second: ");
  Serial.println(second);
    Firebase.setString(firebaseData, "/Time/Seconds", second); // Send year to Firebase
  Serial.println();
}

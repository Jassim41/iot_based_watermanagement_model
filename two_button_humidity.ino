#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "OPPO A54"
#define WIFI_PASSWORD "11111111"
#define API_KEY "AIzaSyAQ2YT_FFlIWglXtqs2vbun5CQ0PBces24"
#define DATABASE_URL "https://sample-b8bc5-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;

unsigned long sendDataPrevMillis = 0;
const long sendDataIntervalMillis = 5000;
const int On_Board_LED = 2; // Replace 2 with the actual pin number of the onboard LED
// Counter for entry key
int entryCounter = 0;

// NTP Client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800); // UTC offset for IST is 5 hours and 30 minutes (19800 seconds)

const int soilMoisturePin = 34; // Replace with the actual pin number for your ESP32 (e.g., D34)
const int rainSensorPin = 35;   // Replace with the actual pin number for your ESP32 (e.g., D35)
const int dhtPin = 32;          // Replace with the actual pin number connected to the DHT11

#define DHTTYPE DHT11 // DHT11 sensor type

DHT dht(dhtPin, DHTTYPE);

void setup()
{
  
  Serial.begin(115200);
  Serial.println();
  pinMode(18, INPUT);
  pinMode(19, INPUT);

    pinMode(On_Board_LED, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("---------------Connection");
  Serial.print("Connecting to : ");
  Serial.println(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    digitalWrite(On_Board_LED, HIGH);
    delay(250);
    digitalWrite(On_Board_LED, LOW);
    delay(250);
  }
  digitalWrite(On_Board_LED, LOW);
  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(WIFI_SSID);

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  Serial.println();
  Serial.println("---------------Sign up");
  Serial.print("Sign up new user... ");
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("ok");
    signupOK = true;
  }
  else
  {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  Serial.println("---------------");

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Start NTP client
  timeClient.begin();
  dht.begin();

  // Retrieve the current entry counter from Firebase
  Firebase.RTDB.getInt(&fbdo, "Counter/entryCounter");
  if (fbdo.dataType() == "int")
  {
    entryCounter = fbdo.intData();
    Serial.println("Retrieved entryCounter from Firebase: " + String(entryCounter));
  }
}

void loop()
{
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > sendDataIntervalMillis || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    // Increment the entry counter
    entryCounter++;

    // Get the current date and time from NTP server
    timeClient.update();
    String timeStamp = timeClient.getFormattedTime();

    // Read data from sensors
    int soilMoisture = analogRead(soilMoisturePin);
    int rainValue = analogRead(rainSensorPin);
    int humidityPercentage = dht.readHumidity();
    float temperatureCelsius = dht.readTemperature();
    int soilMoisturePercentage = map(soilMoisture, 0, 4095, 100, 0);
    int rainPercentage = map(rainValue, 0, 4095, 100, 0);
    int pbutton_1=digitalRead(18);//orange wire
    int pbutton_2=digitalRead(19);
    int nitrogenValue;
int phosphorusValue;
int potassiumValue;
  if(pbutton_1==1)
  {
         nitrogenValue = 0;
        phosphorusValue = 0;
        potassiumValue = 0;

        Serial.println("Button pressed. Default values:");
       
}
  if(pbutton_1==0)
  {
        int Rvalue = random(40, 45);
        nitrogenValue = 4.1*Rvalue;
        phosphorusValue = 2.2*Rvalue;
        potassiumValue = 1.45*Rvalue;

  }
  if(pbutton_2==1)
  {
        int Rvalue = random(40, 45);
        nitrogenValue = 4.1+Rvalue;
        phosphorusValue = 2.2*Rvalue;
        potassiumValue = 1.45*Rvalue;

  }

    Serial.println();
    Serial.println("---------------Sensor Data");
    Serial.print("Entry Counter     : ");
    Serial.println(entryCounter);
    Serial.print("Soil Moisture     : ");
    Serial.println(soilMoisturePercentage);
    Serial.print("Rain Percentage   : ");
    Serial.println(rainPercentage);
    Serial.print("Temperature (C)   : ");
    Serial.println(temperatureCelsius);
    Serial.print("Humidity Percentage : ");
    Serial.println(humidityPercentage);
    
    
    Serial.println("---------------");
    Serial.print("Nitrogen: ");
    Serial.print(nitrogenValue);
    Serial.println(" mg/kg");
    Serial.print("Phosphorus: ");
    Serial.print(phosphorusValue);
    Serial.println(" mg/kg");
    Serial.print("Potassium: ");
    Serial.print(potassiumValue);
    Serial.println(" mg/kg");
      

    Serial.println();
    Serial.println("---------------Store Data");
    digitalWrite(On_Board_LED, HIGH);

    // Create a unique key for each entry (e.g., based on counter)
    String entryKey = String(entryCounter);

    // Write date, time, and sensor data to the database path SensorData/<entryKey>.
    if (Firebase.RTDB.setString(&fbdo, "SensorData/" + entryKey + "/DateTime", timeStamp) &&
        Firebase.RTDB.setInt(&fbdo, "SensorData/" + entryKey + "/SoilMoisture", soilMoisturePercentage) &&
        Firebase.RTDB.setInt(&fbdo, "SensorData/" + entryKey + "/RainPercentage", rainPercentage) &&
        Firebase.RTDB.setFloat(&fbdo, "SensorData/" + entryKey + "/TemperatureC", temperatureCelsius) &&
        Firebase.RTDB.setFloat(&fbdo, "SensorData/" + entryKey + "/HumidityPercentage",humidityPercentage ) &&
        Firebase.RTDB.setInt(&fbdo, "SensorData/" + entryKey + "/NitrogenValue", nitrogenValue) &&
        Firebase.RTDB.setInt(&fbdo, "SensorData/" + entryKey + "/PhosphorusValue", phosphorusValue) &&
        Firebase.RTDB.setInt(&fbdo, "SensorData/" + entryKey + "/PotassiumValue", potassiumValue) &&
        Firebase.RTDB.setInt(&fbdo, "Counter/entryCounter", entryCounter)) // Update the counter in Firebase
    {
      Serial.println("Data entry PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else
    {
      Serial.println("Data entry FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    digitalWrite(On_Board_LED, LOW);
    Serial.println("---------------");
  }
}

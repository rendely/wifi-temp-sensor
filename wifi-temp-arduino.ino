#include <config.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h> // Used to establied serial communication on the I2C bus
#include <SparkFunTMP102.h> // Used to send and recieve specific information from our sensor


// Load in credentials from config.h
const String ssid = NETWORK_NAME;
const String password = NETWORK_PASSWORD;
 

// Temperature reading, boilerplate code
const int ALERT_PIN = 13;

TMP102 sensor0;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);
  Wire.begin(0x48); //Join I2C Bus
  
  pinMode(ALERT_PIN,INPUT);  // Declare alertPin as an input
  
  /* The TMP102 uses the default settings with the address 0x48 using Wire.
  
     Optionally, if the address jumpers are modified, or using a different I2C bus,
   these parameters can be changed here. E.g. sensor0.begin(0x49,Wire1)
   
   It will return true on success or false on failure to communicate. */
  if(!sensor0.begin())
  {
    Serial.println("Cannot connect to TMP102.");
    Serial.println("Is the board connected? Is the device ID correct?");
    while(1);
  }
  
  Serial.println("Connected to TMP102!");
  delay(100);

  // Initialize sensor0 settings
  // These settings are saved in the sensor, even if it loses power
  
  // set the number of consecutive faults before triggering alarm.
  // 0-3: 0:1 fault, 1:2 faults, 2:4 faults, 3:6 faults.
  sensor0.setFault(0);  // Trigger alarm immediately
  
  // set the polarity of the Alarm. (0:Active LOW, 1:Active HIGH).
  sensor0.setAlertPolarity(1); // Active HIGH
  
  // set the sensor in Comparator Mode (0) or Interrupt Mode (1).
  sensor0.setAlertMode(0); // Comparator Mode.
  
  // set the Conversion Rate (how quickly the sensor gets a new reading)
  //0-3: 0:0.25Hz, 1:1Hz, 2:4Hz, 3:8Hz
  sensor0.setConversionRate(2);
  
  //set Extended Mode.
  //0:12-bit Temperature(-55C to +128C) 1:13-bit Temperature(-55C to +150C)
  sensor0.setExtendedMode(0);

  //set T_HIGH, the upper limit to trigger the alert on
  sensor0.setHighTempF(85.0);  // set T_HIGH in F
  //sensor0.setHighTempC(29.4); // set T_HIGH in C
  
  //set T_LOW, the lower limit to shut turn off the alert
  sensor0.setLowTempF(84.0);  // set T_LOW in F
  //sensor0.setLowTempC(26.67); // set T_LOW in C

  float temperature;
  boolean alertPinState, alertRegisterState;
  
  // Turn sensor on to start temperature measurement.
  // Current consumtion typically ~10uA.
  sensor0.wakeup();

  // read temperature data
  temperature = sensor0.readTempF();
  //temperature = sensor0.readTempC();

  // Check for Alert
  alertPinState = digitalRead(ALERT_PIN); // read the Alert from pin
  alertRegisterState = sensor0.alert();   // read the Alert from register
  
  // Place sensor in sleep mode to save power.
  // Current consumtion typically <0.5uA.
  sensor0.sleep();

  // Print temperature and alarm state
  Serial.print("Temperature: ");
  Serial.print(temperature);
  
  Serial.print("\tAlert Pin: ");
  Serial.print(alertPinState);
  
  Serial.print("\tAlert Register: ");
  Serial.println(alertRegisterState);
  wifi(temperature);
  Serial.print("Going to deep sleep");
  ESP.deepSleep(300e6);
  delay(100);  
}

 
// Upload temperature to Google Sheets 
void wifi(float temperature) {
   
   WiFi.begin(ssid, password);
   Serial.println("Connecting");
   while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
  // The remote host we will connect to
  const char* host ="script.google.com";
  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  const int httpPort = 443; // 80 is for HTTP / 443 is for HTTPS!
  client.setInsecure(); 
  
  if (!client.connect(host, httpPort)) { //works!
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = BASE_URL;
  url += "?temp="+String(temperature,2);
  
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

  Serial.println();
  Serial.println("closing connection");
  WiFi.disconnect();
}

void loop() {}
  

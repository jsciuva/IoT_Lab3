#include <WiFi.h>
#include <HttpClient.h>
#include <Adafruit_AHTX0.h>



// This example downloads the URL "http://arduino.cc/"

char ssid[] = "ATTJvVQfYs";    // your network SSID (name) 
char pass[] = "5n95evrc3xm4"; // your network password (use for WPA, or use as key for WEP)

// Name of the server we want to connect to
const char kHostname[] = "18.217.73.232";
// Path to download (this is the bit after the hostname in the URL
// that you want to download
// const char kPath[] = "/?var=20";
const int kPort = 5000;

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(9600);

  // We start by connecting to a WiFi network
  delay(1000);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());

  // Sensor setup
  Serial.println("sensor setup");

  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");


}

void loop() {

  // get sensor data
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

  delay(500);

  // connect to cloud server
  int err =0;
  
  WiFiClient c;
  HttpClient http(c);

  // create a url path with data from sensor
  String path = "/";
  path += "?temp=" + String(temp.temperature);
  path += "&hum=" + String(humidity.relative_humidity);
  
  
  err = http.get(kHostname, kPort, path.c_str());
  if (err == 0)
  {
    Serial.println("startedRequest ok");

    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
      
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                // Print out this character
                Serial.print(c);
               
                bodyLen--;
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                delay(kNetworkDelay);
            }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();

  // And just stop, now that we've tried a download
  delay(2000);
}
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>

char ssid[] = "220ROWAN-504"; // replace MySSID with your WiFi network name
char pass[] = "220ROWAN504$$"; //replace MyPASS with your WiFi network password
unsigned long myChannelNumber = 1566826; // replace 0000000 with your channel number
const char * myWriteAPIKey = "APFBZBJ3MZIIAM2V"; // replace MyAPIKEY with your thingspeak write API key

String light, water, temp, humidity, pressure, ultra;

// Create WiFi client and ThingSpeak class.
WiFiClient client;
ThingSpeakClass ts;

// WiFi initialization
void init_WIFI()
{
  // Serial print commands for testing. 
  // If you are using an FTDI and aren't in programming mode, you'll be able to see these.
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);

  // While not connected, attempt to connect
  while (WiFi.status() != WL_CONNECTED) 
  {
    // Connect to the network
    WiFi.begin(ssid, pass);
    // Delay 5 seconds
    delay(5000);
    
    Serial.print(".");
  }
  
  Serial.println("\nConnected.");
}

// Initial board set-up
void setup() 
{
  // Initialize UART to 115200 baud rate
  Serial.begin(115200);

  // Set WiFi mode
  WiFi.mode(WIFI_STA);

  // Initialize ThingSpeak client
  ts.begin(client);
}

// Loop runs constantly
void loop() 
{
  //If not connected, connect to wifi
  if (WiFi.status() != WL_CONNECTED) 
  {
    init_WIFI();
  }
  
  int newByte = 0; // incoming byte from serial input
  char c; //Variable to hold incoming character
  String output = ""; //Variable to hold the concatanated string

  // Wait until there is any data available on the serial buffer
  while (Serial.available() == 0)
  {
    delay(100);
  }
  
  // Boolean to track if the message is finished
  bool receivedEOM = false;

  // Until an end-of-message character is received...
  while (!receivedEOM)
  {
    // If there is data in the serial buffer...
    if (Serial.available() > 0)
    {
      // Read in the new data and convert to a character
      newByte = Serial.read();
      c = (char) newByte;

      // If a semi-colon (end-of-message character) is received, end the message
      if ((c == 'l') || (c == 't') || (c == 'm') || (c == 'p') || (c == 'h') || (c == 'u'))
      {
        if(c == 'l'){
          light = output;
        }
        else if(c == 't'){
          temp = output;
        }
        else if(c == 'm'){
          water = output;
        }
        else if(c == 'p'){
          pressure = output;
        }
        else if(c == 'h'){
          humidity = output;
        }
        else if(c == 'u'){
          ultra = output;
        }
        
        receivedEOM = true;
      }
      // Otherwise, add it to the message
      else
      {
        output += c; //add it to the string
        Serial.println(output);
      }

    }
  }

  // Set a ThingSpeak field. If you have multiple sensors, you'll use multiple fields.
  ts.setField(1, light);
  ts.setField(2, water);
  ts.setField(3, temp);
  ts.setField(4, humidity);
  ts.setField(5, pressure);
  ts.setField(6, ultra);

  // Upload the data to ThingSpeak. Receive an integer code back from the site.
  int code = ts.writeFields(myChannelNumber, myWriteAPIKey);

  // Code 200 indicates a successful upload. Other codes indicate errors.
  if (code == 200) 
  {
    Serial.println("Channel update successful.");
  }
  else 
  {
    Serial.println("Problem setting Field 1. HTTP error code " +
    String(code));
  }
  
  // Wait 15.5 seconds to update the channel again since ThingSpeak is rate limited.
  // You can only upload data every 15 seconds.
  delay(15500);
}

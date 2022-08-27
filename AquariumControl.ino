#include "WiFi.h"
#include <string>
#include <sstream>

// Set web server port number to 80
WiFiServer server(80);

class WifiHelper
{
public:
  // Variable to store the HTTP request
  String header;
  // Current time
  unsigned long currentTime = millis();
  // Previous time
  unsigned long previousTime = 0;
  // Define timeout time in milliseconds (example: 2000ms = 2s)
  const long timeoutTime = 2000;

  // connect to wifi and start server
  void initWiFi()
  {
    const char *ssid = "shababroom";
    const char *wifipassword = "shabab123archer";
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, wifipassword);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print('.');
      delay(1000);
    }
    Serial.println("Wifi Connected..");
    Serial.print(WiFi.localIP());

    server.begin();
  }

  // server request listener
  void listen()
  {
    WiFiClient client = server.available(); // Listen for incoming clients

    if (client)
    {
      // If a new client connects,
      currentTime = millis();
      previousTime = currentTime;
      Serial.println("New Client."); // print a message out in the serial port
      String currentLine = "";       // make a String to hold incoming data from the client

      while (client.connected() && currentTime - previousTime <= timeoutTime)
      { // loop while the client's connected
        currentTime = millis();
        if (client.available())
        {                         // if there's bytes to read from the client,
          char c = client.read(); // read a byte, then
          // Serial.write(c);        // print it out the serial monitor
          header += c;

          if (c == '\n')
          {
            // if you got a newline, then clear currentLine
            currentLine = "";
          }
          else
          {                   // if you got anything else but a carriage return character,
            currentLine += c; // add it to the end of the currentLine
          }
        }
      }

      // header generated, now check whats in there
      if (header.indexOf("POST /config") >= 0)
      {
        // post request for updating settings recieved
        Serial.println("Request recieved for updating settings");
        Serial.print("Change requested : ");
        Serial.println(currentLine);

        breakDownRequest(currentLine);

        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();
        client.println();
        client.println("OK");
        client.println();
        client.println();
      }

      // Clear the header variable
      header = "";
      // Close the connection
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    }
  }

private:
  void breakDownRequest(String query)
  {
    Serial.println("Breaking down");

    // convert arduino String to std::string for operation
    std::string queryString = std::string(query.c_str());
    std::stringstream ss(queryString);
    std::string segment;
    
    //string tokenize by & percent
    while(std::getline(ss, segment, '&'))
    {

      /*//split each segment by = for param name and value
      size_t found;
      if ((found = segment.find("=")) != std::string::npos)      
      {
          std::string param = segment.substr(0,found);
          Serial.print("Parameter : ");
          Serial.println(param.c_str());
          std::string value = segment.substr(found+1, std::string::npos);
          Serial.print("Value : ");
          Serial.println(value.c_str());
      }   */   
    }
  }
} wifiHelper;

// the setup function runs once when you press reset or power the board
void setup()
{
  Serial.begin(115200);
  wifiHelper.initWiFi();
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(GPIO_NUM_2, OUTPUT);
}

// the loop function runs over and over again forever
void loop()
{
  wifiHelper.listen();
  delay(3000);
  Serial.print(".");
}

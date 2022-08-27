#include "WiFi.h"

// Set web server port number to 80
WiFiServer server(80);

class WifiHelper
{
public:
  // Variable to store the HTTP request
  String header;

  // Auxiliar variables to store the current output state
  String outputState = "off";
  // Assign output variables to GPIO pins
  const int output = GPIO_NUM_2;
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
          Serial.write(c);        // print it out the serial monitor
          header += c;
          if (c == '\n')
          { // if the byte is a newline character
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0)
            {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();

              // turns the GPIOs on and off
              if (header.indexOf("GET /26/on") >= 0)
              {
                Serial.println("GPIO on");
                outputState = "on";
                digitalWrite(output, HIGH);
              }
              else if (header.indexOf("GET /26/off") >= 0)
              {
                Serial.println("GPIO off");
                outputState = "off";
                digitalWrite(output, LOW);
              }

              // The HTTP response ends with another blank line
              client.println();
              // Break out of the while loop
              break;
            }
            else
            { // if you got a newline, then clear currentLine
              currentLine = "";
            }
          }
          else if (c != '\r')
          {                   // if you got anything else but a carriage return character,
            currentLine += c; // add it to the end of the currentLine
          }
        }
      }
      // Clear the header variable
      header = "";
      // Close the connection
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
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
  delay(2000);
  Serial.println("Do the usual stuff");
}

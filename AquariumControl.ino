#include <math.h>

#include <sstream>
#include <string>

#include "WiFi.h"

class Time {
 public:
  static const int WEEKINSECONDS = 604800;  // 24 * 60 * 60 * 7
  static const int DAYINSECONDS =
      86400;  // 24 * 60 * 60; // 86400 seconds in a day
  static const int HOURINSECONDS = 3600;  // 60 * 60; // 3600 seconds in an hour
  static const int MINUTEINSECONDS = 60;  // 60 seconds in a minute

  static unsigned int getDaySeconds(unsigned long int timestamp) {
    return timestamp % Time::DAYINSECONDS;
  }

  static unsigned short int getWeekday(unsigned long int timestamp) {
    return floor((timestamp % WEEKINSECONDS) / DAYINSECONDS);
  }

  /*void buildDayHourMinutes(unsigned long int timestamp){
        day = floor((timestamp % WEEK) / DAY);
        hour = floor((timestamp % DAY) / HOUR);
        minute = floor((timestamp % HOUR) / MINUTE);
        second = floor((timestamp % HOUR) % MINUTE );
  }*/
};

class LEDStrip {
 private:
  int sunriseSteps = 0;
  int sunsetSteps = 0;
  char color[1];

 public:
  unsigned int setBrightness;
  unsigned int onTime;
  unsigned int offTime;
  unsigned int sunrise;
  unsigned int sunset;
  unsigned int currentBrightness;

  LEDStrip() {}
  LEDStrip(unsigned int d, unsigned int on, unsigned int off, unsigned int sr,
           unsigned int ss, char c) {
    setBrightness = d;
    onTime = on;
    offTime = off;
    sunset = ss;
    sunrise = sr;
    color[0] = c;
    currentBrightness = 0;
    if (sunrise != 0) {
      sunriseSteps = ceil(setBrightness / sunrise);
    }
    if (sunset != 0) {
      sunsetSteps = ceil(setBrightness / sunset);
    }
  }

  void updateStatus(unsigned int time) {
    // we are in the time were it should be on
    if (time >= onTime && time < offTime) {
      // if its not upto the required brightness, and needed to be on
      if (currentBrightness < setBrightness) {
        // check sunrise status, do the gradual brightness increaes
        if (sunrise > 0) {
          currentBrightness += sunriseSteps;
        } else {
          currentBrightness = setBrightness;
        }
      }
    } else {
      // we are in the time were it should be off. aka brightness 0
      if (currentBrightness > 0) {
        // check sunset setatus,
        if (sunset > 0) {
          // do gradual decrease until off
          currentBrightness -= sunsetSteps;
        } else {
          currentBrightness = 0;
        }
      } else if (currentBrightness < 0) {
        currentBrightness = 0;
      }
    }
  }
};

// each led strips
LEDStrip white1, white2, red, green, blue;
void processRequest(String query);

// Set web server port number to 80
WiFiServer server(80);
class WifiHelper {
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
  void initWiFi() {
    const char *ssid = "shababroom";
    const char *wifipassword = "shabab123archer";
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, wifipassword);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(1000);
    }
    Serial.println("Wifi Connected..");
    Serial.print(WiFi.localIP());

    server.begin();
  }

  // server request listener
  void listen() {
    WiFiClient client = server.available();  // Listen for incoming clients

    if (client) {
      // If a new client connects,
      currentTime = millis();
      previousTime = currentTime;
      // print a message out in the serial port
      Serial.println("New Client.");
      // make a String to hold incoming data from the client
      String currentLine = "";

      while (client.connected() &&
             currentTime - previousTime <=
                 timeoutTime) {  // loop while the client's connected
        currentTime = millis();
        if (client.available()) {  // if there's bytes to read from the client,
          char c = client.read();  // read a byte, then
          // Serial.write(c);        // print it out the serial monitor
          header += c;

          if (c == '\n') {
            // if you got a newline, then clear currentLine
            currentLine = "";
          } else {  // if you got anything else but a carriage return character,
            currentLine += c;  // add it to the end of the currentLine
          }
        }
      }

      // header generated, now check whats in there
      if (header.indexOf("POST /config") >= 0) {
        // post request for updating settings recieved
        Serial.println("Request recieved for updating settings");
        Serial.print("Change requested : ");
        Serial.println(currentLine);

        processRequest(currentLine);

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

} wifiHelper;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  wifiHelper.initWiFi();
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(GPIO_NUM_2, OUTPUT);

  white1 = LEDStrip(
      80,                                                        /*Brightness*/
      16 * Time::HOURINSECONDS + 0 * Time::MINUTEINSECONDS,      /*Start time*/
      16 * Time::HOURINSECONDS + 0 * Time::MINUTEINSECONDS + 18, /*End time*/
      15, /*sunrise seconds*/
      7,  /*sunset seconds*/
      'w');
}

// the loop function runs over and over again forever
void loop() {
  wifiHelper.listen();
  delay(1000);
  Serial.print(".");
}



void processRequest(String query) {
  Serial.println("Breaking down");

  // convert arduino String to std::string for operation
  std::string queryString = std::string(query.c_str());
  std::stringstream ss(queryString);
  std::string segment;

  // string tokenize by & percent
  while (std::getline(ss, segment, '&')) {
    // split each segment by = for param name and value
    size_t found;
    if ((found = segment.find("=")) != std::string::npos) {
      std::string param = segment.substr(0, found);
      std::string value = segment.substr(found + 1, std::string::npos);

      if (param == "white1_pwm") {
        Serial.print("Set white brightness");
        white1.setBrightness = atoi(value.c_str());
      }
    }
  }
}

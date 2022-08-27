#include <math.h>
#include <iostream>
#include <string>

using namespace std;

class Time{
public:

  static const int WEEKINSECONDS = 604800; //24 * 60 * 60 * 7
  static const int DAYINSECONDS = 86400; //24 * 60 * 60; // 86400 seconds in a day
  static const int HOURINSECONDS = 3600; //60 * 60; // 3600 seconds in an hour
  static const int MINUTEINSECONDS = 60; // 60 seconds in a minute

  static unsigned int getDaySeconds(unsigned long int timestamp){
    return timestamp % Time::DAYINSECONDS;
  }

  static unsigned short int getWeekday(unsigned long int timestamp){
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
      int sunriseSteps;
      int sunsetSteps;
      char color[1];
      unsigned int setBrightness;
      unsigned int onTime;
      unsigned int offTime;
      unsigned int sunrise;
      unsigned int sunset;
      unsigned int currentBrightness;

      void printDebug(unsigned int time){
        cout << color[0];
        cout << " -- " << time << " -- ";
        cout << currentBrightness;
      }

  public:

    LEDStrip (unsigned int d, unsigned int on, unsigned int off, unsigned int sr, unsigned int ss, char c )
    {
      setBrightness = d;
      onTime = on;
      offTime = off;
      sunset = ss;
      sunrise = sr;
      color[0] = c;

      currentBrightness = 0;

      sunriseSteps = round(setBrightness / sunrise) ;
      sunsetSteps  = round(setBrightness / sunset);
    }

    void updateStatus(unsigned int time)
    {

      printDebug(time);

      //we are in the time were it should be on
      if (time >= onTime  && time < offTime) {
        //if its not upto the required brightness, and needed to be on
        if (currentBrightness < setBrightness) {
          //check sunrise status, do the gradual brightness increaes
          if (sunrise > 0) {
            currentBrightness += sunriseSteps;
          } else {
            currentBrightness = setBrightness;
          }
        }
      } else {
        //we are in the time were it should be off. aka brightness 0
        if (currentBrightness > 0) {
          //check sunset setatus,
          if (sunset > 0) {
            //do gradual decrease until off
            currentBrightness -= sunsetSteps;
          } else {
            currentBrightness = 0;
          }
        } else if(currentBrightness < 0) {
          currentBrightness = 0;
        }
      }

    }
};

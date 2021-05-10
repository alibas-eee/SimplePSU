//avrdude -c arduino -p t85 -P COM3 -b 19200 -U flash:w:psu.hex

#include "TinyWireM.h"
#include "TinyOzOLED.h"
#include "SimpleKalmanFilter.h"
#include <avr/wdt.h>
#include <avr/sleep.h>

OzOLED oled;
SimpleKalmanFilter KalmanA(8, 1, 0.5);
SimpleKalmanFilter KalmanV(8, 1, 0.5);

void setup(){
  oled.init();
  oled.sendCommand(0xA1); // set Orientation
  oled.sendCommand(0xC8);  
  oled.clearDisplay();
   
}


void loop(){
  
PrintVolt(ReadVolt());
PrintAmper(ReadAmp());
delay(100);  
}

/*
5v = 1024
1/3 resistor division
val = 3* (adc*5)/1024)
((res/10)*5)/1024)*3
res/682
*/
float ReadVolt()
{
  int t = 0;
  float res = 0;
for(int i = 0; i < 10; i++)
  t += analogRead(A2);  
  res = t;
  
  res = KalmanV.updateEstimate(res);  
  res /=682.6;  
  return res;
}


/*
100mV/A
*/
float ReadAmp()
{
  int t = 0;
  float res = 0;
for(int i = 0; i < 10; i++)
  t += analogRead(A3);
  
  res = t;
  res = KalmanA.updateEstimate(res);  
  res /=10;
  res -=512;
  return (res/20.48);
}


void PrintVolt(float val)
{
  char tmp[10];
  dtostrf(val, 5, 2, tmp);
  oled.printChar('v', 0, 2);
  oled.printBigNumber(tmp, 1, 0, 5);//row 4 col 3
}

void PrintAmper(float val)
{
  char tmp[10];
  dtostrf(val, 5, 2, tmp);
  oled.printChar('A', 0, 6);
  oled.printBigNumber(tmp, 1, 4, 5);//row 4 col 3
}

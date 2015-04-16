 /* Read Jostick
  * ------------
  *
  * Reads two analog pins that are supposed to be
  * connected to a jostick made of two potentiometers
  *
  * We send three bytes back to the comp: one header and two
  * with data as signed bytes, this will take the form:
  *     Jxy\r\n
  *
  * x and y are integers and sent in ASCII
  *
  * http://www.0j0.org | http://arduino.berlios.de
  * copyleft 2005 DojoDave for DojoCorp
  */

 int joyPin1 = 0;                 // slider variable connecetd to analog pin 0
 int joyPin2 = 1;                 // slider variable connecetd to analog pin 1
 int value1 = 0;                  // variable to read the value from the analog pin 0
 int value2 = 0;                  // variable to read the value from the analog pin 1

 void setup() {
  Serial.begin(9600);
 }

 int treatValue(int data) {
  return (data * 9 / 1024) + 48;
 }
 
 /*
 X Y
 52 52 - middle/noise(0)
 48 52 - left(1)
 56 52 - right(2)
 52 56 - up(3)
 52 48 - down(4)
 */
 int getJoyDirection(int x, int y){
   if ((x == 48) && (y == 52)){
     return 1;
   }
   if ((x == 56) && (y == 52)){
     return 2;
   }
   if ((x == 52) && (y == 56)){
     return 3;
   }
   if ((x == 52) && (y == 48)){
     return 4;
   }
   return 0;
 }

 void loop() {
  // reads the value of the variable resistor
  value1 = analogRead(joyPin1);  
  // this small pause is needed between reading
  // analog pins, otherwise we get the same value twice
  delay(1000);            
  // reads the value of the variable resistor
  value2 = analogRead(joyPin2);  
  Serial.print('J');
  Serial.print(treatValue(value1));
  Serial.print('|');
  Serial.print(treatValue(value2));
  Serial.print('|');
  Serial.println(getJoyDirection(treatValue(value1), treatValue(value2)));
 }

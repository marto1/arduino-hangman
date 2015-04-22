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
int joybutton = 9;
 
 
// include the library code:
#include <LiquidCrystal.h>
 
 
class Window
{
private:
    LiquidCrystal& l;
    String firstLine;
    String secondLine;
    char rawLine[17];
public:
    Window(LiquidCrystal& lvar):
	l(lvar){}

    String getFirstLine() const {
	return firstLine;
    }
    void setFirstLine(String line){
        firstLine = String(line);
    }
      
    void setSecondLine(String line){
        secondLine = String(line);
    }
      
    void displayLine(String line, int linenum){
        memset(rawLine, 0, 17);
        line.toCharArray(rawLine, 17); 
        l.setCursor(0,linenum);
        l.print(rawLine);
        l.setCursor(0,linenum);
    }
      
    void displayFirstLine(){
        displayLine(firstLine, 0);
    }

    void setFirstLineHangman(String word, int tries){
	int wordLength = word.length();
	String result = "";
	String strTries = String(tries);
	for (int i = 0; i < 14;i++){
	    if (wordLength > 0) {
		result += '_';
	    } else {
		result += ' ';
	    }
	    wordLength--;
 	}
	result += ' ';
	result += strTries;
	setFirstLine(result);
    }
      
    void displaySecondLine(){
        displayLine(secondLine, 1);
    }
      
    void scrollSecondLine(boolean side, String fill){
        unsigned int len = secondLine.length();
        if (side) {
	    secondLine = String(secondLine.substring(1, len));
	    secondLine += String(fill);
        } else { 
	    secondLine = String(fill) + secondLine.substring(0, len-1);
        }
        l.clear();
        displayFirstLine();
        displaySecondLine();
        l.setCursor(0, 1);
    }
};

class GameGuess
{
private:
    String word;
    unsigned int guessed;
    int tries;
public:
    GameGuess(String wordvar):
	word{wordvar},
	guessed(0x0)
    {}

    unsigned int guessLetter(String letter){
	unsigned int lenl = word.length();
	for(int i; i < lenl; i++){
	    if (String(word[i]) == letter){
		guessed |= 1 << i; //guessed position set
	    }
	}
	return guessed;
    }

    boolean isGuessed(){
	unsigned int lenl = word.length();
	char check;
	for(int p; p < lenl; p++){
	    check = (guessed >> p) & 1;
	    if (!check){
		return false;
	    }
	}
	return true;
    }
    
};

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Window win = Window(lcd);
String name = "donkey";
int tries = 6;
GameGuess guess = GameGuess(name);

int thisChar = 'a';
char* alphabet = "abcdefghijklmnopqrstuvwxyz";

void initFirstLine(int wordLength, int tries){
    String result = "";
    for (int i; i < wordLength;i++){
	result += '_';
    }
    win.setFirstLineHangman(result, tries);
    win.displayFirstLine();
    // String result = ""; ("____"
    // String strTries = String(tries);
    // for (int i; i < 14;i++){
    // 	if (wordLength > 0) {
    // 	    result += '_';
    // 	} else {
    // 	    result += ' ';
    // 	}
    // 	wordLength--;
    // }
    // result += ' ';
    // result += strTries;
    // win.setFirstLine(result);
    // win.displayFirstLine();
}

void initSecondLine() {
    win.setSecondLine(String(alphabet).substring(0, 16));
    win.displaySecondLine();
}
 
int leftOffset = 16; 
int rightOffset = 25;
int currentLetter = 0;
void setup() {
    // set up the LCD's number of columns and rows: 
    lcd.begin(16, 2);
    // turn on the cursor:
    lcd.cursor();
    initFirstLine(6, 4);
    initSecondLine();
    //win.scrollSecondLine(true, "!");
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
 
void scrollAlphabet(boolean side){
    String c;
    if (leftOffset >= 26) {
	leftOffset = 0;
    }
    if (leftOffset < 0){
	leftOffset = 25;
    }
    if (rightOffset >= 26){
	rightOffset = 0;
    }
    if (rightOffset < 0) {
	rightOffset = 25;
    }
    if (side){
	c = String(alphabet[leftOffset]);
	leftOffset++;
	rightOffset++;
	currentLetter++;
    } else {
	c = String(alphabet[rightOffset]);
	rightOffset--;
	leftOffset--;
	currentLetter--;
    }
    if (currentLetter >= 26){
	currentLetter = 0;
    }
    if (currentLetter < 0){
	currentLetter = 25;
    }
    win.scrollSecondLine(side, c);
}

void loop() {
    // reads the value of the variable resistor
    value1 = analogRead(joyPin1);  
    // this small pause is needed between reading
    // analog pins, otherwise we get the same value twice
    delay(100);            
    // reads the value of the variable resistor
    value2 = analogRead(joyPin2);
    //  Serial.print('J');
    //  Serial.print(treatValue(value1));
    //  Serial.print('|');
    //  Serial.print(treatValue(value2));
    //  Serial.print('|');
    int dir = getJoyDirection(treatValue(value1), treatValue(value2));
    if (dir == 2){
	scrollAlphabet(true);
    } else if (dir == 1){
	scrollAlphabet(false);
    } else if (dir == 3){
	Serial.println(alphabet[currentLetter]);
	guess.guessLetter(String(alphabet[currentLetter]));
	Serial.println(guess.isGuessed());
    }  
}


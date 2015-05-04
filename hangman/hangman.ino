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


byte smiley[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};

byte sad[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B01110,
  B10001,
  B00000,
};

 
 
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
		result += word[i];//'_';
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
    boolean match;
public:
    GameGuess(String wordvar):
	word{wordvar},
	guessed(0x0)
    {}

    void setWord(String w){
	word = w;
    }

    unsigned int guessLetter(String letter){
	match = false;
	unsigned int lenl = word.length();
	for(int i = 0; i < lenl; i++){
	    if (String(word[i]) == letter){
		match = true;
		guessed |= 1 << i; //guessed position set
	    }
	}
	return guessed;
    }

    boolean isGuessed() const {
	unsigned int lenl = word.length();
	char check;
	for(int p = 0; p < lenl; p++){
	    check = (guessed >> p) & 1;
	    if (!check){
		return false;
	    }
	}
	return true;
    }

    boolean hasMatch() const {
	return match;
    }

    unsigned int wordLen() const {
	return word.length();
    }

    String word() const {
	return word;
    }
};

void produce_guessed_word(unsigned int guessed,
			  String word,
			  String& result){
    unsigned int len = word.length();
    unsigned char check;
    result = "";
    for (unsigned char i=0; i < len; i++){
	check = (guessed >> i) & 1;
	if (check){
	    result += word[i];
	} else {
	    result += "_";
	}
    }
}

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

class Hangman: public GameGuess { //Involves game logic and display
private:
    // initialize the library with the numbers of the interface pins
    char* alphabet = "_________________________g";
    const char* const topic = "--------------------------";
    const char* const letters = "abcdefghijklmnopqrstuvwxyz";
    static const unsigned char tlen = 4;
    static const unsigned char wlen = 2;
    String topics[tlen][wlen+1] = {{"animals         ", "lion", "zebra"},
			   {"body parts      ", "boobs", "penis"},
			   {"transportation  ", "submarine", "plane"},
                           {"chem elements   ", "plutonium", "uranium"}};
    int topicCounter=0;
    Window window;
    int mode = 0;
    int tries;
    int triesTotal;
    int leftOffset = 16; 
    int rightOffset = 25;
    int currentLetter = 0;
public:
    Hangman(int tr, Window win):
	GameGuess(""),
	tries(tr),
	triesTotal(tr),
	window(win)
    {}

    void hangmanMode() {
	leftOffset = 16; 
	rightOffset = 25;
	currentLetter = 0;
	tries = triesTotal;
	//memset(alphabet, 0, 26);
	strcpy(alphabet, letters);
	Serial.println(alphabet);
	mode = 0;
    }

    void topicMode() {
	leftOffset = 16; 
	rightOffset = 25;
	currentLetter = 0;
	//memset(alphabet, 0, 26);
	strcpy(alphabet, topic);
	mode = 1;
    }

    void winningMode() {
	leftOffset = 16; 
	rightOffset = 25;
	currentLetter = 0;
	mode = 2;
    }

    void winning(){ //show winning
	int i = 0;
	bool side = true;
	String line = "                ";
	window.setFirstLine("  Y O U  W I N! ");
	window.displayFirstLine();
	window.setSecondLine(line);
	window.displaySecondLine();
	while(true){
	    lcd.noDisplay();
	    delay(200);
	    lcd.display();
	    window.setSecondLine(line);
	    window.displaySecondLine();
	    lcd.setCursor(i, 1);
	    lcd.write(byte(0));
	    lcd.setCursor(i, 1);
	    delay(200);
	    if (side){
		i++;
		if (i >= 15){
		    side = false;
		}
	    } else {
		i--;
		if (i <= 0){
		    side = true;
		}
	    }
	}
    }

    void losing(){ //show losing
	int i = 0;
	bool side = true;
	String line = "                ";
	window.setFirstLine("  Y O U  L O S E!");
	window.displayFirstLine();
	window.setSecondLine(line);
	window.displaySecondLine();
	while(true){
	    lcd.noDisplay();
	    delay(200);
	    lcd.display();
	    lcd.setCursor(i, 1);
	    lcd.write(byte(1));
	    lcd.setCursor(i, 1);
	    delay(200);
	    if (side){
		i++;
		if (i >= 15){
		    side = false;
		}
	    } else {
		i--;
		if (i <= 0){
		    side = true;
		}
	    }
	}

    }

    void losingMode(){
	leftOffset = 16; 
	rightOffset = 25;
	currentLetter = 0;
	mode = 3;
    }

    void initTopicFirstLine(){
	window.setFirstLine(topics[0][0]);
	window.displayFirstLine();
    }

    void initFirstLine(){
	unsigned int wordLength = wordLen();
	String result = "";
	for (int i = 0; i < wordLength;i++){
	    result += '_';
	}
	window.setFirstLineHangman(result, tries);
	window.displayFirstLine();
    }

    void initSecondLine() {
	window.setSecondLine(String(alphabet).substring(0, 16));
	window.displaySecondLine();
    }

    void makeGuess(){
	unsigned int guessed = guessLetter(String(alphabet[currentLetter]));
	String g;
	if (!hasMatch()){
	    tries--;
	}
	produce_guessed_word(guessed, word(), g);
	window.setFirstLineHangman(g, tries);
	window.displayFirstLine();
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
	window.scrollSecondLine(side, c);
    }

    void changeTopicFirstLine(){
	topicCounter++;
	if (topicCounter == 4){
	    topicCounter = 0;
	}
	window.setFirstLine(topics[topicCounter][0]);
	window.displayFirstLine();	
    }

    void startHangman(){
	hangmanMode();
	initFirstLine();
	initSecondLine();
    }

    void selectTopic(){
	unsigned char r = random(1, wlen+1);
	setWord(String(topics[topicCounter][r]));
	startHangman();
    }

    void guessLoop(int value) {
	if (value == 2){
	    scrollAlphabet(true);
	    if (mode == 1) {
		changeTopicFirstLine();
	    }
	} else if (value == 1){
	    scrollAlphabet(false);
	} else if (value == 3){
	    if (mode == 1){
		selectTopic();
	    } else {
		Serial.println(alphabet);
		makeGuess();
		if (isGuessed()) { //if you win
		    delay(1000);
		    winningMode();
		    winning();
		    return;
		}
		if (tries == 0) { //if you lose
		    delay(1000);
		    losingMode();
		    losing();
		    return;
		}
	    }
	}
    }
};


Window win = Window(lcd);
Hangman hangman = Hangman(6, win);

void setup() {
    Serial.begin(9600);
    lcd.createChar(0, smiley);
    lcd.createChar(1, sad);
    // set up the LCD's number of columns and rows: 
    lcd.begin(16, 2);
    // turn on the cursor:
    lcd.cursor();
    //hangman.initFirstLine();
    hangman.topicMode();
    hangman.initTopicFirstLine();
    hangman.initSecondLine();

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
    delay(200);
    // reads the value of the variable resistor
    value2 = analogRead(joyPin2);
    int dir = getJoyDirection(treatValue(value1), treatValue(value2));
    hangman.guessLoop(dir);
}

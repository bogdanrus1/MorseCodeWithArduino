/****PROJECT: MORSE ENCODER AND DECODER WITH AN ARDUINO UNO BOARD****/
/****CODERS:  BOGDAN SEVER-CRISTIAN & RUS BOGDAN-ALEXANDRU****/
/****VERSION: 1.0****/
/****DATE:    08.11.2021****/
/****DESCRIPTION:  By pressing a button on the breadboard, you can easily intorduce the morse code of letters and numbers
                   which will be displayed on the LCD screen. Also, when the button is pressed a LED will be on indicating
                   that the circuit it is working.****/


#include <LiquidCrystal.h>

//****Pins section****//
const int but = 8;  //Pin for button

const int led = 13; //Pin for led

const int rs = 12, en = 9, d4 = 5, d5 = 4, d6 = 6, d7 = 2;  //Pins for LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);  //Connecting the lcd to arduino through pins



//****Period before clearing the LCD screen****//
const unsigned long period = 40;



//****The definitions for the morse code****//
String code = ""; 
int len = 0;
char error_msg[]= "  This is not a valid code!  ";
int cursorPos = 0;
int error = 0;



//****Times for the morse difference between dash and dot****//
unsigned long pres_len = 0, rel_time = 0, pres_time = 0, space_time = 0, lcd_time;
int unit_delay = 250;
int min_delay = 10;



//****Collumn and row for Display****//
int iLineNum = 1; //collumn (0 - first, 1 - second)
int iCursor = 0;  //cursor postion (0 - 15)

void(* resetFunc)(void) = 0;

//****Scrolling Error Message Function//
void LCDDisplay()
{
  int iLenofText = strlen(error_msg); //Calculate length of string
  if(iCursor == (iLenofText - 1)) //Reset variable for rollover effect
  {
    iCursor = 0;
  }
  lcd.setCursor(0, iLineNum);
  if(iCursor < iLenofText - 16) //Loop for normal 16 character error message
  {
    for(int i = iCursor; i < iCursor + 16; i++)
    {
      lcd.print(error_msg[i]);  //Print error message to the LCD
    }
  }else
    {
      for(int i = iCursor; i < iLenofText - 1; i++) //Loop that takes care of printing characters of current string
      {
        lcd.print(error_msg[i]);  //Print error message to the LCD
      }

      for(int j = 0; j <= 16 - (iLenofText - iCursor); j++) //Remaining characters will be printed by this loop
      {
        lcd.print(error_msg[j]);  //Print error message to the LCD
      }
      
    }
    iCursor++;
}



//***********Function to be called to make a string from the inputs of the user**************//
char MakeString()
{
  if(pres_len < (unit_delay * 3) && pres_len > 50)  //It is a dot if the length (in ms) which the button 
                                                    //is pressed is more than 0.05 seconds 
                                                    //and is smaller than 0.75 seconds
  {
    return '.';
  }else if(pres_len > (unit_delay * 3)) //It is a dash if the length 
                                        //(in ms) which the button is pressed is more than 0.75 seconds
  {
    return '-'; 
  }
}

//***********Function to display the Morse Code in light form**************//
void light()
{
  int char_length = code.length();
  for(int i = 0; i < char_length; i++)
  {
     if( code[i] == '.'){
        digitalWrite(led, HIGH);
        delay(750);  
        digitalWrite(led, LOW);
        delay(500);
     }
     if( code[i] == '-'){
        digitalWrite(led, HIGH);
        delay(1750);
        digitalWrite(led, LOW);
        delay(500);
     }
  }

  lcd_time = millis();
  
}


//***********Morse decoder function**************//
void Morse_decod()
{
  static String morseL[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....",
                             "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-",          
                             ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "!"
                            };
                            //The alphabet in dashes and dots

  static String morseN[] = {"-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----.", "?"};   
                            //The numbers in dashses and dots


  int i = 0, switchL = 0; //A switch if the code was recognised as a letter
  
  while (morseL[i] != "!")  
  {
    if (morseL[i] == code)
    {
      lcd.print(char('A' + i));  //The letter will be printed on the LCD if it was found in the string defined above
      switchL = 1;
      cursorPos++;
      if(cursorPos == 16)
      {
        lcd.scrollDisplayLeft();
        cursorPos--;
      }
      light();
      break;
    }
    i++;
  }

  int switchN=0;  //A switch if the code was recognised as a number
  i = 0;

 
  while(morseN[i] != "?" && switchL==0) //If it was not found a letter, the decoder will 
                                        //look through the numbers and decide if it is a valid code or not
  {
    switchN=1;  
    if(morseN[i] == code)
    {
      lcd.print(char('0' + i)); //The number will be printed on the 
                                //LCD if it was found in the string defined above
      light();
      cursorPos++;
      if(cursorPos == 16)
      {
        lcd.scrollDisplayLeft();
        cursorPos--;
      }
      break;
    }
    i++;
  }
  
  if (morseN[i] == "?" && switchN==1)
  {
    error = 1;
    int i = 0;
    while(i <= period)
    {
      LCDDisplay();  // If it was not recognised as a valid letter 
                     //or a valid number, a short meassage will be displayed on the LCD
      delay(500);
      i++;
    }
    resetFunc(); //The LCD will be cleared after 20 seconds
  }
  
  code = ""; //The code will be reseted for the next iteration of the function
}



void setup() 
{
  lcd.begin(16, 2); //Setup for LCD 16x2
  lcd.setCursor(0, 0);  //Setup for cursor on row 0, collumn 0
  pinMode(but, INPUT_PULLUP); //Setup for button as input
  pinMode(led, OUTPUT); //Setup for led as output
}



void loop() 
{
label:
  while (digitalRead(but) == HIGH) //Getting input from the arduino that the button is pressed
  {
    
  }
  pres_time = millis(); //Getting the time that passed from the start/reset of the arduino board
 
  if (error == 0 && lcd_time != 0 && (pres_time - lcd_time) > (unit_delay * 12))
  {
    lcd.print(" ");
    lcd_time = 0;
    cursorPos++;
    if(cursorPos == 16)
    {
      lcd.scrollDisplayLeft();
      cursorPos--;
    }
  }

  error = 0;

  while (digitalRead(but) == LOW) //Getting input from the arduino that the button is NOT pressed 
  {
    
  }
  rel_time = millis();  //Getting the time that passed from the start/reset of the arduino board
 
  
  pres_len = rel_time - pres_time;  //The length (in ms) the button was pressed
  if (pres_len > min_delay)
  {
    code += MakeString();   //Button pressed valid if the length (in ms) 
                            //is greater than min_delay and the -/. is added to code
  }
  
  while ((millis() - rel_time) < (unit_delay * 3))  //Goes back to recognise the input if the time between the 
                                                    //pressings of the button (new vs old) is smaller than 0.75 seconds
  {
    if (digitalRead(but) == LOW)
    {
      goto label; //Goes back to label to get another input of the same code
    }
  }
  
  Morse_decod();  //Goes to decoder of the code if the time between
                  //the new vs old pressing of the button is higher than 0.75 seconds
}

/*

*/

#include <LiquidCrystal.h>
#include <Keypad.h>

//Set up keypad
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {46, 44, 42, 40}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {52, 50, 48}; //connect to the column pinouts of the keypad
Keypad keypadA = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//Set up LCD
LiquidCrystal lcd(43, 45, 47, 49, 51, 53);

//Pin definations 
const int RLED = 23;
const int GLED = 25;
const int BLED = 27;
const int largeKey = 31;
const int smallKey = 30;
const int wire1 = 22;
const int wire2 = 24;
const int wire3 = 26;
const int wire4 = 28;

//State variables
int mode = 1;
int run = 0;
int progState = 0;
int statea = 0;

//Keypad stuff
int row = 0;
int col = 0;
volatile boolean butPress = false;
volatile char but = '-';
int thresh = 206;

//
boolean keypadBool = false;
boolean wireBool = false;
boolean keyBool = false;
boolean timerBool = false;
//
boolean bang = false;
boolean keypadDis = false;
boolean keyDis = false;
boolean wireDis = false;
boolean disarmed = false;
//
boolean lKey = false;
boolean armed = false;
boolean programed = false;
boolean lcdLck = false;
boolean attem = false;
int attempts = 0;
int attemCount = 0;
char code[4] = {'-','-','-','-'};
char butt[4] = {'-','-','-','-'};
char time[4] = {'-','-','-','-'};
int safeWire = 0;
int i = 0;


//==============================================================================================
//=============================================SETUP LOOP=======================================
//==============================================================================================
void setup(){
  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(BLED, OUTPUT);
  pinMode(wire1, INPUT);
  pinMode(wire2, INPUT);
  pinMode(wire3, INPUT);
  pinMode(wire4, INPUT);
  pinMode(largeKey, INPUT);
  pinMode(smallKey, INPUT);

  TCCR1A |= (1 << COM1A0);
  TCCR1B |= (1 << WGM12) | (1 << CS12);
  TIMSK1 |= (1 << TOIE1);
  sei();
  
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Bomb prop V1.1");
  lcd.setCursor(0,1);
  lcd.print("J McKenna [MMS]");
  for (int i = 0 ; i <= 3 ; i++){
    delay(500);
    setLED(0);
    delay(500);
    setLED(1);
  }
  lcd.clear();
}


//==============================================================================================
//=============================================MAIN LOOP========================================
//==============================================================================================
void loop(){

  switch(run){
   case 0:
     program();
   break;
   
   case 1:
     running();
   break;
  }
  
  if(digitalRead(smallKey) == LOW){
    run = 1; 
  }
  else{
    run = 0;
  }
}

//===============================================================================
void running(){
  
  if(programed == true){
  switch(statea){
    case 1:
        lcd.setCursor(0, 0);
        lcd.print("     Armed      ");
        lcd.setCursor(0, 1);
        
        if((keyBool == true) && (keyDis != true)){
          lcd.print("K=");
          if((digitalRead(largeKey) == HIGH) && (lKey == true)){
             keyDis = true;
             lcd.print("D");
          }
          else if((digitalRead(largeKey) == LOW) && (lKey == false)){
             keyDis = true;
             lcd.print("D");
          }
          else
            lcd.print("A");
        }
        
        if((keypadBool == true) && (keypadDis != true)){
          
           but = keypadA.getKey();
           
           if((but != NO_KEY) && (i < 4)){
             butt[i] = but;
             i++;
             if(i == 4){
               if((code[0] == butt[0]) && (code[1] == butt[1]) && (code[2] == butt[2]) && (code[3] == butt[3]))
                 keypadDis = true;
               else{
                 butt[0] = '-';
                 butt[1] = '-';
                 butt[2] = '-';
                 butt[3] = '-';
                 i = 0;
                 if(attem == true){
                   attemCount--;
                   if(attemCount == 0){
                     bang = true; 
                   }
                 }
               }
             }
             but = '-';
           }
           lcd.print(butt[0]);
           lcd.print(butt[1]);
           lcd.print(butt[2]);
           lcd.print(butt[3]);
           lcd.print(" ");
           lcd.print(attemCount);
        }
        
        if(wireBool == true){
          if((digitalRead(wire1) == LOW) && (safeWire == 1))
            wireDis = true;
          else if((digitalRead(wire1) == LOW) && (safeWire != 1))
            bang = true;
            
          if((digitalRead(wire2) == LOW) && (safeWire == 2))
            wireDis = true;
          else if((digitalRead(wire2) == LOW) && (safeWire != 2))
            bang = true;
       
          if((digitalRead(wire3) == LOW) && (safeWire == 3))
            wireDis = true;
          else if((digitalRead(wire3) == LOW) && (safeWire != 3))
            bang = true;
            
          if((digitalRead(wire4) == LOW) && (safeWire == 4))
            wireDis = true;
          else if((digitalRead(wire4) == LOW) && (safeWire != 4))
            bang = true;
        }
        
        if(bang == true){
          statea = 3;
        }
        
        if((wireDis == true) || ((keyDis == keyBool) && (keypadDis == keypadBool))){
          statea = 2;
        }
      break;
      
      case 2://Disarmed
        lcd.setCursor(0, 0);
        lcd.print("    Disarmed    ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
      break;
      
      case 3://Bang
        lcd.setCursor(0, 0);
        lcd.print("      BANG      ");
        lcd.setCursor(0, 1);
        lcd.print("    You lose    ");
      break;
    }
  }
  else{
    lcd.setCursor(0, 0);
    lcd.print("Not configured"); 
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}

//===============================================================================
void program(){
 
 switch(progState){
  
  case 0:
  
    lcd.setCursor(0,0);
    lcd.print("Use keypad ?    ");
    lcd.setCursor(0,1);
    lcd.print("*-Yes       #-No");
    programed = false;
    
    but = keypadA.getKey();
    
    if (but == '#'){
      keypadBool = false;
      progState = 1;
      but = NO_KEY;
    }
    else if (but == '*'){
      keypadBool = true;
      progState = 1;
      but = NO_KEY;
    }

  break;
    
  //----------------------------------------------------------------------
  case 1:
    lcd.setCursor(0,0);
    lcd.print("Use key ?       ");
    lcd.setCursor(0,1);
    lcd.print("*-Yes       #-No");
    
    but = keypadA.getKey();
    
    if (but == '#'){
      keyBool = false;
      progState = 2;
      but = NO_KEY;
    }
    else if (but == '*'){
      keyBool = true;
      progState = 2;
      but = NO_KEY;
    }
  break;
  
  //----------------------------------------------------------------------
  case 2:
    lcd.setCursor(0,0);
    lcd.print("Use Wires ?     ");
    lcd.setCursor(0,1);
    lcd.print("*-Yes       #-No");
    
    but = keypadA.getKey();
    
    if (but == '#'){
      wireBool = false;
      progState = 3;
      but = NO_KEY;
    }
    else if (but == '*'){
      wireBool = true;
      progState = 3;
      but = NO_KEY;
    }
  break;
  
  //----------------------------------------------------------------------
  case 3:
    lcd.setCursor(0,0);
    lcd.print("Use Timer ?     ");
    lcd.setCursor(0,1);
    lcd.print("*-Yes       #-No");
    
    but = keypadA.getKey();
    
    if (but == '#'){
      timerBool = false;
      progState = 4;
      but = NO_KEY;
    }
    else if (but == '*'){
      timerBool = true;
      progState = 4;
      but = NO_KEY;
    }
  break;
  
  //----------------------------------------------------------------------
  case 4:
    lcd.setCursor(0,0);
    lcd.print("KP-");
    lcd.print(keypadBool);
    lcd.print("Ky-");
    lcd.print(keyBool);
    lcd.print("W-");
    lcd.print(wireBool);
    lcd.print("T-");
    lcd.print(timerBool);
    lcd.setCursor(0,1);
    lcd.print("*-Yes       #-No");
    
    but = keypadA.getKey();
    
    if (but == '#'){
      progState = 5;
      but = NO_KEY;
    }
    else if (but == '*'){
      progState = 5;
      statea = 1;
      i = 0;
      but = NO_KEY;
    }
  break;
  
  //----------------------------------------------------------------------
  case 5:
  
    switch(mode){
      
      case 1:
        if(keypadBool == true){
          switch (statea){
            case 1:
                lcd.setCursor(0, 0);
                lcd.print("Enter 4Digit pin");
                lcd.setCursor(0, 1);
                lcd.print(code[0]);
                lcd.print(code[1]);
                lcd.print(code[2]);
                lcd.print(code[3]);
                lcd.print("           ");
                
                but = keypadA.getKey();
                
                if((but != NO_KEY) && (i < 4)){
                  code[i] = but;
                  i++;
                  but = '-';
                }
                
                if(i >= 4){
                  statea = 2; 
                  i = 0;
                }
            break;
            
            case 2:
              lcd.setCursor(0, 0);
              lcd.print("# of attempts   ");
              lcd.setCursor(0, 1);
              lcd.print(time[0]);
              lcd.print(time[1]);
              lcd.print(time[2]);
              lcd.print(time[3]);
              
              but = keypadA.getKey();
              
              if((i >= 4) || (but == '#')){
                attempts = atoi(time);
                if(attempts == 0)
                  attem = false;
                else
                  attem = true;
                statea = 3;
                but = NO_KEY;
              }
              
              if((but != NO_KEY) && (i < 4)){
                time[i] = but;
                i++;
                but = '-';
              }
            break;
            
            case 3:
              lcd.setCursor(0, 0);
              lcd.print("C -");
              lcd.print(code[0]);
              lcd.print(code[1]);
              lcd.print(code[2]);
              lcd.print(code[3]);
              lcd.print(" A -");
              lcd.print(attempts);
              lcd.setCursor(0, 1);
              lcd.print("*-Yes       #-No");
        
              but = keypadA.getKey();
              
            if (but == '#'){
              statea = 1;
              i = 0;
              code[0] = '-';
              code[1] = '-';
              code[2] = '-';
              code[3] = '-';
              but = NO_KEY;
            }
            else if (but == '*'){
              statea = 5;
              mode = 2;
              attemCount = attempts;
              but = NO_KEY;
            }
          break;
        }
      }
      else
        mode = 2;
    break;
    
    case 2:
      if(keyBool == true){
        if(digitalRead(largeKey) == HIGH)
          lKey = false;
        else
          lKey = true;
      }
      statea = 1;
      mode = 3;
    break;
    
    case 3:
      if(wireBool == true){
        switch(statea){
          case 1:
            lcd.setCursor(0, 0);
            lcd.print("Sel disarm wire");
            lcd.setCursor(0, 1);
            lcd.print("                ");
            
            but = keypadA.getKey();
              
            if(but == '1'){
              safeWire = 1;
              but = '-';
              statea = 2;
            }
            else if(but == '2'){
              safeWire = 2;
              but = '-';
              statea = 2;
            }
            else if(but == '3'){
              safeWire = 3;
              but = '-';
              statea = 2;
            }
            else if(but == '4'){
              safeWire = 4;
              but = '-';
              statea = 2;
            }
          break;
          
          case 2:
            lcd.setCursor(0, 0);
            lcd.print("Disarm wire - ");
            lcd.print(safeWire);
            lcd.setCursor(0, 1);
            lcd.print("*-Yes       #-No");
      
            but = keypadA.getKey();
            
            if (but == '#'){
              statea = 1;
              safeWire = 0;
              but = NO_KEY;
            }
            else if (but == '*'){
              statea = 5;
              progState = 6;
              but = NO_KEY;
            }
          break;
        }
      }
      else{
        progState = 6; 
      } 
    break; 
  }
  break;
  
  //----------------------------------------------------------------------
  case 6:
    lcd.setCursor(0, 0);
    lcd.print("Remove prog key");
    lcd.setCursor(0, 1);
    lcd.print("     to arm    ");
    
    programed = true;
    statea = 1;
    i = 0;
    wireDis = false;
    keyDis = false;
    keypadDis = false;
    bang = false;
  break;
 }
}

//==============================================================================================
//=====================================BACKGROUND FUNCTIONS=====================================
//==============================================================================================

void setLED(int i){
  
 switch(i){
  case 0:
    digitalWrite(RLED, LOW);
    digitalWrite(GLED, LOW);
    digitalWrite(BLED, LOW);
  break;
  
  case 1:
    digitalWrite(RLED, HIGH);
    digitalWrite(GLED, HIGH);
    digitalWrite(BLED, HIGH);
  break;
  
  case 2:
    
  break;
  
  case 3:
  
  break;
  
  case 4:
  
  break;
 } 
}


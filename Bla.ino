/*
Airsoft bomb prop V1.1
James McKenna (Black team-2) [MMS]

Development board: Arduino mega 2560

Code:
Version: V1.1.5
last updated: 18-12-2013

Notes:
- Tilt switch not currently added


*/

#include <LiquidCrystal.h>
#include <Keypad.h>

//Set up keypad
const byte ROWS = 4; 
const byte COLS = 3; 
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {46, 44, 42, 40}; 
byte colPins[COLS] = {52, 50, 48}; 
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

//--------------------------------------------------
//---------------Variable definations---------------
//--------------------------------------------------

//State variables
int mode = 1;
int run = 0;
int progState = 0;
int statea = 0;

//Keypad stuff
volatile char but = '-';

//Prog variables
boolean keypadBool = false;
boolean wireBool = false;
boolean keyBool = false;
boolean timerBool = false;
//Running variables
boolean bang = false;
boolean keypadDis = false;
boolean keyDis = false;
boolean wireDis = false;
boolean timerTrig = false;
boolean disarmed = false;

//Genral use variables
boolean lKey = false;
boolean armed = false;
boolean programed = false;
boolean lcdLck = false;
boolean armLck = false;
boolean attem = false;
boolean timeUse = false;
int attempts = 0;
int attemCount = 0;
unsigned long countTime = 0;
unsigned long startTime = 0;
unsigned long tempTime = 0;
char code[4] = {'-','-','-','-'};
char butt[4] = {'-','-','-','-'};
char time[4] = {'-','-','-','-'};
int safeWire = 0;
float battLow = 8.8;
float battEmp = 8.0;
boolean bla = false;
volatile boolean flag = false;
float volt = 0;
int voltInt = 0;
int i = 0;
int j = 0;


//==============================================================================================
//===========================================SETUP FUNCTION=====================================
//==============================================================================================
void setup(){
  
  //Set pin modes
  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(BLED, OUTPUT);
  pinMode(wire1, INPUT);
  pinMode(wire2, INPUT);
  pinMode(wire3, INPUT);
  pinMode(wire4, INPUT);
  pinMode(largeKey, INPUT);
  pinMode(smallKey, INPUT);

  //Set up timer for blinking LED
  noInterrupts();          
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 31250;           
  TCCR1B |= (1 << WGM12);   
  TCCR1B |= (1 << CS12);                                                                                         
  TIMSK1 |= (1 << OCIE1A);  
  interrupts(); 
  
  //Welcome message + LED test
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

  //switch between modes
  switch(run){
   case 0:
     program();
   break;
   
   case 1:
     running();
   break;
  }
  
  //
  if(digitalRead(smallKey) == LOW){
    run = 1; 
  }
  else{
    run = 0;
  }
  
  if(flag == true){
    boo();
    flag = false;
  }
}

//=======================================Running mode========================================
void running(){
  
  if(programed == true){
    
    //Clear the lcd once at beginning, get starting time for timer
    if(lcdLck == false){
     lcd.clear();
     lcdLck = true;
     startTime = millis();
    }
    
    switch(statea){
      case 1://Armed
          lcd.setCursor(0, 0);
          lcd.print("     Armed      ");
		  //lcd.print(volt);
		  //lcd.print(" , ");
		  //lcd.print(voltInt);
          lcd.setCursor(0, 1);
          
          //Timer
          if(timerBool == true){
            tempTime = millis() - startTime;
            tempTime = countTime - tempTime;
            if(tempTime >= countTime){
              timerTrig = true;
            }
            lcd.print((tempTime/1000)/ 60);//display min
            lcd.print(":");
            tempTime = (tempTime/1000)%60;//calc sec
            if(tempTime <= 9){// pad out if less than 10
              lcd.print("0");
              lcd.print(tempTime);
            }
            else
              lcd.print(tempTime);
            
          }
          
          //Key
          lcd.setCursor(6, 1);
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
          
          //Keypad 
          lcd.setCursor(10, 1);
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
          
          //Wire checking
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
          
          //Check if the device has detonated
          if((bang == true) || (timerTrig == true)){
            statea = 3;
            armLck = false;
            armed = false;
          }
          
          //Check if the device is safe
          if((wireDis == true) || ((keyDis == keyBool) && (keypadDis == keypadBool) && ((keyBool != false) || (keypadBool != false)))){
            statea = 2;
            armLck = false;
            armed = false;
            disarmed = true;
            setLED(5);
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
  else{//Alert user device is not configured
    lcd.setCursor(0, 0);
    lcd.print("Not configured"); 
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}

//======================================Program=========================================
void program(){
 setLED(4);//Turn Blue LED on
 if((programed == false) || (armed == true)){
   switch(progState){
    //---------------------------------Use keypad------------------------------------
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
      
    //---------------------------------Use Key-------------------------------------
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
    
    //---------------------------------Use Wires-------------------------------------
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
    
    //-----------------------------------Use Timer-----------------------------------
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
    
    //---------------------------------Confirm selection-------------------------------------
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
        progState = 0;
        but = NO_KEY;
      }
      else if (but == '*'){
        progState = 5;
        statea = 1;
        mode = 1;
        i = 0;
        but = NO_KEY;
      }
    break;
    
    //------------------------------------Set up--------------------------------------
    case 5:
    
      switch(mode){
        
        case 1://-------------------------------------Set code------------------------------------
          if(keypadBool == true){
            switch (statea){
              case 1://Enter code
                  lcd.setCursor(0, 0);
                  lcd.print("Enter 4Digit pin");
                  lcd.setCursor(0, 1);
                  lcd.print(code[0]);
                  lcd.print(code[1]);
                  lcd.print(code[2]);
                  lcd.print(code[3]);
                  lcd.print("            ");
                  
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
              
              case 2://enter number of attempts at code
                lcd.setCursor(0, 0);
                lcd.print("# of attempts   ");
                lcd.setCursor(0, 1);
                lcd.print(time[0]);
                lcd.print(time[1]);
                lcd.print(time[2]);
                lcd.print(time[3]);
                lcd.print("            ");
                
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
              
              case 3://Confirm selection
                lcd.setCursor(0, 0);
                lcd.print("C -");
                lcd.print(code[0]);
                lcd.print(code[1]);
                lcd.print(code[2]);
                lcd.print(code[3]);
                lcd.print(" A -");
                lcd.print(attempts);
                lcd.print("    ");
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
                  time[0] = '-';
                  time[1] = '-';
                  time[2] = '-';
                  time[3] = '-';
                  i = 0;
                  but = NO_KEY;
                }
                else if (but == '*'){
                  statea = 5;
                  mode = 2;
                  attemCount = attempts;
                  time[0] = '-';
                  time[1] = '-';
                  time[2] = '-';
                  time[3] = '-';
                  i = 0;
                  but = NO_KEY;
                }
            break;
          }
        }
        else
          mode = 2;
      break;
      
      case 2://-------------------------------------Find key state------------------------------------
        if(keyBool == true){
          if(digitalRead(largeKey) == HIGH)
            lKey = false;
          else
            lKey = true;
        }
        statea = 1;
        mode = 3;
      break;
      
      case 3://------------------------------------Select disarm wire-------------------------------------
        if(wireBool == true){
          switch(statea){
            case 1://------select wire
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
            
            case 2://Confirm disarm wire
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
                mode = 4;
                time[0];
                time[1];
                time[2];
                time[3];
                but = NO_KEY;
              }
            break;
          }
        }
        else{
          mode = 4;
        } 
      break; 
      
      case 4://------------------------------------Select time limit-------------------------------------
        if(timerBool == true){
          switch(statea){
            case 1: // Select time limit
              lcd.setCursor(0, 0);
              lcd.print("Set time limit  ");
              lcd.setCursor(0, 1);
              lcd.print(time[0]);
              lcd.print(time[1]);
              lcd.print(time[2]);
              lcd.print(time[3]);
              lcd.print("            ");
              
              but = keypadA.getKey();
              
              if((i >= 4) || (but == '#')){
                countTime = atol(time);
                if(countTime == 0)
                  timeUse = false;
                else
                  timeUse = true;
                statea = 2;
                but = NO_KEY;
              }
              
              if((but != NO_KEY) && (i < 4) && (but != '*')){
                time[i] = but;
                i++;
                but = '-';
              }
            break;
            
            case 2://--------------------------------Confirm time limit-----------------------------------
              lcd.setCursor(0, 0);
              lcd.print("Time limit- ");
              lcd.print(countTime);
              lcd.print("   ");
              lcd.setCursor(0, 1);
              lcd.print("*-Yes       #-No");
        
              but = keypadA.getKey();
              
              if (but == '#'){
                statea = 1;
                time[0] = '-';
                time[1] = '-';
                time[2] = '-';
                time[3] = '-';
                i = 0;
                but = NO_KEY;
              }
              else if (but == '*'){
                countTime = countTime * 1000 * 60;
                if(countTime == 0)
                  timerBool = false;
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
    
    //------------------------------------Set up complete, move key----------------------------------
    case 6:
      lcd.setCursor(0, 0);
      lcd.print("Remove prog key ");
      lcd.setCursor(0, 1);
      lcd.print("     to arm     ");
      
      programed = true;
      armed = true;
      statea = 1;
      i = 0;
      wireDis = false;
      keyDis = false;
      keypadDis = false;
      bang = false;
	  timerTrig = false;
	  attemCount = attempts;
      disarmed = false;
      lcdLck = false;
      setLED(1);
    break;
   }
 }
 else if(armed == false){//--------------------------------Set up is already complete-----------------------------------
 
   //reset variables
   if(armLck == false){
     statea = 11;
     armLck = true; 
   }
   
   switch(statea){
     case 11:// ask if they want to reset
       lcd.setCursor(0, 0);
       lcd.print("Reset settings  ");
       lcd.setCursor(0, 1);
       lcd.print("*-Yes       #-No");
        
       but = keypadA.getKey();
      
       if (but == '#'){
		 statea = 13;
         but = NO_KEY;
       }
       else if (but == '*'){
         statea = 12;
         but = NO_KEY;
       }
     break;
     
     case 12:// confirm resting
       lcd.setCursor(0, 0);
       lcd.print("  Are you sure  ");
       lcd.setCursor(0, 1);
       lcd.print("*-Yes       #-No");
        
       but = keypadA.getKey();
      
       if (but == '*'){
         progState = 0;
         statea = 0;
         programed = false;
         i = 0;
         bang = false;
         lcdLck = false;
         keypadDis = false;
         keyDis = false;
         wireDis = false;
         disarmed = false;
         keypadBool = false;
         wireBool = false;
         keyBool = false;
         timerBool = false;
         timerTrig = false;
         countTime = 0;
         startTime = 0;
         tempTime = 0;
         code[0] = '-';
         code[1] = '-';
         code[2] = '-';
         code[3] = '-';
         butt[0] = '-';
         butt[1] = '-';
         butt[2] = '-';
         butt[3] = '-';
         time[0] = '-';
         time[1] = '-';
         time[2] = '-';
         time[3] = '-';
         attempts = 0;
         but = NO_KEY;
         setLED(1);
       }
       else if (but == '#'){
         statea = 11;
         but = NO_KEY;
       }
     break;
     
     case 13:
		 lcd.setCursor(0, 0);
		 lcd.print("Reuse current   ");
		 lcd.setCursor(0, 1);
		 lcd.print("*-Yes       #-No");
     
		 but = keypadA.getKey();
		 
		 if (but == '*'){
			 statea = 14;
			 but = NO_KEY;
		 }
		 else if (but == '#'){
			 statea = 11;
			 but = NO_KEY;
		 }
     break;
	 
	 case 14:
		lcd.setCursor(0, 0);
		lcd.print("  Are you sure  ");
		lcd.setCursor(0, 1);
		lcd.print("*-Yes       #-No");
		
		but = keypadA.getKey();
		
		if (but == '*')
		{
			armed = true;
			progState = 6;
			lKey =  !digitalRead(largeKey);
			butt[0] = '-';
			butt[1] = '-';
			butt[2] = '-';
			butt[3] = '-';
			but = NO_KEY;
		}
		if (but == '#')
		{
			statea = 13;
			but = NO_KEY;
		}
		
		
	 break;
   }
 }
}

//==============================================================================================
//=====================================BACKGROUND FUNCTIONS=====================================
//==============================================================================================

//
void setLED(int q){
  
 switch(q){
  case 0:// all on
    digitalWrite(RLED, LOW);
    digitalWrite(GLED, LOW);
    digitalWrite(BLED, LOW);
  break;
  
  case 1:// all off
    digitalWrite(RLED, HIGH);
    digitalWrite(GLED, HIGH);
    digitalWrite(BLED, HIGH);
  break;
  
  case 2:// red on
    digitalWrite(RLED, LOW);
  break;
  
  case 3:// Green on
    digitalWrite(GLED, LOW);
  break;
  
  case 4:// Blue on
    digitalWrite(BLED, LOW);
  break;
  
  case 5:// red off
    digitalWrite(RLED, HIGH);
  break;
  
  case 6:// green off
    digitalWrite(GLED, HIGH);
  break;
  
  case 7:// blue off
    digitalWrite(BLED, HIGH);
  break;
 } 
}

void boo(){
  //Get batt voltage / by 2
  voltInt = analogRead(A5);
  
  //convert to Volts then * by 2 to accounnt for voltage divider
  volt = (float) ((((voltInt - 0.0)*5.0)/1024.0) * 2.0);
  
  //
  if((armed == true) && (statea == 1)){
    if((volt <= battLow) && (volt >= battEmp)){
      if(bla){
        lcd.setCursor(0, 0);
        lcd.print("     Armed      ");
        lcd.setCursor(0, 1);
        bla = false;
      }
      else{
        lcd.setCursor(0, 0);
        lcd.print("   Batt low     ");
        lcd.setCursor(0, 1);
        bla = true;
      }
    }
    else if(volt <= battEmp){
      if(bla){
        lcd.setCursor(0, 0);
        lcd.print("     Armed      ");
        lcd.setCursor(0, 1);
        bla = false;
      }
      else{
        lcd.setCursor(0, 0);
        lcd.print("  Batt Empty   ");
        lcd.setCursor(0, 1);
        bla = true;
      }
    }
    else{
      lcd.setCursor(0, 0);
      lcd.print("     Armed      ");
      lcd.setCursor(0, 1);
    }
  }
}

//Timer 1 Compare A interupt vector
ISR(TIMER1_COMPA_vect){
  
  flag = true;
  
  //Flash the red LED once armed and running
  if((armed == true) && (digitalRead(smallKey) == LOW)){
    if(j == 0){
      setLED(2);
      j = 1;
    }
    else{
      setLED(5);
      j = 0; 
    }
  }
  
  //Set green LED on when disarmed
  if(disarmed == true){
     setLED(3);
  }
  
  //Flash all LEDS when the device detonates 
  if((bang == true) || (timerTrig == true)){
    if(j == 0){
      setLED(0);
      j = 1;
    }
    else{
      setLED(1);
      j = 0; 
    }
  }
}



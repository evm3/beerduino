#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Timer.h>

// Data wire is plugged into port 7 on the Arduino
#define ONE_WIRE_BUS 7

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

DeviceAddress thermometer = { 0x28, 0xE8, 0xA3, 0x10, 0x05, 0x00, 0x00, 0xE6};
LiquidCrystal lcd(12, 11, 22, 24, 26, 28);
 
int backLight = 30;    // pin 6 will control the backlight
float tempC; 
int on = 9; //On button RF remote
int off = 8; // off button
float temp_setpoint = 18.00;    //Temperature Set Point
const int buttonPin = 3;  // Pushbutton for backlight
int buttonState = 0;         // variable for reading the pushbutton status

const int WAIT_MODE = 0;
const int TEMP_MODE = 1;
int state = TEMP_MODE;

Timer t;
const long FIVE_MINUTES = 300000L;

boolean areWeOn = false;

void setup(void)
{
    // start serial port
    Serial.begin(9600);
    pinMode(backLight, OUTPUT);
    pinMode(on, OUTPUT);
    digitalWrite (on, LOW);
    pinMode (off, OUTPUT);
    digitalWrite (off, LOW);
    pinMode (buttonPin, INPUT);
    lcd.begin(16, 2);    
    sensors.begin();  
    sensors.setResolution(thermometer, 12);
}

void loop(void)
{   
  switch(state){
      case WAIT_MODE:        
        break;
      case TEMP_MODE:
        doTempMode();
        break;       
  }
  
  t.update();
  measureTemp();
  updateDisplay();
  checkBackLightButtons();
}

void measureTemp(){
    sensors.requestTemperatures(); // Send the command to get temperatures
    tempC = sensors.getTempC(thermometer);
}

void updateDisplay(){
    Serial.print ( "Temperature:   ");
    Serial.println(sensors.getTempC(thermometer));
    lcd.setCursor(0, 0);
    lcd.print("Temp : ");
    lcd.print(tempC,1);
    lcd.print(" \337C");   
    lcd.setCursor(0, 1);
    
     switch(state){
      case WAIT_MODE:  
        lcd.print("WAIT MODE");      
        break;
      case TEMP_MODE:
        lcd.print("TEMP MODE");
        break;       
  }
  
  if(areWeOn){
    lcd.print(" *ON*"); 
  }else{
    lcd.print(" *OFF*"); 
  }
}

void doTempMode(){
    
  if ((tempC > temp_setpoint)  && (areWeOn == false)) {
      digitalWrite(on, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(300);               // wait for a second
      digitalWrite(on, LOW);
      areWeOn = true;
      state = WAIT_MODE;
      t.after(FIVE_MINUTES, switchToTempMode);
  }  // turn the LED off by making the voltage LOW
 else if((tempC <= temp_setpoint) && (areWeOn == true)) {
      digitalWrite(off, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(300);               // wait for a second
      digitalWrite(off, LOW);
      areWeOn = false;
      state = WAIT_MODE;
      t.after(FIVE_MINUTES, switchToTempMode);
 }            
}

void switchToTempMode(){
  state = TEMP_MODE;
}

void checkBackLightButtons(){
  buttonState = digitalRead(buttonPin);  
  
  if (buttonState == HIGH) {     
    // turn backlight on:    
    digitalWrite(backLight, HIGH);  
  } 
  else {
    // turn backlight off:
    digitalWrite(backLight, LOW); 
  } 
}

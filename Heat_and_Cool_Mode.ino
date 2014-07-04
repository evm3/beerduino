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
 
int backLight = 30;    
float tempC; 
int CoolOnPin = 9; 
int CoolOffPin = 8; 
int HeatOnPin = 5;
int HeatOffPin = 4;
float temp_setpoint = 19.00;    
const int buttonPin = 3; 
int buttonState = 0;        

const int WAIT_MODE = 0;
const int TEMP_MODE = 1;
const int HEAT_MODE = 2;
int state = TEMP_MODE;

Timer timer;
const long FIVE_MINUTES = 300000L;

boolean areWeOnCool = false;
boolean areWeOnHeat = false;

void setup(void)
{
    
    Serial.begin(9600);
    pinMode(backLight, OUTPUT);
    pinMode(CoolOnPin, OUTPUT);
    digitalWrite (CoolOnPin, LOW);
    pinMode (CoolOffPin, OUTPUT);
    digitalWrite (CoolOffPin, LOW);
    pinMode(HeatOnPin, OUTPUT);
    digitalWrite (HeatOnPin, LOW);
    pinMode (HeatOffPin, OUTPUT);
    digitalWrite (HeatOffPin, LOW);
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
        doCoolMode();
        break; 
      case HEAT_MODE:
        doHeatMode();
        break;      
  }
  
  timer.update();
  measureTemp();
  updateDisplay();
  checkBackLightButtons();
}

void measureTemp(){
    sensors.requestTemperatures(); 
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
  
  if(areWeOnCool){
    lcd.print(" *ONC*"); 
  }else{
    lcd.print(" *OFFC*"); 
  }
  
   if(areWeOnHeat){
    lcd.print(" * ONH*"); 
  }else{
    lcd.print(" * OFFH*"); 
  }
}

void doCoolMode(){
    
  if ((tempC > temp_setpoint)  && (areWeOnCool == false)) {
      delay(300);
      digitalWrite(CoolOnPin, HIGH);   
      delay(300);               
      digitalWrite(CoolOnPin, LOW);
      delay(300);
      areWeOnCool = true;
      state = WAIT_MODE && HEAT_MODE;
      timer.after(FIVE_MINUTES, switchToTempMode);
  }  
 else if((tempC <= temp_setpoint) && (areWeOnCool == true)) {
      delay(300);
      digitalWrite(CoolOffPin, HIGH);   
      delay(300);             
      digitalWrite(CoolOffPin, LOW);
      delay(300);
      areWeOnCool = false;
      state = WAIT_MODE && HEAT_MODE;
      timer.after(FIVE_MINUTES, switchToTempMode);
 }            
}

void switchToTempMode(){
  state = TEMP_MODE && HEAT_MODE;
}

void doHeatMode (){
 if ((tempC < temp_setpoint)  && (areWeOnHeat == false)) {
      delay(300);
      digitalWrite(HeatOnPin, HIGH);   
      delay(300);               
      digitalWrite(HeatOnPin, LOW);
      delay(300);
      areWeOnHeat = true;
 } else if ((tempC > temp_setpoint)  && (areWeOnHeat == true)) {
      delay(300);
      digitalWrite(CoolOffPin, HIGH);   
      delay(300);               
      digitalWrite(CoolOffPin, LOW);
      delay(300);
      areWeOnHeat = false;
 }
}
void checkBackLightButtons(){
  buttonState = digitalRead(buttonPin);  
  
  if (buttonState == HIGH) {     
       
    digitalWrite(backLight, HIGH);  
  } 
  else {

    digitalWrite(backLight, LOW); 
  } 
}

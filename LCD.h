#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define LCD_ADDRESS 0x27

// Set the LCD dimensions (16 columns and 2 rows for a 16x2 LCD)
#define LCD_COLUMNS 16
#define LCD_ROWS 2

// Initialize the LCD object with the LCD address and dimensions
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

void lcdInit()
{
  lcd.init();
  lcd.backlight();
}

void page_1(int _waterlevel)
 {
    lcd.clear();
    lcd.print("Water Level:");
    lcd.setCursor(13,0); 
    lcd.print(_waterlevel);

    lcd.setCursor(0,1);
    if (_waterlevel > 3 && _waterlevel < 8){
    lcd.print("WaterLevel OK");}
    else if (_waterlevel > 8 ){
    lcd.print("Low WaterLevel");}
     else if (_waterlevel < 4){ 
    lcd.print("High WaterLevel");}
 }

void page_2( float _ph)
 {  
    lcd.clear();
    lcd.print("pH Value: ");
    lcd.setCursor(10,0);
    lcd.print(_ph);
   

    if (_ph > 6.5 && _ph < 7.5){
     lcd.setCursor(5,1); lcd.print("Neutral");}
    else if (_ph < 6.5 ){
     lcd.setCursor(5,1); lcd.print("Acidic");}
     else if (_ph > 7.5){ 
    lcd.setCursor(4,1); lcd.print("Basic");}
 }
  void page_3(float _do)
 {
    lcd.clear();
    lcd.print("DO Value: ");
    lcd.setCursor(10,0);
    lcd.print(_do);
   

    if (_do > 14 && _do < 27){
     lcd.setCursor(5,1); lcd.print("Do OK");}
    else if (_do < 14 ){
    lcd.setCursor(4,1); lcd.print("Low Do");}
     else if (_do > 27){ 
     lcd.setCursor(4,1); lcd.print("Good Do");}
 }

 void page_4(float _temp)
 {
    lcd.clear();
    lcd.print("Water Temp: ");
    lcd.setCursor(12,0);
    lcd.print(_temp);


    if (_temp > 23 && _temp < 33){
       lcd.setCursor(4,1); lcd.print("Temp OK");}
    else if (_temp < 23 ){
      lcd.setCursor(4,1);   lcd.print("Low Temp");}
     else if (_temp > 32){ 
       lcd.setCursor(3,1); lcd.print("High Temp");}
 }

void page_5(int _ir)
 {   
    lcd.clear();
    lcd.print("Movement: ");
    lcd.setCursor(10,0);
    if (_ir==0){
    lcd.print("Good");
    }else
    {
    lcd.print("Slow");
    }
    
 }


#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

Servo lockServo;
int lockPos = -15;
int unlockPos = 75;
bool locked_s = true;

int redLEDPin = 5;
int greenLEDPin = 6;

String KeyCode1 = "E1 EE 06 1E";
String KeyCode2 = "30 A0 B6 A5";

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16,2);
/*** 
 *  
if (locked_s == true)
    {
      lockServo.write(unlockPos);
      locked_s = false;
    }

    else if (locked_s == false)
    {
      lockServo.write(lockPos);
      locked_s = true;
    }
    
***/

void Lock()
{
  locked_s = false;

  digitalWrite(redLEDPin, HIGH);
  digitalWrite(greenLEDPin, LOW);
  
  if (locked_s == false)
  {
    lockServo.write(lockPos);
    locked_s = true;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Access denied!");
  delay(1000);
  lcd.clear();
  lcd.print("Please try again...");
}

void Unlock()
{
  locked_s = true;
  
  digitalWrite(redLEDPin, LOW);
  digitalWrite(greenLEDPin, HIGH);
  
  if (locked_s == true)
  {
    lockServo.write(unlockPos);
    locked_s = false;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Access granted!");
}

void Idle()
{
  locked_s = false;

  digitalWrite(redLEDPin, LOW);
  digitalWrite(greenLEDPin, LOW);
  
  if (locked_s == false)
  {
    lockServo.write(lockPos);
    locked_s = true;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Insert key");
}

void QuickFlash(int light)
{
  digitalWrite(light, LOW);
  delay(150);
  digitalWrite(light, HIGH);
  /**
  delay(50);
  digitalWrite(light, LOW);
  delay(50);
  digitalWrite(light, HIGH);
  **/
}

void scrollMessage(int row, String message, int delayTime, int totalColumns) {
  for (int i=0; i < totalColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int position = 0; position < message.length(); position++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(position, position + totalColumns));
    delay(delayTime);
  }
}

void setup() 
{
  // Initializing the display and turning the backlights
  lcd.init();
  lcd.backlight();

  //lcd.print("Insert a key");
  Idle();
  
  pinMode(redLEDPin,OUTPUT);
  pinMode(greenLEDPin,OUTPUT);
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  // Servo init
  lockServo.attach(3);
  lockServo.write(lockPos);
 
  Serial.println("Approximate your card to the reader...");
  Serial.println();

}
void loop() 
{
 
  //scrollMessage(1, "Insert a key", 300, 16);
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) ==  KeyCode1|| content.substring(1) == KeyCode2) //to find card uid go to the serial moniter, scan your card and copy uid into the section

  {
    Unlock();
    
    Serial.println("Authorized access");
    Serial.println();
    delay(3000);
    Idle();
    
  }
 else if (content.substring(1) !=  KeyCode1|| content.substring(1) != KeyCode2) {
    Lock();
    Serial.println("Denied access");
    delay(3000);
    Idle();
  }
 else if (content.substring(0)){
    Idle();
    
  }
  
} 

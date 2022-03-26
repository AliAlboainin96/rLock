#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

Servo lockServo;
int lockPos = 15;
int unlockPos = 75;
bool locked_s = true;

int redLEDPin = 5;
int greenLEDPin = 6;

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

void setup() 
{
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
  Lock();
}
void loop() 
{
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
  if (content.substring(1) == "E1 EE 06 1E" || content.substring(1) == "30 A0 B6 A5") //to find card uid go to the serial moniter, scan your card and copy uid into the section

  {
    Unlock();
    
    Serial.println("Authorized access");
    Serial.println();
    delay(3000);
    Lock();
  }
 
 else   {
    Lock();
    Serial.println(" Access denied");
    delay(500);
    QuickFlash(redLEDPin);
    
  }
} 

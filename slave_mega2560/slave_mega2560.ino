#include <ArduinoJson.h>

#include <UTFT.h>
#include <URTouch.h>
#include <UTFT_Buttons.h>
#include <SoftwareSerial.h>

// Declare which fonts we will be using

extern uint8_t GroteskBold16x32[];
extern uint8_t BigFont[];
extern uint8_t CalibriBold32x48[];
extern uint8_t arial_bold[];

UTFT myGLCD(ILI9341_16, 38, 39, 40, 41);
URTouch myTouch(6, 5, 4, 3, 2);
UTFT_Buttons myButtons(&myGLCD, &myTouch);
SoftwareSerial linkSerial(10, 11); //RX, TX
SoftwareSerial linkflow(14, 15);   //RX, TX

int Vaule = 0;
int Token = 0;

int pressed_button, btnAir, btnStop, btnPause;
void setup()
{
  linkSerial.begin(115200);
  linkflow.begin(9600);
  Serial.begin(9600);
  Serial.print(" I received:");
  myGLCD.InitLCD(1);

  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myTouch.InitTouch(1);
  myTouch.setPrecision(PREC_MEDIUM);
  //myGLCD.rotateDisplay(true);
  myGLCD.fillScr(255, 255, 255);
  myGLCD.setBackColor(255, 255, 255);
  //myButtons.setTextFont(BigFont);
  //myGLCD.fillScr(248,255,131);
  //pinMode(8, OUTPUT);
  //digitalWrite(8, HIGH);
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
}

void start()
{
  myButtons.deleteAllButtons();
  myGLCD.fillScr(255, 255, 255);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.setColor(144, 222, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print("SPAIRUM DWS", CENTER, 5);
  myButtons.setTextFont(arial_bold);
  myGLCD.print("Tekan Start AMBIL", CENTER, 21);

  myGLCD.print("TOKEN:", 20, 50);
  myGLCD.setColor(17, 0, 225);
  myGLCD.setFont(CalibriBold32x48);
  myGLCD.printNumI(Token, 20, 80);
  myGLCD.setColor(144, 222, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print("x10 ML", 35, 150);

  myButtons.setTextFont(arial_bold);
  btnAir = myButtons.addButton(190, 50, 120, 70, "START");
  myButtons.drawButtons();
  myButtons.setTextFont(arial_bold);

  while (Token == 0)
  {
    if (linkSerial.available() > 0)
    {
      StaticJsonDocument<300> doc;
      DeserializationError err = deserializeJson(doc, linkSerial);
      if (err == DeserializationError::Ok)
      {
        Serial.print("Token: ");
        Vaule = doc["vaule"];
        Serial.print(Vaule);
        Token = Vaule;
        if (Token > 1)
        {
          start();
        }
      }
      else
      {
        // Print error to the "debug" serial port
        Serial.print("deserializeJson() returned ");
        Serial.println(err.c_str());

        while (linkSerial.available() > 0)
          linkSerial.read();

        start();
      }
    }
  }

  //myGLCD.clrScr();

  while (1)
  {
    if (myTouch.dataAvailable() == true)
    {
      pressed_button = myButtons.checkButtons();
      if (pressed_button == btnAir)
      {
        menu();
        myButtons.deleteButton(btnAir);
      }
    }
  }
}

void menu()
{
  myGLCD.clrScr();
  myGLCD.fillScr(255, 255, 255);
  myGLCD.setBackColor(255, 255, 255);

  myButtons.deleteButton(btnAir);

  myButtons.setTextFont(arial_bold);
  myGLCD.print("STOP TOKEN", LEFT, 5);
  myGLCD.print("Pause ", LEFT, 21);
  btnStop = myButtons.addButton(190, 50, 120, 70, "STOP");
  btnPause = myButtons.addButton(190, 130, 120, 70, "PAUSE");
  pressed_button = myButtons.checkButtons();
  myButtons.drawButtons();

  while (Token > 0)
  {
    digitalWrite(8, HIGH);
    if (myTouch.dataAvailable() == true)
    {
      pressed_button = myButtons.checkButtons();
      if (pressed_button == btnStop)
      {
        digitalWrite(8, LOW);
        Token = 1;
      }
      if (pressed_button == btnPause)
      {
        Token = Token;
        digitalWrite(8, LOW);
        myGLCD.clrScr();
        start();
      }
    }
    myGLCD.print("TOKEN:", 20, 50);
    myGLCD.setColor(17, 0, 225);
    myGLCD.setFont(CalibriBold32x48);
    myGLCD.printNumI(Token, 20, 80, 4, '0');
    myGLCD.setColor(144, 222, 255);
    myGLCD.setFont(BigFont);
    myGLCD.print("x10 ML", 35, 150);

    Token--;
    Serial.println(Token);
    delay(250);
  }

  delay(1000);
  digitalWrite(8, LOW);
  StaticJsonDocument<200> doc;
  doc["Sisa"] = Token;
  serializeJson(doc, linkSerial);
  //myGLCD.clrScr();
  start();
}

void loop()
{
  boolean default_colors = true;

  start();
}

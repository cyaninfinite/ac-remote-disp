#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>     //For I2C
#include <DYIRDaikin.h>

DYIRDaikin irdaikin;
bool isOn {true};
bool prevState {false};
int currentTemp {27};

const byte btnOn = 5;     //Btn pin declaration

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void turnOn() {
  isOn = !isOn;
  sendCmd();
}

void sendCmd() {
  if (isOn) {
    irdaikin.on();
    irdaikin.setSwing_off();
    irdaikin.setMode(1);      // 0=FAN, 1=COOL, 2=DRY, 3=HEAT(if you have one)
    irdaikin.setFan(5);       // 0~4=speed(1,2,3,4,5),5=auto,6=moon
    irdaikin.setTemp(currentTemp);
  } else {
    irdaikin.off();
  }
  irdaikin.sendCommand();
}

void serialDebug() {
  while (Serial.available() > 0) {
    if (Serial.read() == 'p') {
      if (isOn == 0) {
        isOn = 1;
        irdaikin.off();
        Serial.println("Turn Off");
      } else {
        isOn = 0;
        irdaikin.on();
        Serial.println("Turn On");
      }
      irdaikin.sendCommand();
      Serial.println("Execute Command!");
    }
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(btnOn, INPUT);
  attachInterrupt(digitalPinToInterrupt(btnOn), turnOn, RISING);

  irdaikin.begin(9);           // Declare IR Pin

  irdaikin.on();
  irdaikin.setSwing_off();
  irdaikin.setMode(1);        //0=FAN, 1=COOL, 2=DRY, 3=HEAT(if you have one)
  irdaikin.setFan(5);         // 0~4=speed(1,2,3,4,5),5=auto,6=moon
  irdaikin.setTemp(currentTemp);

  irdaikin.sendCommand();
  isOn = 0;
  u8g2.begin();
}

void loop() {
  serialDebug();
  if (prevState != isOn) {
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_ncenB10_tr);
      u8g2.drawStr(0, 24, isOn ? "Aircon ON" : "Aircon OFF");
    } while ( u8g2.nextPage() );
    prevState = isOn;
  }
}

#include <TM1637TinyDisplay.h>
#include <Adafruit_MLX90614.h>

#define PIEZO_BUZZER 3
#define BTN 7
#define CLK 9
#define DIO 10
#define LED_RED 5
#define LED_GREEN 6
#define LED_BLUE 11

TM1637TinyDisplay display(CLK, DIO);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

float calibrationValue = 5.5;

void setup() {
  Serial.begin(9600);
  mlx.begin();
  display.setBrightness(BRIGHT_5);

  pinMode(BTN, INPUT_PULLUP);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
}

void loop() {
  if(Serial.available()){
    String strRead = Serial.readStringUntil('\n');
    if(strRead.indexOf("RGB=") != -1){
      int commaIndex1 = strRead.indexOf(",");
      int commaIndex2 = strRead.indexOf(",", commaIndex1 + 1);

      int redValue = strRead.substring(4, commaIndex1).toInt();
      int greenValue = strRead.substring(commaIndex1 + 1, commaIndex2).toInt();
      int blueValue = strRead.substring(commaIndex2 + 1, strRead.length()).toInt();
      redLedSet(redValue, greenValue, blueValue);
      Serial.println("OKRGB");
    }
  }

  float objectTempC = objectTemp();
  if (objectTemp != 0) {
    if (objectTempC >= 34) {
      Serial.println(objectTempC);
      if (objectTempC >= 37) {
        while (true) {
          display.showNumber(objectTempC);
          for (int freq = 150; freq <= 1800; freq += 2) {
            tone(PIEZO_BUZZER, freq, 10);
          }
          for (int freq = 1800; freq >= 150; freq -= 2) {
            tone(PIEZO_BUZZER, freq, 10);
          }
          if(digitalRead(BTN)==HIGH) break;
        }
        // delay(3000);
      }
    } 
    display.showString("");
  }
}

float objectTemp() {
  static unsigned long currTime = 0;
  static unsigned long prevTime = 0;

  currTime = millis();
  if (currTime - prevTime >= 500) {
    prevTime = currTime;
    float ambientTemp = mlx.readAmbientTempC();
    float objectTemp = mlx.readObjectTempC();
    objectTemp = objectTemp + calibrationValue;
    return objectTemp;
  }
  return 0;
}

void redLedSet(int red, int green, int blue)
{
  analogWrite(LED_RED, red);
  analogWrite(LED_GREEN, green);
  analogWrite(LED_BLUE, blue);
}

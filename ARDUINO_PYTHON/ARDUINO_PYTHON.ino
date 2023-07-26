#include <DHT.h>
#include <DHT_U.h>

#include <TM1637TinyDisplay.h>
#include <Adafruit_MLX90614.h>
#include <Servo.h>

#define PIEZO_BUZZER 3
#define BTN1 4
#define BTN2 7
#define CLK 9
#define DIO 10
#define LED_RED 5
#define LED_GREEN 6
#define LED_BLUE 11
#define SERVO_PIN 8
#define VR_PIN A0
#define DHTPIN 2

#define DHTTYPE DHT11

TM1637TinyDisplay display(CLK, DIO);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Servo myservo;
DHT dht(DHTPIN, DHTTYPE);

float calibrationValue = 5.5;

void setup() {
  Serial.begin(9600);
  mlx.begin();
  display.setBrightness(BRIGHT_5);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  myservo.attach(SERVO_PIN);
}

void loop() {
  if(Serial.available() > 0){
    String strRead = Serial.readStringUntil('\n');
    if(strRead.indexOf("RGB=") != -1){
      int commaIndex1 = strRead.indexOf(",");
      int commaIndex2 = strRead.indexOf(",", commaIndex1 + 1);

      int redValue = strRead.substring(4, commaIndex1).toInt();
      int greenValue = strRead.substring(commaIndex1 + 1, commaIndex2).toInt();
      int blueValue = strRead.substring(commaIndex2 + 1, strRead.length()).toInt();
      redLedSet(redValue, greenValue, blueValue);
      Serial.println("OKRGB");
    }else if(strRead.indexOf("SERVO=") != -1){
      int servoDigree = strRead.substring(6, strRead.length()).toInt();
      if(servoDigree <= 180){
        myservo.write(servoDigree);
        Serial.println("OKSERVO");
      }
    } else if(strRead.indexOf("FND=") != -1){
      float fndValue = strRead.substring(4, strRead.length()).toFloat();
      display.showNumber(fndValue);
      Serial.println("OKFND");
    } else if(strRead.indexOf("VR=?") != -1){
      Serial.print("VR=");
      Serial.println(analogRead(VR_PIN));
    } else if(strRead.indexOf("TEMPERATURE=?") != -1){
      sendTemperature();
    } else if(strRead.indexOf("HUMIDITY=?") != -1){
      sendHumidity();
    } else if(strRead.indexOf("AMBIENT=?") != -1){
      float ambientTemp = mlx.readAmbientTempC();
      Serial.print("AMBIENT=");
      Serial.println(ambientTemp);
    } else if(strRead.indexOf("OBJECT=?") != -1){
      float objectTemp = mlx.readObjectTempC();
      Serial.print("OBJECT=");
      Serial.println(objectTemp);
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
          if(digitalRead(BTN2)==HIGH) break;
        }
        // delay(3000);
      }
    } 
    // display.showString("");
  }

  if (btn1() == 1) Serial.println("BUTTON1=CLICKED");
  if (btn2() == 1) Serial.println("BUTTON2=CLICKED");
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

int btn1() {
  static int currBtn = 0;
  static int prevBtn = 0;

  currBtn = digitalRead(BTN1);
  if(currBtn != prevBtn){
    prevBtn = currBtn;
    if(currBtn == 1){
      return 1;
    }
    delay(50);
  }
  return 0;
}

int btn2(){
  static int currBtn = 0;
  static int prevBtn = 0;

  currBtn = digitalRead(BTN2);
  if(currBtn != prevBtn){
    prevBtn = currBtn;
    if(currBtn == 1){
      return 1;
    }
    delay(50);
  }
  return 0;
}

void sendTemperature()
{
  float temperature = dht.readTemperature();
  if (!isnan(temperature))
  {
    Serial.print("TEMPERATURE=");
    Serial.println(temperature);
  }
  else
  {
    Serial.print("TEMPERATURE=");
    Serial.println(0);
  }
}

void sendHumidity()
{
  float humidity = dht.readHumidity();
  if (!isnan(humidity))
  {
    Serial.print("HUMIDITY=");
    Serial.println(humidity);
  }
  else
  {
    Serial.print("HUMIDITY=");
    Serial.println(0);
  }
}

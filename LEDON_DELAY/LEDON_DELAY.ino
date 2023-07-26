#define LED_RED 5
#define LED_GREEN 6
#define LED_BLUE 11
#define BRIGHT_PIN A1

unsigned long currTime = 0;
unsigned long prevTime = 0;
int sec = 0;
int state = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
}

void loop() {
  int brightValue = analogRead(BRIGHT_PIN);
  Serial.print("Bright value=");
  Serial.println(brightValue);

  currTime = millis();
  if(currTime - prevTime >= 1000){
    prevTime = currTime;
    sec++;
  }

  if (brightValue <= 300){ 
    if(sec >= 3){
      sec = 3;
      ledOn();
    }
  }

  else{
    sec = 0;
    ledOff();
  }
  
}

void ledOn(){
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_GREEN, HIGH);
}

void ledOff(){
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);
}

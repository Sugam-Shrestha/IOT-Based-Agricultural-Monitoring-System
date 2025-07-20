// defining template id, template name, and authentication token for blynk
#define BLYNK_TEMPLATE_ID "TMPL6vz_hl7sX"
#define BLYNK_TEMPLATE_NAME "Agricultural Monitoring System"
#define BLYNK_AUTH_TOKEN "2_5uaqB2kfyEOVY3MBl8nXV6BsUwMaWF"

// Library for DHT11 sensor
#include <DHT.h>
// Library for Liquid crystal display (I2C)
#include <LiquidCrystal_I2C.h>
// Libraries for blynk Iot platform
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Blynk.h>

// Defining the pins for sensors
#define Trig 33
#define Echo 25
#define buzz 14
#define dhtsen 15
#define moist 34
#define relay 32

// authentication for Blynk, ssid and pass for wifi
char auth[] = "2_5uaqB2kfyEOVY3MBl8nXV6BsUwMaWF";
char ssid[] = "WIFI";
char pass[] = "123456789@";

// Defining the lcd address, cloumns and rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Defining dht11 sensor
DHT dht11(dhtsen, DHT11);

void setup() {
  // beggining serial communication with baud rate of 9600
  Serial.begin(9600);
  // Initializing trig pin of ultarsonic sensor
  pinMode(Trig, OUTPUT);
  // Initializing echo pin of ultrasonic sensor
  pinMode(Echo, INPUT);
  // Initializing the Buzzer
  pinMode(buzz, OUTPUT);
  // initializing dht11 sensor
  dht11.begin();
  // Initialing the LCD
  lcd.init();
  // Initializing the LCD Backlight
  lcd.backlight();
  // Initializing the soil moisture sensor
  pinMode(moist, INPUT);
  // Initalizing relay
  pinMode(relay, OUTPUT);
  // Initializing WiFi
  WiFi.begin(ssid, pass);
  // Initializing Blynk 
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  // generating pulse from the ultrasonic sensor
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);

  // calculating duration of the pulse
  float duration = pulseIn(Echo, HIGH);

  // calculating distance from the object using the pulse
  long distance = 0.017 * duration;

  // printing the distance
  Serial.print("Distance:");
  Serial.print(distance);
  Serial.println("cm");

  // condition for buzzer to ring via distance
  if (distance < 3 || distance > 13)
  {
    digitalWrite(buzz, HIGH);
  }
  else
  {
    digitalWrite(buzz, LOW);
  }

  // water level in percentage
  float waterlevel = map(distance, 1, 20, 0, 100);

  // read humidity
  float humidity = dht11.readHumidity();
  // read humidty in celsius
  float temperature = dht11.readTemperature();

  // humidity in percentage
  float humidityper = (humidity/95)*100;

  //printing the values of temperature and humidity.
  Serial.print("humidity:");
  Serial.print(humidityper);
  Serial.println("%");
  Serial.print("Temperature:");
  Serial.println(temperature);

  // Calculating soil moisture 
  int moisture = analogRead(moist);
  // converting moisture to percentage
  float moistper = map(moisture, 4095, 0, 0, 100);

  // printing the moisture value
  Serial.print("moisture value: ");
  Serial.println(moisture);

  String moistread;
  if (moistper >= 0 && moistper <= 25)
  {
    // Turn on water pump if moisture is low
    digitalWrite(relay, LOW);
    moistread = "Dry  ";
  }
  else if (moistper > 25 && moistper <= 100)
  {
    // Turn off water pump if moisture is high
    digitalWrite(relay, HIGH);
    moistread = "Moist";
  }

  int relayState = digitalRead(relay);
  if (relayState == HIGH)
  {
    Blynk.virtualWrite(V4, LOW);
  }
  else
  {
    Blynk.virtualWrite(V4, HIGH);
  }

  // printing the value of soil moisture 
  Serial.print("Moisture percentage: ");
  Serial.print(moistper);
  Serial.println("%");

  // // LCD's first row
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print(" H:");
  lcd.print(humidityper);
  lcd.print("%");
  // // LCD's second row
  lcd.setCursor(0, 1);
  lcd.print("Soil is ");
  lcd.print(moistread);

  // sending the readings to Blynk through virtual pins
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidityper);
  Blynk.virtualWrite(V2, moistper);
  Blynk.virtualWrite(V3, waterlevel);

  delay(500);
}

#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <BH1750.h>
BH1750 lightMeter(0x23);



#define ONE_WIRE_BUS 26


const char* ssid = "telenet-32924";// jouw eigen wifi
const char* password = "YmutaKcebv8p";// u wachtwoord
const char* mqttServer = "192.168.0.121"; //u ip adress van de raspberry pi
const char* light_topic = "home/livingroom/light";
const char* temperature_topic = "home/livingroom/temperature";
const int mqttPort = 1883;
const char* mqttUser = "nico"; // u user vanm MQTT
const char* mqttPassword = "raspberry"; // u wachtwoord van MQTT
const char* clientID = "client_livingroom"; // MQTT client ID


const int RS = 2, EN = 4, d4 = 5, d5 = 18, d6 = 19, d7 = 23;

const int buttonPin1 = 27;     // the number of the pushbutton pin
const int buttonPin2 = 25;     // the number of the pushbutton pin

int red_light_pin= 13;
int green_light_pin = 12;
int blue_light_pin = 14;
int Counter = 18;
int Temp = 0;
int stand = LOW;
int omlaag = LOW;
int omhoog = LOW;
int buttonState1 = 0;
int buttonState2 = 0;
int led1Pin= 32;
int led2Pin= 33;

LiquidCrystal lcd(RS, EN, d4, d5, d6, d7);
WiFiClient espClient;
PubSubClient client(espClient);
OneWire oneWire(ONE_WIRE_BUS);
float temp=0.0;
DallasTemperature sensors(&oneWire);


void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  client.setServer(mqttServer, mqttPort);
  Wire.begin();
  ;sensors.begin();
  lightMeter.begin();
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  lcd.begin(16, 2);
 
}

void loop() {
  client.loop();

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect(clientID, mqttUser, mqttPassword)){
    {
      Serial.println("connected");
    }}
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  sensors.requestTemperatures(); 
  Temp = (sensors.getTempCByIndex(0));
  uint16_t lux = lightMeter.readLightLevel();

 
  lcd.setCursor(0, 0);
  lcd.print("Temp=");
  lcd.setCursor(0, 1);
  lcd.print("Set=");
  lcd.setCursor(6, 0);
  lcd.print(sensors.getTempCByIndex(0));
  lcd.setCursor(5, 1);
  lcd.print(Counter);


  
    buttonState1 = digitalRead(27);
    if (buttonState1 == 1){
    Counter = Counter + 1;
    Serial.println(Counter);
    lcd.setCursor(5, 1);
    lcd.print(Counter);
    }
    buttonState2 = digitalRead(25);
    if (buttonState2 == 1){
    Counter = Counter - 1;
    Serial.println(Counter);
    lcd.setCursor(5, 1);
    lcd.print(Counter);
    }
    if (Counter < Temp){
    RGB_color(0, 255, 0); // Green
    delay(1000);
  
    }
    if (Counter > Temp){
    RGB_color(255, 0, 0); // Red
    delay(1000);

    if (lux < 50 && stand == LOW && omlaag == LOW){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Gordijn gaat");
    lcd.setCursor(0, 2);
    lcd.print("Omlaag");
    digitalWrite(led1Pin, HIGH);
    digitalWrite(led2Pin, LOW);
    omlaag = HIGH;
    omhoog = LOW;
    delay(1000 * 5);
    }
 
    if (lux > 50 && stand == LOW && omhoog ==LOW){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Gordijn gaat");
    lcd.setCursor(0, 2);
    lcd.print("Omhoog");
    digitalWrite(led1Pin, LOW);
    digitalWrite(led2Pin, HIGH);
    omlaag = LOW;
    omhoog = HIGH;
    delay(1000 * 5);
    }
  
    }
  // check if returns are valid and print the sensor data
    if (isnan(Temp) || isnan(lux)) {
    Serial.println("Failed to read");
  } else {
    Serial.print("light: "); 
    Serial.print(lux);
    Serial.println(" lx");
    Serial.print("Temperature: "); 
    Serial.print(Temp);
    Serial.println(" *C");

  String hs="Light: "+String((float)lux)+" lx ";
  String ts="Temp: "+String((float)Temp)+" C ";


  (client.publish(temperature_topic, String(Temp).c_str())); {
    Serial.println("Temperature sent!");
  (client.publish(light_topic, String(lux).c_str())); {
    Serial.println("Light sent!");
  }

  }

  client.disconnect();  // disconnect from the MQTT broker
  delay(1000*5);      
}}
void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
 digitalWrite(red_light_pin, red_light_value);
 digitalWrite(green_light_pin, green_light_value);
 digitalWrite(blue_light_pin, blue_light_value);
}

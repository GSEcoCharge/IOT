#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define TRIG_PIN_SENSOR1 12
#define ECHO_PIN_SENSOR1 14
#define LED_PIN_SENSOR1 27
#define TRIG_PIN_SENSOR2 16
#define ECHO_PIN_SENSOR2 4
#define LED_PIN_SENSOR2 17

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

int sensor1Distance;
int sensor2Distance;

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado!");
}

void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(TRIG_PIN_SENSOR1, OUTPUT);
  pinMode(ECHO_PIN_SENSOR1, INPUT);
  pinMode(LED_PIN_SENSOR1, OUTPUT);

  pinMode(TRIG_PIN_SENSOR2, OUTPUT);
  pinMode(ECHO_PIN_SENSOR2, INPUT);
  pinMode(LED_PIN_SENSOR2, OUTPUT);


  lcd.setCursor(3, 0);
  lcd.print("EcoCharge!");
  delay(1000);
  lcd.clear();

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  connectMQTT();
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (client.connect("espClient")) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falha de conexão. Código de erro: ");
      Serial.print(client.state());
      Serial.println(" | Tentando de novo em 2 segundos");
      delay(2000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();

  sensor1Distance = measureDistance(TRIG_PIN_SENSOR1, ECHO_PIN_SENSOR1);
  sensor2Distance = measureDistance(TRIG_PIN_SENSOR2, ECHO_PIN_SENSOR2);

  String sensor1Status = (sensor1Distance <= 30) ? "Ocupado" : "Vazio";
  String sensor2Status = (sensor2Distance <= 30) ? "Ocupado" : "Vazio";

  lcd.setCursor(0, 0);
  lcd.print("Ponto 1: " + sensor1Status + "  ");
  digitalWrite(LED_PIN_SENSOR1, (sensor1Distance <= 30) ? HIGH : LOW);

  lcd.setCursor(0, 1);
  lcd.print("Ponto 2: " + sensor2Status + "  ");
  digitalWrite(LED_PIN_SENSOR2, (sensor2Distance <= 30) ? HIGH : LOW);

  client.publish("ecocharge/sensor1status", sensor1Status.c_str());
  client.publish("ecocharge/sensor1distance", String(sensor1Distance).c_str());
  client.publish("ecocharge/sensor2status", sensor2Status.c_str());
  client.publish("ecocharge/sensor2distance", String(sensor2Distance).c_str());

  delay(500);
}

int measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  return distance;
}

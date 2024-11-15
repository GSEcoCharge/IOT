#include <WiFi.h>
#include <PubSubClient.h>

#define TRIG_DISTANCE_SENSOR_PIN 12
#define ECHO_DISTANCE_SENSOR_PIN 14
#define CURRENT_SENSOR_PIN 36
#define LED_PIN 32
#define SLIDE_SWITCH_PIN 34
#define MODE_SLIDE_SWITCH_PIN 16

#define SUPPLY_VOLTAGE 220 // Padrão dos Postos EV Semirrápidos

// Baseado no BYD Dolphin Mini
#define MAX_BATTERY_CAPACITY 38.0
#define AUTONOMIA_TOTAL 340.0

// Wifi
const char* ssid = "Wokwi-GUEST";
const char* password = "";
// Broker
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Sensor de distância
int distanceToVehicle;
String distanceSensorStatus;

// Info Elétrica
float chargingCurrentA;
float chargingPowerKW;
int phaseSystem = 1;

// Info Carregamento
String chargingState;
float batteryChargePercent = 0.0;
float accumulatedChargeKWh = 0.0;
int estimatedTimeRemaining = 0;
float estimatedRangeKm = 0.0;

String display = "MQTT";

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado!");
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (client.connect("espClient")) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falha de conexão. Código de erro: ");
      Serial.print(client.state());
      Serial.println(" | Tentando de novo em 1 segundos");
      delay(1000);
    }
  }
}

void setup() {
  pinMode(TRIG_DISTANCE_SENSOR_PIN, OUTPUT);
  pinMode(ECHO_DISTANCE_SENSOR_PIN, INPUT);
  pinMode(CURRENT_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SLIDE_SWITCH_PIN, INPUT_PULLUP);
  pinMode(MODE_SLIDE_SWITCH_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  connectMQTT();
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();

  updateDistanceSensor();
  updatePhaseSystem();
  updateCurrentSensor();
  updateChargingStatus();

  displayCheck();
  delay(1000);
}

void displayCheck() {
  display = digitalRead(MODE_SLIDE_SWITCH_PIN) == HIGH ? "MQTT" : "Serial";
  if (display == "Serial") {
    displayStatus();
  } else if (display == "MQTT") {
    publishStatus();
  }
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

void updateDistanceSensor() {
  distanceToVehicle = measureDistance(TRIG_DISTANCE_SENSOR_PIN, ECHO_DISTANCE_SENSOR_PIN);
  distanceSensorStatus = (distanceToVehicle <= 30) ? "Ocupado" : "Vazio";
}

void updatePhaseSystem() {
  phaseSystem = digitalRead(SLIDE_SWITCH_PIN) == HIGH ? 2 : 1;
}

void updateCurrentSensor() {
  int currentValue = analogRead(CURRENT_SENSOR_PIN);
  chargingCurrentA = (float) currentValue * 32.0 / 4095.0;
  chargingPowerKW = (SUPPLY_VOLTAGE * chargingCurrentA * phaseSystem) / 1000.0;
}

void updateChargingStatus() {
  if (batteryChargePercent >= 100.0) {
    chargingState = "Totalmente carregada";
    digitalWrite(LED_PIN, LOW);
    estimatedTimeRemaining = 0;
    return;
  }

  if (chargingCurrentA <= 0.0) {
    chargingState = "Não está carregando";
    digitalWrite(LED_PIN, LOW);
    estimatedTimeRemaining = -1;
    return;
  }

  chargingState = "Carregando";
  digitalWrite(LED_PIN, HIGH);
  updateBatteryCharge();
  calculateEstimatedTimeRemaining();
}

void updateBatteryCharge() {
  if (accumulatedChargeKWh >= MAX_BATTERY_CAPACITY) {
    accumulatedChargeKWh = MAX_BATTERY_CAPACITY;
    batteryChargePercent = 100.0;
    return;
  }

  float chargeIncrementKWh = chargingPowerKW / 3600.0;
  accumulatedChargeKWh += chargeIncrementKWh;

  batteryChargePercent = (accumulatedChargeKWh / MAX_BATTERY_CAPACITY) * 100.0;
  if (batteryChargePercent > 100.0) batteryChargePercent = 100.0;

  estimatedRangeKm = (batteryChargePercent / 100.0) * AUTONOMIA_TOTAL;
}

void calculateEstimatedTimeRemaining() {
  float remainingCapacityKWh = MAX_BATTERY_CAPACITY - accumulatedChargeKWh;
  if (chargingPowerKW > 0) {
    estimatedTimeRemaining = (remainingCapacityKWh * 3600) / chargingPowerKW;
  } else {
    estimatedTimeRemaining = -1;
  }
}

String formatTime(int totalSeconds) {
  int days = totalSeconds / 86400;
  int hours = (totalSeconds % 86400) / 3600;
  int minutes = (totalSeconds % 3600) / 60;
  int seconds = totalSeconds % 60;

  String result = "";

  if (days > 0) {
    result += String(days) + "d ";
  }
  if (hours > 0 || days > 0) {
    result += String(hours) + "h ";
  }
  if (minutes > 0 || hours > 0 || days > 0) {
    result += String(minutes) + "m ";
  }
  result += String(seconds) + "s";

  return result;
}

void displayStatus() {
  Serial.println("========= STATUS DO CARREGADOR EV =========");

  Serial.println("[Sensor de Distância]");
  Serial.printf("Distância: %dcm\n", distanceToVehicle);
  Serial.printf("Status do Sensor: %s\n", distanceSensorStatus.c_str());
  Serial.println("-------------------------------------------");

  Serial.println("[Informações Elétricas]");
  Serial.printf("Corrente: %.2fA | Tensão: %dV\n", chargingCurrentA, SUPPLY_VOLTAGE);
  Serial.printf("Potência: %.2f kW (%s)\n", chargingPowerKW, phaseSystem == 1 ? "Monofásico" : "Bifásico");
  Serial.println("-------------------------------------------");

  Serial.println("[Status de Carregamento]");
  Serial.printf("Estado: %s\n", chargingState.c_str());
  Serial.printf("Nível de Bateria: %.2f%%\n", batteryChargePercent);
  Serial.printf("Autonomia Estimada: %.2fkm\n", estimatedRangeKm);

  if (batteryChargePercent >= 100.0) {
    Serial.println("Bateria completamente carregada!");
  } else if (estimatedTimeRemaining == -1) {
    Serial.println("Tempo Restante Estimado: Carregamento pausado");
  } else {
    String formattedTime = formatTime(estimatedTimeRemaining);
    Serial.printf("Tempo Restante Estimado: %s\n", formattedTime.c_str());
  }

  Serial.println("===========================================\n");
}

void publishStatus() {
  client.publish("ev/charger/distanceToVehicle", String(distanceToVehicle).c_str());
  client.publish("ev/charger/status", distanceSensorStatus.c_str());
  client.publish("ev/charger/supplyVoltage", String(SUPPLY_VOLTAGE).c_str());
  client.publish("ev/charger/chargingCurrent", String(chargingCurrentA).c_str());
  client.publish("ev/charger/phaseSystem", String(phaseSystem).c_str());
  client.publish("ev/charger/chargingPower", String(chargingPowerKW).c_str());
  client.publish("ev/charger/batteryPercent", String(batteryChargePercent).c_str());
  client.publish("ev/charger/chargingState", chargingState.c_str());
  client.publish("ev/charger/estimatedTimeRemaining", String(estimatedTimeRemaining).c_str());
  client.publish("ev/charger/estimatedRange", String(estimatedRangeKm).c_str());
}

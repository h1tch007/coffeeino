#include "config.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

void callback(char* topic, byte* payload, unsigned int length);
bool oldWaterLevel;

WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

void setup() {
  // Инициализируем вывод как OUTPUT и устанавливаем в положение выключено
  pinMode(pinPower, OUTPUT); // Реле 1
  digitalWrite(pinPower, LOW);

  pinMode(pinWater, OUTPUT); // Реле 2
  digitalWrite(pinWater, LOW);

  pinMode(pinLevel, INPUT_PULLUP); // Датчик уровня воды
  oldWaterLevel = !digitalRead(pinLevel);
  ArduinoOTA.setHostname(otahostname);
  ArduinoOTA.begin(); // Инициализируем OTA

  // Запускаем вывод отладочной информации
  Serial.begin(115200);
  delay(100);

  // Запускаем подсистему WIFI
  WiFi.begin(ssid, password);
  // Подключаемся к сети WIFI и серверу MQTT
  reconnect();

  // Ждем перед запуском основного цикла
      delay(2000);
}

void loop(){
  // Считываем текущее состояние датчика уровня воды
  int waterLevel = digitalRead(pinLevel);
  // Если изменилось - записываем и отправляем команду
  if (waterLevel != oldWaterLevel)
  {
    client.publish(cmndLevel, "1");
    oldWaterLevel = waterLevel;
  }
  
  // Переподключаемся если пропала связь
  if (!client.connected() && WiFi.status() == 3) {reconnect();}

  // Поддерживаем соединение MQTT
  client.loop();

  delay(10); 
  ArduinoOTA.handle();
}

void callback(char* topic, byte* payload, unsigned int length) {

  // Конвертируем topic в string
  String topicStr = topic; 
  
  // Выводим отладочную информацию
  Serial.println("Callback update.");
  Serial.print("Topic: ");
  Serial.println(topicStr);

if (topicStr == cmndLevel)
  {
   if(payload[0] == '1'){
    if (digitalRead(pinLevel)==HIGH)
      client.publish(statLevel, "1");
    else
      client.publish(statLevel, "0");}
  }

   else if (topicStr == cmndPower) 
    {
     // Включаем реле если получена команда "1" и обновляем статус
     if(payload[0] == '1'){
       digitalWrite(pinPower, HIGH);
       client.publish(statPower, "1");
       }

      // Выключаем реле если получена команда "0" и обновляем статус
     else if (payload[0] == '0'){
       digitalWrite(pinPower, LOW);
       client.publish(statPower, "0");
       }
     }

     // Для добавления поддержки более 2 реле можно скопировать else-if блок
     else if (topicStr == cmndWater) 
     {
     // Включаем реле если получена команда "1" и обновляем статус
     if(payload[0] == '1'){
       digitalWrite(pinWater, HIGH);
       client.publish(statWater, "1");
       }

     // Выключаем реле если получена команда "0" и обновляем статус
     else if (payload[0] == '0'){
       digitalWrite(pinWater, LOW);
       client.publish(statWater, "0");
       }
     }
}

void reconnect() {

  // Переподключаемся к WIFI если пропала связь
  if(WiFi.status() != WL_CONNECTED){
    // Выводим отладочную информацию
    Serial.print("Connecting to ");
    Serial.println(ssid);

    // Ждем подключения WIFI
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    // Выводим отладочную информацию
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // Проверяем подключение к WIFI перед подключением к MQTT
  if(WiFi.status() == WL_CONNECTED){
  // Ждем подключения MQTT
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");

      // Генерируем имя клиента на основе MAC
      String clientName;
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);

      // Подписываемся на топики
      if (client.connect((char*) clientName.c_str(),"mqtt_username", "mqtt_password")) {  // Имя и пароль для MQTT
        Serial.print("\tMQTT Connected");
        client.subscribe(cmndPower);
        client.subscribe(cmndWater);
        client.subscribe(cmndLevel);
      }

      // Выводим отладочную информацию
      else{Serial.println("\tFailed."); abort();}
    }
  }
}

// Генерируем имя клиента на основе MAC
String macToStr(const uint8_t* mac){
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5){
      result += ':';
    }
  }
  return result;
}


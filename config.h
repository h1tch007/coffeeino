// Настраиваем подключение WIFI и MQTT
#define MQTT_SERVER "192.168.1.10"  // IP адрес MQTT сервера
const char* ssid = "********";  // Имя сети WIFI
const char* password = "********";  // Пароль сети WIFI
const char* otahostname = "Delonghi";  // Имя хоста для OTA обновлений

// Настраиваем пины для подключения реле и датчика
const int pinPower = D6;  // Реле на кнопку питания
const int pinWater = D7;  // Реле на кнопку воды
const int pinLevel = D5;  // Вывод датчика уровня воды

// Настраиваем топики управления MQTT 
char const* cmndPower = "home/cmnd/coffee_power";
char const* cmndWater = "home/cmnd/coffee_water";
char const* cmndLevel = "home/cmnd/coffee_water_level";
char const* statPower = "home/stat/coffee_power";
char const* statWater = "home/stat/coffee_water";
char const* statLevel = "home/stat/coffee_water_level";


#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <time.h>  // Untuk NTP

const char *ssid = "Lab Navigasi 2022";  // Nama WiFi router Anda
const char *password = "ppnssukses2022"; // Password WiFi router Anda

WebSocketsServer webSocket = WebSocketsServer(12345);

// Zona waktu (WIB untuk GMT+7)
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600;
const int   daylightOffset_sec = 0;

String getFormattedTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "N/A";
  }
  char timeStr[10];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo); // Format: HH:MM:SS
  return String(timeStr);
}

String getFormattedDate() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain date");
    return "N/A";
  }
  char dateStr[12];
  strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &timeinfo); // Format: YYYY-MM-DD
  return String(dateStr);
}

void sendData()
{
  StaticJsonDocument<300> jsonDoc;
  double latitude = -7.2950 + (random(-10, 10) / 10000.0);   // Latitude sekitar -7.2950
  double longitude = 112.7550 + (random(-10, 10) / 10000.0); // Longitude sekitar 112.7550

  jsonDoc["time"] = getFormattedTime();    // Waktu saat ini
  jsonDoc["date"] = getFormattedDate();    // Tanggal saat ini
  jsonDoc["latitude"] = latitude;
  jsonDoc["longitude"] = longitude;
  jsonDoc["altitude"] = random(0, 100); // Altitude acak, dikirim sebagai angka
  jsonDoc["speed"] = random(0, 100);    // Kecepatan acak, dikirim sebagai angka (km/h)
  jsonDoc["course"] = random(0, 360);   // Course acak (derajat), dikirim sebagai angka
  jsonDoc["satellite"] = 10;            // Jumlah satelit statis, dikirim sebagai angka
  jsonDoc["roll"] = 0;                  // Roll statis, dikirim sebagai angka
  jsonDoc["pitch"] = 0;                 // Pitch statis, dikirim sebagai angka
  jsonDoc["yaw"] = 0;                   // Yaw statis, dikirim sebagai angka
  jsonDoc["depth"] = 0;                 // Depth statis, dikirim sebagai angka

  String jsonString;
  serializeJson(jsonDoc, jsonString);

  webSocket.broadcastTXT(jsonString);
  Serial.println("Data sent: " + jsonString);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if (type == WStype_CONNECTED)
  {
    Serial.println("Client connected");
  }
  else if (type == WStype_DISCONNECTED)
  {
    Serial.println("Client disconnected");
  }
}

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop()
{
  webSocket.loop();

  static unsigned long lastTime = 0;
  if (millis() - lastTime > 2000)
  {
    lastTime = millis();
    sendData();
  }
}

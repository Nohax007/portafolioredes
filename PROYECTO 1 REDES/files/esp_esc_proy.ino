#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SLAVE_ADDR 9           // Dirección del esclavo (ESP32)
float receivedData = 0.0;      // Variable para almacenar el dato recibido

/**********************
 * Configuración WiFi *
 **********************/
const char* SSID     = "Wokwi-GUEST"; // Cambia por tu SSID
const char* PASSWORD = "";            // Cambia por tu contraseña

/*****************************************
 * Configuración para process.php (Replit)
 *****************************************/
String processURL = "https://replit.com/@DiegoHorcasitas/proyectoIOT-1#process.php";

/***************************************
 * Configuración para ThingSpeak
 ***************************************/
String thingspeakURL = "https://api.thingspeak.com/update?";
String api_key       = "8YHTZ0SP1VPFWVAZ"; // Tu API Key de ThingSpeak
String field1        = "field1";           // Usaremos field1 para enviar el dato recibido

/****************************************************
 * Función callback para recibir datos vía I2C
 ****************************************************/
void receiveData(int byteCount) {
  while (Wire.available()) {
    Wire.readBytes((byte*)&receivedData, sizeof(receivedData)); // Leer el dato recibido
  }
  Serial.print("Dato recibido: ");
  Serial.println(receivedData, 3);
}

void setup() {
  Serial.begin(115200);

  // Inicializar I2C en modo esclavo
  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(receiveData);
  Serial.println("Esclavo ESP32 iniciado...");

  // Conectar a WiFi
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Enviar dato a ThingSpeak
    HTTPClient httpTs;
    String tsURL = thingspeakURL + "api_key=" + api_key + "&" + field1 + "=" + String(receivedData);
    httpTs.begin(tsURL);
    int httpCodeTs = httpTs.GET();
    Serial.print("ThingSpeak -> GET code: ");
    Serial.println(httpCodeTs);
    httpTs.end();

    // Enviar dato a process.php en Replit
    HTTPClient httpProc;
    String urlProcess = processURL + "?data=" + String(receivedData);
    httpProc.begin(urlProcess);
    int httpCodeProc = httpProc.GET();
    Serial.print("process.php -> GET code: ");
    Serial.println(httpCodeProc);
    httpProc.end();
  } else {
    Serial.println("Error en la conexión WiFi");
  }
  
  // Esperar 3 segundos antes de la siguiente lectura/envío
  delay(3000);
}

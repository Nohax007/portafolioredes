
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "TU_WIFI";
const char* password = "TU_PASSWORD";

const char* serverName = "https://TU_REPLIT_URL/salon3";
const char* thingspeakApiKey = "THINGSPEAK_API_KEY_3";
const char* thingspeakURL = "http://api.thingspeak.com/update";

float medicionesHora[1200];
int indiceMedicion = 0;
float promediosHora[24];
int indiceHora = 0;
float promediosDia[7];
int indiceDia = 0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado");
}

void loop() {
  static String buffer = "";

  while (Serial1.available()) {
    char c = Serial1.read();
    if (c == '\n') {
      processLine(buffer);
      buffer = "";
    } else {
      buffer += c;
    }
  }
}

void processLine(String linea) {
  if (linea.indexOf("Corriente:") >= 0) {
    int index1 = linea.indexOf(":") + 2;
    int index2 = linea.indexOf(" A");
    String valorStr = linea.substring(index1, index2);
    float valor = valorStr.toFloat();
    Serial.println("Recibido: " + String(valor));

    medicionesHora[indiceMedicion++] = valor;
    enviarReplit("realtime", valor);

    if (indiceMedicion >= 1200) {
      float suma = 0;
      for (int i = 0; i < 1200; i++) suma += medicionesHora[i];
      float promedioHora = suma / 1200.0;
      enviarReplit("hora", promedioHora);
      enviarThingSpeak(promedioHora);
      promediosHora[indiceHora++] = promedioHora;
      indiceMedicion = 0;

      if (indiceHora >= 24) {
        float sumaDia = 0;
        for (int i = 0; i < 24; i++) sumaDia += promediosHora[i];
        float promedioDia = sumaDia / 24.0;
        enviarReplit("dia", promedioDia);
        promediosDia[indiceDia++] = promedioDia;
        indiceHora = 0;

        if (indiceDia >= 7) {
          float sumaSemana = 0;
          for (int i = 0; i < 7; i++) sumaSemana += promediosDia[i];
          float promedioSemana = sumaSemana / 7.0;
          enviarReplit("semana", promedioSemana);
          indiceDia = 0;
        }
      }
    }
  }
}

void enviarReplit(String tipo, float valor) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(serverName) + "?tipo=" + tipo + "&valor=" + String(valor, 2);
    http.begin(url);
    int httpCode = http.GET();
    http.end();
    Serial.println("Enviado a Replit [" + tipo + "]: " + String(valor));
  }
}

void enviarThingSpeak(float valor) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(thingspeakURL) + "?api_key=" + thingspeakApiKey + "&field1=" + String(valor, 2);
    http.begin(url);
    int httpCode = http.GET();
    http.end();
    Serial.println("Enviado a ThingSpeak: " + String(valor));
  }
}

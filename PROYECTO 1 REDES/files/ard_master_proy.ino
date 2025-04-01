#include <Wire.h>

#define SLAVE_ADDR 9  // Dirección del esclavo (ESP32)

void setup() {
  Serial.begin(115200);  // Iniciar el monitor serie
  analogReference(INTERNAL);  // Usar referencia interna de 1.1V
  Wire.begin();  // Iniciar la comunicación I2C como maestro
  Serial.println("Maestro Arduino iniciado...");
}

void loop() {
  int sensorValue = analogRead(A0);  // Lectura analógica del sensor SCT013
  float voltajeSensor = sensorValue * (1.1 / 1023.0);  // Convertir a voltaje
  float corriente = voltajeSensor * 30.0;  // Convertir voltaje a corriente (30A / 1V)

  Wire.beginTransmission(SLAVE_ADDR);  // Iniciar la transmisión al esclavo (ESP32)
  Wire.write((byte*)&corriente, sizeof(corriente));  // Enviar la corriente como datos binarios
  Wire.endTransmission();  // Terminar la transmisión

  Serial.print("Enviando dato de corriente: ");
  Serial.println(corriente, 3);  // Mostrar el valor de corriente en el monitor serie

  delay(1000);  // Esperar 1 segundo antes de enviar el siguiente dato
}

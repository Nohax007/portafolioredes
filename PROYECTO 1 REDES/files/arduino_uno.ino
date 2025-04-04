
#include <EmonLib.h>

EnergyMonitor emon1;

void setup() {
  Serial.begin(9600);
  emon1.current(A0, 111.1); // Calibración para SCT-013 100A:50mA
}

void loop() {
  double Irms = emon1.calcIrms(1480); // Número de muestras
  Serial.print("Corriente: ");
  Serial.print(Irms, 2);
  Serial.println(" A");
  delay(3000);
}

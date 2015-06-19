#include <SoftwareSerial.h>

//Pines conectados a LEDs
const int pinLedIzq = A5;
const int pinLedDer = 9;
const int pinLedCen1 = 3;
const int pinLedCen2 = 6;

//Pin de estado del modulo bluetooth (opcional)
const int pinEstadoBT = 11;

//Puerto serie bluetooth (virtual)
SoftwareSerial SerieBT(13, 12);

//Estado de los leds
bool ledIzq = false;
bool ledDer = false;
bool ledCen = false;
bool ledsActivos = false;

//Variables con tiempos
unsigned long tActual;
unsigned long tPulsoLed = 0;

void setup() {
  //Inicializa el pin de estado del bluetooth
  pinMode(pinEstadoBT, INPUT);

  //Inicializa los pines de los led:
  pinMode(pinLedIzq, OUTPUT);
  pinMode(pinLedDer, OUTPUT);
  pinMode(pinLedCen1, OUTPUT);
  pinMode(pinLedCen2, OUTPUT);

  digitalWrite(pinLedIzq, LOW);
  digitalWrite(pinLedDer, LOW);
  digitalWrite(pinLedCen1, LOW);
  digitalWrite(pinLedCen2, LOW);

  //Inicializa los puertos seriales
  Serial.begin(9600);
  SerieBT.begin(9600);
}

void loop() {
  char dato;

  //Primero se copia el tiempo actual en la variable
  tActual = millis();

  //Cada vez que elapsan 0.25 segundos, alterna el estado de
  //activacion de los leds (para hacerlos parpadear)
  if (tActual - tPulsoLed >= 250) {
    ledsActivos = !ledsActivos;
    tPulsoLed = tActual;
  }

  //Actualiza los leds (los enciende o apaga)
  actualizarLeds();

  //Si no hay datos disponibles, retorna (reinicia loop)
  if (!SerieBT.available()) return;

  //Si hay datos, entonces toma el siguiente
  dato = SerieBT.read();

  //Habilita o deshabilita los leds dependiendo del dato
  //recibido
  switch (dato) {
    case 'I': ledIzq = true;  break;
    case 'i': ledIzq = false; break;
    case 'D': ledDer = true;  break;
    case 'd': ledDer = false; break;
    case 'C': ledCen = true;  break;
    case 'c': ledCen = false; break;
  }
}

void actualizarLeds() {
  //Enciende cada led solo si los leds estan activos
  //(se encienden periodicamente) y si el led correspondiente
  //esta habilitado
  digitalWrite(pinLedIzq,  ledIzq && ledsActivos);
  delay(1);
  digitalWrite(pinLedIzq,  LOW);
  digitalWrite(pinLedDer,  ledDer && ledsActivos);
  delay(1);
  digitalWrite(pinLedDer, LOW);
  digitalWrite(pinLedCen1, ledCen && ledsActivos);
  delay(1);
  digitalWrite(pinLedCen1, LOW);
  digitalWrite(pinLedCen2, ledCen && ledsActivos);
  delay(1);
  digitalWrite(pinLedCen2, LOW);
}

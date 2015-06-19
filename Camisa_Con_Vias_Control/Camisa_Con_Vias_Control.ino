#include <SoftwareSerial.h>

//Direccion bluetooth de la camisa
//(Solo para referencia, ignorar)
//1014,07,102457 (esclavo)

//Pines con los botones
const int pinBotIzq = 2;
const int pinBotDer = 3;
const int pinBotCen = 4;

//Pines con LEDs
const int pinLedIndicador = 13;

//Pin de control del bluetooth
const int pinKeyBT = 10;

//Estado de los botones
bool botonIzq = false;
bool botonDer = false;
bool botonCen = false;
bool botonCenLargo = false;

//Estado de los leds (remotos)
bool ledIzq = false;
bool ledDer = false;
bool ledCen = false;

//Estado del led local
bool ledIndicador = false;

//Conteo del tiempo
unsigned long tActual = 0;
unsigned long tPulsoLed = 0;
unsigned long tPulsoBoton = 0;

//Puerto serie bluetooth (virtual)
SoftwareSerial SerieBT(11, 12);

void setup() {
  //Inicializa el pin de control del bluetooth
  pinMode(pinKeyBT, OUTPUT);

  //Inicializa los pines de los botones
  pinMode(pinBotIzq, INPUT);
  pinMode(pinBotDer, INPUT);
  pinMode(pinBotCen, INPUT);

  //Inicializa el pin del LED indicador
  pinMode(pinLedIndicador, OUTPUT);

  //Inicializa los puertos serie
  Serial.begin(9600);
  SerieBT.begin(9600);

  //Apaga el modo de comandos AT
  digitalWrite(pinKeyBT, LOW);
}

void loop() {
  //Primero se actualiza el tiempo
  tActual = millis();

  //Luego se leen los botones
  leerBotones();

  //Se controla el estado de los led dependiendo de que
  //botones se presionan. Si se presiona un boton, se
  //invierte el estado del led y transmite el estado del
  //mismo

  if (botonIzq) {
    ledIzq = !ledIzq;
    enviarEstadoLeds();
  }

  if (botonDer) {
    ledDer = !ledDer;
    enviarEstadoLeds();
  }

  if (botonCen) {
    ledCen = !ledCen;
    enviarEstadoLeds();
  }

  //En caso de mantenerse presionado el boton central por
  //mas de 1 segundo, se apagan todos los led y se transmite
  //el comando
  if (botonCenLargo) {
    ledIzq = 0;
    ledDer = 0;
    ledCen = 0;
    enviarEstadoLeds();
  }

  //Se pulsa el led indicador cada 0.25 segundos para indicar
  //al usuario que tiene encendida una lampara
  if (tActual - tPulsoLed > 250) {
    //Este codigo se ejecuta cada 0.25 segundos

    //El led se conmuta con cualquiera de las luces que este
    //activa
    if (ledIzq || ledDer || ledCen)
      ledIndicador = !ledIndicador;
    else
      ledIndicador = false;

    //Envia el estado del LED al puerto de I/O
    digitalWrite(pinLedIndicador, ledIndicador);

    //Actualiza el tiempo, para prepararse al proximo pulso
    tPulsoLed = tActual;
  }

  //Retarda la ejecucion 10ms (ayuda a la eliminacion de
  //rebote de los botones)
  delay(10);
}

void leerBotones() {
  //Variables con el estado anterior de los botones. Al
  //ser static, preservan su valor entre llamadas
  static bool botonIzqAnt = false;
  static bool botonDerAnt = false;
  static bool botonCenAnt = false;

  //Variables con el estado actual de los botones
  bool botonIzqAct;
  bool botonDerAct;
  bool botonCenAct;

  //Se lee el estado actual de los botones
  botonIzqAct = digitalRead(pinBotIzq);
  botonDerAct = digitalRead(pinBotDer);
  botonCenAct = digitalRead(pinBotCen);

  //Si el estado anterior es inactivo pero el actual es
  //activo, entonces se marca el boton como presionado
  //(con este procedimiento se elimina el rebote)
  if (!botonIzqAnt && botonIzqAct)
    botonIzq = true;
  else
    botonIzq = false;

  if (!botonDerAnt && botonDerAct)
    botonDer = true;
  else
    botonDer = false;

  if (!botonCenAnt && botonCenAct)
    botonCen = true;
  else
    botonCen = false;

  //Se guarda el tiempo en que se presiono el boton central,
  //en caso que se presione por un momento
  if (botonCen) tPulsoBoton = tActual;

  //Si el boton sigue presionado despues de 1 segundo, se
  //marca la bandera de presion larga
  if (botonCenAct && tActual - tPulsoBoton > 1000) {
    botonCenLargo = true;
    tPulsoBoton = tActual;
  }
  else
    botonCenLargo = false;

  //Se guardan los valores actuales, para que conviertan
  //en los anteriores en la siguiente llamada
  botonIzqAnt = botonIzqAct;
  botonDerAnt = botonDerAct;
  botonCenAnt = botonCenAct;
}

void enviarEstadoLeds() {
  //Se envian caracteres dependiendo del estado de cada LED
  if (ledIzq) SerieBT.write('I');
  else        SerieBT.write('i');

  if (ledDer) SerieBT.write('D');
  else        SerieBT.write('d');

  if (ledCen) SerieBT.write('C');
  else        SerieBT.write('c');
}

//Funcion de depuracion solamente, usar para configurar
//modulo bluetooth mediante comandos AT
void puenteSerial() {
  //Enciende el modo de comandos AT
  digitalWrite(pinKeyBT, HIGH);

  //Se queda atrapado en un bucle infinito intercambiando
  //datos
  for (;;) {
    if (SerieBT.available()) Serial.write(SerieBT.read());
    if (Serial.available())  SerieBT.write(Serial.read());
  }
}

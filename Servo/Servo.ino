//Agregamos las librerías del servo
#include <ESP32Servo.h>

//Instanciamos nuestro servo
Servo servo;
int pinServo = 18;

void setup()
{
  //Inicializamos la posición del servo
  servo.attach(pinServo, 500, 2500);
}

//Inicializamos la posición (en grados) del servo
int pos = 0;

void loop() {
  //Ciclo que posicionará el servo desde 0 hasta 180 grados
  for (pos = 0; pos <= 180; pos += 1) {
    //Movemos el servo a los grados que le entreguemos
    servo.write(pos);
    //Esperamos 15 milisegundos
    delay(15);
  }
  
  //Ciclo que posicionará el servo desde 180 hasta 0 grados
  for (pos = 180; pos >= 0; pos -= 1) {
    //Movemos el servo a los grados que le entreguemos
    servo.write(pos);
    //Esperamos 15 milisegundos
    delay(15);
  }
}
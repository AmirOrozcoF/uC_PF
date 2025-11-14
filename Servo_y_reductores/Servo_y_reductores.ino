/*
 * Control de Motores DC con TB6612FNG + Servo usando ESP32
 * Control mediante teclas del computador vía Serial
 * Motores: W/S/A/D para movimiento, X para detener
 * Servo: Se orienta según la dirección del carrito
 *        - Derecha (D): 0 grados
 *        - Izquierda (A): 180 grados
 *        - Otros casos: 90 grados
 */

// ============================================
// LIBRERÍAS
// ============================================
#include <ESP32Servo.h>

// ============================================
// CONFIGURACIÓN DE PINES - TB6612FNG
// ============================================
// Motor A (Motor Derecho)
const int PWMA = 25;  // PWM para velocidad Motor A
const int AIN1 = 26;  // Dirección Motor A
const int AIN2 = 27;  // Dirección Motor A

// Motor B (Motor Izquierdo)
const int PWMB = 14;  // PWM para velocidad Motor B
const int BIN1 = 12;  // Dirección Motor B
const int BIN2 = 13;  // Dirección Motor B

// Standby del TB6612FNG
const int STBY = 5;

// ============================================
// CONFIGURACIÓN DEL SERVO
// ============================================
Servo servo;
const int pinServo = 18;

// Posiciones del servo según dirección
const int SERVO_DERECHA = 0;
const int SERVO_CENTRO = 90;
const int SERVO_IZQUIERDA = 180;

// ============================================
// CONFIGURACIÓN PWM MOTORES
// ============================================
const int PWM_FREQ = 1000;      // Frecuencia PWM en Hz
const int PWM_RESOLUTION = 8;   // Resolución de 8 bits (0-255)

// ============================================
// VARIABLES GLOBALES
// ============================================
const int VELOCIDAD_MAXIMA = 255;
const int VELOCIDAD_MINIMA = 150;
int velocidad_actual = 200;

// ============================================
// FACTORES DE CORRECCIÓN DE MOTORES
// ============================================
// Ajusta estos valores para balancear las velocidades
// Valor entre 0.0 y 1.0 (1.0 = 100% de velocidad)
float factorMotorDerecho = 1;   // Si gira más rápido, reduce a 0.9, 0.8, etc.
float factorMotorIzquierdo = 1.0; // Si gira más rápido, reduce a 0.9, 0.8, etc.

// ============================================
// FUNCIONES DE CONTROL DE MOTORES
// ============================================
void detenerMotores() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  servo.write(SERVO_CENTRO);
  Serial.println("🛑 Motores detenidos - Servo centrado");
}

void motorDerechoAdelante(int velocidad) {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, velocidad * factorMotorDerecho);
}

void motorDerechoAtras(int velocidad) {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, velocidad * factorMotorDerecho);
}

void motorIzquierdoAdelante(int velocidad) {
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, velocidad * factorMotorIzquierdo);
}

void motorIzquierdoAtras(int velocidad) {
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, velocidad * factorMotorIzquierdo);
}

// ============================================
// FUNCIONES DE MOVIMIENTO DEL ROBOT
// ============================================
void avanzar() {
  motorDerechoAdelante(velocidad_actual);
  motorIzquierdoAdelante(velocidad_actual);
  servo.write(SERVO_CENTRO);
  Serial.println("⬆️  Avanzando - Servo centrado (90°)");
}

void retroceder() {
  motorDerechoAtras(velocidad_actual);
  motorIzquierdoAtras(velocidad_actual);
  servo.write(SERVO_CENTRO);
  Serial.println("⬇️  Retrocediendo - Servo centrado (90°)");
}

void girarDerecha() {
  motorIzquierdoAdelante(velocidad_actual);
  motorDerechoAtras(velocidad_actual / 2);
  servo.write(SERVO_DERECHA);
  Serial.println("➡️  Girando a la derecha - Servo a 0°");
}

void girarIzquierda() {
  motorDerechoAdelante(velocidad_actual);
  motorIzquierdoAtras(velocidad_actual / 2);
  servo.write(SERVO_IZQUIERDA);
  Serial.println("⬅️  Girando a la izquierda - Servo a 180°");
}

void aumentarVelocidad() {
  velocidad_actual = min(velocidad_actual + 20, VELOCIDAD_MAXIMA);
  Serial.print("⚡ Velocidad: ");
  Serial.println(velocidad_actual);
}

void disminuirVelocidad() {
  velocidad_actual = max(velocidad_actual - 20, VELOCIDAD_MINIMA);
  Serial.print("🐌 Velocidad: ");
  Serial.println(velocidad_actual);
}

// ============================================
// FUNCIÓN PRINCIPAL DE CONTROL
// ============================================
void procesarComando(char tecla) {
  tecla = tolower(tecla);
  
  switch(tecla) {
    // Control de motores
    case 'w':
      avanzar();
      break;
    case 's':
      retroceder();
      break;
    case 'd':
      girarDerecha();
      break;
    case 'a':
      girarIzquierda();
      break;
    case 'x':
      detenerMotores();
      break;
    case '+':
      aumentarVelocidad();
      break;
    case '-':
      disminuirVelocidad();
      break;
      
    default:
      Serial.print("❌ Comando desconocido: ");
      Serial.println(tecla);
      break;
  }
}

// ============================================
// SETUP
// ============================================
void setup() {
  // Inicializar comunicación serial
  Serial.begin(115200);
  delay(500);
  
  // IMPORTANTE: Inicializar servo PRIMERO para que reserve sus canales PWM
  servo.attach(pinServo, 500, 2500);
  delay(100);  // Pausa para estabilización
  servo.write(SERVO_CENTRO);  // Posición inicial centrada
  
  // Configurar pines de dirección de motores como salidas
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  
  // Configurar pines PWM de motores DESPUÉS del servo
  ledcAttach(PWMA, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(PWMB, PWM_FREQ, PWM_RESOLUTION);
  
  // Activar el driver (salir de standby)
  digitalWrite(STBY, HIGH);
  
  // Detener motores al inicio
  detenerMotores();
  
  // Mostrar menú
  Serial.println("\n==================================================");
  Serial.println("   CONTROL DE MOTORES TB6612FNG + SERVO");
  Serial.println("==================================================");
  Serial.println("\n📋 CONTROLES:");
  Serial.println("   W : Avanzar");
  Serial.println("   S : Retroceder");
  Serial.println("   D : Girar derecha");
  Serial.println("   A : Girar izquierda");
  Serial.println("   X : Detener");
  Serial.println("   + : Aumentar velocidad");
  Serial.println("   - : Disminuir velocidad");
  Serial.println("\n🎯 POSICIONES DEL SERVO:");
  Serial.println("   Derecha (D): 0°");
  Serial.println("   Izquierda (A): 180°");
  Serial.println("   Centro (W/S/X): 90°");
  Serial.println("\n⚙️  CALIBRACIÓN DE MOTORES:");
  Serial.print("   Motor Derecho: ");
  Serial.print(factorMotorDerecho * 100);
  Serial.println("%");
  Serial.print("   Motor Izquierdo: ");
  Serial.print(factorMotorIzquierdo * 100);
  Serial.println("%");
  Serial.println("\n✅ Sistema listo. Esperando comandos...\n");
}

// ============================================
// LOOP PRINCIPAL
// ============================================
void loop() {
  // Verificar si hay datos disponibles en el serial
  if (Serial.available() > 0) {
    char comando = Serial.read();
    
    // Ignorar saltos de línea y retornos de carro
    if (comando != '\n' && comando != '\r') {
      procesarComando(comando);
    }
  }
  
  delay(10);  // Pequeña pausa para no saturar el loop
}
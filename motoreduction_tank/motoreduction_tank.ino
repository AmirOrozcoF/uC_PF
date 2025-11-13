/*
 * Control de Motores DC con TB6612FNG usando ESP32
 * Control mediante teclas del computador vía Serial
 * Basado en pinout recomendado
 */

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
// CONFIGURACIÓN PWM
// ============================================
const int PWM_FREQ = 1000;      // Frecuencia PWM en Hz
const int PWM_RESOLUTION = 8;   // Resolución de 8 bits (0-255)

// ============================================
// VARIABLES GLOBALES
// ============================================
const int VELOCIDAD_MAXIMA = 255;  // Duty cycle máximo (0-255 con resolución de 8 bits)
const int VELOCIDAD_MINIMA = 150;  // Velocidad mínima para mover los motores
int velocidad_actual = 200;        // Velocidad por defecto

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
  Serial.println("Motores detenidos");
}

void motorDerechoAdelante(int velocidad) {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, velocidad);
}

void motorDerechoAtras(int velocidad) {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, velocidad);
}

void motorIzquierdoAdelante(int velocidad) {
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, velocidad);
}

void motorIzquierdoAtras(int velocidad) {
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, velocidad);
}

// ============================================
// FUNCIONES DE MOVIMIENTO DEL ROBOT
// ============================================

void avanzar() {
  motorDerechoAdelante(velocidad_actual);
  motorIzquierdoAdelante(velocidad_actual);
  Serial.println("Avanzando");
}

void retroceder() {
  motorDerechoAtras(velocidad_actual);
  motorIzquierdoAtras(velocidad_actual);
  Serial.println("Retrocediendo");
}

void girarDerecha() {
  motorIzquierdoAdelante(velocidad_actual);
  motorDerechoAtras(velocidad_actual / 2);
  Serial.println("Girando a la derecha");
}

void girarIzquierda() {
  motorDerechoAdelante(velocidad_actual);
  motorIzquierdoAtras(velocidad_actual / 2);
  Serial.println("Girando a la izquierda");
}

void aumentarVelocidad() {
  velocidad_actual = min(velocidad_actual + 20, VELOCIDAD_MAXIMA);
  Serial.print("Velocidad: ");
  Serial.println(velocidad_actual);
}

void disminuirVelocidad() {
  velocidad_actual = max(velocidad_actual - 20, VELOCIDAD_MINIMA);
  Serial.print("Velocidad: ");
  Serial.println(velocidad_actual);
}

// ============================================
// FUNCIÓN PRINCIPAL DE CONTROL
// ============================================

void procesarComando(char tecla) {
  tecla = tolower(tecla);  // Convertir a minúscula
  
  switch(tecla) {
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
      Serial.print("Comando desconocido: ");
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
  
  // Configurar pines de dirección como salidas
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  
  // Configurar pines PWM con ledcAttach (nueva API de ESP32 Arduino Core 3.x)
  ledcAttach(PWMA, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(PWMB, PWM_FREQ, PWM_RESOLUTION);
  
  // Activar el driver (salir de standby)
  digitalWrite(STBY, HIGH);
  
  // Detener motores al inicio
  detenerMotores();
  
  // Mostrar menú
  Serial.println("\n==================================================");
  Serial.println("CONTROL DE MOTORES TB6612FNG");
  Serial.println("==================================================");
  Serial.println("\nControles:");
  Serial.println("  W : Avanzar");
  Serial.println("  S : Retroceder");
  Serial.println("  D : Girar derecha");
  Serial.println("  A : Girar izquierda");
  Serial.println("  X : Detener");
  Serial.println("  + : Aumentar velocidad");
  Serial.println("  - : Disminuir velocidad");
  Serial.println("\nEsperando comandos...\n");
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
#include <Wire.h>

// Dirección I2C del MPU6050
const int MPU_ADDR = 0x68;

// Variables para datos crudos del acelerómetro
int16_t AcX, AcY, AcZ;

// Variables para ángulos de inclinación
float angleX = 0;
float angleY = 0;

// Variables para sensor touch
int touchValue = 0;
const int touchThreshold = 250;  // Umbral de detección (ajustar según necesidad)
bool ledState = false;

// Pines I2C del ESP32 (puedes cambiarlos si usas otros pines)
#define SDA_PIN 21
#define SCL_PIN 22

// Pin del sensor touch y LED integrado
#define TOUCH_PIN T0  // GPIO4 = Touch0
#define LED_PIN 2     // LED integrado en la mayoría de ESP32

void setup() {
  Serial.begin(115200);
  
  // Configurar LED integrado
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Inicializar I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Inicializar MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);  // Registro PWR_MGMT_1
  Wire.write(0);     // Despertar el MPU6050
  Wire.endTransmission(true);
  
  // Configurar el rango del acelerómetro a ±2g
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C);  // Registro ACCEL_CONFIG
  Wire.write(0x00);  // ±2g
  Wire.endTransmission(true);
  
  Serial.println("MPU6050 inicializado");
  Serial.println("Sensor touch en GPIO4 activo");
  Serial.println("Formato: AnguloX | AnguloY | Direccion | Touch");
  delay(1000);
}

void loop() {
  // Leer sensor touch
  touchValue = touchRead(TOUCH_PIN);
  
  // Controlar LED según toque
  if (touchValue < touchThreshold) {
    ledState = true;
    digitalWrite(LED_PIN, HIGH);
  } else {
    ledState = false;
    digitalWrite(LED_PIN, LOW);
  }
  
  // Leer datos del acelerómetro
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);  // Registro inicial del acelerómetro
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  
  // Calcular ángulos de inclinación usando el acelerómetro
  // Estos ángulos son independientes de la rotación en el eje Z
  angleX = atan2(AcY, sqrt(AcX * AcX + AcZ * AcZ)) * 180.0 / PI;
  angleY = atan2(-AcX, sqrt(AcY * AcY + AcZ * AcZ)) * 180.0 / PI;
  
  // Determinar dirección de inclinación
  String direccion = obtenerDireccion(angleX, angleY);
  
  // Mostrar resultados
  Serial.print("X: ");
  Serial.print(angleX, 1);
  Serial.print("° | Y: ");
  Serial.print(angleY, 1);
  Serial.print("° | ");
  Serial.print(direccion);
  Serial.print(" | Touch: ");
  Serial.print(touchValue);
  Serial.print(ledState ? " [LED ON]" : " [LED OFF]");
  Serial.println();
  
  delay(100);
}

String obtenerDireccion(float angX, float angY) {
  // Umbral de inclinación en grados
  const float umbral = 15.0;
  
  // Si está prácticamente horizontal
  if (abs(angX) < umbral && abs(angY) < umbral) {
    return "HORIZONTAL";
  }
  
  // Determinar dirección principal
  String dir = "";
  
  // Inclinación en eje Y (adelante/atrás)
  if (abs(angY) > umbral) {
    if (angY > 0) {
      dir += "ADELANTE";
    } else {
      dir += "ATRAS";
    }
  }
  
  // Inclinación en eje X (izquierda/derecha)
  if (abs(angX) > umbral) {
    if (dir.length() > 0) dir += "-";
    if (angX > 0) {
      dir += "DERECHA";
    } else {
      dir += "IZQUIERDA";
    }
  }
  
  return dir;
}
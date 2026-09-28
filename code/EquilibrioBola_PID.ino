// Automatic 2-DOF ball-balancing platform
// PID CONTROL

#include <stdint.h>
#include "TouchScreen.h"
#include <Servo.h>

// Pines de control de la pantalla táctil resistiva
#define YP A2
#define XM A3
#define YM 6
#define XP 5

// Valores de calibración de posición de la pantalla táctil
const int TS_MINX = 74;
const int TS_MAXX = 969;
const int TS_MINY = 104;
const int TS_MAXY = 934;

// Pines de servomotores
#define XAxis 11
#define YAxis 10

// Variables de filtrado
const int maxData = 5;
const float alpha = 0.8f;

//Estructura de datos de errores (Algoritmo PID)
struct ErrorState {
  float previous;
  float actual;
};

//Estructura de datos de ganancias de controlador PID
struct PIDConstants {
  float p;  // Ganancia proporcional
  float i;  // Ganancia integral
  float d;  // Ganancia derivativa
};

//Estructura de datos de valores límites
struct limitValues {
  float min;
  float max;
};

// Estructura de datos de control por eje
struct AxisControl {
  // Filtrado medida (mediana + EMA)
  int medianArray[maxData]; //Array para obtener la mediana de medidas
  float previousPosition;
  float filteredPosition;

  // Algoritmo control PID
  ErrorState error;
  PIDConstants K;
  const float reference;
  float integral;
  float derivative;
  bool pidInitialized;
  limitValues outputLimit;
  float debug;
};

// Función para cálculo de mediana de un array
int median(int a[maxData]) {
  int b[maxData];
  memcpy(b, a, sizeof(b));  // copia, para no tocar el buffer original
  for (int i = 1; i < maxData; i++) {   // ordenación por inserción
    int key = b[i];
    int j = i - 1;
    while (j >= 0 && b[j] > key) {
      b[j + 1] = b[j];
      j--;
    }
    b[j + 1] = key;
  }
  return b[maxData/2];  // valor central
}

// Función para cálculo de EMA
float ema(AxisControl axis){
  return alpha * median(axis.medianArray) + (1-alpha) * axis.previousPosition;
}

// Algoritmo de control PID
float PID(AxisControl& axis, float calcTime){
  axis.error.actual = axis.reference - axis.filteredPosition;

  // Control proporcional
  float proportional = axis.K.p * axis.error.actual;

  // Control integral
  axis.integral += (axis.error.actual + axis.error.previous) / 2.0f * calcTime;
  float integral = constrain(axis.K.i * axis.integral, -30, 30); //Anti-windup

  // Control derivativo
  float derivative = 0;
  if (axis.pidInitialized) { // Eliminar derivative kick
    axis.derivative = axis.K.d * (axis.error.actual - axis.error.previous) / calcTime;
  }
  axis.pidInitialized = true;

  // Ley de control
  float output = proportional + integral + axis.derivative;

  // Actualización del error
  axis.debug = axis.error.previous;
  axis.error.previous = axis.error.actual;

  // Enviar señal de control saturada a planta
  return constrain(output,axis.outputLimit.min, axis.outputLimit.max);
}

// Función de reinicio de variables
void restartAxis(AxisControl& axis){
  axis.integral = 0;
  axis.error.previous = 0;
  axis.error.actual = 0;
  axis.pidInitialized = false;
}

// Inicialización de la pantalla táctil
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 299);  // Resistencia en eje X medida con multímetro

// Inicialización de servomotores
Servo servoX;
Servo servoY;

// Inicialización de ejes
AxisControl controlX = {};
AxisControl controlY = {};


void setup() {
  // Asignación de pines de servomotores
  servoX.attach(XAxis);
  servoY.attach(YAxis);
  ts.pressureThreshhold = 5;

  // Valores de constantes del control del eje X
  controlX.K.p = 0.2;
  controlX.K.i = 0.05;
  controlX.K.d = 0.1;
  controlX.outputLimit.min = -50;
  controlX.outputLimit.max = 50;

  // Valores de constantes del control del eje Y
  controlY.K.p = 0.15;
  controlY.K.i = 0.05;
  controlY.K.d = 0.1;
  controlY.outputLimit.min = -40;
  controlY.outputLimit.max = 40;

  //Posición inicial servomotores
  servoX.write(90);
  servoY.write(90);
}


void loop() {
  // Inicio de tiempo
  unsigned long startTime = millis();
  unsigned long lastRead = 0;
  unsigned long lastDetection = 0;
  int numData = 0;
  
  // Recopilación de medidas de pantalla resistiva
  while (numData < maxData){
    // Leer solo cada 10 ms
    if (millis() - lastRead >= 10) {
      lastRead = millis();
      // Lectura de pantalla resistiva
      TSPoint p = ts.getPoint();  

      // Si existe detección, se registra la posición
      if (p.z > ts.pressureThreshhold) {
        lastDetection = millis();
        //Calibración de posición
        int x = map(p.x, TS_MINX, TS_MAXX, -85, 85);
        int y = map(p.y, TS_MINY, TS_MAXY, 64, -64);

        //Filtrado de coordenadas
        controlX.medianArray[numData] = x;
        controlY.medianArray[numData] = y;
        numData++;
      }
    }
    //Salir del bucle while y volver a posición inicial tras tiempo sin detección
    if (millis() - lastDetection > 500){
      break;
    }
  }

  //Medida correcta - Inicio algoritmo PID
  if (numData == maxData){
  //Filtrado mediana + EMA 
  controlX.filteredPosition = ema(controlX);
  controlY.filteredPosition = ema(controlY);
  
  //Actualización valores previos
  controlX.previousPosition = controlX.filteredPosition;
  controlY.previousPosition = controlY.filteredPosition;

  //Control PID
  float calcTime = (millis() - startTime) / 1000.0f;
  float outputX = PID(controlX, calcTime);
  float outputY = PID(controlY, calcTime);

  //Enviar señal de control a la planta
  servoX.write(90 - outputX);
  servoY.write(90 + outputY);
  }
  
  //Tiempo de espera agotado - Reinicio de sistema
  else {
    //Posición inicial servomotores
    servoX.write(90);
    servoY.write(90);

    //Reinicio errores e integral de ejes
    restartAxis(controlX);
    restartAxis(controlY);
  }
}

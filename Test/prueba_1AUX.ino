

bool enablePin1 = false;
bool enablePin2 = false;

const int pin1 = 0; // Pin analógico 1
const int pin2 = 1; // Pin analógico 2
const int numSamples = 2000;

int muxPins[3] = {2, 3, 4}; // Pines para controlar el multiplexor
int muxPins2[3] = {5, 6, 7};


float average1 = 0.0;
float average2 = 0.0;

int muxValue1 = 0;
int muxValue2 = 0;

void setup() {
  Serial.begin(115200);

  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);

  // Configura los pines del multiplexor como salidas
  for (int i = 0; i < 3; i++) {
  pinMode(muxPins[i], OUTPUT);
  pinMode(muxPins2[i], OUTPUT);
  }

  Serial.println("init ok");

}


void recibirYProcesarDatosDesdePython() {
  if (Serial.available() > 0) {
    String mensajeRecibido = Serial.readStringUntil('\n');

    if (mensajeRecibido.length() > 0) {
      // Dividir el mensaje en partes usando comas como delimitador
      String partes[4];
      int posComa = -1;

      for (int i = 0; i < 4; i++) {
        int nuevaPosComa = mensajeRecibido.indexOf(',', posComa + 1);

        if (nuevaPosComa != -1) {
          partes[i] = mensajeRecibido.substring(posComa + 1, nuevaPosComa);
          posComa = nuevaPosComa;
        } else {
          // Si no se encuentra una coma, se detiene el bucle
          break;
        }
      }

      // Verificar si se obtuvieron las cuatro partes
      if (partes[0].length() > 0 && partes[1].length() > 0 && partes[2].length() > 0 && partes[3].length() > 0) {
        int escala_seleccionada = partes[0].toInt();
        int canal_seleccionado = partes[1].toInt();
        bool canal1_seleccionado = partes[2].toInt() == 1; // Se espera 1 para true
        bool canal2_seleccionado = partes[3].toInt() == 1; // Se espera 1 para true

        // Actualizar las variables en el Arduino con los valores recibidos
        if (canal_seleccionado == 1) {
          controlarMultiplexor(1, escala_seleccionada);
        } else if (canal_seleccionado == 2) {
          controlarMultiplexor(2, escala_seleccionada);
        }
        enablePin1 = canal1_seleccionado;
        enablePin2 = canal2_seleccionado;

        // Para depuración, puedes imprimir los valores
        Serial.print("Escala seleccionada: ");
        Serial.println(escala_seleccionada);
        Serial.print("Canal seleccionado: ");
        Serial.println(canal_seleccionado);
        Serial.print("Canal 1 seleccionado: ");
        Serial.println(canal1_seleccionado);
        Serial.print("Canal 2 seleccionado: ");
        Serial.println(canal2_seleccionado);
      }
    }
  }
}


void controlarMultiplexor(int muxNumber, int value) {
  if (value >= 0 && value <= 5) {
    int *muxPinsToControl = (muxNumber == 1) ? muxPins : muxPins2;
    Serial.print("Multiplexor ");
    Serial.print(muxNumber);
    Serial.println(" controlado con éxito.");
    for (int i = 0; i < 3; i++) {
      digitalWrite(muxPinsToControl[i], bitRead(value, i));
    }
  } else {
    Serial.print("Valor del multiplexor ");
    Serial.print(muxNumber);
    Serial.println(" no válido.");
  }
}


void leerDatosDesdePython(){
  Serial.print(average1);
  Serial.print(",");
  Serial.println(average2);
  delay(1000);
}

void promedioAnalogico(int pin1, int pin2) {
  float sum1 = 0;
  float sum2 = 0;

  if (enablePin1) {
    for (int i = 0; i < numSamples; i++) {
      float analog1rawvalue = analogRead(pin1);
      float analogVoltage1 = (analog1rawvalue / 1023.0) * 5;
      sum1 += analogVoltage1;
      delayMicroseconds(500); // Espera 500 microsegundos entre lecturas
    }
  }

  if (enablePin2) {
    for (int i = 0; i < numSamples; i++) {
      float analog2rawvalue = analogRead(pin2);
      float analogVoltage2 = (analog2rawvalue / 1023.0) * 5;
      sum2 += analogVoltage2;
      delayMicroseconds(500); // Espera 500 microsegundos entre lecturas
    }
  }

  average1 = enablePin1 ? (sum1 / numSamples) : 0.0;
  average2 = enablePin2 ? (sum2 / numSamples) : 0.0;

  //Serial.print("Promedio del Pin 1: ");
  //Serial.println(average1);
  //Serial.print("Promedio del Pin 2: ");
  //Serial.println(average2);

}  




void loop() {
  recibirYProcesarDatosDesdePython();
  promedioAnalogico(pin1,pin2);
  leerDatosDesdePython();
}

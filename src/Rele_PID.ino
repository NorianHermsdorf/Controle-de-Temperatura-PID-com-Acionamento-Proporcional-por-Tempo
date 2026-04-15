// INCLUSÃO DE BIBLIOTECAS
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <PID_v1.h>  // Biblioteca PID

// DEFINIÇÕES DE PINOS
#define pinZC  2
#define pinDIM 4
#define ONE_WIRE_BUS 7  // Porta de Leitura do sensor
#define intervaloLeitura 1000
#define backlightPin 11
#define contrastPin 10
#define POT_PIN A0 

#define periodo 8333 // MICROSSEGUNDOS
#define iMin 20 // INTENSIDADE MINIMA
#define iMax 90 // INTENSIDADE MÁXIMA

// Inicialização do sensor de temperatura e do LCD
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal lcd(3, 5, 6, 8, 9, 12);

// Variáveis de controle do tempo
unsigned long controleTempo;
int brilho = 255;
int contraste = 50;
float temperaturaIdeal = 0.0;

// Variáveis de controle de intensidade
volatile int intensidade = 0; // Carregar variável da memória RAM em vez do registrador

// Variáveis PID
double Setpoint, Input, Output;
double Kp = 5.0, Ki = 0, Kd = 0.0; // Ajustar conforme necessário
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);  // Configuração PID

void setup() {
  pinMode(A0, INPUT);
  Serial.begin(115200);
  lcd.begin(16, 2); // Inicia o LCD com 16 colunas e 2 linhas
  pinMode(pinDIM, OUTPUT);
  pinMode(pinZC, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinZC), sinalZC, RISING);
  
  controleTempo = millis();
  analogWrite(backlightPin, brilho);
  analogWrite(contrastPin, contraste);
  
  lcd.setCursor(0, 0);
  lcd.print("Temp Atual: ");
  lcd.setCursor(0, 1);
  lcd.print("Temp Desej: ");
  
  sensors.begin();

  // Inicializa o PID
  myPID.SetMode(AUTOMATIC);
  
  Serial.println("Fim do Setup()");
}

void loop() {
  int potValue = analogRead(POT_PIN);
  temperaturaIdeal = map(potValue, 0, 1023, 0, 100);  // Ajustar o range conforme necessário
  Setpoint = temperaturaIdeal;

  if (millis() - controleTempo > intervaloLeitura) {
    sensors.requestTemperatures(); 
    Input = sensors.getTempCByIndex(0); // Atualiza a temperatura atual

    // Se a temperatura ultrapassar o setpoint, desligar o aquecedor
    if (Input >= temperaturaIdeal) {
      intensidade = 0; // Desliga o aquecedor
      Serial.print("Temperatura Atual: "); Serial.print(Input, 2);
      Serial.println("Aquecedor desligado. Temperatura atingida.");
    } else {
      // Calcula o PID
      myPID.Compute();

      // Mapeia a saída do PID (0-255) para a intensidade do dimmer (iMin a iMax)
      intensidade = map((int)Output, 0, 255, iMin, iMax);

      // Garante que a intensidade esteja entre iMin e iMax
      intensidade = constrain(intensidade, iMin, iMax);

      // Serial: Exibe a temperatura e intensidade a cada leitura
      Serial.print("Temperatura Atual: "); Serial.print(Input, 2);
      Serial.print(" C, Temperatura Ideal: "); Serial.print(Setpoint, 2);
      Serial.print(" C, Intensidade: "); Serial.println(intensidade);
    }

    // Atualiza o display LCD
    lcd.setCursor(12, 0);
    lcd.print(Input, 2); // Atualiza a temperatura atual no LCD
    lcd.setCursor(12, 1);
    lcd.print(Setpoint, 2); // Atualiza a temperatura desejada no LCD
    
    controleTempo += intervaloLeitura;
  }

  // Função de controle do dimmer
  lcd.setCursor(0, 0);
  lcd.print("Temp Atual: ");
  lcd.print(Setpoint, 2);  // Limita a 2 casas decimais
  lcd.setCursor(0, 1);
  lcd.print("Temp Desej: ");
  lcd.clear();
}

// Implementação de função de controle de dimmer via interrupção
void sinalZC() {
  if (intensidade < iMin) return;
  if (intensidade > iMax) intensidade = iMax;

  int delayInt = periodo - (intensidade * (periodo / 100));

  delayMicroseconds(delayInt);
  digitalWrite(pinDIM, HIGH);
  delayMicroseconds(6);
  digitalWrite(pinDIM, LOW);
}

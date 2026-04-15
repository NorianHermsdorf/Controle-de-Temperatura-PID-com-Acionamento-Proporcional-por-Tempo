#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

// ======================= CONSTANTES =========================
const byte ONE_WIRE_BUS = 7;
const byte RELAY_PIN = 4;
const byte POT_PIN = A0;

const int INTERVALO_LEITURA_SENSOR = 1000; // ms para ler o sensor

// --- NOVAS CONSTANTES PARA O CONTROLE PROPORCIONAL ---
const unsigned long JANELA_PWM = 5000; // Janela de tempo de 5 segundos (5000 ms)
const double MAX_SAIDA_PD = 100.0;     // Valor máximo esperado da saída do PD (ajustável)

// ======================= OBJETOS ===============================
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ======================= VARIÁVEIS GLOBAIS =============================
unsigned long tempoLeituraSensor;
float temperaturaIdeal = 50.0;
float temperaturaAtual = 0.0;

// Ganhos PD
double Kp = 15.0;
double Kd = 5.0;

// Variáveis do PD
double erroAnterior = 0;
bool estadoRele = false;

unsigned long tempoInicioJanela;
double potenciaSaida = 0; 

// ======================= SETUP ================================
void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  lcd.init();
  lcd.backlight();
  
  sensors.begin();

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.setCursor(0, 1);
  lcd.print("Alvo: ");
  
  tempoLeituraSensor = millis();
  tempoInicioJanela = millis(); // Inicia a primeira janela de tempo
}


void loop() { 
   //Leitura do sensor e cálculo do PD --
  if (millis() - tempoLeituraSensor >= INTERVALO_LEITURA_SENSOR) {
    // Lê o potenciômetro para definir a temperatura ideal
    int potValue = analogRead(POT_PIN);
    temperaturaIdeal = map(potValue, 0, 1023, 20, 99);

    // Lê o sensor de temperatura
    sensors.requestTemperatures();
    float leitura = sensors.getTempCByIndex(0);

    if (leitura != DEVICE_DISCONNECTED_C && leitura > -55 && leitura < 125) {
      temperaturaAtual = leitura;
    } else {
    }

    // Controle PD
    double erro = temperaturaIdeal - temperaturaAtual;
    double derivada = erro - erroAnterior;
    double saidaPD = (Kp * erro) + (Kd * derivada);

    // Mapeia e limita a saída do PD para a faixa de potência (0 a MAX_SAIDA_PD)
    potenciaSaida = constrain(saidaPD, 0, MAX_SAIDA_PD);
    
    erroAnterior = erro;

    tempoLeituraSensor = millis();
  }

  //Controle do Relé Proporcional ao Tempo (executa em todo loop) ---
  unsigned long tempoAtual = millis();
  
  // Reinicia a janela de tempo se ela terminou
  if (tempoAtual - tempoInicioJanela > JANELA_PWM) {
    tempoInicioJanela = tempoAtual;
  }

  // Calcula por quanto tempo o relé deve ficar ligado dentro da janela atual
  unsigned long tempoLigado = JANELA_PWM * (potenciaSaida / MAX_SAIDA_PD);

  // Liga ou desliga o relé baseado no tempo decorrido dentro da janela
  if (tempoAtual - tempoInicioJanela < tempoLigado) {
    digitalWrite(RELAY_PIN, HIGH);
    estadoRele = true;
  } else {
    digitalWrite(RELAY_PIN, LOW);
    estadoRele = false;
  }

   unsigned long now = millis();
  static unsigned long lastPrintTime = 0;
  static unsigned long lastTime = 0;
  lcd.setCursor(6, 0);
  lcd.print(temperaturaAtual, 1);
  lcd.print((char)223);
  lcd.print("C ");

  lcd.setCursor(6, 1);
  lcd.print(temperaturaIdeal, 1);
  lcd.print((char)223);
  lcd.print("C ");

  lcd.setCursor(13, 0);
  lcd.print(estadoRele ? "ON " : "OFF");

  if(now  - lastPrintTime >= 500){
    lastPrintTime =  now;
    Serial.print("Temperatura:"); Serial.print(temperaturaAtual); Serial.print("\t");
    Serial.print("On_Off:"); Serial.println(tempoLigado/100);
  }
}

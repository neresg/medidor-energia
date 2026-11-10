/*
  Medidor de Energia com Alerta - Simulação no Tinkercad Circuits
  Arduino Uno + LCD 16x2 + 3 potenciômetros + 2 botões + 2 LEDs + buzzer

  MODO_DIAGNOSTICO = true  -> LCD mostra os valores brutos (0-1023) de A0, A1, A2
  MODO_DIAGNOSTICO = false -> funcionamento normal do medidor
*/
#include <LiquidCrystal.h>

const bool MODO_DIAGNOSTICO = false;

// ---------- Pinos ----------
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);   // RS, E, D4, D5, D6, D7
const int PIN_V = A0;                    // pot: tensão
const int PIN_I = A1;                    // pot: corrente
const int PIN_FP = A2;                   // pot: fator de potência
const int PIN_BTN_TELA = 7;
const int PIN_BTN_RESET = 8;
const int PIN_LED_OK = 9;
const int PIN_LED_ALERTA = 10;
const int PIN_BUZZER = 6;

// ---------- Parâmetros (ajuste aqui) ----------
const float V_MAX = 250.0;               // V  (fundo de escala do pot)
const float I_MAX = 20.0;                // A
const float FP_MIN = 0.50;
const float FP_MAX = 1.00;
const float TARIFA = 0.95;               // R$/kWh (exemplo)
const float LIMITE_W = 3000.0;           // potência ativa que dispara o alerta
const float HISTERESE = 0.95;            // alerta desliga abaixo de 95% do limite
const float ACEL = 60.0;                 // 1 s real = 60 s simulados
const int AMOSTRAS = 8;                  // leituras por média

const unsigned long T_LCD = 250;         // ms
const unsigned long T_SERIAL = 1000;     // ms
const unsigned long T_IGNORA_BTN = 1000; // ignora botões no início

// ---------- Estado ----------
int rawV, rawI, rawFP;
float tensao, corrente, fp, potAparente, potAtiva;
float energiaKWh = 0.0;
bool alerta = false;
int tela = 0;
unsigned long tInicio, tAnterior, tLcd, tSerial, tBuzzer;
bool buzzerOn = false;
bool ultTela = HIGH;
bool ultReset = HIGH;

// Lê um pino analógico com média de várias amostras
int lerAnalogico(int pino) {
  analogRead(pino);                      // descarta a 1ª leitura (estabiliza)
  delay(2);
  long soma = 0;
  for (int i = 0; i < AMOSTRAS; i++) {
    soma += analogRead(pino);
  }
  return (int)(soma / AMOSTRAS);
}

bool pressionou(int pino, bool &ultimo) {
  bool atual = digitalRead(pino);
  bool evento = (ultimo == HIGH && atual == LOW);
  ultimo = atual;
  if (evento) delay(30);                 // debounce simples
  return evento;
}

void linha(int row, const char *txt) {
  char buf[17];
  snprintf(buf, sizeof(buf), "%-16s", txt);
  lcd.setCursor(0, row);
  lcd.print(buf);
}

void atualizaLcd() {
  char a[17], b[17], n1[10], n2[10];

  if (MODO_DIAGNOSTICO) {
    snprintf(a, sizeof(a), "A0:%4d A1:%4d", rawV, rawI);
    snprintf(b, sizeof(b), "A2:%4d", rawFP);
    linha(0, a);
    linha(1, b);
    return;
  }

  switch (tela) {
    case 0:
      dtostrf(tensao, 5, 1, n1);
      dtostrf(corrente, 5, 2, n2);
      snprintf(a, sizeof(a), "Tensao: %s V", n1);
      snprintf(b, sizeof(b), "Corr.: %s A", n2);
      break;
    case 1:
      dtostrf(potAtiva, 6, 0, n1);
      dtostrf(fp, 4, 2, n2);
      snprintf(a, sizeof(a), "P: %s W", n1);
      snprintf(b, sizeof(b), "FP:%s %s", n2, alerta ? "ALERTA" : "OK");
      break;
    default:
      dtostrf(energiaKWh, 7, 3, n1);
      dtostrf(energiaKWh * TARIFA, 7, 2, n2);
      snprintf(a, sizeof(a), "E:%s kWh", n1);
      snprintf(b, sizeof(b), "R$%s", n2);
      break;
  }
  linha(0, a);
  linha(1, b);
}

void setup() {
  pinMode(PIN_BTN_TELA, INPUT_PULLUP);
  pinMode(PIN_BTN_RESET, INPUT_PULLUP);
  pinMode(PIN_LED_OK, OUTPUT);
  pinMode(PIN_LED_ALERTA, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial.println(F("t_s,V,A,FP,S_VA,P_W,E_kWh,custo_R$,alerta,rawA0,rawA1,rawA2"));

  linha(0, "Medidor Energia");
  linha(1, MODO_DIAGNOSTICO ? "Modo diagnostico" : "Iniciando...");
  delay(1500);

  tInicio = tAnterior = tLcd = tSerial = tBuzzer = millis();
}

void loop() {
  unsigned long agora = millis();

  // 1) Leituras
  rawV = lerAnalogico(PIN_V);
  rawI = lerAnalogico(PIN_I);
  rawFP = lerAnalogico(PIN_FP);

  tensao = rawV / 1023.0 * V_MAX;
  corrente = rawI / 1023.0 * I_MAX;
  fp = FP_MIN + rawFP / 1023.0 * (FP_MAX - FP_MIN);

  // 2) Cálculos elétricos
  potAparente = tensao * corrente;         // VA
  potAtiva = potAparente * fp;             // W

  // 3) Integração de energia (kWh)
  float dt = (agora - tAnterior) / 1000.0; // s reais
  tAnterior = agora;
  energiaKWh += (potAtiva / 1000.0) * (dt * ACEL / 3600.0);

  // 4) Alerta com histerese
  if (!alerta && potAtiva > LIMITE_W) alerta = true;
  else if (alerta && potAtiva < LIMITE_W * HISTERESE) alerta = false;

  digitalWrite(PIN_LED_ALERTA, alerta ? HIGH : LOW);
  digitalWrite(PIN_LED_OK, alerta ? LOW : HIGH);

  if (alerta) {
    if (agora - tBuzzer >= 300) {
      tBuzzer = agora;
      buzzerOn = !buzzerOn;
      if (buzzerOn) tone(PIN_BUZZER, 1000); else noTone(PIN_BUZZER);
    }
  } else {
    noTone(PIN_BUZZER);
    buzzerOn = false;
  }

  // 5) Botões (ignorados no primeiro segundo)
  if (agora - tInicio > T_IGNORA_BTN) {
    if (pressionou(PIN_BTN_TELA, ultTela)) tela = (tela + 1) % 3;
    if (pressionou(PIN_BTN_RESET, ultReset)) energiaKWh = 0.0;
  }

  // 6) Saídas periódicas
  if (agora - tLcd >= T_LCD) {
    tLcd = agora;
    atualizaLcd();
  }
  if (agora - tSerial >= T_SERIAL) {
    tSerial = agora;
    Serial.print(agora / 1000);           Serial.print(',');
    Serial.print(tensao, 1);              Serial.print(',');
    Serial.print(corrente, 2);            Serial.print(',');
    Serial.print(fp, 2);                  Serial.print(',');
    Serial.print(potAparente, 0);         Serial.print(',');
    Serial.print(potAtiva, 0);            Serial.print(',');
    Serial.print(energiaKWh, 4);          Serial.print(',');
    Serial.print(energiaKWh * TARIFA, 2); Serial.print(',');
    Serial.print(alerta);                 Serial.print(',');
    Serial.print(rawV);                   Serial.print(',');
    Serial.print(rawI);                   Serial.print(',');
    Serial.println(rawFP);
  }
}

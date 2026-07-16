#include <FreqCount.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Definições de Pinos do Sensor e Servos
#define S0 4
#define S1 9
#define S2 6
#define S3 7
#define OUT 5 
#define PINO_SERVO1 3 
#define PINO_SERVO2 11
#define PINO_SERVO3 13

// Definições do Encoder Rotativo
#define ENC_CLK 2  // Pino com suporte a interrupção no Uno/Nano
#define ENC_DT  8
#define ENC_SW  10

// Inicialização do LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Máquina de Estados do Sistema
enum EstadoSistema { MODO_OPERACAO, MODO_MENU };
EstadoSistema estadoAtual = MODO_OPERACAO;

// Variáveis de Controle do Encoder
volatile int posicaoMenu = 0;
volatile bool encoderMovido = false;
int ultimaPosicaoMenu = -1;
unsigned long tempoUltimoBotao = 0;
unsigned long tempoUltimaInteracaoMenu = 0;
const unsigned long TIMEOUT_MENU = 8000; // 8 segundos de inatividade voltam para a operação

const unsigned long TEMPO_DEBOUNCE = 1000; 
unsigned long ultimaIdentificacao = 0;

// Variáveis dos Servos e Timers
int larguraPulso1 = 1570;   
unsigned long ultimoPulso = 0;
unsigned long tempoResetServo = 0;
int larguraPulso2 = 1570;   
unsigned long tempoResetServo2 = 0;
int larguraPulso3 = 530;
unsigned long tempoMoverServo3 = 0;
bool direcaoServo3 = true;
unsigned long intervaloServo3 = (estadoAtual == MODO_MENU) ? 3000 : 1000;

// Estrutura do Perfil de Cor
struct ColorProfile {
  long rRaw, gRaw, bRaw;
  float rn, gn, bn;
  bool configurada;
};

// Modificado para 5 posições (Tubo + 3 Tampas + Reset)
ColorProfile alvos[4]; // Apenas os 4 primeiros guardam cores
const char* nomes[] = {"TUBO", "Tampa 1", "Tampa 2", "Tampa 3", "RESET EEPROM"};
int threshold = 20; 

// Contadores de Tampas para o Modo Operação
int contagemTampas = 0;
String ultimaTampaDetectada = "Nenhuma";

// ISR para leitura rápida do Encoder
void lerEncoder() {
  if (digitalRead(ENC_CLK) == digitalRead(ENC_DT)) {
    posicaoMenu++;
    if (posicaoMenu > 4) posicaoMenu = 0; // Limite ajustado para 4 (5 itens)
  } else {
    posicaoMenu--;
    if (posicaoMenu < 0) posicaoMenu = 4; 
  }
  encoderMovido = true;
}

void setup() {
  // Inicialização do LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  SEPARADOR DE   ");
  lcd.setCursor(0, 1);
  lcd.print("    TAMPINHAS    ");
  delay(1500);

  // Setup Servos e Sensor
  pinMode(PINO_SERVO1, OUTPUT);
  pinMode(PINO_SERVO2, OUTPUT);
  pinMode(PINO_SERVO3, OUTPUT);
  pinMode(S0, OUTPUT); pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT); pinMode(S3, OUTPUT);
  digitalWrite(S0, HIGH); digitalWrite(S1, HIGH);
  FreqCount.begin(3); 

  // Setup Encoder
  pinMode(ENC_CLK, INPUT);
  pinMode(ENC_DT, INPUT);
  pinMode(ENC_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_CLK), lerEncoder, CHANGE);

  // Carrega dados da EEPROM
  for(int i=0; i<4; i++) {
    EEPROM.get(i * sizeof(ColorProfile), alvos[i]);
    if(isnan(alvos[i].rn) || !alvos[i].configurada) {
      alvos[i].configurada = false;
    }
  }
  
  lcd.clear();
  atualizarTelaOperacao();
}

void loop() {
  
  // Captura segura das flags da ISR (Evita Race Conditions)
  noInterrupts();
  bool mudouEncoder = encoderMovido;
  int menuAtual = posicaoMenu;
  encoderMovido = false;
  interrupts();

  // Transição de Estado: Se mexer no encoder, vai para o Menu
  if (mudouEncoder && estadoAtual == MODO_OPERACAO) {
    estadoAtual = MODO_MENU;
    ultimaPosicaoMenu = -1; // Força atualização do LCD
    tempoUltimaInteracaoMenu = millis();
  }

  // LÓGICA DE CADA ESTADO
  if (estadoAtual == MODO_MENU) {
    // Gerenciamento do Menu no LCD
    if (menuAtual != ultimaPosicaoMenu || mudouEncoder) {
      tempoUltimaInteracaoMenu = millis();
      ultimaPosicaoMenu = menuAtual;
      
      lcd.setCursor(0, 0);
      lcd.print("MENU:           ");
      lcd.setCursor(0, 1);
      lcd.print("                "); // Limpa linha
      lcd.setCursor(0, 1);
      lcd.print(nomes[menuAtual]);
      
      // Se for um índice de cor e estiver configurado, mostra [OK]
      if (menuAtual < 4 && alvos[menuAtual].configurada) {
        lcd.print(" [OK]");
      }
    }

    // Timeout do Menu (Volta para operação se ficar inativo)
    if (millis() - tempoUltimaInteracaoMenu > TIMEOUT_MENU) {
      estadoAtual = MODO_OPERACAO;
      atualizarTelaOperacao();
    }

    // Clique do Botão no Modo Menu
    if (digitalRead(ENC_SW) == LOW) {
      if (millis() - tempoUltimoBotao > 300) { // Debounce do botão
        if (menuAtual == 4) {
          executarResetEEPROM();
        } else {
          executarCalibracao(menuAtual);
        }
        tempoUltimoBotao = millis();
        estadoAtual = MODO_OPERACAO; // Volta para operação após calibrar/resetar
        atualizarTelaOperacao();
      }
    }

  } else {
    // MODO_OPERACAO: Executa a leitura e separação ativamente
    long r, g, b;
    lerRGB(r, g, b);

    unsigned long tempoAtual = millis(); // Captura o tempo atual para o debounce da queda

    // O gatilho agora só dispara se o threshold for atingido E se passou o TEMPO_DEBOUNCE
    if (alvos[0].configurada && 
        (tempoAtual - ultimaIdentificacao >= TEMPO_DEBOUNCE) && // <- Trava de tempo adicionada aqui
        (abs(r - alvos[0].rRaw) > threshold || 
         abs(g - alvos[0].gRaw) > threshold || 
         abs(b - alvos[0].bRaw) > threshold)) {
      
      ultimaIdentificacao = tempoAtual; // Bloqueia novas leituras falsas
      processarQueda();
    }
  }


  if (millis() >= 30000) { 
    if (millis() - tempoMoverServo3 >= intervaloServo3) {
      tempoMoverServo3 = millis();
      if (direcaoServo3) {
        larguraPulso3 = 2470; // 180 graus
      } else {
        larguraPulso3 = 530;  // 0 graus
      }
      direcaoServo3 = !direcaoServo3;
    }
  } else {
    // Garante que nos primeiros 10 segundos ele permaneça estrito em 530
    larguraPulso3 = 530;
    tempoMoverServo3 = millis(); // Mantém o timer zerado para começar a contar o 1s após o timeout
  }

  // PWM MANUAL DOS SERVOS (Permanece rodando em paralelo em ambos os estados)
  atualizarServosPWM();

  // Gerenciadores de retorno ao neutro
  if (tempoResetServo > 0 && millis() > tempoResetServo) {
    larguraPulso1 = 1570; 
    tempoResetServo = 0;
  }
  if (tempoResetServo2 > 0 && millis() > tempoResetServo2) {
    larguraPulso2 = 1570;
    tempoResetServo2 = 0;
  }
}

// ================== NOVO: PWM extraído para função própria ==================
// Gera o pulso dos 3 servos com base nas variáveis globais larguraPulsoX.
// Precisa ser chamada com frequência (a cada ~20ms) mesmo dentro de trechos
// bloqueantes (delay/while de calibração), senão o servo 3 "congela" na
// última posição em vez de ir para 530 como esperado.
void atualizarServosPWM() {
  if (micros() - ultimoPulso >= 20000) { 
    digitalWrite(PINO_SERVO1, HIGH);
    delayMicroseconds(larguraPulso1);
    digitalWrite(PINO_SERVO1, LOW);

    digitalWrite(PINO_SERVO2, HIGH);
    delayMicroseconds(larguraPulso2);
    digitalWrite(PINO_SERVO2, LOW);

    digitalWrite(PINO_SERVO3, HIGH);
    delayMicroseconds(larguraPulso3);
    digitalWrite(PINO_SERVO3, LOW);
    
    ultimoPulso = micros();
  }
}

// Substituto do delay() que mantém os servos recebendo pulso durante a espera.
// Use isso no lugar de delay() dentro de qualquer função bloqueante
// (calibração, reset da EEPROM, etc).
void delayComServo(unsigned long ms) {
  unsigned long inicio = millis();
  while (millis() - inicio < ms) {
    atualizarServosPWM();
  }
}
// ==============================================================================

void atualizarTelaOperacao() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MODO: SEPARACAO");
  lcd.setCursor(0, 1);
  lcd.print("Tot:");
  lcd.print(contagemTampas);
  lcd.print(" Lst:");
  lcd.print(ultimaTampaDetectada);
}

void lerRGB(long &r, long &g, long &b) {
  digitalWrite(S2, LOW);  digitalWrite(S3, LOW);  r = getFastFreq();
  digitalWrite(S2, HIGH); digitalWrite(S3, HIGH); g = getFastFreq();
  digitalWrite(S2, LOW);  digitalWrite(S3, HIGH); b = getFastFreq();
}

unsigned long getFastFreq() {
  while (FreqCount.available()) FreqCount.read(); 
  while (!FreqCount.available());                 
  return FreqCount.read();
}

void processarQueda() {
  float bRn, bGn, bBn, maxDiff = -1.0;
  unsigned long start = millis();

  while (millis() - start < 18) {
    long r, g, b;
    lerRGB(r, g, b);
    float total = r + g + b + 1;
    float rn = r/total, gn = g/total, bn = b/total;
    float diff = abs(rn - alvos[0].rn) + abs(gn - alvos[0].gn) + abs(bn - alvos[0].bn);
    
    if (diff > maxDiff) {
      maxDiff = diff; bRn = rn; bGn = gn; bBn = bn;
    }
  }

  if (maxDiff > 0.05) identificarCor(bRn, bGn, bBn);
}

void identificarCor(float rn, float gn, float bn) {
  int melhorMatch = -1;
  float menorDist = 999.0;

  for (int i = 1; i < 4; i++) {
    if (!alvos[i].configurada) continue;

    float dR = rn - alvos[i].rn;
    float dG = gn - alvos[i].gn;
    float dB = bn - alvos[i].bn;
    float dist = sqrt(dR*dR + dG*dG + dB*dB);

    if (dist < menorDist) {
      menorDist = dist; melhorMatch = i;
    }
  }

  if (melhorMatch != -1 && menorDist < 0.09) {
    moverServos(melhorMatch);
    ultimaTampaDetectada = "T" + String(melhorMatch);
    contagemTampas++;
  } else {
    moverServos(0); 
    ultimaTampaDetectada = "Desc";
  }
  
  // Atualiza rapidamente o visor após a detecção
  atualizarTelaOperacao();
}

void moverServos(int tipo) {
  switch (tipo) {
    case 1: larguraPulso1 = 2050; larguraPulso2 = 2100; break; 
    case 2: larguraPulso1 = 2050;  larguraPulso2 = 1570;  break; 
    case 3: larguraPulso1 = 1570; larguraPulso2 = 950; break; 
    default: larguraPulso1 = 1570; larguraPulso2 = 1570; break; 
  }
  
  if (tipo != 0) {
    tempoResetServo = millis() + 800;
    tempoResetServo2 = millis() + 1000;
  }
}

void executarResetEEPROM() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LIMPANDO EEPROM...");
  
  // Desconfigura na RAM e limpa os blocos correspondentes na EEPROM
  for(int i = 0; i < 4; i++) {
    alvos[i].configurada = false;
    alvos[i].rn = 0.0; alvos[i].gn = 0.0; alvos[i].bn = 0.0;
    alvos[i].rRaw = 0; alvos[i].gRaw = 0; alvos[i].bRaw = 0;
    EEPROM.put(i * sizeof(ColorProfile), alvos[i]);
  }
  
  delayComServo(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EEPROM Resetada!");
  delayComServo(1500);
}

void executarCalibracao(int idx) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CALIBRANDO:");
  lcd.setCursor(0, 1);
  lcd.print(nomes[idx]);
  delayComServo(1000);
  
  if (idx == 0) { // CALIBRAÇÃO DO TUBO
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lendo Tubo...");
    delayComServo(1000);
    long r, g, b; lerRGB(r, g, b);
    alvos[0].rRaw = r; alvos[0].gRaw = g; alvos[0].bRaw = b;
    float total = r + g + b + 1;
    alvos[0].rn = r/total; alvos[0].gn = g/total; alvos[0].bn = b/total;
    alvos[0].configurada = true;
  } 
  else { // CALIBRAÇÃO DAS TAMPAS (MÉDIA DE 5 QUEDAS)
    // Garante que o servo 3 vá para a posição de repouso (0 graus) durante a calibração
    larguraPulso3 = 530; 
    direcaoServo3 = true;
    
    float somaRn = 0, somaGn = 0, somaBn = 0;
    int coletas = 0;

    while (coletas < 5) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Solte tampa "); lcd.print(coletas + 1);
      lcd.setCursor(0, 1);
      lcd.print("Aguardando...   ");
      
      bool detectou = false;
      while (!detectou) {
        atualizarServosPWM(); // <- mantém o servo 3 travado em 530 enquanto espera a tampa
        long r, g, b; lerRGB(r, g, b);
        if (abs(r - alvos[0].rRaw) > threshold || abs(g - alvos[0].gRaw) > threshold || abs(b - alvos[0].bRaw) > threshold) {
          
          float pRn, pGn, pBn, pMax = -1.0;
          unsigned long s = millis();
          while(millis() - s < 50) { 
            atualizarServosPWM(); // <- idem durante a janela de captura da queda
            long rL, gL, bL; lerRGB(rL, gL, bL);
            float t = rL + gL + bL + 1;
            float rn = rL/t, gn = gL/t, bn = bL/t;
            float diff = abs(rn - alvos[0].rn) + abs(gn - alvos[0].gn) + abs(bn - alvos[0].bn);
            if(diff > pMax) { pMax = diff; pRn = rn; pGn = gn; pBn = bn; }
          }
          
          somaRn += pRn; somaGn += pGn; somaBn += pBn;
          coletas++;
          detectou = true;
          
          lcd.setCursor(0, 1);
          lcd.print("Capturado! OK.  ");
          delayComServo(1200);
        }
      }
    }

    alvos[idx].rn = somaRn / 5.0;
    alvos[idx].gn = somaGn / 5.0;
    alvos[idx].bn = somaBn / 5.0;
    alvos[idx].configurada = true;
  }

  if (alvos[idx].configurada) {
    EEPROM.put(idx * sizeof(ColorProfile), alvos[idx]);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Salvo na EEPROM!");
    delayComServo(1500);
  }
}

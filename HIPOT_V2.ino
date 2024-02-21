//=========== CONSTANTES E VARIÁVEIS ===========
const int buttonPin = 13;                         //Botão de Início da JIG de Hipot

const int Start1KV = 4;                           //Relé de Start1KV da máquina de Hipot
const int Start2KV = 5;                           //Relé de Start2kV da máquina de Hipot
const int relay1KV = 6;                           //Relé de ativação contatora de 1KV para Pima e Borne
const int relay2KV = 7;                           // Relé de ativação contatora de 2KV para Borne

const int fail = 2;                               // Entrada GND do Hipot quando o medidor falha no teste
const int pass = 3;                               // Entrada GND do Hipot quando o medidor passa no teste

int buttonStateAtual = HIGH;                      // Definindo a variável para armazenar o estado do botão atual
int buttonStateAnterior = HIGH;                   // Definindo a variável para armazenar o estado do botão anterior
int passState = 1;                                // Variavél booleana de aprovado
int failState = 1;                                // Variavél booleana de reprovado

unsigned long currentMillis = 0;                  // Variavél que armazena o tempo do início dos testes
unsigned long timerLastButton = millis();         // Variavél que armazena o tempo da última vez q o botão foi pressionado

//========= CONFIGURAÇÃO ENTRADAS/SAÍDAS =========
void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(pass, INPUT_PULLUP);
  pinMode(fail, INPUT_PULLUP);
  
  pinMode(Start2KV, OUTPUT);
  pinMode(Start1KV, OUTPUT);
  pinMode(relay1KV, OUTPUT);
  pinMode(relay2KV, OUTPUT);
  
  // Desligando os relés no início
  digitalWrite(relay1KV, LOW);
  digitalWrite(relay2KV, LOW);
  digitalWrite(Start2KV, LOW);
  digitalWrite(Start1KV, LOW);

}

//========= TESTE DE 2KV =========
void teste2KV(){
  digitalWrite(relay2KV, HIGH);                   // Acionamento da contactora responsável pelo 2KV
  delay(500);
  digitalWrite(Start2KV, HIGH);                   // Liga e desliga o relé, para apenas criar um pulso para a máquina de HIPOT de 2KV
  delay(500);
  digitalWrite(Start2KV, LOW);
}

//========= TESTE DE 1KV =========
void teste1KV(){
  digitalWrite(relay1KV, HIGH);                   // Acionamento da contactora responsável pelo 1KV
  delay(500);
  digitalWrite(Start1KV, HIGH);                    // Liga e desliga o relé, para apenas criar um pulso para a máquina de HIPOT de 2KV
  delay(500);
  digitalWrite(Start1KV, LOW);
}

//========= FUNÇÃO DE LOOP PARA ESPERA DA RESPOSTA DE PASS OU FAIL =========
void looppassfail(){
  currentMillis = millis();                       // Faz o armazenamento do tempo de início do teste
  while( millis() < currentMillis + 5000){        // Condição que fará rodar o teste até que o tempo atual millis, passe 5 segundos após o inicio de teste
    failState = digitalRead(fail);                // To do: fazer o arduino receber a falha e resetar o HIPOT após uma quantidade de tempo de alarme e parar os teste seguintes
    //Serial.print("failState:");
    //Serial.println(failState);
    /*if(failState == LOW){
      delay(1000);
      digitalWrite(ResetHP, HIGH);
      delay(200);
      digitalWrite(ResetHP, LOW);
      delay(100);
      digitalWrite(ResetHP, HIGH);
      delay(100);
      digitalWrite(ResetHP, LOW);
      //asm volatile("jmp 0");
    }
    passState = digitalRead(pass);
    //Serial.println(passState);
    if(passState == LOW){
      break;
    }*/
  }
}

void loop() {

  buttonStateAtual = digitalRead(buttonPin);      // Lendo o estado do botão

  if (buttonStateAtual == HIGH && buttonStateAnterior == HIGH) {  // Condição para saber se o botão não foi precionado
    timerLastButton = millis();                   // Caso não seja precionado, o estado de não precionado recebe um novo tempo atual
  }
  
  if (buttonStateAtual == LOW && buttonStateAnterior == HIGH && (millis() - timerLastButton) >= 250) {  //Além da lógica do aperto do botão ...
    teste2KV();                                   //... foi adcionado um tempo, onde precisasse segurar por pouco tempo o botão ...
    looppassfail();                               //... para evitar ativações por ruídos
    digitalWrite(relay2KV, LOW);                  // Após o tempo de teste, desliga a contatora de 2KV
    delay(200);
    //fim do teste de 2KV

    teste1KV();                                   // Função que pulsa o botão de start do HIPOT de 1KV e liga a contatora
    looppassfail();                               // Contagem do tempo de teste
    digitalWrite(relay1KV, LOW);                  // Após o tempo de teste, desliga a contatora de 1KV
    //fim do teste de 1kV
    
  } else {
    digitalWrite(relay1KV, LOW);                  // Mantém desligado as contatoras enquanto nao for precionado o botão 
    digitalWrite(relay2KV, LOW);
  }
  if (buttonStateAtual == HIGH){                  // Atualização do estado do botão anterior para termos a lógica de rampa de subida
    buttonStateAnterior = buttonStateAtual;
    timerLastButton = millis();
  }
}

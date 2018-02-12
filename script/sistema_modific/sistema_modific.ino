/*
##################################################
#
#	Arquivo: Sistema.ino
#	Micro-processador: Arduino Mega 2560
#	Objetivo:
#                + Aplicaço do microprocessador citado a automaçao predial
#                      - Fechadura eletrica com abertura RFID, comunicaçao via LCD e status por LED
#                      - Alarme com sensor de PIR, teclado matricial para desativaçao, LEDs de status e buzzer para disparo de segurança
#                      - Banheiro com luz automatizada com sensor PIR
#                      - Temperatura controlada com sensor DHT 11, automatizando um aquecedor e um arcondicionado.
#    Autor: Cleber Júnior (DwCleb)
#
#################################################
*/
//Inclusao das bibliotecas
#include <SPI.h>
#include <MFRC522.h>
#include<LiquidCrystal_I2C.h>
#include<Wire.h>
#include <Keypad.h>
#include <DHT.h>


// PORTA

#define SS_PIN 53 //define o pino 53 como entrada de dados sdk
#define RST_PIN 41 //define o pino 41 como entrada de dados reset
  MFRC522 mfrc522(SS_PIN, RST_PIN);   //Cria a instacia do MFRC522 
  LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);//define o LCD para transmiçao serial
  char st[20];// declara uma variavel st de tamanho 20 como caractere
  int buzzer = 6;// define a saida do buzer como pino 6
  int led_status = 5; //define pino 5 para led status
  int trava = 8;//trava definida como pino 8
  
  //Alarme
  char* senha = "2580"; //senha para acesso
  int position = 0; //variavel posiçao com valor 0
  int erro = 0; //variavel erro com valor 0
  const byte LINHAS = 4; //definindo num linhas
  const byte COLUNAS = 3; // definindo num colunas
  char teclas[LINHAS][COLUNAS] = { //mapeando teclado
                                 {'1','2','3'},
                                 {'4','5','6'},
                                 {'7','8','9'},
                                 {'*','0','#'}
  };
  byte pinoLinha[LINHAS] = {31,32,33,34}; // pinos referentes as linhas
  byte pinoColuna[COLUNAS] = {28,29,30}; //pinos referentes as colunas
  Keypad keypad = Keypad(makeKeymap(teclas), pinoLinha, pinoColuna, LINHAS, COLUNAS); //teclado configurado
  int desativado_alarme = 38; //define pino 38 para led vermelho do alarme
  int ativado_alarme = 39; //define pino 39 para led verde do alarme
  #define tempo 10 // definindo tempo com valor 10
  int sirene = 49; // sirene em pino 49
  int sensor_alarme = 37; // sensor PIR do alarme em pino 22
  int estado_sensor_alarme;//define a variavel estado_sensor_alarme como inteira
  int periodo_alarme;//define a variavel periodo_alarme como inteira
  int estado_desativado_alarme;//define a variavel estado_desativado_alarme como inteira
  int energia = 42; // pino 42 ligado ao relay para liberar a energia do escritorio
  int energia1 = 24;// pino 24 ligado ao relay para liberar a energia para a luz do escritorio
  int periodoOn; //define a variavel periodoON como inteira
  int estado_alarme;//define a variavel estado_alarme como inteira
  int estado_sirene;//define a variavel estado_sirene como inteira
   int count = 0; // define a variavel com o valor 0
   
   
  //REFRIGERAÇAO
  
  #define DHTPIN A1 // sensor de temperatura em pino A1
  #define DHTTYPE DHT11 // DHT 11
  /*
  INSTRUÇÕES DO SENSOR DE TEMPERATURA
  // Conecte pino 1 do sensor (esquerda) ao +5V
  // Conecte pino 2 do sensor ao pino de dados definido em seu Arduino
  // Conecte pino 4 do sensor ao GND
  */
  DHT dht(DHTPIN, DHTTYPE); //definiçao do sensor, a partir de sua biblioteca
  int ar_condicionado = 26;// define o pino 26 para ar_condicionado
  int aquecedor = 25; //define o pino 25 para o aquecedor
  int arc = 0; // variavel para auxiliar estado()
  int aq = 0;  // variavel para auxiliar estado()
 
  // BANHEIRO
  
  int  periodo = 0;  // declara a variavel periodo como inteira e com valor 0
  int  luz =  22 ; // declara o pino 22 como luz, luz do banheiro
  int estado_luz;     // declara a variavel estado_luz como inteira
  int sensor_wc = 23 ; // declara o pino 23 como sensor_wc, sensor PIR do banheiro
  int estado_sensor_wc;   //declara a variavel estado_sensor_wc como inteira
  int periodo_wc; // delara a variavel periodo_wc como inteira
  int on = 0; // declara a variavel on como inteira e define seu valor a 0

  //ESTADO SISTEMA
  
  int periodo_estado = 0;// variavel usada como temporizador para emitir status do sistema, inicialmente em valor 0
  
 void setup() //FUNÇÃO SETUP, PARA CONFIGURAR PINOS e variaveis
{
    
    //Porta
  pinMode(led_status, OUTPUT); // define o LED verde como saida
  pinMode(trava, OUTPUT); // define o pino da trava como saida
  Serial.begin(9600);   // Inicia a serial
  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522   
  pinMode (buzzer, OUTPUT);// define buzzer como saida
  lcd.begin(16,2);// define 16 colunas e 2 linhas para o lcd
    
    //Alarme
  pinMode(desativado_alarme, OUTPUT);//led vermelho como saida 
  pinMode(ativado_alarme, OUTPUT);//led verde como saida
  fechadoOk(true);// informa o que a funçao fechadoOk tem o parametro TRUE
  pinMode(sirene,OUTPUT); //pino da sirene como saida
  pinMode(sensor_alarme, INPUT); //pino do alarme como saida
  pinMode(energia, OUTPUT); // pino que fornece a energia para o escritorio
  pinMode(energia1, OUTPUT);// pino que fornece a energia da lampada do escritorio
  estado_alarme = HIGH; //Define a variavel como HIGH
  periodo_alarme =LOW; 
  periodoOn =LOW;  
   //refrigeraçao
  pinMode(ar_condicionado, OUTPUT); // define a varivael ar condicionado como saida
  pinMode(aquecedor, OUTPUT); // define a variavel aquecedor como saida
  dht.begin(); // define a funçao dht para inicializar
    
    //Banheiro
  pinMode(luz, OUTPUT); // define a varivael luz como pino de saída
  pinMode(sensor_wc, INPUT); // define a variavel sensor_wc como entrada, sensor PIR 
  digitalWrite(ar_condicionado, LOW); // desliga o ar condicionado
  digitalWrite(aquecedor, LOW); // deliga o aquecedor
        

 //chamada de funções
  mensageminicial(); //Funcao para emitir mensagem  inicial no LCD
  desativar();// Funcao para desativar os atuadores, para comecarem no estado inicial, desligados

}

 void loop()//Funcao LOOP()
 {
  mensageminicial();//chama a funçao mensageminicial
  porta();//chama a funcao porta()
  alarme();//chama a funcao alarme()
  delay(30);// delay de 50 ms
  if ( periodo_estado == 50){ //Condição para o monitoramento do Loop()
      Serial.println("Fim Loop");// emite no monitor da IDE a seguinte mensagem citada
  }
  if ( periodo_estado == 110){// condição para o monitoramento do sistema
      estado();//Chama a funcao Estado()
  }
periodo_estado ++; //incrementação na variavel para ser usada como temporizador para monitoramento do sistema
 } 
 
 void porta() //Funcao PORTA 
{
  if ( periodo_estado == 50){//Condição para o monitoramento do Porta()
      Serial.println("Porta");// emite no monitor da IDE a seguinte mensagem citada
  }
   // Look for new cards
   if ( ! mfrc522.PICC_IsNewCardPresent()) //se nao detectar o cartao
   {
   return;//retorna para o inicio
   }
    // Seleciona o cartao para a leitura
  if ( ! mfrc522.PICC_ReadCardSerial()) // se o cartao nao tiver serial
  {
   return;// retorna ao inicio
  }
  String conteudo= ""; //define "conteudo  como string
  byte letra; // define "letra" como Byte
  for (byte i = 0; i < mfrc522.uid.size; i++) // laço de repetiçao, para i de 1 ate o tamanho da tag, incremento em i.
  {
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));//recolhe a tag do cartão
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));//transforma a tag para hexadecimal
  }
  conteudo.toUpperCase();//chama a funçao que esta na biblioteca do RFID
  if (conteudo.substring(1) == "F3 55 D8 00" || conteudo.substring(1) == "92 2F DD 76") //se as tags forem uma das citadas
    {
     autorizado();//chama a funçao autorizado
    }
  else{//senao
     negado();//chama a funçao negado
      }
  mensageminicial();//chama a funçao mensageminicial
} 
 
 void mensageminicial()//Funcao Mensagem Inicial()
{
  lcd.clear();//Limpa o LCD
  lcd.setCursor(0,0);// cursor do LCD na linha 1
  lcd.print(" Aproxime o seu"); // emite a frase apresentada no LCD 
  lcd.setCursor(0,1);// cursor do LCD na linha 2
  lcd.print("cartao do leitor");  // emite a frase apresentada no LCD
}
void autorizado() //Funcao Autorizado()
{
  lcd.clear();// limpa o LCD
  lcd.setCursor(0,0);// cursor do LCD na linha 1
  lcd.print("     Acesso ");// emite a frase apresentada no LCD
  lcd.setCursor(0,1);// cursor do LCD na linha 2
  lcd.print("   AUTORIZADO"); // emite a frase apresentada no LCD
  Serial.println("Acesso Autorizado");// emite a frase apresentada no LCD
  Serial.println("");//pula uma linha no serial da IDE
  digitalWrite(trava, HIGH);// manda energia para o pino da trava da fechadura
  tone(buzzer, 7000);// emite uma sonoridade em 7000Mhz no buzzer
  delay(300);//espera 300 ms
  tone(buzzer, 4500);// emite uma sonoridade em 4500Mhz no buzzer
  delay(200);//espera 200 ms
  noTone(buzzer);// cancela a sonoridade no buzzer
  delay(400);//espera 400 ms
  digitalWrite(trava, LOW);// manda energia para o pino da trava
  mensageminicial();//Chama a Funcao Mensagem Inicial()
}

 void negado()//Funcao Negado()
{
  lcd.clear();// limpa o LCD
  lcd.setCursor(0,0);// cursor do LCD na linha 1
  lcd.print("     Acesso ");// emite a frase apresentada no LCD
  lcd.setCursor(0,1);// cursor do LCD na linha 2
  lcd.print("     NEGADO"); // emite a frase apresentada no LCD
  Serial.println(" Acesso Negado");// emite a frase apresentada no LCD
  Serial.println("");//pula linha o serial da IDE
  digitalWrite(led_status, HIGH);//acede led de status de acesso
  digitalWrite(led_status, LOW);//apaga led de status de acesso
  digitalWrite(led_status, HIGH);//acede led de status de acesso
  digitalWrite(led_status, LOW);//apaga led de status de acesso
  digitalWrite(led_status, HIGH);//acede led de status de acesso
  digitalWrite(led_status, LOW);//apaga led de status de acesso
  tone(buzzer, 4000);// emite uma sonoridade em 4000Mhz no buzzer
  delay(300);//espera 300 ms
  tone(buzzer, 1500);// emite uma sonoridade em 1500Mhz no buzzer
  delay(200);//espera 200 ms
  noTone(buzzer); // cancela a sonoridade no buzzer
  delay(400);//espera 400 ms
   mensageminicial();//chama a funçao mensageminicial
}
  
  //ALARME 
 
  void alarme()// Funcao Alarme()
{
  
  cont_alarme++;
  if ( periodo_estado == 50)//Condição para o monitoramento do Alarme()
  {
      Serial.println("Alarme");// emite no monitor da IDE a seguinte mensagem citada
  }
  if(estado_alarme == HIGH) //Se a variavel estiver em estado 1. Configurada em inicialmente ligada em Setup() . Desligada quando a senha de segurança for obtida
    { 
      
      
      if ( cont_alarme > 5){
      estado_sensor_alarme = digitalRead(sensor_alarme); //a variavel estado_sensor_alarme armazena a leitura do sensor
      if( estado_sensor_alarme == HIGH) // se estado_sensor_alarme estiver ligado, estado em 1
        { 
          periodoOn = HIGH; // periodoOn recebera HIGH, para que o temporizador seja iniciado
        }
      }
    }
  if(periodoOn == HIGH)// se periodoOn for igual a HIGH, se sensor detectou presença
    { 
      periodo_alarme = periodo_alarme + 1; // perido_alarme e incrementado, usado como temporizador
  }
  if(periodo_alarme > 130) // se periodo_alarme for maior que 130, tempo estimado para se digitar a senha do alarme, o alarme é soado
    { 
      periodoOn = LOW; // periodoOn recebe LOW
      periodo_alarme = 0; // periodo_alarme recebe o valor 0
      Sirene(); //chama a funçao Sirene()
  }
  if( estado_alarme == LOW)// Caso a alarme tenha sido desativado
    { 
      periodoOn = LOW; // estado periodoOn recebe LOW
      periodo_alarme = 0; //periodo_alarme recebe valor 0
      wc();//chama a funçao wc()
      refrigeracao();//chama a funçao refrigeraçao
    }
  if(estado_sirene == HIGH)//se estado da sirene for igual a HIGH
    { 
      periodo_alarme = periodo_alarme + 1; //periodo do alarme incrementado
  }
    
    // Verificaçao da senha...
    
  char key = keypad.getKey();// key, varivel do tipo Char, recebe um comando da biblioteca do teclado, referente a senha
  if (key == '#' || key == '*'){ // se o que foi digitado for * ou #
      position = 0; //recebe a posiçao 0
      fechadoOk(true); // o alarme e ativado
      desativar(); //chama a Funcao desativar() para desligar toda a energia
    }
  if (key == senha[position]){// se o numero digitado for um dos numeros pertecentes a senha
      position ++; // posiçao recebe incremento
    }
  if(key != senha[position]){// se o numero digitado for diferente dos numeros pertecentes a senha
      erro ++; // erro recebe incremento
  }
  if(erro == 2){ //se a pessoa errar 2 numeros da senha
      Sirene(); // Funcao Sirene() e chamada
  }
  if (position == 4){ //se a posiçao chegar a 4
      fechadoOk(false); // o alarme e desativado  
      digitalWrite(sirene, LOW);// Desliga a sirene
    }
}

void fechadoOk(int fechado)//Funcao fechadoOK() com paramentro inteiro fechado 
{
  if (fechado){ //se fechado for TRUE, verdade, se o alarme for desativado
     digitalWrite(ativado_alarme, HIGH); // a led verde e acessa idicando que a alarme esta ligado
     desativar();//chama a funcao desativar() para desligar tudo no escritorio
     estado_alarme = HIGH; // a variavel estado do alarme recebe HIGH == ON
     digitalWrite(sirene, HIGH);//liga a sirene
     delay(200);//espera 200ms
     digitalWrite(sirene,LOW);//desliga a sirene
     delay(100);//espera 100ms
     digitalWrite(sirene, HIGH);//liga a sirene
     delay(100);//espera 100ms
     digitalWrite(sirene, LOW);//desliga a sirene
     /*tone(buzzer_a, 1000); // emite uma sonoridade a 5000 Mhz no buzzer
     delay(300);// espera 300 milesegundos
     tone(buzzer_a, 1300);// emite uma sonoridade a 3000 Mhz no buzzer
     delay(200);// espera 200 milesegundos
     noTone(buzzer_a);// indica que o buzzer nao tocara mais
     */
  }
  else{ //se a senha for digitada corretamente, desativa o alarme
      digitalWrite(ativado_alarme, LOW);// led verde, que indica alarme ativado, e desligada
      digitalWrite(energia, HIGH);//libera a energia do escritorio (desliga o relay)
      digitalWrite(energia1, HIGH);//liga a luz do escritorio (desliga o relay)
      estado_alarme = LOW; // estado do alarme recebe LOW == OFF, desligado
      estado_sirene = LOW; // estado da sirene recebe LOW == OF, desligado
      digitalWrite(desativado_alarme, HIGH);// a led vermelha, que indica que o alarme esta desativado, e acessa
      digitalWrite(sirene, LOW);//liga a sirene
      delay(200);//espera 200ms
      digitalWrite(sirene,HIGH);//liga a sirene
      delay(100);//espera 100ms
      digitalWrite(sirene, LOW);//liga a sirene
      delay(200);//espera 200ms
      digitalWrite(sirene, HIGH);//liga a sirene
      delay(100);//espera 100ms
      digitalWrite(sirene, LOW);//liga a sirene
      /*
      tone(buzzer_a, 1300); // emite uma sonoridade a 5000 Mhz no buzzer
      delay(300);// espera 300 milesegundos
      tone(buzzer_a, 1000);// emite uma sonoridade a 3000 Mhz no buzzer
      delay(200);// espera 200 milesegundos
      noTone(buzzer_a);// indica que o buzzer nao tocara mais
      */ 
    }
}

void Sirene(){// funçao da sirene
 digitalWrite(sirene, HIGH);//Liga sirene
 estado_sirene = HIGH; //estado sa sirene recebe HIGH == ON, para auxiliar em Alarme()
}

//refrigeraçao

void refrigeracao() // funçao para a refrigeraçao
{
  if ( periodo_estado == 50){//condicao para monitorar a funcao Refrigeracao(), incrementada em Loop()
      Serial.println("Refrigracao");// emite no monitor da IDE a seguinte mensagem citada
  }
  float t = dht.readTemperature(); // variavel t armazena o valor da temperatura lido pelo sensor
  if (isnan(t))// testa se retorno é valido, caso contrário algo está errado.
     {
      Serial.println("Algo de errado com o sensor DHT");//Emite no monitor da IDE a frase apresentada  
     } 
  if ( t > 24){ //se o valor da temperatura for maior que 24 gruas celsius
     digitalWrite(ar_condicionado, HIGH); // liga o cooler, ar condicionado
      arc = 1;//variavel para auxiliar na funcao estado()
}
  else{//senao
      if ( t >= 22 && t <= 23 ) {// se a temperatura estiver entre 22 e 23 gruas celsius
        digitalWrite(ar_condicionado, LOW); // desliga o ar condicionado
        digitalWrite(aquecedor, LOW); // deliga o aquecedor
        arc = 0;//variavel para auxiliar na funcao estado()
        aq = 0;//variavel para auxiliar na funcao estado()
         }
      if ( t < 21){//se o valor da temperatura for menor que 21
          digitalWrite(aquecedor, HIGH); // liga o aquecedor
          aq = 1;//variavel para auxiliar na funcao estado()
        }
      }
      /*
      Minesterio da sade aconselha que deve-se regular a temperatura do ar condicionado
      entre 21 a 24 graus. Segundo isso, o programa foi ajustado para
      que esse conselho seja seguido e a saude de quem utilizar a sala
      seja preservada.
      */
}

// BANHEIRO

void wc()//Funcao Banheiro
{ 
  if ( periodo_estado == 50){//condicao para monitorar a funcao Wc(), incrementada em Loop()
      Serial.println("Banheiro");// emite no monitor da IDE a seguinte mensagem citada
  }
  if( on == 1){ //se a variavel luz for diferente de desligada
      periodo_wc = periodo_wc + 1; //periodo do wc  incrementado
      if( periodo_wc > 300){ // se o periodo for mais que 300, temporizador
         digitalWrite(luz, LOW); //desliga a luz
         on = 0;//Variavel para auxiliar em Estado()
         periodo_wc = 0; //periodo do wc vai receber 
         }
     }
  estado_sensor_wc = digitalRead(sensor_wc); //uma variavel vai guardar a leitura do sensor do WC
  if( estado_sensor_wc == HIGH ){ // se o sensor for ligado
      digitalWrite(luz, HIGH);//luz acende
      on = 1;//Variavel para auxiliar em Estado()
      periodo_wc = 0;// periodo do wc recebe 0
     }
}
void desativar(){ // Funcao desativar, usada para interromper a energia dos atuadores, desliga-lo
     digitalWrite(desativado_alarme, LOW); // a led vermemlha fica apagada
     digitalWrite(energia, LOW);//interrompe a energia do escritorio (desliga o relay)
     digitalWrite(energia1, LOW);//deliga a luz do escritorio (desliga o relay)
     digitalWrite(ar_condicionado, LOW); // desliga o ar condicionado
     digitalWrite(aquecedor, LOW); // deliga o aquecedor
     digitalWrite(luz, LOW); //desliga a luz do banheiro
}
void estado()//Funcao estado(), para o monitoramento de todo o sistema
{
  Serial.println("+ = = = = = ESTADO GERAL DO SISTEMA = = = = = +");//Emite a mensagem entre "..." no monitor do IDE
  Serial.print("");// usado para pular linha no monitor serial da IDE 
  //sala
  Serial.println("Status da Sala");//Emite a mensagem entre "..." no monitor do IDE
  Serial.print("Estado da energia: ");//Emite a mensagem entre "..." no monitor do IDE e ao lado apresenta a condição a baixo
  if (estado_alarme == LOW)//se a variavel estiver em estado 0. Se o alarme estiver desligado
  {
    Serial.println("Liberada!");//Emite a mensagem entre "..." no monitor do IDE ao lado da anterior
  }
  else//senao
     {
      Serial.println("Restringida");//Emite a mensagem entre "..." no monitor do IDE
     }
  Serial.print("Luz: ");//Emite a mensagem entre "..." no monitor do IDE
  if (estado_alarme == LOW)// se o alarme estiver desligado. A variavel estiver como valor 0.
  {
    Serial.println("Ligada!");//Emite a mensagem entre "..." no monitor do IDE
  }else//senao
  {
      Serial.println("Desativada");//Emite a mensagem entre "..." no monitor do IDE
    }
  Serial.print("Seguranca: ");//Emite a mensagem entre "..." no monitor do IDE
  if ( estado_alarme == HIGH)//se a variavel estiver em estado 1. Se o alarme estiver ligado
  {
    Serial.println("Ativada");//Emite a mensagem entre "..." no monitor do IDE
  }
    else{//senao
      Serial.println("Desativada");//Emite a mensagem entre "..." no monitor do IDE
    }
  Serial.println("=============================================");//Emite a mensagem entre "..." no monitor do IDE
   //Alarme
  Serial.println("Status do Alarme");//Emite a mensagem entre "..." no monitor do IDE
  Serial.print("Estado: ");//Emite a mensagem entre "..." no monitor do IDE
  if ( estado_alarme == HIGH){//se a variavel estiver em estado 1. Se o alarme estiver ligado
    Serial.println("Ativado");//Emite a mensagem entre "..." no monitor do IDE
  }
    else{//senao
      Serial.println("Desativado");//Emite a mensagem entre "..." no monitor do IDE
    }
  Serial.print("Periodo: ");//Emite a mensagem entre "..." no monitor do IDE
  Serial.println(periodo_alarme);//Emite o valor da variavel entre  no monitor do IDE
  Serial.print("Status do sensor: ");//Emite a mensagem entre "..." no monitor do IDE
  Serial.println(estado_sensor_alarme);//Emite o valor da variavel no monitor do IDE
  Serial.print("Estado Sirene: ");//Emite a mensagem entre "..." no monitor do IDE
  Serial.println(estado_sirene); //Emite o valor da variavel no monitor do IDE
  Serial.println("=============================================");//Emite a mensagem entre "..." no monitor do IDE
  
  //Banheiro
 Serial.println("Status do Banheiro");//Emite a mensagem entre "..." no monitor do IDE
  Serial.print("Estado Luz:  ");//Emite a mensagem entre "..." no monitor do IDE
  Serial.println(on);//Emite o valor da variavel no monitor do IDE
  Serial.println("Tempo decorrido");//Emite a mensagem entre "..." no monitor do IDE
  Serial.println(periodo_wc);//Emite o valor da variavel no monitor do IDE
  Serial.print("Status do sensor: ");//Emite a mensagem entre "..." no monitor do IDE
  Serial.println(estado_sensor_wc);//Emite o valor da variavel no monitor do IDE
  Serial.println("=============================================");//Emite a mensagem entre "..." no monitor do IDE
  
  //refrigeraçao
   Serial.println("Status Refrigeracao");//Emite a mensagem entre "..." no monitor do IDE
   float t = dht.readTemperature(); // variavel t armazena o valor da temperatura lido pelo sensor
   Serial.print("Temperatura: ");//Emite no monitor da IDE a frase apresentada
   Serial.print(t); // Emite no monitor da IDE o valor da varivavel
   Serial.println("  Graus Celsius"); // Emite no monitor da IDE a frase apresentada, com quebra de linha "ln"
   Serial.print("Componente Ligado:  ");//Emite a mensagem entre "..." no monitor do IDE
       if ((aq == 1) && (arc == 0))//se o aquecedor estiver ligado e o ar estiver ligado
       {
        Serial.println("Aquecedor");//Emite a mensagem entre "..." no monitor do IDE
     }
       else//senao
       {
           if ((aq == 0) && (arc == 1))// se o aquecedor estiver desligado e o ar estiver ligado
           {
               Serial.println("Ar condicionado ");//Emite a mensagem entre "..." no monitor do IDE
       }
         else //senao
         {
       Serial.println("Nenhum componente de refrigeracao esta ativado. ");//Emite a mensagem entre "..." no monitor do IDE
      }
       }
 Serial.println("=============================================");//Emite a mensagem entre "..." no monitor do IDE
 periodo_estado = 0;// a variavel volta a ter o valor 0
}

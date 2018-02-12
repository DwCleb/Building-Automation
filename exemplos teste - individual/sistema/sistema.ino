/*
##################################################
 #
 #	Arquivo: Sistema.ino
 #	Micro-processador: Arduino Mega 2560
 #	Objetivo:
 #                + Aplicaço do microprocessador citado a automaçao predial
 #                   - Fechadura eletrica com abertura RFID, comunicaçao via LCD e status por LED
 #                   - Alarme com sensor de PIR, teclado matricial para desativaçao, LEDs de status e buzzer para disparo de segurança
 #                   - Banheiro com luz automatizada com sensor PIR
 #                   - Temperatura controlada com sensor DHT 11, automatizando um aquecedor e um arcondicionado.
 #       Autor: Cleber Júnior (DwCleb)
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
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);//define o LCD para transmiçao serial
char st[20];// declara uma variavel st de tamanho 20 como caractere
int buzzer = 6;// define a saida do buzer como 7
int led_status = 5; //led de status liberado
int trava = 8;//trava definida como pino 8

//Alarme
char* senha = "2580";//senha para acesso
int position = 0; //char posiçao 0
const byte LINHAS = 4; //definindo linhas
const byte COLUNAS = 3; // definindo colunas
char teclas[LINHAS][COLUNAS] = { //mapeando teclado
  {
    '1','2','3'  }
  ,
  {
    '4','5','6'  }
  ,
  {
    '7','8','9'  }
  ,
  {
    '*','0','#'  }
};
byte pinoLinha[LINHAS] = {
  31,32,33,34}; // pinos referentes as linhas
byte pinoColuna[COLUNAS] = {
  28,29,30}; //pinos referentes as colunas
Keypad keypad = Keypad(makeKeymap(teclas), pinoLinha, pinoColuna, LINHAS, COLUNAS); //teclado configurado
int desativado_alarme = 38; //estado desativado do alarme
int ativado_alarme = 39; //estado ativado do alarme
#define tempo 10 // definindo tempo = 10
int frequencia = 0; // frenquencia em valor 0
int sirene = 49; // pino do buzzer em n 17
int sensor_alarme = 37; // pino do sensor em n 22
int estado_sensor_alarme;//define a variavel estado_sensor_alarme como inteira
int periodo_alarme;//define a variavel periodo_alarme como inteira
int estado_desativado_alarme;//define a variavel estado_desativado_alarme como inteira
int energia = 42; // pino ligado ao relay para liberar a energia do escritorio
int energia1 = 24;// pino ligado ao relay para liberar a energia para a luz do escritorio
int periodoOn; //define a variavel periodoON como inteira
int estado_alarme;//define a variavel estado_alarme como inteira
int estado_sirene;//define a variavel estado_sirene como inteira
int count = 0; 
//REFRIGERAÇAO

#define DHTPIN A1 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11
// Conecte pino 1 do sensor (esquerda) ao +5V
// Conecte pino 2 do sensor ao pino de dados definido em seu Arduino
// Conecte pino 4 do sensor ao GND
DHT dht(DHTPIN, DHTTYPE); //definiçao do sensor, a partir de sua biblioteca
int ar_condicionado = 26;// define a variavel ar_condicionado como inteira e sua saida a 26
int ar = 44; // define a saida 26 para o arcondicionado
int aquecedor = 25; //define a saida 25 para o aquecedor
int periodo_dht;// declara a variavell periodo como inteira
int arc = 0;
int aq = 0;  
// BANHEIRO

int  periodo = 0;  // declara a variavel periodo como inteira
int  luz =  22 ; // declara o pino 22 como luz
int estado_luz;     // declara a cariavel estado_luz como inteira
int sensor_wc = 23 ; // declara o pino 23 como sensor_wc
int estado_sensor_wc;   //declara a variavel estado_sensor_wc como inteira
int periodo_wc; // delara a variavel periodo_wc como inteira
int on = 0; // declara a variavel on como inteira e define seu valor a 0

//ESTADO SISTEMA
int periodo_estado = 0;

void setup() 
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
  pinMode(ativado_alarme, OUTPUT);//led vermelho como saida
  fechadoOk(true);// informa o que a funçao fechadoOk tem o parametro TRUE
  pinMode(sirene,OUTPUT); //Pino do buzzer do alarme
  pinMode(sensor_alarme, INPUT); //pino do alarme como saida
  pinMode(energia, OUTPUT); // pino que fornecera a energia para o escritorio
  pinMode(energia1, OUTPUT);// pino que fornecera a energia da lampada do escritorio
  estado_alarme = HIGH; // define o alarme ligado
  //refrigeraçao
  pinMode(ar_condicionado, OUTPUT); // define a varivael ar condicionado como saida
  pinMode(aquecedor, OUTPUT); // define a variavel aquecedor como saida
  dht.begin(); // define a funçao dht para inicializar

  //BAnheiro
  pinMode(luz, OUTPUT); // define a varivael luz como pino de saída
  pinMode(sensor_wc, INPUT); // define a variavel sensor_wc como entrada
  digitalWrite(luz, LOW); //desliga a luz  
  mensageminicial();
  digitalWrite(led_status, LOW);
}

void loop(){
  mensageminicial();//chama a funçao mensageminicial
  porta();//chama a porta
  alarme();//chama a funçao alarme
  delay(30);// delay de 50 ms
  if ( periodo_estado == 50){
    Serial.println("Fim Loop");// emite no monitor da IDE a seguinte mensagem citada
  }
  if ( periodo_estado == 110){
    estado();
  }
  periodo_estado ++;
} 
void porta() 
{
  if ( periodo_estado == 50){
    Serial.println("Porta");// emite no monitor da IDE a seguinte mensagem citada
  }
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) //se nao detectar o cartao
  {
    return;//retorna para o inicio
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) // se o cartao nao tiver serial
  {
    return;// retorna ao inicio
  }
  String conteudo= ""; //define "conteudo ="" " como string
  byte letra; // define "letra" como Byte
  for (byte i = 0; i < mfrc522.uid.size; i++) // laço de repetiçao, para i de 1 ate o tamanho da serial, incremento em i.
  {
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));//recolhe a serial
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));//transforma a serial para hexadecimal
  }
  conteudo.toUpperCase();//chama a funçao que esta na biblioteca do RFID
  if (conteudo.substring(1) == "F3 55 D8 00" || conteudo.substring(1) == "92 2F DD 76") //se as seriais forem algumas das citadas
  {
    autorizado();//chama a funçao autorizado
  }
  else{//senao
    negado();//chama a funçao negado
  }
  mensageminicial();//chama a funçao mensageminicial
} 

void mensageminicial()
{
  lcd.clear();//Limpa o LCD
  lcd.setCursor(0,0);
  lcd.print(" Aproxime o seu"); // emite a frase apresentada no LCD 
  lcd.setCursor(0,1);// cursor do LCD na linha 1
  lcd.print("cartao do leitor");  // emite a frase apresentada no LCD
}
void autorizado()
{
  lcd.clear();// limpa o LCD
  lcd.setCursor(0,0);// cursor do LCD na linha 1
  lcd.print("     Acesso ");// emite a frase apresentada no LCD
  lcd.setCursor(0,1);// cursor do LCD na linha 2
  lcd.print("   AUTORIZADO"); // emite a frase apresentada no LCD
  Serial.println("Acesso Autorizado");
  Serial.println("");
  digitalWrite(trava, HIGH);// manda energia para o pino da trava
  delay(200);// espera 200ms
  digitalWrite(trava, LOW);//encerra a energia mandada para o pino da trava
  tone(buzzer, 7000);// emite uma sonoridade em 7000Mhz no buzzer
  delay(200);//espera 300 ms
  tone(buzzer, 4500);// emite uma sonoridade em 4500Mhz no buzzer
  delay(100);//espera 200 ms
  noTone(buzzer);// cancela a sonoridade no buzzer
  digitalWrite(led_status, HIGH);
  delay(100);
  digitalWrite(led_status, LOW);
  delay(100);
  digitalWrite(led_status, HIGH);
  delay(100);
  digitalWrite(led_status, LOW);
  mensageminicial();


}

void negado()
{
  lcd.clear();// limpa o LCD
  lcd.setCursor(0,0);// cursor do LCD na linha 1
  lcd.print("     Acesso ");// emite a frase apresentada no LCD
  lcd.setCursor(0,1);// cursor do LCD na linha 2
  lcd.print("     NEGADO"); // emite a frase apresentada no LCD
  Serial.println(" Acesso Negado");
  Serial.println("");
  digitalWrite(led_status, HIGH);
  delay(100);
  digitalWrite(led_status, LOW);
  delay(100);
  digitalWrite(led_status, HIGH);
  delay(100);
  digitalWrite(led_status, LOW);
  tone(buzzer, 4000);// emite uma sonoridade em 4000Mhz no buzzer
  delay(200);//espera 300 ms
  tone(buzzer, 1500);// emite uma sonoridade em 1500Mhz no buzzer
  delay(100);//espera 200 ms
  noTone(buzzer); // cancela a sonoridade no buzzer
  delay(50);//espera 400 ms
  mensageminicial();//chama a funçao mensageminicial
}


//ALARME 


void alarme()// funçao alarme
{
  if ( periodo_estado == 50){
    Serial.println("Alarme");// emite no monitor da IDE a seguinte mensagem citada
  }
  if(estado_alarme == HIGH) // se estado_alarme estiver liga, estado em 1.
  { // se o estado do alarme estiver recebido HIGH
    estado_sensor_alarme = digitalRead(sensor_alarme); //a variavel estado_sensor_alarme armazena a leitura do sensor
    if( estado_sensor_alarme == HIGH) // se estado_sensor_alarme estiver ligado, estado em 1
    { //se o sensor apresentar sinal HIGH
      periodoOn = HIGH; // periodoOn recebera HIGH
    }
  }
  if(periodoOn == HIGH)
  { 
    ;// se periodoOn for igual a HIGH
    periodo_alarme = periodo_alarme + 1; // perido_alarme e incrementado
    digitalWrite(luz, LOW);
  }
  if(periodo_alarme > 130) 
  { // se periodo_alarme for maior que 250
    periodoOn = LOW; // periodoOn recebe LOW
    periodo_alarme = 0; // periodo_alarme = 0<
    Sirene(); //chama a funçao sirene
  }
  if( estado_alarme == LOW)
  { // se estado_alarme for igual a LOW
    periodoOn = LOW; // estado periodoOn recebe LOW
    periodo_alarme = 0; //periodo_alarme recebe 0
    wc();//chama a funçao wc
    refrigeracao();//chama a funçao refrigeraçao
  }
  if(estado_sirene == HIGH)
  { //se estado da sirene for igual a HIGH
    periodo_alarme = periodo_alarme + 1; //periodo do alarme incrementado
    digitalWrite(luz, LOW);  
  }

  // Verificaçao da senha...

  char key = keypad.getKey();// key, varivel do tipo Char, recebe um comando da biblioteca do teclado, referente a senha
  if (key == '#' || key == '*'){ // se o que foi digitado for * ou #
    position = 0; //recebe a posiçao 0
    fechadoOk(true); // o alarme e ativado
    digitalWrite(luz, LOW); //desliga a luz
  }
  if (key == senha[position]){// se o numero digitado for um dos numeros pertecentes a senha
    position ++; // posiçao recebe incremento

  }
  if (position == 4){ //se a posiçao chegar a 4
    fechadoOk(false); // o alarme e desativado  
    digitalWrite(sirene, LOW);
  }
}

void fechadoOk(int fechado)
{
  if (fechado){ //se fechado for TRUE
    digitalWrite(ativado_alarme, HIGH); // a led verde e acessa idicando alarme ON
    estado_alarme = HIGH; // a variavel estado do alarme recebe HIGH == ON
    digitalWrite(desativado_alarme, LOW); // a led vermemlha fica apagada
    digitalWrite(energia, LOW);//interrompe a energia do escritorio (desliga o relay)
    digitalWrite(energia1, LOW);//deliga a luz do escritorio (desliga o relay)
    digitalWrite(ar_condicionado, LOW); // desliga o ar condicionado
    digitalWrite(aquecedor, LOW); // deliga o aquecedor
    digitalWrite(luz, LOW); //desliga a luz
    digitalWrite(sirene, HIGH);
    delay(200);
    digitalWrite(sirene,LOW);
    delay(100);
    digitalWrite(sirene, HIGH);
    delay(100);
    digitalWrite(sirene, LOW);
    /*tone(buzzer_a, 1000); // emite uma sonoridade a 5000 Mhz no buzzer
     delay(300);// espera 300 milesegundos
     tone(buzzer_a, 1300);// emite uma sonoridade a 3000 Mhz no buzzer
     delay(200);// espera 200 milesegundos
     noTone(buzzer_a);// indica que o buzzer nao tocara mais
     */
  }
  else{ //senao
    digitalWrite(ativado_alarme, LOW);// led verde, que indica alarme ativado, e desligada
    digitalWrite(energia, HIGH);//libera a energia do escritorio (desliga o relay)
    digitalWrite(energia1, HIGH);//liga a luz do escritorio (desliga o relay)
    estado_alarme = LOW; // estado do alarme recebe LOW == OFF
    estado_sirene = LOW; // estado da sirene recebe LOW == OF
    digitalWrite(desativado_alarme, HIGH);// a led vermelha, que indica que o alarme esta desativado, e acessa
    digitalWrite(sirene, LOW);
    delay(200);
    digitalWrite(sirene,HIGH);
    delay(100);
    digitalWrite(sirene, LOW);
    delay(200);
    digitalWrite(sirene, HIGH);
    delay(100);
    digitalWrite(sirene, LOW);

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
  digitalWrite(sirene, HIGH);
  estado_sirene = HIGH; //estado sa sirene recebe HIGH == ON

}


//refrigeraçao

void refrigeracao() // funçao para a refrigeraçao
{
  if ( periodo_estado == 50){
    Serial.println("Refrigracao");// emite no monitor da IDE a seguinte mensagem citada
  }
  float t = dht.readTemperature(); // variavel t armazena o valor da temperatura lido pelo sensor
  if (isnan(t))// testa se retorno é valido, caso contrário algo está errado.
  {
    Serial.println("Algo de errado com o sensor DHT");//Emite no monitor da IDE a frase apresentada  
  } 
  if ( t > 24){ //se o valor da temperatura for maior que 24 gruas celsius
    digitalWrite(ar_condicionado, HIGH); // liga o cooler, ar condicionado
    arc = 1;  
  }
  else{//senao
    if ( t >= 22 && t <= 23 ) {// se a temperatura estiver entre 22 e 23 gruas celsius
      digitalWrite(ar_condicionado, LOW); // desliga o ar condicionado
      digitalWrite(aquecedor, LOW); // deliga o aquecedor
      arc = 0;
      aq = 0;
    }
    if ( t < 21){//se o valor da temperatura for menor que 21
      digitalWrite(aquecedor, HIGH); // liga o aquecedor
      aq = 1;
    }
  }
  periodo_dht = 0; // define o valor 0 a variavel periodo_dht
  /*
  Minesterio da sade aconselha que deve-se regular a temperatura do ar condicionado
   entre 21 a 24 graus. Segundo isso, o programa foi ajustado para
   que esse conselho seja seguido e a saude de quem utilizar a sala
   seja preservada.
   */
}

// BANHEIRO

void wc()
{ 
  if ( periodo_estado == 50){
    Serial.println("Banheiro");// emite no monitor da IDE a seguinte mensagem citada
  }
  if( on == 1){ //se a variavel luz for diferente de desligada
    periodo_wc = periodo_wc + 1; //periodo do wc  incrementado
    if( periodo_wc > 300){ // se o periodo for mais que 300
      digitalWrite(luz, LOW); //desliga a luz
      on = 0;
      periodo_wc = 0; //periodo do wc vai receber 
    }
  }
  estado_sensor_wc = digitalRead(sensor_wc); //uma variavel vai guardar a leitura do sensor do WC
  if( estado_sensor_wc == HIGH ){ // se o sensor for ligado
    digitalWrite(luz, HIGH);//luz acende
    on = 1;
    periodo_wc = 0;// periodo do wc recebe 0
  }
}
void estado(){


  Serial.println("+ = = = = = ESTADO GERAL DO SISTEMA = = = = = +");
  Serial.print("");
  //sala
  Serial.println("# STATUS SALA #");
  Serial.print("- Estado da energia: ");
  if (estado_alarme == LOW){
    Serial.println("Liberada!");
  }
  else{
    Serial.println("Restringida");
  }
  Serial.print("- Luz: ");
  if (estado_alarme == LOW){
    Serial.println("Ligada!");
  }
  else{
    Serial.println("Desativada");
  }
  Serial.print("- Seguranca: ");
  if ( estado_alarme == HIGH){
    Serial.println("Ativada");
  }
  else{
    Serial.println("Desativada");
  }
  Serial.println("=============================================");    
  //Alarme
  Serial.println("# STATUS ALARME #");
  Serial.print("- Estado: ");
  if ( estado_alarme == HIGH){
    Serial.println("Ativado");
  }
  else{
    Serial.println("Desativado");
  }
  Serial.print("- Periodo: ");
  Serial.println(periodo_alarme);
  Serial.print("- Status do sensor: ");
  Serial.println(estado_sensor_alarme);
  Serial.print("- Estado Sirene: ");
  Serial.println(estado_sirene); 
  Serial.println("=============================================");

  //Banheiro
  Serial.println("# STATUS DO BANHEIRO #");
  Serial.print("- Estado Luz:  ");
  Serial.println(on);
  Serial.print("- Tempo decorrido: ");
  Serial.println(periodo_wc);
  Serial.print("- Status do sensor: ");
  Serial.println(estado_sensor_wc);
  Serial.println("=============================================");

  //refrigeraçao
  Serial.println("# STATUS DA REFRIGERACAO #");
  float t = dht.readTemperature(); // variavel t armazena o valor da temperatura lido pelo sensor
  Serial.print("- Temperatura: ");//Emite no monitor da IDE a frase apresentada
  Serial.print(t); // Emite no monitor da IDE o valor da varivavel
  Serial.println("  Graus Celsius"); // Emite no monitor da IDE a frase apresentada, com quebra de linha "ln"
  Serial.print("- Componente Ligado:  ");
  if ((aq == 1) && (arc == 0)){
    Serial.println("Aquecedor");
  }
  else{
    if ((aq == 0) && (arc == 1)){
      Serial.println("Ar condicionado ");
    }
    else{

      Serial.println("Nenhum componente de refrigeracao esta ativado. ");

    }
  }

  Serial.println("=============================================");
  periodo_estado = 0;
}


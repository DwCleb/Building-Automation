/*
##################################################
#
#	Arquivo: dht.ino
#	Micro-processador: Arduino Mega
#	Objetivo: refrigeraçao automatica
#	Autor: Cleber Júnior (DwCleb)
#
#################################################
*/
#include <DHT.h>
 
#define DHTPIN A1 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11
 
// Conecte pino 1 do sensor (esquerda) ao +5V
// Conecte pino 2 do sensor ao pino de dados definido em seu Arduino
// Conecte pino 4 do sensor ao GND
DHT dht(DHTPIN, DHTTYPE); //definiçao do sensor, a partir de sua biblioteca
int ar_condicionado = 26; // define a saida 26 para o arcondicionado
int aquecedor = 25; //define a saida 25 para o aquecedor
int periodo_dht;

void setup() 
{
  Serial.begin(9600);//define a comunicaçao serial para inicializar em 9600 baud
  pinMode(ar_condicionado, OUTPUT); // define a varivael ar condicionado como saida
  pinMode(aquecedor, OUTPUT); // define a variavel aquecedor como saida
  dht.begin(); // define a funçao dht para inicializar
}
 void loop(){
  refrigeracao(); // chama a funçao refrigeraçao
 }
void refrigeracao() // funçao para a refrigeraçao
{
  float t = dht.readTemperature(); // variavel t armazena o valor da temperatura lido pelo sensor
  if (isnan(t))// testa se retorno é valido, caso contrário algo está errado.
  {
    Serial.println("Algo de errado com o sensor DHT, impossibilitado de fazer leitura");//Emite no monitor da IDE a frase apresentada  
  } 
  else // senao houver problemas com a leitura
  {
    Serial.print("Temperatura: ");//Emite no monitor da IDE a frase apresentada
    Serial.print(t); // Emite no monitor da IDE o valor da varivavel
    Serial.println("  Graus C"); // Emite no monitor da IDE a frase apresentada, com quebra de linha "ln"
  }

  if ( t > 24){ //se o valor da temperatura for maior que 24 gruas celsius
   digitalWrite(ar_condicionado, HIGH); // liga o cooler, ar condicionado
  }
  else{//senao
    if ( t >= 22 && t <= 23 ) {// se a temperatura estiver entre 20 e 22 gruas celsius
      digitalWrite(ar_condicionado, LOW); // desliga o ar condicionado
      digitalWrite(aquecedor, LOW); // deliga o aquecedor
    }
      if ( t < 21){//se o valor da temperatura for menor que 18
        digitalWrite(aquecedor, HIGH); // liga o aquecedor
      }
    }
    periodo_dht = 0;
  
}


/*
##################################################
#
#	Arquivo: alarme.Ino
#	Micro-processador: Arduino Mega
#	Objetivo: Alarme com teclado matricial, sirene e leds de status
#	Autor: Cleber Júnior (DwCleb)
#
#
#################################################
*/

#include <Keypad.h>

char senha = "2580";//senha para acesso
int position = 0; //char posiçao 0
const byte LINHAS = 4; //definindo linhas
const byte COLUNAS = 3; // definindo colunas
char teclas[LINHAS][COLUNAS] = { //mapeando teclado
                               {'1','2','3'},
                               {'4','5','6'},
                               {'7','8','9'},
                               {'*','0','#'}
};
byte pinoLinha[LINHAS] = {31,32,33,34}; // pinos referentes as linhas
byte pinoColuna[COLUNAS] = {28,29,30}; //pinos referentes as colunas
Keypad keypad = Keypad(makeKeymap(teclas), pinoLinha, pinoColuna, LINHAS, COLUNAS); //teclado configurado
int desativado_alarme = 38; //estado desativado do alarme
int ativado_alarme = 39; //estado ativado do alarme
#define tempo 10 // definindo tempo = 10
int frequencia = 0; // frenquencia em valor 0
int buzzer_a = 40; // pino do buzzer em n 17
int sensor_alarme = 37; // pino do sensor em n 22
int estado_sensor_alarme;
int periodo_alarme;
int estado_desativado_alarme;
int energia = 42; // pino ligado ao relay para liberar a energia do escritorio
int periodoOn;
int estado_alarme;
int estado_sirene;

void setup(){ //definiçao  
  
  pinMode(desativado_alarme, OUTPUT);//led vermelho como saida 
  pinMode(ativado_alarme, OUTPUT);//led vermelho como saida
  fechadoOk(true);// informa o que a funçao fechadoOk tem o parametro TRUE
  pinMode(buzzer_a,OUTPUT); //Pino do buzzer do alarme
  pinMode(sensor_alarme, INPUT); //pino do alarme como saida
  pinMode(energia, OUTPUT); // pino que fornecera a energia para o escritorio
  Serial.begin (9600);//define a transmiçao serial em baud 9600
}

void loop(){ //inicia o loop
  
  Serial.println("estado alarme");//imprime no monitor da IDE "estado alarme"
  Serial.println(estado_alarme);//imprime no monitor da IDE o valor da variavel estado_alarme
  Serial.println("estado periodo");//imprime no monitor da IDE "estado periodo"
  Serial.println(periodo_alarme);//imprime no monitor da IDE o valor da variacel periodo_alarme
  
  alarme();//chama a funçao alarme
  
  if(estado_alarme == HIGH){ // se o estado do alarme estiver recebido HIGH
    estado_sensor_alarme = digitalRead(sensor_alarme); //a variavel estado_sensor_alarme armazena a leitura do sensor
    if( estado_sensor_alarme == HIGH){ //se o sensor apresentar sinal HIGH
      periodoOn = HIGH; // periodoOn recebera HIGH
    }
  }
  if(periodoOn == HIGH){ ;// se periodoOn for igual a HIGH
    periodo_alarme = periodo_alarme + 1; // perido_alarme e incrementado
  }
  if(periodo_alarme > 100) { // se periodo_alarme for maior que 100
    periodoOn = LOW; // periodoOn recebe LOW
    periodo_alarme = 0; // periodo_alarme = 0
    
    sirene(); //chama a funçao sirene
  
}
  if( estado_alarme == LOW){ // se estado_alarme for igual a LOW
    periodoOn = LOW; // estado periodoOn recebe LOW
    periodo_alarme = 0; //periodo_alarme recebe 0
  }
  if(estado_sirene == HIGH){ //se estado da sirene for igual a HIGH
    periodo_alarme = periodo_alarme + 1; //periodo do alarme incrementado
  }
  delay(100); // atrassa o loop em 100 milesegundos
}

void alarme(){ // funçao alarme
  
  
  char key = keypad.getKey();// key, varivel do tipo Char, recebe um comando da biblioteca do teclado, referente a senha
  if (key == '*' || key == '#'){ // se o que foi digitado for * ou #
    position = 0; //recebe a posiçao 0
    fechadoOk(true); // o alarme e ativado
  }
  if (key == senha[position]){// se o numero digitado for um dos numeros pertecentes a senha
    position ++; // posiçao recebe incremento
  }
  if (position == 4){ //se a posiçao chegar a 4
    fechadoOk(false); // o alarme e desativado
   
  }
}

void fechadoOk(int fechado){
  
  if (fechado){ //se fechado for TRUE
    digitalWrite(ativado_alarme, HIGH); // a led verde e acessa idicando alarme ON
    estado_alarme = HIGH; // a variavel estado do alarme recebe HIGH == ON
    digitalWrite(energia, LOW); // a energia do escritorio nao e liberada
    digitalWrite(desativado_alarme, LOW); // a led vermemlha fica apagada
   
  }
  else{ //senao
    digitalWrite(ativado_alarme, LOW);// led verde, que indica alarme ativado, e desligada
    estado_alarme = LOW; // estado do alarme recebe LOW == OFF
    estado_sirene = LOW; // estado da sirene recebe LOW == OFF
    digitalWrite(energia, HIGH); // a energia do escritorio e liberada
    digitalWrite(desativado_alarme, HIGH);// a led vermelha, que indica que o alarme esta desativado, e acessa
  }
}

void sirene(){ // funçao da sirene
  
 tone(buzzer_a, 5000); // emite uma sonoridade a 5000 Mhz no buzzer
 delay(300);// espera 300 milesegundos
 tone(buzzer_a, 3000);// emite uma sonoridade a 3000 Mhz no buzzer
 delay(200);// espera 200 milesegundos
 tone(buzzer_a, 5000);// emite uma sonoridade a 5000 Mhz no buzzer
 delay(300);// espera 300 milesegundos
 tone(buzzer_a, 3000);// emite uma sonoridade a 3000 Mhz no buzzer
 delay(200);// espera 200 milesegundos
 tone(buzzer_a, 5000);// emite uma sonoridade a 5000 Mhz no buzzer
 delay(300);// espera 300 milesegundos
 noTone(buzzer_a);// cancela a emissao de sonoridade
 estado_sirene = HIGH; //estado sa sirene recebe HIGH == ON
 periodo_alarme = periodo_alarme + 70; // periodo alarme recebe um incremento 70
}

/*
##################################################
#
#	Arquivo: WC.cc
#	Micro-processador: Arduino Mega
#	Objetivo: Acionar lampada com PIR
#	Autor: Cleber Júnior (DwCleb)
#
#################################################
*/
 int  periodo = 0;  // declara a variavel periodo como inteira
 int  luz =  22 ; // declara o pino X como luz
 int estado_luz;     // declara a cariavel estado_luz como inteira
 int sensor_wc = 23 ; // declara o pino Y como sensor_wc
 int estado_sensor_wc;   //declara a variavel estado_sensor_wc como inteira
 int periodo_wc;
 int on = 0;
//PROGRAMA PRINCIPAL:
  void setup ()
{
     pinMode(luz, OUTPUT); // define a varivael luz como pino de saída
     pinMode(sensor_wc, INPUT); // define a variavel sensor_wc como entrada
     Serial.begin(9600);  // inicia o serial
}
//LOOP PRINCIPAL: 
 void loop ()//inicia o loop
{
   wc();//chama a funçao WC
   delay(100); //espera 100 milesegundos
}
void wc(){ //funçao WC
    if( on == 1){ //se a variavel luz for diferente de desligada
       periodo_wc = periodo_wc + 1; //periodo do wc  incrementado
       if( periodo_wc > 100){ // se o periodo for mais que 100
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
     Serial.println( "Periodo wc");//imprime no monitor da IDE "banheiro"
     Serial.println(periodo_wc); //imprime no monitor da IDE o valor da variavel
     Serial.println( "estado sensor");//imprime no monitor da IDE "banheiro"
     Serial.println(estado_sensor_wc); //imprime no monitor da IDE o valor da variavel
     Serial.println( "estado luz");//imprime no monitor da IDE "banheiro"
     Serial.println(estado_luz); //imprime no monitor da IDE o valor da variavel
}

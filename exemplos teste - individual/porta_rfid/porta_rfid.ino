 
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include<Wire.h>

 
#define SS_PIN 53
#define RST_PIN 41
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
 
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
 
char st[20];
 int buzzer = 7;
 int led_liberado = 5; //led de status liberado
int led_negado = 6; //led de status negado
int trava = 8;//trava
void setup() 
{
  pinMode(led_liberado, OUTPUT); // define o LED verde como saida
  pinMode(led_negado, OUTPUT); // define o LED vermelho como saida
  pinMode(trava, OUTPUT); // define o pino da trava como saida
  Serial.begin(9600);   // Inicia a serial
  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522   
  pinMode (buzzer, OUTPUT);
  lcd.begin(16,2);
    // Um breve blink de backlight  
    lcd.backlight();//liga
    delay(250);//espera 250 milesegundos
    lcd.noBacklight();//desliga
    delay(250); //espera 250 milesegundos
    lcd.backlight();//liga o LCD
}
 void loop(){
 mensageminicial();
 porta();
 }
void porta() 
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Mostra UID na serial
  Serial.print("UID da tag :");
  String conteudo= "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Mensagem : ");
  conteudo.toUpperCase();
  if (conteudo.substring(1) == "F3 55 D8 00" || conteudo.substring(1) == "92 2F DD 76") 
  {
   autorizado();
  }
  else{
   negado();
  }
  mensageminicial();
} 
 
void mensageminicial()
{
  lcd.clear();
  lcd.print(" Aproxime o seu");  
  lcd.setCursor(0,1);
  lcd.print("cartao do leitor");  
}
void autorizado()
  {
  lcd.clear();// limpa o LCD
  lcd.setCursor(0,0);// cursor do LCD na linha 1
  lcd.print("     Acesso ");// emite a frase apresentada no LCD
  lcd.setCursor(0,1);// cursor do LCD na linha 2
  lcd.print("   AUTORIZADO"); // emite a frase apresentada no LCD
  digitalWrite(trava, HIGH);// manda energia para o pino da trava
  delay(200);// espera 200ms
  digitalWrite(trava, LOW);//encerra a energia mandada para o pino da trava
    tone(buzzer, 7000);// emite uma sonoridade em 7000Mhz no buzzer
    delay(300);//espera 300 ms
    tone(buzzer, 4500);// emite uma sonoridade em 4500Mhz no buzzer
    delay(200);//espera 200 ms
    noTone(buzzer);// cancela a sonoridade no buzzer
    delay(400);//espera 400 ms
   }
void negado()
  {
  lcd.clear();// limpa o LCD
  lcd.setCursor(0,0);// cursor do LCD na linha 1
  lcd.print("     Acesso ");// emite a frase apresentada no LCD
  lcd.setCursor(0,1);// cursor do LCD na linha 2
  lcd.print("     NEGADO"); // emite a frase apresentada no LCD
    tone(buzzer, 4000);// emite uma sonoridade em 4000Mhz no buzzer
    delay(300);//espera 300 ms
    tone(buzzer, 1500);// emite uma sonoridade em 1500Mhz no buzzer
    delay(200);//espera 200 ms
    noTone(buzzer); // cancela a sonoridade no buzzer
    delay(400);//espera 400 ms
  }

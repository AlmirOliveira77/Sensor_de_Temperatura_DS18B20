/*******************************************************************************
*     
*    IFPB - Campina Grande
*    Sensor de Temperatura DS18B20  
*    Disciplinas: Sistemas Embarcados 
*                 Técnicas de Prototipagem
*    Autores: Almir de Oliveira Pereira
*             Gabriel Faustino da Silva
*             Jhônata Vinícius da Silva Negreiros  
*    Data: 24/09/2021                    
*
*******************************************************************************/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

//Conexões do terminal de sinal do DS18B20 e o botão com o microcontrolador atmega328p
const byte sensorPin = 9;
const byte botPin = 10;
const byte buzzerPin = 13;
const byte ledVermelho = 8;
const byte ledVerde = 7;

//Estado do botão
boolean estado = 0;

//Bibliotecas
OneWire pin(sensorPin); 
DallasTemperature bus(&pin);
DeviceAddress sensor;
 
//LCD
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

float temperatura_minima;
float temperatura_maxima;
float temperatura;

//Temperaturas de alarme
const float maxima = 22;
const float minima = 18;

void setup() {
  Serial.begin(9600); //inicia monitor serial
  lcd.begin(16, 2); //inicia lcd
  pinMode(botPin, INPUT_PULLUP); 
  pinMode(ledVermelho, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(buzzerPin, OUTPUT); 
  lcd.clear(); //limpa o disply
  bus.begin(); //inicializa o objeto bus
  bus.getAddress(sensor, 0); 
  temperatura_minima = 999;
  temperatura_maxima = -999;
}

void loop() {
  bus.requestTemperatures(); 
  temperatura = bus.getTempC(sensor); 
  
  if (temperatura < temperatura_minima){
   temperatura_minima = temperatura;
  }
  if (temperatura > temperatura_maxima){
   temperatura_maxima = temperatura;
  }
    
  mostra_temperatura(); 
  ativa_buzzer();   
  
  if (temperatura > maxima){ 
    Serial.println("TEMPERATURA ACIMA DO IDEAL");     
    sirene(); 
  } 
  else if  (temperatura < minima) {
    Serial.println("TEMPERATURA ABAIXO DO IDEAL"); 
    sirene();   
  } 
  else {
    Serial.println("TEMPERATURA ESTÁ IDEAL"); 
    digitalWrite(ledVermelho, LOW);
    noTone(buzzerPin); 
  }
}
 
void ativa_buzzer() { 
  if (!digitalRead(botPin)) {
       estado = !estado; 
       digitalWrite(ledVerde, estado);                     
       while(!digitalRead(botPin)) {}
       delay(150);
    }   
}

void mostra_temperatura() {
  lcd.setCursor(0,0);
  lcd.print("Temp.: "); 
  lcd.print(temperatura);
  lcd.write(B11011111); 
  lcd.print("C ");
  lcd.setCursor(0,1);
  lcd.print("L=");
  lcd.print (temperatura_minima);
  lcd.print("  H=");
  lcd.print (temperatura_maxima);
}

void sirene(){ 
  digitalWrite(ledVermelho, LOW);
  delay(50);
  ativa_buzzer();
  
  if (estado){ 
    noTone(buzzerPin);
    digitalWrite(ledVermelho, HIGH); 
    delay(150); 
    ativa_buzzer(); 
  } 
  else {
    for (int k=0; k<5; k++){
      ativa_buzzer(); 
      tone(buzzerPin, 1350); 
      delay(150);
      tone(buzzerPin, 1050); 
      delay(150); 
      digitalWrite(ledVermelho, HIGH);        
    }           
  }
} 

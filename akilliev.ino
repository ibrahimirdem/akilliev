#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <dht11.h> 
#include<Servo.h>

#define PIR 7 //PIR Sensörünü Digital 5 pinine bağlıyoruz.
#define DHT11PIN A1 // DHT11'imizi A1 pinimize bağlıyoruz.
#define NEMSENSOR A0 // Toprak Nem Sensörümüzü A3 pinimize bağlıyoruz.
dht11 DHT11; //DHT11 nesnemizi oluşturuyoruz.
Servo garajservo;
Servo perdeservo;
int Ndeger;
int Bdeger;
int perdedurum;
int alarmdeger;
int Buzzer = A2;
int alarmLed = 6;
int sicaklikdurum;
int isitmaoto = 5;
int isitmasicak = 4;
int isitmasoguk = 3;
int toprakon = 2;
int toprakoff = A3;
float derecedeger;

SoftwareSerial Bbaglanti(10,11); //Bluetooth Modülü nesnemizi oluşturup. 10->Rx,  11->Tx bacağını bağlıyoruz.

void setup(){
  Bbaglanti.begin(9600); // Bluetooth Modülü ile haberleşme frekansımızı ayarlıyoruz.
  Serial .begin(9600); // Blgisayar ile haberleşmemizi sağlıyor.
  pinMode(13, OUTPUT); //Salon oda ledi (ışığı)
  pinMode(12, OUTPUT); //Garaj oda ledi (ışığı)
  pinMode(PIR, INPUT); //PIR Sensörünü giriş olarak ayarlıyoruz.
  pinMode(Buzzer, OUTPUT); //Buzzer çıkış olarak ayarlanıyor.
  pinMode(A3, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite(1,LOW);
  digitalWrite(2,LOW);
  perdeservo.attach(9);
  garajservo.attach(8);
  EEPROM.write(10, 0); //Perdeler kapalı.       0->Kapalı, 1->Açık
  EEPROM.write(15, 0); //Garaj Kapası Kapalı    0->Kapalı, 1->Açık
  EEPROM.write(20, 0); //Güvenlik Sistemi       0->Kapalı, 1->Açık
  EEPROM.write(25, 0); //Isıtma Otomatik Kapalı 0->Kapalı, 1->Açık
  EEPROM.write(30, 0); //Isıtma                 Soguk->0, Sıcak->1
  EEPROM.write(35, 0); //Sulama için            Kapalı->0, Açık->1
  garajservo.write(180); //Garaj kapısı başlangıçta kapalı konuma getirildi.
}

void loop(){
  
  if (Bbaglanti.available()>0){ //Eğer Bluetooth'tan veri alındıysa. İşlemi gerçekleştir.
    delay(100);
    Bdeger = (int)Bbaglanti.read();
    Serial.print("Deger :");
    Serial.println(Bdeger);
    if (Bdeger == 1){ // Ledi yani oda lambamızı yakmak için kullandığımız kısım.
      digitalWrite(13, HIGH);
      Serial.println("Aciklama : Salon Oda isigi acildi.");
    }
    else if (Bdeger == 2){ //Ledi yani oda lambamımız söndürmek için oluşan kısım.
      digitalWrite(13, LOW);
      Serial.println("Aciklama :Salon Oda isigi kapandi.");
    }
    else if (Bdeger == 13){ //Sıcaklık değerini okumak için olan kısım. 
      int chk = DHT11.read(DHT11PIN);  
      Bbaglanti.print((float)DHT11.temperature, 2);
      Serial.println((float)DHT11.temperature, 2);
      Serial.println("Aciklama : Oda sicakligi degeri gonderildi.");
    }
    else if (Bdeger == 8 && (int)EEPROM.read(10)==1){ //Servo motoru 2 saniye sola döndür.
      Serial.print("Aciklama : Perdeler kapaniyor.");
      perdeservo.write(0);
      delay(2000);
      perdeservo.write(94);
      Serial.println(" Kapandi. ");  
      EEPROM.write(10, 0);
    }
    else if (Bdeger == 7 && (int)EEPROM.read(10)== 0){//Servo motoru 2 saniye sağa döndür.
      Serial.print("Aciklama : Perdeler aciliyor.");
      perdeservo.write(180); 
      delay(2000);
      perdeservo.write(94);
      Serial.println(" Acildi. ");
      EEPROM.write(10, 1);
    }
    else if (Bdeger == 3){ //Garaj oda ışığını açmak için kullanıldı.
      digitalWrite(12, HIGH);
      Serial.println("Aciklama :Garaj Oda isigi acildi.");
    }
    else if (Bdeger == 4){ //Garaj oda ışığını kapatmak için kullanıldı.
      digitalWrite(12, LOW);
      Serial.println("Aciklama :Garaj Oda isigi kapandi.");
    }
    else if (Bdeger == 9  && (int)EEPROM.read(15)==0){ //Garaj Kapısı Açma Kapama (Açık)
      garajservo.write(0);
      EEPROM.write(15,1);
      Serial.println("Aciklama :Garaj kapisi acildi.");
      }
    else if (Bdeger == 10 && (int)EEPROM.read(15)==1){ //Garaj Kapısı Açma Kapama (Kapalı)
      garajservo.write(180);
      Serial.println("Aciklama :Garaj kapisi kapandi.");
      EEPROM.write(15,0);
      }
    else if (Bdeger == 5  && (int)EEPROM.read(20)==0){ //Güvenlik sistemini AÇMA tıklandı ise
      EEPROM.write(20,1);
      digitalWrite(alarmLed, HIGH);
      Serial.println("Aciklama :Guvenlik Sistemi ACIK.");
      }
    else if (Bdeger == 6  && (int)EEPROM.read(20)==1){ //Güvenlik sistemini KAPAMA tıklandı ise
      EEPROM.write(20,0);
      digitalWrite(alarmLed, LOW);
      analogWrite(Buzzer,0);
      Serial.println("Aciklama :Guvenlik Sistemi KAPALI.");
      }
    else if (Bdeger == 11 && (int)EEPROM.read(25)==0){ //Isıtma Otomatik Tıklandı ise
      digitalWrite(isitmaoto, HIGH);
      EEPROM.write(25, 1);
      Serial.println("Aciklama :Isitma OTOMATIK yapildi.");
      }
    else if (Bdeger == 12 && (int)EEPROM.read(25)==1){ //Isıtma Manuel Tıklandı ise
      digitalWrite(isitmaoto, LOW);
      digitalWrite(isitmasicak, LOW);
      digitalWrite(isitmasoguk, LOW);
      EEPROM.write(25, 0);
      Serial.println("Aciklama :Isitma MANUEL yapildi.");
      }
    else if (Bdeger == 14 && (int)EEPROM.read(25)==0){ //Manuel tıklandı ise
      digitalWrite(isitmasicak,HIGH);
      digitalWrite(isitmasoguk,LOW);
      Serial.println("Aciklama :Manuel olarak Sıcak ayarlandı.");
    }
    else if (Bdeger == 15 && (int)EEPROM.read(25)==0){
      digitalWrite(isitmasicak,LOW);
      digitalWrite(isitmasoguk,HIGH);
      Serial.println("Aciklama :Manuel olarak Soguk ayarlandı.");  
    }
    Serial.println("\n-------------------------------");
    delay(75);
  }
  else{
    ////////////////////////////////x"//////////////////////////////////
    Ndeger = analogRead(NEMSENSOR); 
    if (Ndeger > 800 && (int)EEPROM.read(35)== 0){
      Serial.print("Aciklama :Sulama icin gerekli vakit.\nDeger :");
      digitalWrite(toprakon, HIGH);
      analogWrite(toprakoff, 0);
      Serial.println(Ndeger);
      Serial.println("-------------------------------");
      EEPROM.write(35, 1);
    }
    else if(Ndeger < 801 && (int)EEPROM.read(35)== 1){
      Serial.print("Aciklama :Sulama gerekmiyor. Toprak nemli.\nDeger :");
      digitalWrite(toprakon, LOW);
      analogWrite(toprakoff, 1023);
      Serial.println(Ndeger);
      Serial.println("-------------------------------");
      EEPROM.write(35, 0);
      }
    //////////////////////////////////////////////////////////////////
    alarmdeger = digitalRead(PIR);
    if (alarmdeger == HIGH && (int)EEPROM.read(20) == 1){ 
      Serial.println("Aciklama :Hırsız Tespit Edildi.");
      analogWrite(Buzzer,1023);
    }
    /////////////////////////////////////////////////////////////////
    if ((int)EEPROM.read(25)==1){ // Otomatik Isıtma Açık ise
      delay(200);
      int chk = DHT11.read(DHT11PIN);  
      derecedeger = (float)DHT11.temperature;
      if (derecedeger < 28 ){
        digitalWrite(isitmasoguk,LOW);
        digitalWrite(isitmasicak,HIGH);
        Serial.print("Derece Deger :");
        Serial.println(derecedeger);
        Serial.println("Aciklama :28 derece altina indi. Sicak Acildi.");
        EEPROM.write(30,1);
      }
      else if(derecedeger > 35 ){
        digitalWrite(isitmasoguk,HIGH);
        digitalWrite(isitmasicak,LOW);
        Serial.print("Derece Deger :");
        Serial.println(derecedeger);
        Serial.println("Aciklama :35 derece ustune cikti. Soguk Acildi.");
        EEPROM.write(30,0);
      }
      else{
        digitalWrite(isitmasoguk,LOW);
        digitalWrite(isitmasicak,LOW);
        Serial.print("Derece Deger :");
        Serial.println(derecedeger);
        Serial.println("Aciklama :20-30 derece arasinda.");
        EEPROM.write(30,3);
      }
    }
    /////////////////////////////////////////////////////////////////     
 }
}
  
  
  

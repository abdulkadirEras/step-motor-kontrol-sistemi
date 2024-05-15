//DWIN Ekran ile 7 tane step motoru hız ayarı ile sürme
//V1.0
//Abdulkadir Eraslan tarafından tasarlanmıştır.
//Eksik olan step motor kayıdı yapıldıktan sonra enerjisi gidip geldiğinde kayıt siliniyor

#include <Arduino.h>
#include "driver/gpio.h"
#include <iostream>
#include <HardwareSerial.h>
#include <EEPROM.h>

#define EEPROM_Boyut  512
#define yon  1
#define adim  0
#define aktif 2
#define tam_adim  600//200 adım tam tur full stepte
#define sure 400
#define runLed  GPIO_NUM_23

#define motorSaatYonu  0
#define motorSaatYonuSureBaslanic 1
#define motorSaatYonuSureBitis 2
#define motorSaatYonuTersi  3
#define motorSaatYonuTersiSureBaslangic 4
#define motorSaatYonuTersiSureBitis 5

gpio_num_t stepMotor_1[3]= { GPIO_NUM_14,GPIO_NUM_12,GPIO_NUM_13 };
gpio_num_t stepMotor_2[3]={GPIO_NUM_25,GPIO_NUM_26,GPIO_NUM_27};
gpio_num_t stepMotor_3[3]={GPIO_NUM_4,GPIO_NUM_2,GPIO_NUM_15};
gpio_num_t stepMotor_4[3]={GPIO_NUM_5,GPIO_NUM_33,GPIO_NUM_32};
gpio_num_t stepMotor_5[3]={GPIO_NUM_21,GPIO_NUM_19,GPIO_NUM_18};
gpio_num_t stepMotor_6[3]={GPIO_NUM_22,GPIO_NUM_1,GPIO_NUM_3};
gpio_num_t stepMotor_7[3]={GPIO_NUM_39,GPIO_NUM_34,GPIO_NUM_35};

uint32_t stepMotor1Kayit[6];
uint32_t stepMotor2Kayit[6];
uint32_t stepMotor3Kayit[6];
uint32_t stepMotor4Kayit[6];
uint32_t stepMotor5Kayit[6];
uint32_t stepMotor6Kayit[6];
uint32_t stepMotor7Kayit[6];

bool stepMotor1SaatYonuMu=0;
bool stepMotor1SaatYonuTersiMi=0;
bool stepMotor2SaatYonuMu=0;
bool stepMotor2SaatYonuTersiMi=0;
bool stepMotor3SaatYonuMu=0;
bool stepMotor3SaatYonuTersiMi=0;
bool stepMotor4SaatYonuMu=0;
bool stepMotor4SaatYonuTersiMi=0;
bool stepMotor5SaatYonuMu=0;
bool stepMotor5SaatYonuTersiMi=0;
bool stepMotor6SaatYonuMu=0;
bool stepMotor6SaatYonuTersiMi=0;
bool stepMotor7SaatYonuMu=0;
bool stepMotor7SaatYonuTersiMi=0;


int sayac=0;
int kayitSayaci=0;
int kayitSayacininSonDegeri;
uint32_t hiz=1000;

bool durum=0;
bool kayitAlinsinMi=false;
bool tekSefer[14];
bool oynatBakalim=false;

hw_timer_t *zamanlayicim = NULL;

const byte rxPin = 16; //rx2
const byte txPin = 17; //tx2
HardwareSerial dwin(2); // Connect DWIN via UART2
unsigned char Buffer[9];

void stepMotorAktifPasifButonu(uint8_t adres,uint8_t deger);
void akifKontrolu();
void pinAyarlamalari();
void stepMotorDonusYonuAktifPasifButonu(uint8_t adres,uint8_t deger);
void hangiMotorlarDonecek();
void ekranMotorDonusIkonlariDurdur();
void aktifMotorButonlariAyarlamalari();
void motorKayit();
void motorlarCalissin();
void motorOynat();
void zamanlayiciAyarlari();
void acilStop();
void slideDuzenle(uint32_t deger);

void IRAM_ATTR zamanlayiciDoldu()
{
sayac++;

if(kayitAlinsinMi==true || oynatBakalim==true)
  kayitSayaci++;

if(oynatBakalim==false && kayitAlinsinMi==false)
  kayitSayaci=0;

}


void setup()
{
  zamanlayiciAyarlari();
  
  Serial.begin(9600);
  dwin.begin(115200, SERIAL_8N1, rxPin, txPin);

  Serial.println("ElVa Elektronik V1.0");
  Serial.println("DWIN Ekranli Step Motor Kontrol Devresi");
  
 EEPROM.begin(EEPROM_Boyut);
  
  
  

  pinAyarlamalari();
  ekranMotorDonusIkonlariDurdur();
  aktifMotorButonlariAyarlamalari();

  if(EEPROM.readUInt(8)!=0xff)
    hiz=EEPROM.readUInt(8);

  
  slideDuzenle(hiz);



  uint8_t adresler[6]={15,20,25,30,35,40};
  for (uint8_t i = 0; i < 6; i++)
  {
    stepMotor2Kayit[i]=EEPROM.readUInt(adresler[i]);
    delay(1);
  }
}

void loop()
{

  if( dwin.available())
  {

  
  Serial.println("okunan deger:");
  for (int i = 0; i <= 8; i++) //this loop will store whole frame in buffer array.
  {
    Buffer[i] = dwin.read();
   
    
  }

  if (Buffer[0]==0x5A)
  { 
    akifKontrolu();
    hangiMotorlarDonecek();
    //Acil Stop
    if(Buffer[4]==0x40 && Buffer[5]==0x55 && Buffer[8]==0x01)
    {
      acilStop();
      oynatBakalim=false;
      kayitAlinsinMi=false;

    }
    //hız verisini alma
    if(Buffer[4]==0x50 && Buffer[5]==0x00)
    {
      hiz=(Buffer[7] << 8) | Buffer[8];
      EEPROM.writeUInt(8,hiz);
      EEPROM.commit();
    }
    //Kaydı Durdurma
    if(Buffer[4]==0x40 && Buffer[5]==0x00 && Buffer[8]==0x00)
    {
      kayitAlinsinMi=0;
      kayitSayacininSonDegeri=kayitSayaci;
    }
    //Kayıt Etme
    if(Buffer[4]==0x40 && Buffer[5]==0x00 && Buffer[8]==0x01)
    {
      kayitAlinsinMi=1;
      kayitSayacininSonDegeri=0;
      for(uint8_t i=0;i<6;i++)
      {
        stepMotor1Kayit[i]=0;
        stepMotor2Kayit[i]=0;
        stepMotor3Kayit[i]=0;
        stepMotor4Kayit[i]=0;
        stepMotor5Kayit[i]=0;
        stepMotor6Kayit[i]=0;
        stepMotor7Kayit[i]=0;
      }
      for(uint8_t i=0;i<sizeof(tekSefer);i++)
        tekSefer[i]=true;

    }
    //Oynatmayı Durdurma
    if(Buffer[4]==0x40 && Buffer[5]==0x50 && Buffer[8]==0x00)
      oynatBakalim=false;
    //Oynatmayı Başlat
    if(Buffer[4]==0x40 && Buffer[5]==0x50 && Buffer[8]==0x01)
      {
        oynatBakalim=true; 

      }
    }
  }

  


  motorlarCalissin();
  motorKayit();
  if(kayitSayacininSonDegeri>=kayitSayaci && oynatBakalim==true && kayitAlinsinMi==false)
    motorOynat();

  if(kayitSayacininSonDegeri<kayitSayaci && oynatBakalim==true && kayitAlinsinMi==false)
    kayitSayaci=0;
}

void acilStop()
{
  //Step Motorlar pasif edildi
  gpio_set_level(stepMotor_1[aktif],HIGH);
  gpio_set_level(stepMotor_2[aktif],HIGH);
  gpio_set_level(stepMotor_3[aktif],HIGH);
  gpio_set_level(stepMotor_4[aktif],HIGH);
  gpio_set_level(stepMotor_5[aktif],HIGH);
  gpio_set_level(stepMotor_6[aktif],HIGH);
  gpio_set_level(stepMotor_7[aktif],HIGH);

  EEPROM.writeBool(0,0);
  EEPROM.writeBool(1,0);
  EEPROM.writeBool(2,0);
  EEPROM.writeBool(3,0);
  EEPROM.writeBool(4,0);
  EEPROM.writeBool(5,0);
  EEPROM.writeBool(6,0);
  EEPROM.commit();

  //Step Motorların adim pinleri low edildi
  gpio_set_level(stepMotor_1[adim],LOW);
  gpio_set_level(stepMotor_2[adim],LOW);
  gpio_set_level(stepMotor_3[adim],LOW);
  gpio_set_level(stepMotor_4[adim],LOW);
  gpio_set_level(stepMotor_5[adim],LOW);
  gpio_set_level(stepMotor_6[adim],LOW);
  gpio_set_level(stepMotor_7[adim],LOW);

}

void motorlarCalissin()
{
  if (sayac>=hiz/100)
  {
    durum=!durum;
    gpio_set_level(runLed,durum);
    if(stepMotor1SaatYonuMu==1 || stepMotor1SaatYonuTersiMi==1)
      gpio_set_level(stepMotor_1[adim],durum);
    if(stepMotor2SaatYonuMu==1 || stepMotor2SaatYonuTersiMi==1)
      gpio_set_level(stepMotor_2[adim],durum);
    if(stepMotor3SaatYonuMu==1 || stepMotor3SaatYonuTersiMi==1)
      gpio_set_level(stepMotor_3[adim],durum);
    if(stepMotor4SaatYonuMu==1 || stepMotor4SaatYonuTersiMi==1)
      gpio_set_level(stepMotor_4[adim],durum);
    if(stepMotor5SaatYonuMu==1 || stepMotor5SaatYonuTersiMi==1)
      gpio_set_level(stepMotor_5[adim],durum);
    if(stepMotor6SaatYonuMu==1 || stepMotor6SaatYonuTersiMi==1)
      gpio_set_level(stepMotor_6[adim],durum);
    if(stepMotor7SaatYonuMu==1 || stepMotor7SaatYonuTersiMi==1)
      gpio_set_level(stepMotor_7[adim],durum);
    sayac=0;
  }

}

void motorKayit()
{
  
  if(kayitAlinsinMi==true && oynatBakalim==false)
  { 
    /*            Step Motor 1 Saat Yönü Kayıt              */
    if(stepMotor1SaatYonuMu==1)
    {
      if(tekSefer[0]==true)
      {
        stepMotor1Kayit[motorSaatYonuSureBaslanic]=kayitSayaci;
        tekSefer[0]=false;
      }
      stepMotor1Kayit[motorSaatYonu]=1;
      stepMotor1Kayit[motorSaatYonuSureBitis]=kayitSayaci;
    }
    /*            Step Motor 1 Saat Yönü Tersi Kayıt              */
    if(stepMotor1SaatYonuTersiMi==1)
    {
      stepMotor1Kayit[motorSaatYonuTersi]=1;
      if(tekSefer[1]==true)
      {
        stepMotor1Kayit[motorSaatYonuTersiSureBaslangic]=kayitSayaci;
        tekSefer[1]=false;
      }
      stepMotor1Kayit[motorSaatYonuTersiSureBitis]=kayitSayaci;
      
    }
    //*************************************************************************
    /*            Step Motor 2 Saat Yönü Kayıt              */
    if(stepMotor2SaatYonuMu==1)
    {
      if(tekSefer[2]==true)
      {
        stepMotor2Kayit[motorSaatYonuSureBaslanic]=kayitSayaci;
        tekSefer[2]=false;
      }
      stepMotor2Kayit[motorSaatYonu]=1;
      stepMotor2Kayit[motorSaatYonuSureBitis]=kayitSayaci;
    }
    /*            Step Motor 2 Saat Yönü Tersi Kayıt              */
    if(stepMotor2SaatYonuTersiMi==1)
    {
      stepMotor2Kayit[motorSaatYonuTersi]=1;
      if(tekSefer[3]==true)
      {
        stepMotor2Kayit[motorSaatYonuTersiSureBaslangic]=kayitSayaci;
        tekSefer[3]=false;
      }
      stepMotor2Kayit[motorSaatYonuTersiSureBitis]=kayitSayaci;
      
    }
    //*************************************************************************
    /*            Step Motor 3 Saat Yönü Kayıt              */
    if(stepMotor3SaatYonuMu==1)
    {
      if(tekSefer[4]==true)
      {
        stepMotor3Kayit[motorSaatYonuSureBaslanic]=kayitSayaci;
        tekSefer[4]=false;
      }
      stepMotor3Kayit[motorSaatYonu]=1;
      stepMotor3Kayit[motorSaatYonuSureBitis]=kayitSayaci;
    }
    /*            Step Motor 3 Saat Yönü Tersi Kayıt              */
    if(stepMotor3SaatYonuTersiMi==1)
    {
      stepMotor3Kayit[motorSaatYonuTersi]=1;
      if(tekSefer[5]==true)
      {
        stepMotor3Kayit[motorSaatYonuTersiSureBaslangic]=kayitSayaci;
        tekSefer[5]=false;
      }
      stepMotor3Kayit[motorSaatYonuTersiSureBitis]=kayitSayaci;
      
    }

    //*************************************************************************
    /*            Step Motor 4 Saat Yönü Kayıt              */
    if(stepMotor4SaatYonuMu==1)
    {
      if(tekSefer[6]==true)
      {
        stepMotor4Kayit[motorSaatYonuSureBaslanic]=kayitSayaci;
        tekSefer[6]=false;
      }
      stepMotor4Kayit[motorSaatYonu]=1;
      stepMotor4Kayit[motorSaatYonuSureBitis]=kayitSayaci;
    }
    /*            Step Motor 4 Saat Yönü Tersi Kayıt              */
    if(stepMotor4SaatYonuTersiMi==1)
    {
      stepMotor4Kayit[motorSaatYonuTersi]=1;
      if(tekSefer[7]==true)
      {
        stepMotor4Kayit[motorSaatYonuTersiSureBaslangic]=kayitSayaci;
        tekSefer[7]=false;
      }
      stepMotor4Kayit[motorSaatYonuTersiSureBitis]=kayitSayaci;
      
    }
    //*************************************************************************
    /*            Step Motor 5 Saat Yönü Kayıt              */
    if(stepMotor5SaatYonuMu==1)
    {
      if(tekSefer[8]==true)
      {
        stepMotor5Kayit[motorSaatYonuSureBaslanic]=kayitSayaci;
        tekSefer[8]=false;
      }
      stepMotor5Kayit[motorSaatYonu]=1;
      stepMotor5Kayit[motorSaatYonuSureBitis]=kayitSayaci;
    }
    /*            Step Motor 5 Saat Yönü Tersi Kayıt              */
    if(stepMotor5SaatYonuTersiMi==1)
    {
      stepMotor5Kayit[motorSaatYonuTersi]=1;
      if(tekSefer[9]==true)
      {
        stepMotor5Kayit[motorSaatYonuTersiSureBaslangic]=kayitSayaci;
        tekSefer[9]=false;
      }
      stepMotor5Kayit[motorSaatYonuTersiSureBitis]=kayitSayaci;
      
    }
    //*************************************************************************
    /*            Step Motor 6 Saat Yönü Kayıt              */
    if(stepMotor6SaatYonuMu==1)
    {
      if(tekSefer[10]==true)
      {
        stepMotor6Kayit[motorSaatYonuSureBaslanic]=kayitSayaci;
        tekSefer[10]=false;
      }
      stepMotor6Kayit[motorSaatYonu]=1;
      stepMotor6Kayit[motorSaatYonuSureBitis]=kayitSayaci;
    }
    /*            Step Motor 6 Saat Yönü Tersi Kayıt              */
    if(stepMotor6SaatYonuTersiMi==1)
    {
      stepMotor6Kayit[motorSaatYonuTersi]=1;
      if(tekSefer[11]==true)
      {
        stepMotor6Kayit[motorSaatYonuTersiSureBaslangic]=kayitSayaci;
        tekSefer[11]=false;
      }
      stepMotor6Kayit[motorSaatYonuTersiSureBitis]=kayitSayaci;
      
    }
    //*************************************************************************
    /*            Step Motor 7 Saat Yönü Kayıt              */
    if(stepMotor7SaatYonuMu==1)
    {
      if(tekSefer[12]==true)
      {
        stepMotor7Kayit[motorSaatYonuSureBaslanic]=kayitSayaci;
        tekSefer[12]=false;
      }
      stepMotor7Kayit[motorSaatYonu]=1;
      stepMotor7Kayit[motorSaatYonuSureBitis]=kayitSayaci;
    }
    /*            Step Motor 7 Saat Yönü Tersi Kayıt              */
    if(stepMotor7SaatYonuTersiMi==1)
    {
      stepMotor7Kayit[motorSaatYonuTersi]=1;
      if(tekSefer[13]==true)
      {
        stepMotor7Kayit[motorSaatYonuTersiSureBaslangic]=kayitSayaci;
        tekSefer[13]=false;
      }
      stepMotor7Kayit[motorSaatYonuTersiSureBitis]=kayitSayaci;
      
    }
  }

}

void motorOynat()
{
  /*                        Step Motor 1 Kaydedilen Değerle Saat Yönüne Hareket Ediyor                */
  if(stepMotor1Kayit[motorSaatYonu]==1 &&  stepMotor1Kayit[motorSaatYonuSureBitis] >= kayitSayaci && stepMotor1Kayit[motorSaatYonuSureBaslanic] <= kayitSayaci)
  {
    gpio_set_level(stepMotor_1[yon],HIGH);
    gpio_set_level(stepMotor_1[adim],durum);
  }
  /*                        Step Motor 1 Kaydedilen Değerle Saat Yönün TERSİNE Hareket Ediyor                */
  if(stepMotor1Kayit[motorSaatYonuTersi]==1 && stepMotor1Kayit[motorSaatYonuTersiSureBitis] >= kayitSayaci && stepMotor1Kayit[motorSaatYonuTersiSureBaslangic] <= kayitSayaci)
  {
    gpio_set_level(stepMotor_1[yon],LOW);
    gpio_set_level(stepMotor_1[adim],durum);
  }
  /*********************************************************************************************************************/
  /*                        Step Motor 2 Kaydedilen Değerle Saat Yönüne Hareket Ediyor                */
  if(stepMotor2Kayit[motorSaatYonu]==1 && stepMotor2Kayit[motorSaatYonuSureBitis] >= kayitSayaci && stepMotor2Kayit[motorSaatYonuSureBaslanic] <= kayitSayaci)
  {
    gpio_set_level(stepMotor_2[yon],HIGH);
    gpio_set_level(stepMotor_2[adim],durum);
  }
  /*                        Step Motor 2 Kaydedilen Değerle Saat Yönün TERSİNE Hareket Ediyor                */
  if(stepMotor2Kayit[motorSaatYonuTersi]==1 && stepMotor2Kayit[motorSaatYonuTersiSureBitis] >= kayitSayaci && stepMotor2Kayit[motorSaatYonuTersiSureBaslangic] <= kayitSayaci)
  {
    gpio_set_level(stepMotor_2[yon],LOW);
    gpio_set_level(stepMotor_2[adim],durum);
  }
  /*********************************************************************************************************************/
  /*                        Step Motor 3 Kaydedilen Değerle Saat Yönüne Hareket Ediyor                */
  if(stepMotor3Kayit[motorSaatYonu]==1 && stepMotor3Kayit[motorSaatYonuSureBitis] >= kayitSayaci && stepMotor3Kayit[motorSaatYonuSureBaslanic] <= kayitSayaci)
  {
    gpio_set_level(stepMotor_3[yon],HIGH);
    gpio_set_level(stepMotor_3[adim],durum);
  }
  /*                        Step Motor 3 Kaydedilen Değerle Saat Yönün TERSİNE Hareket Ediyor                */
  if(stepMotor3Kayit[motorSaatYonuTersi]==1 && stepMotor3Kayit[motorSaatYonuTersiSureBitis] >= kayitSayaci && stepMotor3Kayit[motorSaatYonuTersiSureBaslangic] <= kayitSayaci)
  {
    gpio_set_level(stepMotor_3[yon],LOW);
    gpio_set_level(stepMotor_3[adim],durum);
  }
  /*********************************************************************************************************************/
  /*                        Step Motor 4 Kaydedilen Değerle Saat Yönüne Hareket Ediyor                */
  if(stepMotor4Kayit[motorSaatYonu]==1 && stepMotor4Kayit[motorSaatYonuSureBitis] >= kayitSayaci && stepMotor4Kayit[motorSaatYonuSureBaslanic] <= kayitSayaci)
  {
    gpio_set_level(stepMotor_4[yon],HIGH);
    gpio_set_level(stepMotor_4[adim],durum);
  }
  /*                        Step Motor 4 Kaydedilen Değerle Saat Yönün TERSİNE Hareket Ediyor                */
  if(stepMotor4Kayit[motorSaatYonuTersi]==1 && stepMotor4Kayit[motorSaatYonuTersiSureBitis] >= kayitSayaci && stepMotor4Kayit[motorSaatYonuTersiSureBaslangic] <= kayitSayaci)
  {
    gpio_set_level(stepMotor_4[yon],LOW);
    gpio_set_level(stepMotor_4[adim],durum);
  }
  /*********************************************************************************************************************/
  /*                        Step Motor 5 Kaydedilen Değerle Saat Yönüne Hareket Ediyor                */
  if(stepMotor5Kayit[motorSaatYonu]==1 && stepMotor5Kayit[motorSaatYonuSureBitis] >= kayitSayaci && stepMotor5Kayit[motorSaatYonuSureBaslanic] <= kayitSayaci)
  {
    gpio_set_level(stepMotor_5[yon],HIGH);
    gpio_set_level(stepMotor_5[adim],durum);
  }
  /*                        Step Motor 5 Kaydedilen Değerle Saat Yönün TERSİNE Hareket Ediyor                */
  if(stepMotor5Kayit[motorSaatYonuTersi]==1 && stepMotor5Kayit[motorSaatYonuTersiSureBitis] >= kayitSayaci && stepMotor5Kayit[motorSaatYonuTersiSureBaslangic] <= kayitSayaci)
  {
    gpio_set_level(stepMotor_5[yon],LOW);
    gpio_set_level(stepMotor_5[adim],durum);
  }
  /*********************************************************************************************************************/
  /*                        Step Motor 6 Kaydedilen Değerle Saat Yönüne Hareket Ediyor                */
  if(stepMotor6Kayit[motorSaatYonu]==1 && stepMotor6Kayit[motorSaatYonuSureBitis] >= kayitSayaci && stepMotor6Kayit[motorSaatYonuSureBaslanic] <= kayitSayaci)
  {
    gpio_set_level(stepMotor_6[yon],HIGH);
    gpio_set_level(stepMotor_6[adim],durum);
  }
  /*                        Step Motor 6 Kaydedilen Değerle Saat Yönün TERSİNE Hareket Ediyor                */
  if(stepMotor6Kayit[motorSaatYonuTersi]==1 && stepMotor6Kayit[motorSaatYonuTersiSureBitis] >= kayitSayaci && stepMotor6Kayit[motorSaatYonuTersiSureBaslangic] <= kayitSayaci)
  {
    gpio_set_level(stepMotor_6[yon],LOW);
    gpio_set_level(stepMotor_6[adim],durum);
  }
  /*********************************************************************************************************************/
  /*                        Step Motor 7 Kaydedilen Değerle Saat Yönüne Hareket Ediyor                */
  if(stepMotor7Kayit[motorSaatYonu]==1 && stepMotor7Kayit[motorSaatYonuSureBitis] >= kayitSayaci && stepMotor7Kayit[motorSaatYonuSureBaslanic] <= kayitSayaci)
  {
    gpio_set_level(stepMotor_7[yon],HIGH);
    gpio_set_level(stepMotor_7[adim],durum);
  }
  /*                        Step Motor 2 Kaydedilen Değerle Saat Yönün TERSİNE Hareket Ediyor                */
  if(stepMotor7Kayit[motorSaatYonuTersi]==1 && stepMotor7Kayit[motorSaatYonuTersiSureBitis] >= kayitSayaci && stepMotor7Kayit[motorSaatYonuTersiSureBaslangic] <= kayitSayaci)
  {
    gpio_set_level(stepMotor_7[yon],LOW);
    gpio_set_level(stepMotor_7[adim],durum);
  }
}

void zamanlayiciAyarlari()
{
  zamanlayicim = timerBegin(0, 80, true);
  timerAttachInterrupt(zamanlayicim, &zamanlayiciDoldu, true);
  timerAlarmWrite(zamanlayicim, 1000, true);//bir milisaniyede kesmeye girecek 1000
  timerAlarmEnable(zamanlayicim); //aktif edildi

}

void pinAyarlamalari()
{
  pinMode(stepMotor_1[adim], OUTPUT);
  pinMode(stepMotor_1[yon], OUTPUT);
  pinMode(stepMotor_1[aktif], OUTPUT);

  pinMode(stepMotor_2[adim], OUTPUT);
  pinMode(stepMotor_2[yon], OUTPUT);
  pinMode(stepMotor_2[aktif], OUTPUT);

  pinMode(stepMotor_3[adim], OUTPUT);
  pinMode(stepMotor_3[yon], OUTPUT);
  pinMode(stepMotor_3[aktif], OUTPUT);

  pinMode(stepMotor_4[adim], OUTPUT);
  pinMode(stepMotor_4[yon], OUTPUT);
  pinMode(stepMotor_4[aktif], OUTPUT);

  pinMode(stepMotor_5[adim], OUTPUT);
  pinMode(stepMotor_5[yon], OUTPUT);
  pinMode(stepMotor_5[aktif], OUTPUT);

  pinMode(stepMotor_6[adim], OUTPUT);
  pinMode(stepMotor_6[yon], OUTPUT);
  pinMode(stepMotor_6[aktif], OUTPUT);

  pinMode(stepMotor_7[adim], OUTPUT);
  pinMode(stepMotor_7[yon], OUTPUT);
  pinMode(stepMotor_7[aktif], OUTPUT);

  pinMode(runLed, OUTPUT);
}

void aktifMotorButonlariAyarlamalari()
{
  //Step Motor 1
  if(EEPROM.readBool(0)==0 || EEPROM.readBool(0)!=0)
  {
    gpio_set_level(stepMotor_1[aktif], HIGH);
    stepMotorAktifPasifButonu(0x00,0x01);
  }
  if(EEPROM.readBool(0)==1)
  {
    gpio_set_level(stepMotor_1[aktif], LOW);
    stepMotorAktifPasifButonu(0x00,0x00);
  }
  //Step Motor 2
  if(EEPROM.readBool(1)==0 || EEPROM.readBool(1)!=0)
  {
    gpio_set_level(stepMotor_2[aktif], HIGH);
    stepMotorAktifPasifButonu(0x01,0x01);
  }
  if(EEPROM.readBool(1)==1)
  {
    gpio_set_level(stepMotor_2[aktif], LOW);
    stepMotorAktifPasifButonu(0x01,0x00);
  }
  //Step Motor 3
  if(EEPROM.readBool(2)==0 || EEPROM.readBool(2)!=0)
  {
    gpio_set_level(stepMotor_3[aktif], HIGH);
    stepMotorAktifPasifButonu(0x02,0x01);
  }
  if(EEPROM.readBool(2)==1)
  {
    gpio_set_level(stepMotor_3[aktif], LOW);
    stepMotorAktifPasifButonu(0x02,0x00);
  }
  //Step Motor 4
  if(EEPROM.readBool(3)==0 || EEPROM.readBool(3)!=0)
  {
    gpio_set_level(stepMotor_4[aktif], HIGH);
    stepMotorAktifPasifButonu(0x03,0x01);
  }
  if(EEPROM.readBool(3)==1)
  {
    gpio_set_level(stepMotor_4[aktif], LOW);
    stepMotorAktifPasifButonu(0x03,0x00);
  }
  //Step Motor 5
  if(EEPROM.readBool(4)==0 || EEPROM.readBool(4)!=0)
  {
    gpio_set_level(stepMotor_5[aktif], HIGH);
    stepMotorAktifPasifButonu(0x04,0x01);
  }
  if(EEPROM.readBool(4)==1)
  {
    gpio_set_level(stepMotor_5[aktif], LOW);
    stepMotorAktifPasifButonu(0x04,0x00);
  }
  //Step Motor 6
  if(EEPROM.readBool(5)==0 || EEPROM.readBool(5)!=0)
  {
    gpio_set_level(stepMotor_6[aktif], HIGH);
    stepMotorAktifPasifButonu(0x05,0x01);
  }
  if(EEPROM.readBool(5)==1)
  {
    gpio_set_level(stepMotor_6[aktif], LOW);
    stepMotorAktifPasifButonu(0x05,0x00);
  }
  //Step Motor 7
  if(EEPROM.readBool(6)==0 || EEPROM.readBool(6)!=0)
  {
    gpio_set_level(stepMotor_7[aktif], HIGH);
    stepMotorAktifPasifButonu(0x06,0x01);
  }
  if(EEPROM.readBool(6)==1)
  {
    gpio_set_level(stepMotor_7[aktif], LOW);
    stepMotorAktifPasifButonu(0x06,0x00);
  }
}

void akifKontrolu()
{
  /*
    Buffer[0]=0x5a;
    Buffer[1]=0xa5;
    Buffer[2]=0x05;//yazma için 0x05 okuma için 0x06
    Buffer[3]=0x82;//yazma için 0x82 okuma için 0x83
    Buffer[4]=0x30;
    Buffer[5]=0x00;
    Buffer[6]=0x00;//buraya 0x01 yazılırsa direk butonu iptal ediyor
    Buffer[7]=0x00;
    Buffer[8]=0x01;
    dwin.write(Buffer,9);
*/
    //Buffer =0x5a 0xa5 0x05 0x82 0x30 0x00 0x00 0x00 0x01 yazılırsa değiştiriyor

    

    //************************Step Motorların aktif veya pasif edilmesi**************************
    //Step Motor 1
    if(Buffer[4]==0x30 && Buffer[5]==0x00 && Buffer[8]==0x01)
    {
      //PASİF EDİLDİ
      gpio_set_level(stepMotor_1[aktif],HIGH);
      EEPROM.writeBool(0,0);
      EEPROM.commit();
      
    }
    if(Buffer[4]==0x30 && Buffer[5]==0x00 && Buffer[8]==0x00)
    {
      //AKTİF EDİLDİ
      gpio_set_level(stepMotor_1[aktif],LOW);
      EEPROM.writeBool(0,1);
      EEPROM.commit();
    }
    //Step Motor 2
    if(Buffer[4]==0x30 && Buffer[5]==0x01 && Buffer[8]==0x01)
    {
      //Step Motor 2 PASİF EDİLDİ
      gpio_set_level(stepMotor_2[aktif],HIGH);
      EEPROM.writeBool(1,0);
      EEPROM.commit();
    }
    if(Buffer[4]==0x30 && Buffer[5]==0x01 && Buffer[8]==0x00)
    {
      //AKTİF EDİLDİ
      gpio_set_level(stepMotor_2[aktif],LOW);
      EEPROM.writeBool(1,1);
      EEPROM.commit();
    }
    //Step Motor 3
    if(Buffer[4]==0x30 && Buffer[5]==0x02 && Buffer[8]==0x01)
    {
      //PASİF EDİLDİ
      gpio_set_level(stepMotor_3[aktif],HIGH);
      EEPROM.writeBool(2,0);
      EEPROM.commit();
    }
    if(Buffer[4]==0x30 && Buffer[5]==0x02 && Buffer[8]==0x00)
    {
      //AKTİF EDİLDİ
      gpio_set_level(stepMotor_3[aktif],LOW);
      EEPROM.writeBool(2,1);
      EEPROM.commit();
    }
    //Step Motor 4
    if(Buffer[4]==0x30 && Buffer[5]==0x03 && Buffer[8]==0x01)
    {
      //PASİF EDİLDİ
      gpio_set_level(stepMotor_4[aktif],HIGH);
      EEPROM.writeBool(3,0);
      EEPROM.commit();
    }
    
    if(Buffer[4]==0x30 && Buffer[5]==0x03 && Buffer[8]==0x00)
    {
      //AKTİF EDİLDİ
      gpio_set_level(stepMotor_4[aktif],LOW);
      EEPROM.writeBool(3,1);
      EEPROM.commit();
    
    }  
    //Step Motor 5
    if(Buffer[4]==0x30 && Buffer[5]==0x04 && Buffer[8]==0x01)
    {
      //PASİF EDİLDİ
      gpio_set_level(stepMotor_5[aktif],HIGH);
      EEPROM.writeBool(4,0);
      EEPROM.commit();
    }
    
    if(Buffer[4]==0x30 && Buffer[5]==0x04 && Buffer[8]==0x00)
    {
      //AKTİF EDİLDİ
      gpio_set_level(stepMotor_5[aktif],LOW);
      EEPROM.writeBool(4,1);
      EEPROM.commit();
    }
    //Step Motor 6
    if(Buffer[4]==0x30 && Buffer[5]==0x05 && Buffer[8]==0x01)
    {
      //PASİF EDİLDİ
      gpio_set_level(stepMotor_6[aktif],HIGH);
      EEPROM.writeBool(5,0);
      EEPROM.commit();
    }
    
    if(Buffer[4]==0x30 && Buffer[5]==0x05 && Buffer[8]==0x00)
    { 
      //AKTİF EDİLDİ
      gpio_set_level(stepMotor_6[aktif],LOW);
      EEPROM.writeBool(5,1);
      EEPROM.commit();
    }
    //Step Motor 7
    if(Buffer[4]==0x30 && Buffer[5]==0x06 && Buffer[8]==0x01)
    {
      //PASİF EDİLDİ
      gpio_set_level(stepMotor_7[aktif],HIGH);
      EEPROM.writeBool(6,0);
      EEPROM.commit();
    }
    
    if(Buffer[4]==0x30 && Buffer[5]==0x06 && Buffer[8]==0x00)
    {
      //AKTİF EDİLDİ
      gpio_set_level(stepMotor_7[aktif],LOW);
      EEPROM.writeBool(6,1);
      EEPROM.commit();
    }

}

void stepMotorAktifPasifButonu(uint8_t adres,uint8_t deger)
{
  Buffer[0]=0x5a;
  Buffer[1]=0xa5;
  Buffer[2]=0x05;//yazma için 0x05 okuma için 0x06
  Buffer[3]=0x82;//yazma için 0x82 okuma için 0x83
  Buffer[4]=0x30;
  Buffer[5]=adres;
  Buffer[6]=0x00;//buraya 0x01 yazılırsa direk butonu iptal ediyor
  Buffer[7]=deger;
  Buffer[8]=0x00;

  dwin.write(Buffer,9);
}

void hangiMotorlarDonecek()
{
    //Step motor 1 saat yönüne dönüyor
    if(Buffer[4]==0x20 && Buffer[5]==0x00 && Buffer[8]==0x01)
    {
      if (stepMotor1SaatYonuTersiMi==1)
        stepMotorDonusYonuAktifPasifButonu(0x10,0);
      stepMotor1SaatYonuMu=1;
      stepMotor1SaatYonuTersiMi=0;
      gpio_set_level(stepMotor_1[yon], HIGH);
    }

    if(Buffer[4]==0x20 && Buffer[5]==0x00 && Buffer[8]==0x00)
      stepMotor1SaatYonuMu=0;
    
    //Step Motor 1 saat yönü tersine dönüyor
    if(Buffer[4]==0x20 && Buffer[5]==0x10 && Buffer[8]==0x01)
    {
      if (stepMotor1SaatYonuMu==1)
        stepMotorDonusYonuAktifPasifButonu(0x00,0);
      stepMotor1SaatYonuTersiMi=1;
      stepMotor1SaatYonuMu=0;
      gpio_set_level(stepMotor_1[yon], LOW);
    }
    if(Buffer[4]==0x20 && Buffer[5]==0x10 && Buffer[8]==0x00)
      stepMotor1SaatYonuTersiMi=0;

    //****************************************************************Step motor 2 saat yönüne dönüyor
    if(Buffer[4]==0x20 && Buffer[5]==0x01 && Buffer[8]==0x01)
    {
      if (stepMotor2SaatYonuTersiMi==1)
        stepMotorDonusYonuAktifPasifButonu(0x11,0);
      stepMotor2SaatYonuMu=1;
      stepMotor2SaatYonuTersiMi=0;
      gpio_set_level(stepMotor_2[yon], HIGH);
    }

    if(Buffer[4]==0x20 && Buffer[5]==0x01 && Buffer[8]==0x00)
      stepMotor2SaatYonuMu=0;
    
    //***************************************************************Step Motor 2 saat yönü tersine dönüyor
    if(Buffer[4]==0x20 && Buffer[5]==0x11 && Buffer[8]==0x01)
    {
      if (stepMotor2SaatYonuMu==1)
        stepMotorDonusYonuAktifPasifButonu(0x01,0);
      stepMotor2SaatYonuTersiMi=1;
      stepMotor2SaatYonuMu=0;
      gpio_set_level(stepMotor_2[yon], LOW);
    }
    if(Buffer[4]==0x20 && Buffer[5]==0x11 && Buffer[8]==0x00)
      stepMotor2SaatYonuTersiMi=0;

    //****************************************************************Step motor 3 saat yönüne dönüyor
    if(Buffer[4]==0x20 && Buffer[5]==0x02 && Buffer[8]==0x01)
    {
      if (stepMotor3SaatYonuTersiMi==1)
        stepMotorDonusYonuAktifPasifButonu(0x12,0);
      stepMotor3SaatYonuMu=1;
      stepMotor3SaatYonuTersiMi=0;
      gpio_set_level(stepMotor_3[yon], HIGH);
    }

    if(Buffer[4]==0x20 && Buffer[5]==0x02 && Buffer[8]==0x00)
      stepMotor3SaatYonuMu=0;
    
    //***************************************************************Step Motor 3 saat yönü tersine dönüyor
    if(Buffer[4]==0x20 && Buffer[5]==0x12 && Buffer[8]==0x01)
    {
      if (stepMotor3SaatYonuMu==1)
        stepMotorDonusYonuAktifPasifButonu(0x02,0);
      stepMotor3SaatYonuTersiMi=1;
      stepMotor3SaatYonuMu=0;
      gpio_set_level(stepMotor_3[yon], LOW);
    }
    if(Buffer[4]==0x20 && Buffer[5]==0x12 && Buffer[8]==0x00)
      stepMotor3SaatYonuTersiMi=0;

    //****************************************************************Step motor 4 saat yönüne dönüyor
    if(Buffer[4]==0x20 && Buffer[5]==0x03 && Buffer[8]==0x01)
    {
      if (stepMotor4SaatYonuTersiMi==1)
        stepMotorDonusYonuAktifPasifButonu(0x13,0);
      stepMotor4SaatYonuMu=1;
      stepMotor4SaatYonuTersiMi=0;
      gpio_set_level(stepMotor_4[yon], HIGH);
    }

    if(Buffer[4]==0x20 && Buffer[5]==0x03 && Buffer[8]==0x00)
      stepMotor4SaatYonuMu=0;
    
    //***************************************************************Step Motor 4 saat yönü tersine dönüyor
    if(Buffer[4]==0x20 && Buffer[5]==0x13 && Buffer[8]==0x01)
    {
      if (stepMotor4SaatYonuMu==1)
        stepMotorDonusYonuAktifPasifButonu(0x03,0);
      stepMotor4SaatYonuTersiMi=1;
      stepMotor4SaatYonuMu=0;
      gpio_set_level(stepMotor_4[yon], LOW);
    }
    if(Buffer[4]==0x20 && Buffer[5]==0x13 && Buffer[8]==0x00)
      stepMotor4SaatYonuTersiMi=0;

    //****************************************************************Step motor 5 saat yönüne dönüyor
    if(Buffer[4]==0x20 && Buffer[5]==0x04 && Buffer[8]==0x01)
    {
      if (stepMotor5SaatYonuTersiMi==1)
        stepMotorDonusYonuAktifPasifButonu(0x14,0);
      stepMotor5SaatYonuMu=1;
      stepMotor5SaatYonuTersiMi=0;
      gpio_set_level(stepMotor_5[yon], HIGH);
    }

    if(Buffer[4]==0x20 && Buffer[5]==0x04 && Buffer[8]==0x00)
      stepMotor5SaatYonuMu=0;
    
    //***************************************************************Step Motor 5 saat yönü tersine dönüyor
    if(Buffer[4]==0x20 && Buffer[5]==0x14 && Buffer[8]==0x01)
    {
      if (stepMotor5SaatYonuMu==1)
        stepMotorDonusYonuAktifPasifButonu(0x04,0);
      stepMotor5SaatYonuTersiMi=1;
      stepMotor5SaatYonuMu=0;
      gpio_set_level(stepMotor_5[yon], LOW);
    }
    if(Buffer[4]==0x20 && Buffer[5]==0x14 && Buffer[8]==0x00)
      stepMotor5SaatYonuTersiMi=0;
    
    //****************************************************************Step motor 6 saat yönüne dönüyor
    if(Buffer[4]==0x20 && Buffer[5]==0x05 && Buffer[8]==0x01)
    {
      if (stepMotor6SaatYonuTersiMi==1)
        stepMotorDonusYonuAktifPasifButonu(0x15,0);
      stepMotor6SaatYonuMu=1;
      stepMotor6SaatYonuTersiMi=0;
      gpio_set_level(stepMotor_6[yon], HIGH);
    }

    if(Buffer[4]==0x20 && Buffer[5]==0x05 && Buffer[8]==0x00)
      stepMotor6SaatYonuMu=0;
    
    //***************************************************************Step Motor 6 saat yönü tersine dönüyor
    if(Buffer[4]==0x20 && Buffer[5]==0x15 && Buffer[8]==0x01)
    {
      if (stepMotor6SaatYonuMu==1)
        stepMotorDonusYonuAktifPasifButonu(0x05,0);
      stepMotor6SaatYonuTersiMi=1;
      stepMotor6SaatYonuMu=0;
      gpio_set_level(stepMotor_6[yon], LOW);
    }
    if(Buffer[4]==0x20 && Buffer[5]==0x15 && Buffer[8]==0x00)
      stepMotor6SaatYonuTersiMi=0;

      //****************************************************************Step motor 7 saat yönüne dönüyor
    if(Buffer[4]==0x20 && Buffer[5]==0x06 && Buffer[8]==0x01)
    {
      if (stepMotor7SaatYonuTersiMi==1)
        stepMotorDonusYonuAktifPasifButonu(0x16,0);
      stepMotor7SaatYonuMu=1;
      stepMotor7SaatYonuTersiMi=0;
      gpio_set_level(stepMotor_7[yon], HIGH);
    }

    if(Buffer[4]==0x20 && Buffer[5]==0x06 && Buffer[8]==0x00)
      stepMotor7SaatYonuMu=0;
    
    //***************************************************************Step Motor 7 saat yönü tersine dönüyor
    if(Buffer[4]==0x20 && Buffer[5]==0x16 && Buffer[8]==0x01)
    {
      if (stepMotor7SaatYonuMu==1)
        stepMotorDonusYonuAktifPasifButonu(0x06,0);
      stepMotor7SaatYonuTersiMi=1;
      stepMotor7SaatYonuMu=0;
      gpio_set_level(stepMotor_7[yon], LOW);
    }
    if(Buffer[4]==0x20 && Buffer[5]==0x16 && Buffer[8]==0x00)
      stepMotor7SaatYonuTersiMi=0;
    

}

void slideDuzenle(uint32_t deger)
{
  Buffer[0]=0x5a;
  Buffer[1]=0xa5;
  Buffer[2]=0x05;//yazma için 0x05 okuma için 0x06
  Buffer[3]=0x82;//yazma için 0x82 okuma için 0x83
  Buffer[4]=0x50;
  Buffer[5]=0x00;
  Buffer[6]=0x00;//buraya 0x01 yazılırsa direk butonu iptal ediyor
  Buffer[7]=deger>>8;
  Buffer[8]=deger & 0xff;

  dwin.write(Buffer,9);
}

void stepMotorDonusYonuAktifPasifButonu(uint8_t adres,uint8_t deger)
{
  Buffer[0]=0x5a;
  Buffer[1]=0xa5;
  Buffer[2]=0x05;//yazma için 0x05 okuma için 0x06
  Buffer[3]=0x82;//yazma için 0x82 okuma için 0x83
  Buffer[4]=0x20;
  Buffer[5]=adres;
  Buffer[6]=0x00;//buraya 0x01 yazılırsa direk butonu iptal ediyor
  Buffer[7]=0x00;
  Buffer[8]=deger;

  dwin.write(Buffer,9);
  delay(1);
}

void ekranMotorDonusIkonlariDurdur()
{
  stepMotorDonusYonuAktifPasifButonu(0x00,0);
  stepMotorDonusYonuAktifPasifButonu(0x01,0);
  stepMotorDonusYonuAktifPasifButonu(0x02,0);
  stepMotorDonusYonuAktifPasifButonu(0x03,0);
  stepMotorDonusYonuAktifPasifButonu(0x04,0);
  stepMotorDonusYonuAktifPasifButonu(0x05,0);
  stepMotorDonusYonuAktifPasifButonu(0x06,0);
  stepMotorDonusYonuAktifPasifButonu(0x10,0);
  stepMotorDonusYonuAktifPasifButonu(0x11,0);
  stepMotorDonusYonuAktifPasifButonu(0x12,0);
  stepMotorDonusYonuAktifPasifButonu(0x13,0);
  stepMotorDonusYonuAktifPasifButonu(0x14,0);
  stepMotorDonusYonuAktifPasifButonu(0x15,0);
  stepMotorDonusYonuAktifPasifButonu(0x16,0);
}

// Código fuente del "Programa de avisos para personas con discapacidades del tipo medio y bajo en Arduino". 
// Trabajo de Fin de Grado de Ingeniería en Informática de Francisco José Bérchez Ruiz.
// Universidad Internacional de la Rioja - UNIR.
// Marzo 2024.
// LICENCIA: GNU LICENCIA GENERAL PÚBLICA Version 2.

// PROGRAMA PARA EL CUARTO DE BAÑO

#include "SoftwareSerial.h"  //Librería para la Voz y DFPLAYER
#include "DFRobotDFPlayerMini.h" //Librería para la Voz y DFPLAYER
#include <Wire.h> //funciones para comunicar Arduino con reloj RTC
#include <SPI.h> //funciones de comunicación entre dispositivos
#include <RTClib.h> //funciones del reloj RTC
#include <RTC_DS3231.h> //funciones del reloj RTC
#include <LiquidCrystal.h>
#include "Arduino.h"
#include "notas.h"; //notas de música para el buzzer

// LEDS
#define BLUEB 9 
//Para poner más LEDS:
//#define BLUEA 5
//#define BLUEC 3
//#define BLUED 2

// ULTRASONIDOS
const int pinEcho = 12; 
const int pinTrigger = 13; 
const int distanciaAlarma = 20; // Valor en centímetros

// BUZZER
 int pinbuzzer=6; //El buzzer esta conectado en el pin 6

 //RFID
 int pinSDA=10; // conexion pin SDA/NSS en RFID
 int pinRST=9;  // conexion pin RST en RIFID 

//LCD
 int pinRS=2;
 int pinE=3;
 int pinD4=4;
 int pinD5=5;
 int pinD6=7;
 int pinD7=8;
 
String dia;

// Inicialización DFPLAYER
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

// Inicialización DS3231 RTC;
RTC_DS3231 RTC;

// Inicialización LCD
LiquidCrystal lcd(pinRS,pinE,pinD4,pinD5,pinD6,pinD7);

void setup() 
{
// Inicialización LEDS
pinMode(BLUEB, OUTPUT);
//pinMode(BLUEA, OUTPUT); Para añadir más leds
//pinMode(BLUEC, OUTPUT);
//pinMode(BLUED, OUTPUT);
digitalWrite(BLUEB, LOW);
//digitalWrite(BLUEA, LOW); Para añadir más leds
//digitalWrite(BLUEC, LOW);
//digitalWrite(BLUED, LOW);

// ULTRASONIDOS
// Inicialización de la consola
Serial.begin(9600);
// Configura los pines sensor ULTRASONIDOS
pinMode(pinEcho, INPUT);
pinMode(pinTrigger, OUTPUT);

//Se inicicializa DFPLAYER MP3 Y LCD
mySoftwareSerial.begin(9600);
Serial.begin(9600);

// Se inicializan TODOS los dispositivos y se comprueba que estan conectados  
Wire.begin();
RTC.begin();

//COMIENZO DE LA EJECUCIÓN
lcd.begin(16,2);
lcd.setCursor(0,0);
lcd.write("BANO ON - UNIR");
lcd.setCursor(0,1);
lcd.write("Fran. Berchez R.");
delay(2000);
lcd.clear();
tonomensaje();
mostrarmensajes("BANO - NO OLVIDAR","CERRAR GRIFOS");  
delay(3000);
lcd.clear();

Serial.println();
Serial.println(F("INICIALIZANDO DFPLAYER... (DE 3 A 5 Segundos)"));

delay(550);
if (!myDFPlayer.begin(mySoftwareSerial)) // Si no se inicializa
   { 
    Serial.println(F("Imposible comenzar"));
    Serial.println(F("1.Chequea conexión!"));
    Serial.println(F("2.Inserta SD Card!"));
    mostrarmensajes("DFplayer", "NO online");  
    delay(500);
    lcd.clear();
    while(true);
  }
Serial.println(F("DFPlayer Online."));
myDFPlayer.setTimeOut(500); //Set serial communication time out 500ms
// Configuracion Volumen
myDFPlayer.volume(20);  //Configurar Volumen(0~30).
myDFPlayer.volumeUp(); //Subir Volume
myDFPlayer.volumeDown(); //Bajar Volume
//Configuración del tipo de Equalizador DFPLAYER
myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
//En DFPLAYER configurar nuestro SD como dispositivo por defecto
myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
//LECTURA DE TODA LA INFORMACIÓN DEL DFPLAYER
Serial.println(myDFPlayer.readState()); // Lee estado del MP3
Serial.println(myDFPlayer.readVolume()); // Lee el volumen actual
Serial.println(myDFPlayer.readEQ()); // Lee configuración del Equalizador
Serial.println(myDFPlayer.readFileCounts()); // Leer todos los recuentos de archivos en la tarjeta SD
Serial.println(myDFPlayer.readCurrentFileNumber()); // lee  el número del archivo de reproducción actual
Serial.println(myDFPlayer.readFileCountsInFolder(3)); // lee recuentos llenos en la carpeta SD:/03
lcd.clear();

//Inicializamos el RELOJ
RTC.adjust(DateTime(__DATE__, __TIME__)); /*Si lo queremos en automático*/
RTC.adjust(DateTime(2024, 02, 10, 18, 05, 00)); /*Si lo queremos en MANUAL*/
if (! RTC.isrunning()) {
    mostrarmensajes("RTC", "NO FUNCIONA");  
    delay(500);
    lcd.clear();
    Serial.println("RTC is NOT running!");
    // Ajustar la hora del RTC a la hora de compilacion y carga.
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}

void loop() 
{
  // Toma Hora del RTC.
DateTime now = RTC.now();

//Imprime el mensaje detallado de DFPlayer para manejar diferentes errores y estados.
if (myDFPlayer.available()) {
printDetail(myDFPlayer.readType(), myDFPlayer.read()); 
}

// Mensajes de Depuración y control
mostrarmensajes("EN PROCESO","ULTRASONIDOS");  
delay(200);
   
// ULTRASONIDOS, Iniciamos lectura de la distancia
int distancia = sensor(pinTrigger, pinEcho);
if (distancia <= distanciaAlarma)
{
luces();
//Muesta la Hora
lcd.clear();
lcd.setCursor(0,0);
if (now.day()<10){
lcd.print(0);}
lcd.print(now.day());
lcd.setCursor(2,0);
lcd.print("/");
if (now.month()<10){
lcd.print(0);}
lcd.print(now.month());
lcd.print("/");
lcd.setCursor(6,0);
lcd.print(now.year());
lcd.print(" ");
lcd.print(diasemana(now.dayOfWeek()));    
lcd.setCursor(0,1);
if (now.hour()<10){
lcd.print(0);}
lcd.print(now.hour());
lcd.print(":");
if (now.minute()<10){
lcd.print(0);}
lcd.print(now.minute());
lcd.print(":");
if (now.second()<10){
lcd.print(0);}
lcd.print(now.second());
delay(800);

// Presenta los MENSAJES EN CASO DE DETECCIÓN DE LA PERSONA
Serial.println("Persona detectada");
luces();
mostrarmensajes("SI USAS EL ASEO","CERRAR LOS GRIFOS");
myDFPlayer.playFolder(15, 1); 
delay(3000);  

tonomensaje();
luces();
mostrarmensajes("SI USAS EL WC","TIRAR DE LA CISTERNA");
myDFPlayer.playFolder(15, 2); 
delay(3000); 
  if (now.hour()>=7 && now.hour()<=10){
    tonomensaje();
    luces();
    mostrarmensajes("HORA DE ASEARSE","LAVAR DIENTES Y DUCHARME");
    //Ejecutar MP3 específico en SD:/15/001.mp3; Carpeta de nombre(1~99); Fichero de nombre(1~255)
    //Para los próximos MP3, ir modificando el número del fichero: 002.mp3, 003.mp3, 004.mp3...
    myDFPlayer.playFolder(15, 3);
    delay(3000);
    }
       else{
         if (now.hour()>14 && now.hour()<=16){
           tonomensaje();
           luces();
           mostrarmensajes("LAVAR DIENTES","DESPUES DE COMER");
           myDFPlayer.playFolder(15, 4); 
           delay(3000);
           }
          else{
            if (now.hour()>=17 && now.hour()<=19){
               tonomensaje();
               luces();
               mostrarmensajes("LAVAR DIENTES","DESPUES DE MERENDAR");
               myDFPlayer.playFolder(15, 5); 
               delay(3000);
               }
               else{
                  if (now.hour()>22 && now.hour()<=24){
                      tonomensaje();
                      luces();
                      mostrarmensajes("LAVAR LOS DIENTES","DESPUÉS DE CENAR");
                      myDFPlayer.playFolder(15, 6); 
                      delay(3000);
                      }
                  }
              }
     }
delay(1000); //Tiempo hasta volver a leer identificador
}

// Presenta los MENSAJES GENERALES EN TODAS LAS PANTALLAS  EN EL CASO DE NO DETECCIÓN DE PERSONA
// Muestra la Hora.
lcd.clear();
lcd.setCursor(0,0);
if (now.day()<10){
lcd.print(0);}
lcd.print(now.day());
lcd.setCursor(2,0);
lcd.print("/");
if (now.month()<10){
lcd.print(0);}
lcd.print(now.month());
lcd.print("/");
lcd.setCursor(6,0);
lcd.print(now.year());
lcd.print(" ");
lcd.print(diasemana(now.dayOfWeek()));    
lcd.setCursor(0,1);
if (now.hour()<10){
lcd.print(0);}
lcd.print(now.hour());
lcd.print(":");
if (now.minute()<10){
lcd.print(0);}
lcd.print(now.minute());
lcd.print(":");
if (now.second()<10){
lcd.print(0);}
lcd.print(now.second());
delay(1000);

if (now.hour()==8 && now.minute()==0){
  tonomensaje();
  luces();
  mostrarmensajes("Son las 8","HORA DE IR A TRABAJAR");
  myDFPlayer.playFolder(15, 7); 
  delay(3000);
  }
  if (now.hour()==10 && now.minute()==0){
  tonomensaje();
  luces();
  mostrarmensajes("Telf Emergencias","957 000 000");
  myDFPlayer.playFolder(15, 8); 
  delay(3000);
  }
  if (now.hour()==10 && now.minute()==30){
  tonomensaje();
  luces();
  mostrarmensajes("Son las 10:30","HORA DUCHARSE");
  myDFPlayer.playFolder(15, 9); 
  delay(3000);
  }
  if (now.hour()==11 && now.minute()==0){
  tonomensaje();
  luces();
  mostrarmensajes("Son las 11","VENTILA HABITACIONES");
  myDFPlayer.playFolder(15, 10); 
  delay(3000);
  }
  if (now.hour()==11 && now.minute()==30){
  tonomensaje();
  luces();
  mostrarmensajes("Son las 11:30","PONER LAVADORA");
  myDFPlayer.playFolder(15, 11); 
  delay(3000);
  }
  if (now.hour()==12 && now.minute()==0){
  tonomensaje();
  luces();
  mostrarmensajes("Telf Emergencias","957 000 000");
  myDFPlayer.playFolder(15, 12); 
  delay(3000);
  }
  if (now.hour()==12 && now.minute()==30){
  tonomensaje();
  luces();
  mostrarmensajes("Son las 12:30","LLAMA A LA FAMILIA");
  myDFPlayer.playFolder(15, 13); 
  delay(3000);
  }
  if (now.hour()==12 && now.minute()==45){
  tonomensaje();
  luces();
  mostrarmensajes("Son las 12:45","HORA DE MISA");
  myDFPlayer.playFolder(15, 14); 
  delay(3000);
  }
  if (now.hour()==14 && now.minute()==0){
  tonomensaje();
  luces();
  mostrarmensajes("Son las 2:00","HORA DE HACER LA COMIDA");
  myDFPlayer.playFolder(15, 15); 
  delay(3000);
  }
  if (now.hour()==15 && now.minute()==0){
  tonomensaje();
  luces();
  mostrarmensajes("Telf Emergencias","957 000 000");
  myDFPlayer.playFolder(15, 16); 
  delay(3000);
  }
  if (now.hour()==17 && now.minute()==0){
  tonomensaje();
  luces();
  mostrarmensajes("Son las 5:00","TE APETECE MERENDAR?");
  myDFPlayer.playFolder(15, 17); 
  delay(3000);
  }
  if (now.hour()==20 && now.minute()==0){
  tonomensaje();
  luces();
  mostrarmensajes("Son la 8:00h","PREPARAR LA CENA");
  myDFPlayer.playFolder(15, 18); 
  delay(3000);
  }
  if (now.hour()==21 && now.minute()==30){
  tonomensaje();
  luces();
  mostrarmensajes("Son las 9:30","COMIENZA PROGRAMA FAVORITO");
  myDFPlayer.playFolder(15, 19); 
  delay(3000);
  }
  if (now.hour()==22 && now.minute()==0){
  tonomensaje();
  luces();
  mostrarmensajes("Mañana","CITA MÉDICA");
  myDFPlayer.playFolder(15, 20); 
  delay(3000);
  }
  if (now.hour()==22 && now.minute()==15){
  tonomensaje();
  luces();
  mostrarmensajes("Telf Emergencias","957 000 000");
  myDFPlayer.playFolder(15, 21); 
  delay(3000);
  }
  if (now.hour()==22 && now.minute()==30){
  tonomensaje();
  luces();
  mostrarmensajes("Asegura","PUERTAS Y VENTANAS");
  myDFPlayer.playFolder(15, 22); 
  delay(3000);
  }
  if (now.hour()==23 && now.minute()==55){
  tonomensaje();
  luces();
  mostrarmensajes("Es tarde","HORA DE ACOSTARSE");
  myDFPlayer.playFolder(15, 23); 
  delay(3000);
  }
delay(800);
}

void tonomensaje(){
int melody[] = {
NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};
// Duracion de las notas: 8 = octava, 4 = cuarta...
int noteDurations[] = {
        4, 8, 8, 4,4,4,4,4 };
// Leer las notas
for (int thisNote = 0; thisNote < 8; thisNote++) {
int noteDuration = 1000/noteDurations[thisNote];
tone(pinbuzzer, melody[thisNote],noteDuration);
// Retardo 30%
int pauseBetweenNotes = noteDuration * 1.30;
delay(pauseBetweenNotes);
// Parar notas
noTone(pinbuzzer);   
noTone(pinbuzzer);  
  }
}
 
void mostrarmensajes(String m1, String m2){
for (int i=0;i<3;i++){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(m1);
  lcd.setCursor(0,1);
  lcd.print(m2);
  delay(2000);}
 }

String diasemana(int numdia){
  switch (numdia){
      case 1: dia="LUNES";
              break;
      case 2: dia="MARTES";
              break;
      case 3: dia="MIERCOLES";
              break;
      case 4: dia="JUEVES";
              break;
      case 5: dia="VIERNES";
              break;
      case 6: dia="SABADO";
              break;
      case 7: dia="DOMINGO";
              break;
      default: dia="ERROR";
              break;
    } 
return dia;
}

// Función que Imprime el estado o errores del DFPLAYER
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Se acabó el tiempo!"));
      break;
    case WrongStack:
      Serial.println(F("Mal apilamiento!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Tarjeta insertada!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Tarjeta removida!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Tarjeta en línea!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Número:"));
      Serial.print(value);
      Serial.println(F("Reproducción finalizada!"));
      break;
    case DFPlayerError:
      Serial.print(F("Error DFPlayer:"));
      switch (value) {
        case Busy:
          Serial.println(F("Tarjeta no encontrada"));
          break;
        case Sleeping:
          Serial.println(F("Durmiendo"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Fichero no encontrado"));
          break;
        case Advertise:
          Serial.println(F("En publicidad"));
          break;
        default:
          break;
      }
      break;
    default:
  break;
  }
}

// Función para los LEDS
void luces()
{
for(int i=0; i<10; i++)
{
//digitalWrite(BLUEA, HIGH);
digitalWrite(BLUEB, HIGH);
//digitalWrite(BLUEC, HIGH);
//digitalWrite(BLUED, HIGH);
delay(120);
//digitalWrite(BLUEA, LOW);
digitalWrite(BLUEB, LOW);
//digitalWrite(BLUEC, LOW);
//digitalWrite(BLUED, LOW);
delay(120);
}
}

//Función para los ULTRASONIDOS
int sensor(int pinTrigger, int pinEcho) 
{
long duracionPulso, distancia;
// Prepara el sensor enviando un pulso bajo al trigger
digitalWrite(pinTrigger, LOW);
delayMicroseconds(5);
// Iniciamos el sensor enviando un pulso alto
// al pin trigger por un lapso de 10uS
digitalWrite(pinTrigger, HIGH);
delayMicroseconds(10);
// Finaliza el pulso de trigger
digitalWrite(pinTrigger, LOW);
// Tomamos medida del ancho del pulso en el pin echo
duracionPulso = pulseIn(pinEcho, HIGH);
// La lectura del pulso está en microsegundos
// Obtenemos lectura de distancia en centímetros
distancia = duracionPulso / 58;  
return distancia;
} 
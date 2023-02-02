
#include<ESP8266WiFi.h>
#include <DHT.h>    // importa la Librerias DHT
#include <DHT_U.h>
#include <Servo.h>

#define D0 16
#define D1 5
#define D2 4
#define D3 0     //Pin FLASH, Trae problemas al inicio, usar con lógica negativa.
#define D4 2
#define D5 14
#define D6 12
#define D7 13   
#define D8 15   //Pin TX2, usar lógica negativa.
#define SD2 9
#define SD3 10
#define RX 3

const char ssid[]="TeleCentro-945b"; //Ingresar nombre de la red.
const char password[]="GDNLLTNXGDJT"; //Ingresar contraseña.

const byte PinLED= D1;
const byte SensorGas= D5; //Pin D5
const byte SensorTempHume = D4;  //Pin D4
const byte PINSERVO= D2; //Pin D2
const byte SensorPIR= D8;
const byte Buzzer= D6;
const byte Extractor= D7;
const byte SensorLDR=SD3;
const byte luzExterior=D3;

int estado=LOW;
int i=0;
int j=0;
float hume=0;
float temp=0;
float angl=0;
char* Title="ESP8266 Server";
char* alarmaTitle="Alarma inactiva";
char* luzTitle="Luces exteriores manuales";

volatile boolean AlarmaGas=false;
volatile boolean AlarmaPIR=false;
volatile boolean alarmaActiva=false;
volatile boolean luzExteriorAuto=false;

WiFiServer server(80);
DHT dht(SensorTempHume, DHT11);   

/*________________________________________________________________________________________________________________*/

void ICACHE_RAM_ATTR ISRoutine ();
void ICACHE_RAM_ATTR ISRoutine2 ();
void ICACHE_RAM_ATTR ISRoutine3 ();

void leerTemperatura()
{
  hume=dht.readHumidity();
  temp=dht.readTemperature();
 // Serial.println("Temperatura="+String(temp)+", Humedad="+String(hume));
}

/*________________________________________________________________________________________________________________*/

void setup()
{
  Serial.begin(9600);
  dht.begin();      // inicializacion de sensor
  pinMode(PinLED, OUTPUT);
  pinMode(luzExterior, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Extractor, OUTPUT);
  pinMode(PINSERVO, OUTPUT);
  digitalWrite(PinLED, LOW);
  digitalWrite(Buzzer, LOW);
  digitalWrite(Extractor, LOW);
  digitalWrite(luzExterior, HIGH);
  pinMode(SensorGas, INPUT);
  pinMode(SensorPIR, INPUT);
  pinMode(SensorLDR, INPUT);
  attachInterrupt(digitalPinToInterrupt(SensorGas),ISRoutine,FALLING);
  attachInterrupt(digitalPinToInterrupt(SensorPIR),ISRoutine2,RISING);
  attachInterrupt(digitalPinToInterrupt(SensorLDR),ISRoutine3,CHANGE);
  WiFi.begin(ssid, password);
  Serial.printf("\n\n Conectando a la red: %s\n", WiFi.SSID().c_str());
  

  while(WiFi.status()!=WL_CONNECTED) 
  {
    if(i<30)
    {
      delay(500); 
      Serial.print("."); 
      i++;
   }
   else
   {
    Serial.print("\n Error de autenticación, verifique los datos ingresados");
    delay(2000);
   }
  }

  Serial.println("");
  Serial.println("WiFi conectada");

  for(j=0; j<5; j++)
  {
    digitalWrite(PinLED, HIGH);
    delay(200);
    digitalWrite(PinLED, LOW);
    delay(200);
  }
  server.begin();
  Serial.println("Servidor inicializado");
  
  Serial.printf("\n\nUtiliza este link para conectar: http://%s/\n", WiFi.localIP().toString().c_str());
  
}

/*________________________________________________________________________________________________________________*/

void loop() 
{

  WiFiClient client = server.available();  
  if (!client) {
    
    delay(1);
    return;
    
  }

  //Serial.println("Nuevo cliente");
  while(!client.available()){
    delay(1);
  }

  leerTemperatura();
 

  String peticion = client.readStringUntil('\r');
  //Serial.println(peticion);
  client.flush();
 

  if (peticion.indexOf("/LED=ON") != -1) 
  { digitalWrite(PinLED, HIGH);}
  if (peticion.indexOf("/LED=OFF") != -1)
  { digitalWrite(PinLED, LOW);}
  if (peticion.indexOf("/LUZEXT=ON") != -1) 
  { digitalWrite(luzExterior, LOW);}
  if (peticion.indexOf("/LUZEXT=OFF") != -1) 
  { digitalWrite(luzExterior, HIGH);}
  if (peticion.indexOf("/LOCK=ON") != -1) 
  {  
    for(i=0;i<30;i++)
    {
      digitalWrite(PINSERVO, HIGH);
      delayMicroseconds(2000);
      digitalWrite(PINSERVO, LOW);
      delayMicroseconds(18000);
    }
  } 
  if (peticion.indexOf("/LOCK=OFF") != -1)
  {
    for(i=0;i<30;i++)
    {
      digitalWrite(PINSERVO, HIGH);
      delayMicroseconds(1000);
      digitalWrite(PINSERVO, LOW);
      delayMicroseconds(19000);
    }
  }
  
  if (peticion.indexOf("/LUZEXTAUTO=ON") != -1) 
  { luzExteriorAuto=true;}
  if (peticion.indexOf("/LUZEXTAUTO=OFF") != -1)
  { luzExteriorAuto=false;}
 
  if (peticion.indexOf("/RESETGAS=ON") != -1)
  {
    AlarmaGas=false;
    Title="ESP8266 Server";
    digitalWrite(Extractor, LOW);  
  }

if (peticion.indexOf("/PIR=ON") != -1)
{
  alarmaActiva=true;
}

if (peticion.indexOf("/RESETPIR=ON") != -1)
{
  alarmaActiva=false;
  AlarmaPIR=false;
  digitalWrite(Buzzer, LOW);
  Title="ESP8266 Server";
  alarmaTitle="Alarma inactiva";
}
  client.println("HTTP/1.1 200 OK");
  client.println("");                         
  client.println("");
  client.println("");
  client.println("");

  //INICIO HTML


client.println("<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>");
client.println("<title>ESP8266 Server</title></head>");
client.println("<body style='font-family: Century gothic; width: 800;'><center>");                         
client.println("<div style='box-shadow: 0px 0px 20px 8px rgba(0,0,0,0.22); padding: 20px; width: 300px; display: inline-block; margin: 30px;'>");
if(AlarmaGas)
{
    Title="ALERTA FUGA DE GAS"; 
}

if(AlarmaPIR)
{
  Title="ALARMA DE MOVIMIENTO";
}

if(AlarmaPIR&&AlarmaGas)
{
    Title="ALERTA FUGA DE GAS Y MOVIMIENTO";
}

client.println("<h1 style='text-align: center;'>"+String(Title)+"</h1><p style='text-align: center;'>");
client.println("<h2 style='text-align: center;'>Temperatura: "+String(temp)+"ºC</h2>");
client.println("<h2 style='text-align: center;'>Humedad: "+String(hume)+"%</h2>");  

if(alarmaActiva)
{
  alarmaTitle="Alarma activa";
}

client.println("<h3 style='text-align: center;'>"+String(alarmaTitle)+"</h3>");
if(luzExteriorAuto)
{
  luzTitle="Luces exteriores automáticas";     
}
else
{
  luzTitle="Luces exteriores manuales";
}
client.println("<h3 style='text-align: center;'>"+String(luzTitle)+"</h3>");
client.println("<button onclick=location.href='/LED=ON'> Encender LED</button> <br> <br>");
client.println("<button onclick=location.href='/LED=OFF'> Apagar LED </button> <br> <br>");
if(!luzExteriorAuto)
{
client.println("<button onclick=location.href='/LUZEXT=ON'> Encender luces exteriores</button> <br> <br>");
client.println("<button onclick=location.href='/LUZEXT=OFF'> Apagar luces exteriores</button> <br> <br>");
}
client.println("<button onclick=location.href='/LOCK=ON'> Activar cerradura</button> <br> <br>");
client.println("<button onclick=location.href='/LOCK=OFF'> Desactivar cerradura</button> <br> <br>");
client.println("<button onclick=location.href='/PIR=ON'> Activar alarma</button> <br> <br>");
client.println("<button onclick=location.href='/LUZEXTAUTO=ON'> Activar luces exteriores automáticas</button> <br> <br>");
client.println("<button onclick=location.href='/LUZEXTAUTO=OFF'> Desactivar luces exteriores automáticas</button> <br> <br>");
if(AlarmaGas)
{
  client.println("<button onclick=location.href='/RESETGAS=ON'> Desactivar Alarma de Gas</button> <br> <br>");
}

if(AlarmaPIR)
{
  client.println("<button onclick=location.href='/RESETPIR=ON'> Desactivar Alarma</button> <br> <br>");
}
client.println("</p></div> </center></body></html>");  

 

 //FIN HTML
 //Serial.println("Petición finalizada"); // Se finaliza la petición al cliente. Se inicializa la espera de una nueva petición.
// Serial.println("");

}

/*________________________________________________________________________________________________________________*/

void ISRoutine()
{
  AlarmaGas=true;
  digitalWrite(Extractor, HIGH);
}

void ISRoutine2()
{
  if(alarmaActiva)
  {
  AlarmaPIR=true;
  digitalWrite(Buzzer, HIGH);
  }
}

void ISRoutine3()
{
  if(luzExteriorAuto)
  {
    if(digitalRead(SensorLDR))
    {
    digitalWrite(luzExterior, LOW);
    }
    else
    {
    digitalWrite(luzExterior, HIGH); 
    }
 
}
}

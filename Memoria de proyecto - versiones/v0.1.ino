#include<ESP8266WiFi.h>
#include <DHT.h>    // importa la Librerias DHT
#include <DHT_U.h>



const char ssid[]="TeleCentro-945b"; //Ingresar nombre de la red.
const char password[]="GDNLLTNXGDJT"; //Ingresar contraseña.

WiFiServer server(80);

int PinLED= 5;
int estado=LOW;
int i=0;
int j=0;
int SENSOR = 2;     // pin DATA de DHT11 a pin digital 2.
float hume=0;
float temp=0;

DHT dht(SENSOR, DHT11);    // creación del objeto.

void leerTemperatura()
{
  hume=dht.readHumidity();
  temp=dht.readTemperature();
  Serial.println("Temperatura="+String(temp)+", Humedad="+String(hume));
}

void setup()
{
  Serial.begin(9600);
  dht.begin();      // inicializacion de sensor

  pinMode(PinLED, OUTPUT);
  digitalWrite(PinLED, LOW);
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

void loop() 
{

  WiFiClient client = server.available();  
  if (!client) {
    return;
  }

  Serial.println("nuevo cliente");
  while(!client.available()){
    delay(1);
  }

  leerTemperatura();
 

  String peticion = client.readStringUntil('\r');
  Serial.println(peticion);
  client.flush();
 

  if (peticion.indexOf('/LED=ON') != -1) 
  {estado = HIGH;} 
  if (peticion.indexOf('/LED=OFF') != -1)
  {estado = LOW;}
 
  digitalWrite(PinLED, estado);
 

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
client.println("<h1 style='text-align: center;'>Servidor Web ESP8266</h1><p style='text-align: center;'>");
client.println("<h2 style='text-align: center;'>Temperatura: "+String(temp)+"ºC</h2>");
client.println("<h2 style='text-align: center;'>Humedad: "+String(hume)+"%</h2>");                         
client.println("<button onclick=location.href='/LED=ON'> Encender LED</button> <br> <br>");
client.println("<button onclick=location.href='/LED=OFF'> Apagar LED </button> <br> <br>");
client.println("</p></div> </center></body></html>");  

 
 /* client.print("<h1 align=center>CONTROL LED ");
 
 if(estado == LOW) 
 {client.print("<input type='image' src='https://i.imgur.com/00i0BdB.jpg' style='display:block; margin:auto' width='30%' onClick=location.href='/LED=ON'>");}
 else 
 {client.print("<input type='image' src='https://i.imgur.com/uQxhFE5.jpg' style='display:block; margin:auto' width='30%' onClick=location.href='/LED=OFF'>");}
 
 client.println("</html>");
 delay(1);
*/
 //FIN HTML
 Serial.println("Petición finalizada"); // Se finaliza la petición al cliente. Se inicializa la espera de una nueva petición.
 Serial.println("");

 
 
}

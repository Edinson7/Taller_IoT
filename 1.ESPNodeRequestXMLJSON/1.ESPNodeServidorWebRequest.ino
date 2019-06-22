
#include <ESP8266WiFi.h>  
// WiFi parameters  
//const char* ssid = "IoTIC";  
//const char* password = "interaccioneinteligencia"; 
const char* ssid = "264083978257";  
const char* password = "170062167853"; 
//const char* ssid = "AndroidAP";  
//const char* password = "edinson123"; 

WiFiServer server(80);

//Parametros de los pines
const int temperaturaPin = A0;
const int fotoCeldapin = A0;
const int ledPin1 = D4;
const int ledPin2 = D5;

const int umbralLuz = 500;
const int umbralTemp = 32;

void configurarWifi(void){
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  
  
  while (WiFi.status() != WL_CONNECTED)  
  {  
    delay(500);  
    Serial.print(".");
  }  
  Serial.println("");  
  Serial.println("WiFi connected");  
  // Print the IP address  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

char* retornarMetadatosXML() {
  char* id = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><objetoIoT xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" id=\"192.168.127.15\" xsi:noNamespaceSchemaLocation=\"id.xsd\">   <gateway>      <propiedades>         <nombre>ESP8266</nombre>         <modelo>nodemcu 12E</modelo>         <fabricante>EspressIf</fabricante>      </propiedades>      <recursos>     <recursoIoT>            <tipo>sensor</tipo>            <senal>analoga</senal>            <voltaje rangoinicial=\"3\" rangoFinal=\"1023\"/>            <dispositivo>FotoCelda</dispositivo>         </recursoIoT>    <recursoIoT>            <tipo>actuador</tipo>            <senal>digital</senal>            <voltaje rangoInicial=\"0\" rangoFinal=\"1\"/>            <dispositivo>LED</dispositivo>         </recursoIoT>     <recursoIoT>            <tipo>sensor</tipo>            <senal>analoga</senal>            <voltaje rangoinicial=\"3\" rangoFinal=\"1023\"/>            <dispositivo>TMP35</dispositivo>         </recursoIoT>         <recursoIoT>            <tipo>actuador</tipo>            <senal>digital</senal>            <voltaje rangoinicial=\"0\" rangoFinal=\"1\"/>            <dispositivo>LED</dispositivo>         </recursoIoT>      </recursos>   </gateway></objetoIoT>";
  return id;
}

char* retornarMetadatosJSON() {
  char* id = "{   \"@id\": \"192.168.127.15\",   \"@noNamespaceSchemaLocation\": \"id.xsd\",   \"gateway\": {      \"propiedades\": {         \"nombre\": \"ESP8266\",         \"modelo\": \"nodemcu 12E\",         \"fabricante\": \"EspressIf\"      },      \"recursos\": [         {            \"tipo\": \"sensor\",            \"senal\": \"analoga\",            \"voltaje\": {               \"@rangoinicial\": \"3\",               \"@rangoFinal\": \"1023\"            },            \"dispositivo\": \"FotoCelda\"         },         {            \"tipo\": \"actuador\",            \"senal\": \"digital\",            \"voltaje\": {               \"@rangoInicial\": \"0\",               \"@rangoFinal\": \"1\"            },            \"dispositivo\": \"LED\"         },         {            \"tipo\": \"sensor\",            \"senal\": \"analoga\",            \"voltaje\": {               \"@rangoinicial\": \"3\",               \"@rangoFinal\": \"1023\"            },            \"dispositivo\": \"TMP35\"         },         {            \"tipo\": \"actuador\",            \"senal\": \"digital\",            \"voltaje\": {               \"@rangoinicial\": \"0\",               \"@rangoFinal\": \"1\"            },            \"dispositivo\": \"LED\"         }      ]   }}";
  return id;
}

String retornarMedidasXML(float temperatura, int estado1, int estado2, float voltaje) {
  String medidas = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><observacion xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" id=\"192.168.127.15\" xsi:noNamespaceSchemaLocation=\"medida.xsd\">   <entidad nombre=\"mesa\">      <recursoIoT tipo=\"sensor\">         <caracteristica propiedad=\"luz\">";
  medidas += voltaje;
  medidas += "</caracteristica>         <unidad>voltio</unidad>         <lugar lat=\"24.7\" lon=\"76.5\" ele=\"1756\" />      </recursoIoT>      <recursoIoT tipo=\"actuador\">         <dispositivo nombre=\"bombillo\" numero=\"1\">            <estado>";
  medidas += estado1;
  medidas += "</estado>         </dispositivo>         <lugar lat=\"24.7\" lon=\"76.5\" ele=\"1756\" />      </recursoIoT>      <recursoIoT tipo=\"sensor\">         <caracteristica propiedad=\"temperatura\">";
  medidas += temperatura;
  medidas += "</caracteristica>         <unidad>grados centigrados</unidad>         <lugar lat=\"24.7\" lon=\"76.5\" ele=\"1756\" />      </recursoIoT>      <recursoIoT tipo=\"actuador\">         <dispositivo nombre=\"bombillo\" numero=\"2\">            <estado>";
  medidas += estado2;
  medidas += "</estado>         </dispositivo>         <lugar lat=\"24.7\" lon=\"76.5\" ele=\"1756\" />      </recursoIoT>   </entidad></observacion>";
  return medidas;
}

String retornarMedidasJSON(float temperatura, int estado1,  int estado2, float voltaje) {
  String medidas = "{   \"@id\": \"192.168.127.15\",   \"@noNamespaceSchemaLocation\": \"medida.xsd\",   \"entidad\": {      \"@nombre\": \"mesa\",      \"recursoIoT\": [         {            \"@tipo\": \"sensor\",            \"caracteristica\": {               \"@propiedad\": \"luz\",               \"#text\": \"";
  medidas += voltaje;
  medidas += "\"            },            \"unidad\": \"voltio\",            \"lugar\": {               \"@lat\": \"24.7\",               \"@lon\": \"76.5\",               \"@ele\": \"1756\"            }         },         {            \"@tipo\": \"actuador\",            \"dispositivo\": {               \"@nombre\": \"bombillo\",               \"@numero\": \"1\",               \"estado\": \"";
  medidas += estado1;
  medidas += "\"            },            \"lugar\": {               \"@lat\": \"24.7\",               \"@lon\": \"76.5\",               \"@ele\": \"1756\"            }         },         {            \"@tipo\": \"sensor\",            \"caracteristica\": {               \"@propiedad\": \"temperatura\",               \"#text\": \"";
  medidas += temperatura;
  medidas += "\"            },            \"unidad\": \"grados centigrados\",            \"lugar\": {               \"@lat\": \"24.7\",               \"@lon\": \"76.5\",               \"@ele\": \"1756\"            }         },         {            \"@tipo\": \"actuador\",            \"dispositivo\": {               \"@nombre\": \"bombillo\",               \"@numero\": \"2\",               \"estado\": \"";
  medidas += estado2;
  medidas += "\"            },            \"lugar\": {               \"@lat\": \"24.7\",               \"@lon\": \"76.5\",               \"@ele\": \"1756\"            }         }      ]   }}";
  return medidas;
}

void setup()
{
  // Inicializar el puerto serial  
  Serial.begin(115200);
  delay(1500);

  // prepare GPIO2
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  randomSeed(analogRead(A0));
}

void loop()
{
  //En caso de Ser una FotoCelda
  //valoranalogo = analogRead(fotoCeldapin);
  int valorFotoCelda = random(0,1023);
  Serial.print("Valor analogo: ");
  Serial.print(valorFotoCelda);
  Serial.print(" ADC --- ");

  int estado1 = 0;
  //En caso de Ser un Led
  if(valorFotoCelda > umbralLuz) {
    digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
    estado1 = 1;
    Serial.println("Bombillo 1 ON");
  }
  else {
    digitalWrite(ledPin1, LOW);    // turn the LED off by making the voltage LOW
    Serial.println("Bombillo 1 OFF");
  }

  //En caso de Ser un TMP35
  int valorAnalogo = analogRead(temperaturaPin);
  float temperatura = 3.3 * valorAnalogo * 100.0 / 1024.0;
  //impresión en la pantalla seria del IDE arduino                                              
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  //Serial.print(" ");
  Serial.print(" ℃ --- ");

  int estado2 = 0;
  //En caso de Ser un Led
  if(temperatura > umbralTemp){
    digitalWrite(ledPin2, HIGH);   // turn the LED on (HIGH is the voltage level)
    estado2 = 1;
    Serial.println("Bombillo 2 ON");
  }
  else {
      digitalWrite(ledPin2, LOW);    // turn the LED off by making the voltage LOW
      Serial.println("Bombillo 2 OFF");
  }
   
// Comprobar si se ha conectado algun cliente
  WiFiClient cliente = server.available();
  String req = cliente.readStringUntil('r');
  Serial.println(req);
  cliente.flush();
  
  // Match the request
  if (req.indexOf("/metadatos?tipo=xml") != -1) {
    Serial.println("Cliente conectado...");
    Serial.println("Enviando Metadatos XML");
    cliente.println( retornarMetadatosXML() );
  }
  else if (req.indexOf("/metadatos?tipo=json") != -1) {
    Serial.println("Cliente conectado...");
    Serial.println("Enviando Metadatos JSON");
    cliente.println( retornarMetadatosJSON() );
  }
  else if (req.indexOf("/medidas?tipo=xml") != -1 ){
    Serial.println("Cliente conectado...");
    Serial.println("Enviando Medidas XML");
    cliente.println( retornarMedidasXML(temperatura,estado1,estado2,valorFotoCelda) );
  }
  else if (req.indexOf("/medidas?tipo=json") != -1 ){
    Serial.println("Cliente conectado...");
    Serial.println("Enviando Medidas JSON");
    cliente.println( retornarMedidasJSON(temperatura,estado1,estado2,valorFotoCelda) );
  }
  else {
    Serial.println("invalid request");
    cliente.println("ERROR 404");
    //cliente.stop();
    return;
  }
  
  /*if(cliente)
  {
      
//------------------------------------------------WEB--------------------------------------------------------------------------------------          
          
    Serial.println("Cliente conectado...");
    Serial.println("Enviando XML ID");
    cliente.println(retornarId());
  }*/
  delay(1000); // reescribe cada 1 segundo
}


  

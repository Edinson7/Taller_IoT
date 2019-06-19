
#include <ESP8266WiFi.h>  
// WiFi parameters  
//const char* ssid = "IoTIC";  
//const char* password = "interaccioneinteligencia"; 
//const char* ssid = "264083978257";  
//const char* password = "170062167853"; 
const char* ssid = "AndroidAP";  
const char* password = "edinson123"; 

WiFiServer server(80);

//Parametros de los pines
const int temperaturapin = A0;
const int fotoCeldapin = A0;
const int ledpin = D4;
const int umbral = 500;

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
  char* id = "<?xml version='1.0' encoding='UTF-8'?><objetoIoT id='192.168.127.15' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:noNamespaceSchemaLocation = 'id.xsd'>   <gateway>      <propiedades>         <nombre>ESP8266</nombre>         <modelo>nodemcu 12E</modelo>         <fabricante>EspressIf</fabricante>      </propiedades>      <recursos>         <recursoIoT>            <tipo>sensor</tipo>            <senal>analoga</senal>            <voltaje rangoinicial='3'/>      <dispositivo>TMP35</dispositivo>         </recursoIoT>         <recursoIoT>            <tipo>actuador</tipo>            <senal>digital</senal>            <voltaje rangoinicial='0'/>     <dispositivo>LED</dispositivo>         </recursoIoT>     <recursoIoT>            <tipo>sensor</tipo>            <senal>analoga</senal>            <voltaje rangoinicial='3'/>     <dispositivo>FotoCelda</dispositivo>         </recursoIoT>      </recursos>   </gateway></objetoIoT>";
  //char* id = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><objetoIoT id=\"192.168.127.15\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"../estandar/id.xsd\">   <gateway>      <propiedades>         <nombre>ESP8266</nombre>         <modelo>nodemcu 12E</modelo>         <fabricante>EspressIf</fabricante>      </propiedades>      <recursos>         <recursoIoT>            <tipo>sensor</tipo>            <senal>analoga</senal>            <voltaje rangoinicial=\"3\" rangofinal=\"30\"/>         </recursoIoT>         <recursoIoT>            <tipo>actuador</tipo>            <senal>digital</senal>            <voltaje rangoinicial=\"0\" rangofinal=\"5\"/>         </recursoIoT>      </recursos>   </gateway></objetoIoT>"; 
  return id;
}

char* retornarMetadatosJSON() {
  char* id = "{   \"@id\": \"192.168.127.15\",   \"gateway\": {      \"propiedades\": {         \"nombre\": \"ESP8266\",         \"modelo\": \"nodemcu 12E\",         \"fabricante\": \"EspressIf\"     },     \"recursos\": [         {            \"tipo\": \"sensor\",            \"senal\": \"analoga\",            \"voltaje\": {               \"@rangoinicial\": \"3\"            },            \"dispositivo\": \"TMP35\"         },         {            \"tipo\": \"actuador\",            \"senal\": \"digital\",            \"voltaje\": {               \"@rangoinicial\": \"0\"            },            \"dispositivo\": \"LED\"         },         {            \"tipo\": \"sensor\",            \"senal\": \"analoga\",            \"voltaje\": {               \"@rangoinicial\": \"3\"            },            \"dispositivo\": \"FotoCelda\"         }      ]   }}";
  return id;
}

String retornarMedidasXML(float temperatura, int estado, float voltaje) {
  //String medidas = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><observacion id=\"192.168.127.15\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation = \"medida.xsd\">  <entidad nombre=\"mesa\">    <recursoIoT tipo=\"sensor\">      <caracteristica propiedad=\"temperatura\">      </caracteristica>      <medicion></medicion>      <unidad>grados</unidad>      <lugar lat=\"24.7\" lon=\"76.5\" ele=\"1756\"></lugar>    </recursoIoT>    <recursoIoT tipo=\"actuador\">      <caracteristica propiedad=\"luz\">      </caracteristica>      <estado></estado>      <lugar lat=\"24.7\" lon=\"76.5\" ele=\"1756\"></lugar>    </recursoIoT> <recursoIoT tipo=\"sensor\">      <caracteristica propiedad=\"luz\">      </caracteristica>      <medicion></medicion>      <unidad>voltio</unidad>   <lugar lat=\"24.7\" lon=\"76.5\" ele=\"1756\"></lugar>    </recursoIoT>  </entidad></observacion>";
  String medidas = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><observacion id=\"192.168.127.15\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation = \"medida.xsd\">  <entidad nombre=\"mesa\">    <recursoIoT tipo=\"sensor\">      <caracteristica propiedad=\"temperatura\">      </caracteristica>      <medicion>";
  medidas += temperatura; 
  medidas += "</medicion>      <unidad>grados</unidad>      <lugar lat=\"24.7\" lon=\"76.5\" ele=\"1756\"></lugar>    </recursoIoT>    <recursoIoT tipo=\"actuador\">      <caracteristica propiedad=\"luz\">      </caracteristica>      <estado>";
  medidas += estado;
  medidas += "</estado>      <lugar lat=\"24.7\" lon=\"76.5\" ele=\"1756\"></lugar>    </recursoIoT> <recursoIoT tipo=\"sensor\">      <caracteristica propiedad=\"luz\">      </caracteristica>      <medicion>";
  medidas += voltaje;
  medidas += "</medicion>      <unidad>voltio</unidad>   <lugar lat=\"24.7\" lon=\"76.5\" ele=\"1756\"></lugar>    </recursoIoT>  </entidad></observacion>";
  return medidas;
}

String retornarMedidasJSON(float temperatura, int estado, float voltaje) {
  String medidas = "{   \"@id\": \"192.168.127.15\",   \"entidad\": {      \"@nombre\": \"mesa\",      \"recursoIoT\": [         {            \"@tipo\": \"sensor\",            \"caracteristica\": {               \"@propiedad\": \"temperatura\"            },            \"medicion\": ";
  medidas += temperatura;
  medidas += ",            \"unidad\": \"grados\",            \"hora\": {               \"@utc\": \"-5\"            },            \"lugar\": {               \"@lat\": \"24.7\",               \"@lon\": \"76.5\",               \"@ele\": \"1756\"            }         },         {            \"@tipo\": \"actuador\",            \"caracteristica\": {               \"@propiedad\": \"luz\"            },            \"estado\": ";
  medidas += estado;
  medidas += ",            \"hora\": {               \"@utc\": \"-5\"            },            \"lugar\": {               \"@lat\": \"24.7\",               \"@lon\": \"76.5\",               \"@ele\": \"1756\"            }         },         {            \"@tipo\": \"sensor\",            \"caracteristica\": {               \"@propiedad\": \"luz\"            },            \"medicion\": ";
  medidas += voltaje;
  medidas += ",            \"unidad\": \"voltio\",            \"hora\": {               \"@utc\": \"-5\"            },            \"lugar\": {               \"@lat\": \"24.7\",               \"@lon\": \"76.5\",               \"@ele\": \"1756\"            }         }      ]   }}";
  return medidas;
}

void setup()
{
  // Inicializar el puerto serial  
  Serial.begin(115200);
  delay(1500);

  // prepare GPIO2
  pinMode(ledpin, OUTPUT);
  digitalWrite(ledpin, LOW);
  
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
}

void loop()
{
  //En caso de Ser un TMP35
  int valoranalogo = analogRead(temperaturapin);
  float temperatura = 3.3 * valoranalogo * 100.0 / 1024.0;
  //impresión en la pantalla seria del IDE arduino                                              
  Serial.print(temperatura);
  //Serial.print(" ");
  Serial.print(" ℃ ");

  //En caso de Ser una FotoCelda
  valoranalogo = analogRead(fotoCeldapin);

  //En caso de Ser un Led
  int estado = 0;
  if(valoranalogo > umbral){
  //if(temperatura > umbral){
    digitalWrite(ledpin, HIGH);   // turn the LED on (HIGH is the voltage level)
    estado = 1;
    Serial.println("Bombillo ON");
  }
  else {
      digitalWrite(ledpin, LOW);    // turn the LED off by making the voltage LOW
      Serial.println("Bombillo OFF");
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
    cliente.println( retornarMedidasXML(temperatura,estado,valoranalogo) );
  }
  else if (req.indexOf("/medidas?tipo=json") != -1 ){
    Serial.println("Cliente conectado...");
    Serial.println("Enviando Medidas JSON");
    cliente.println( retornarMedidasJSON(temperatura,estado,valoranalogo) );
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


  

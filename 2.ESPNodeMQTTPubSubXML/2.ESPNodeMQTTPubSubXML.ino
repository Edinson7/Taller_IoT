//Este sketch permite demostrar como conectarse a un servidor MQTT para enviar datos de temperatura
//y para recibir comandos que permitan encender - apagar un led desde cualquier aplicación MQTT
//Se recomineda usar las app Android MQTT DASH

#include <PubSubClient.h>
#include <ESP8266WiFi.h> 
// WiFi parameters  
//const char* ssid = "AndroidAP";  
//const char* password = "edinson123"; 
//const char* ssid = "264083978257";  
//const char* password = "170062167853"; 
const char* ssid = "andres";  
const char* password = "1234qwer"; 

//Parametros para los mensajes MQTT
WiFiClient espClient;
PubSubClient client(espClient);

//Broquer MQTT
//const char* mqtt_server = "iot.eclipse.org";
const char* mqtt_server ="192.168.43.82";

//Parametros de los pines
const int temperaturaPin = A0;
const int fotoCeldaPin = A0;
const int ledPin1 = D4;
const int ledPin2 = D5;

//Toma de valores
float temperatura;
int estado1;
int estado2;
int valorAnalogo;
int umbralTemp = 32;
int umbralLuz = 500;

//Para comunicacion
long lastMsg = 0;
char msg[500];
String text;

void verificarPublicacion(byte* payload, unsigned int length){
  int vLuz = 0;
  int vTemp = 0;
  String valor = "";
  
  for (int i = 0; i < length; i++) {
    if((char)payload[i] == 'l' && (char)payload[i+1] == 'u' && (char)payload[i+2] == 'z') { vLuz = 1; }
    
    if((char)payload[i+0] == 't' && (char)payload[i+1] == 'e' && (char)payload[i+2] == 'm' && (char)payload[i+3] == 'p' && (char)payload[i+4] == 'e' && (char)payload[i+5] == 'r' && 
       (char)payload[i+6] == 'a' && (char)payload[i+7] == 't' && (char)payload[i+8] == 'u' && (char)payload[i+9] == 'r' && (char)payload[i+10] == 'a') 
    { 
      vTemp = 1; 
    }

    if((char)payload[i-1] == '>' && vLuz == 1) { vLuz = 2; }
    if((char)payload[i-1] == '>' && vTemp == 1) { vTemp = 2; }

    if((char)payload[i] == '<' && vLuz == 2) { 
      vLuz = 3; 
      break; 
    }
    if((char)payload[i] == '<' && vTemp == 2) { 
      vTemp = 3; 
      break; 
    }

    if(vLuz == 2) { valor += (char)payload[i]; }
    if(vTemp == 2) { valor += (char)payload[i]; }
  }
  if(vLuz == 3) { 
    umbralLuz = valor.toInt();
    Serial.print("Cambio de umbral de luz... --- Valor: ");
  }
  else if(vTemp == 3) { 
    umbralTemp = valor.toInt(); 
    Serial.print("Cambio de umbral de temperatura... --- Valor: ");
  }
  else { Serial.print("No ocurrio cambio de umbrales"); }
  Serial.print(valor);
  Serial.println();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println();
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("]");
  verificarPublicacion( payload, length );
    
  // Switch on the LED if an 0 was received as first character
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      Serial.println("enviando...");
      // ... and resubscribe
      client.subscribe("ESP8266-1/mesa/sensor/temperatura/umbral");
      client.subscribe("ESP8266-1/mesa/sensor/luz/umbral");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void configurarWifi(void){
  delay(10);
  // We start by connecting to a WiFi network
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
  // Print the IP address  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

String retornarMedidaXMLSensor(float valor, String propiedad) {
  String medidas = "<recursoIoT tipo=\"sensor\"><caracteristica propiedad =\"";
  medidas += propiedad;
  medidas += "\">";
  medidas += valor; 
  medidas += "</caracteristica><unidad>";
  
  if(propiedad == "temperatura") { medidas += "grados"; }
  else { medidas += "voltio"; }

  medidas += "</unidad></recursoIoT>";
  return medidas;
}

String retornarMedidaXMLActuador(int estado, int numero) {
  String medidas = "<recursoIoT tipo=\"actuador\"><dispositivo nombre=\"bombillo\" numero=\"";
  medidas += numero;
  medidas += "><estado>";
  medidas += estado;
  medidas += "</estado></dispositivo></recursoIoT>";
  return medidas;
}

void setup(void)  
{  
  //Led en el digital 1ed
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  
  // Inicializar el puerto serial  
  Serial.begin(115200);  
  delay(500);
    
  // Connect to WiFi  
  configurarWifi();  

  //Inicializar el canal MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  randomSeed(analogRead(A0));
 }  

void loop()  
{  
  //Intentarconectarse al servidor MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  //En caso de Ser una FotoCelda
  //valorAnalogo = analogRead(fotoCeldaPin);
  valorAnalogo = random(0,1023);
  Serial.print("\nValor analogo de Fotocelda: ");
  Serial.print(valorAnalogo);
  Serial.print(" ADC --- ");

  //En caso de Ser un Led
  if(valorAnalogo < umbralLuz) {
    digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
    estado1 = 1;
    Serial.println("Bombillo 1 ON");
  }
  else {
    digitalWrite(ledPin1, LOW);    // turn the LED off by making the voltage LOW
    estado1 = 0;
    Serial.println("Bombillo 1 OFF");
  }

  //recibe la temperatura para el sensor LM35
  //Este calculo es para el ESP8266 NODEMCU 12 con LM35
  temperatura = 3.3 * analogRead(temperaturaPin) * 100.0 / 1024.0;
  //impresión en la pantalla seria del IDE arduino                                              
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print(" ℃ --- ");

  //En caso de Ser un Led
  if(temperatura > umbralTemp) {
    digitalWrite(ledPin2, HIGH);   // turn the LED on (HIGH is the voltage level)
    estado2 = 1;
    Serial.println("Bombillo 2 ON");
  }
  else {
    digitalWrite(ledPin2, LOW);    // turn the LED off by making the voltage LOW
    estado2 = 0;
    Serial.println("Bombillo 2 OFF");
  }
 
  //Publicar el mensaje 
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    //snprintf (msg, 75, "%f", temperatura);
    text = retornarMedidaXMLSensor(valorAnalogo, "luz");
    text.toCharArray(msg,500);
    Serial.print("\nPublicando voltaje obtenido de la foto-Celda: ");
    Serial.println(msg);
    client.publish("ESP8266-1/mesa/sensor/luz/datapoint", msg);
    
    text = retornarMedidaXMLActuador(estado1, 1);
    text.toCharArray(msg,500);
    Serial.print("Publicando estado del bombillo 1: ");
    Serial.println(msg);
    client.publish("ESP8266-1/mesa/actuador/bombillo/1/datapoint", msg);

    text = retornarMedidaXMLSensor(temperatura, "temperatura");
    text.toCharArray(msg,500);
    Serial.print("Publicando temperatura: ");
    Serial.println(msg);
    client.publish("ESP8266-1/mesa/sensor/temperatura/datapoint", msg);
    
    text = retornarMedidaXMLActuador(estado2, 2);
    text.toCharArray(msg,500);
    Serial.print("Publicando estado del bombillo 2: ");
    Serial.println(msg);
    client.publish("ESP8266-1/mesa/actuador/bombillo/2/datapoint", msg);
  }
  delay(1000);
}  

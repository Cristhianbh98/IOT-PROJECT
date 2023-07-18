// Importar Librerias
#include <DHT.h>
#include <Wire.h>
#include "ESP8266WiFi.h"
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

//-------------------VARIABLES GLOBALES--------------------------
// Configuración de Red
const char* ssid = "BG"; 
const char* password = "mndz543210"; 
const char* mqtt_server = "broker.hivemq.com";

// Variables y valores iniciales para el cliente WIFI
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long last_send;
int status = WL_IDLE_STATUS;

// Configuración de sensor
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(A0, DHT11);
float h;

// Definición de la variable para publicar el mensaje
String payload;
int rele = 2;
int pause = 800;

// Esta función reconecta su ESP8266 al Broker (MQTT Server)
void reconnect() {
  while ( !client.connected() ) {
    if ( client.connect("A") ) {
      Serial.println("ESP8266 Conectado a MQTT");
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println("Intentando de nuevamente en 5 segundos");
      delay(1000);
    }
  }
}

// Configuración de Red
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Conexión WiFi - Dirección IP De La ESP8266: ");
  Serial.println(WiFi.localIP());
}

// Leer temperatura y humedad
void get_data(){
  h = map(analogRead(A0), 0, 1023, 100, 0);

  if ( isnan(h) ) {
    Serial.println("Fallo al leer el sensor DHT!");
    delay(1500);
    return;
  }
  
  Serial.print("Nivel de Humedad = ");
  Serial.print(h);
  Serial.println("%");
  delay(5000);
}

// Configuración formato Json
void get_payload(){
  String humedad = String(h);
  String temperatura = String(20);
  payload = "{";
  payload += "\"t\":"; payload += temperatura; payload += ",";
  payload += "\"h\":"; payload += humedad;
  payload += "}";
}

// Enviar datos al servidor
void send_payload() {
  char attributes[100];
  payload.toCharArray( attributes, 100 );

  if ( client.publish( "IOT/TEMHUM", attributes ) ) {
    Serial.println( "Mensaje enviado correctamente" );
  } else {
    Serial.println( "Fallo al enviar el mensaje" );
  }

  Serial.println( attributes );
}

// Configuración de la función (setup)
void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  pinMode(rele, OUTPUT);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  dht.begin();
  last_send = 0;
  delay(3000);
}

void loop() {
  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - last_send > 5000 ) {
    get_data();
    get_payload();
    send_payload();
    Serial.println(payload);
    last_send = millis();
  }

  client.loop();

  /* if( h < 30) {
    Serial.println("Nivel de humedad bajo, Iniciando sistema de riego");

    digitalWrite(rele, LOW);
    delay(3000);
    digitalWrite(rele, HIGH);
    Serial.println("Sistema de riego desactivado");   
    delay(3000);
  } */
}

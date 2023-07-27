#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(A0, DHT11);
int rele = 2;
int h;
int pause = 800;
unsigned long start_time;
String payload;

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

// Esta función reconecta su ESP8266 al Broker (MQTT Server)
void reconnect() {
 while ( !client.connected() ) {
   if (client.connect("CLIENT_SP")) {
     Serial.println("ESP8266 Conectado MQTT");
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println("Intentando de nuevamente en 5 segundos");
      delay(1000);
    }
  }
}

// Esta función lee el sensor de humedad
void get_hume() {
  h = map(analogRead(A0), 0, 1023, 100, 0);

  if ( isnan(h) ) {
    lcd.setCursor(0, 0);
    lcd.print("Error sensor");
    delay(1500);
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Humedad = ");
  lcd.print(h);
  lcd.print("%");
  delay(3000);

  if( h < 30) {
    lcd.setCursor(0, 1);
    lcd.print("Iniciando Riego");
    delay(3000);

    digitalWrite(rele, HIGH);
    lcd.clear();
    lcd.print("Regando suelo...");
    delay(3000);
    digitalWrite(rele, LOW);
    
    lcd.clear();
    lcd.print("Apagando riego");

    delay(3000);    
  }
  else {
    lcd.setCursor(0, 1);
    lcd.print("Todo en orden :3");
    delay(3000);
  }

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Tomando nivel");
  lcd.setCursor(2, 1);
  lcd.print("de humedad");
  
  delay(3000);

  lcd.clear();
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
 client.publish( "IOT/TEMHUM", attributes );
 Serial.println( attributes );
}

void setup() {
  //Wire.begin();
  dht.begin();
  lcd.init();
  lcd.clear();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(rele, OUTPUT);
  start_time = millis();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  delay(3000);
}


void loop() {
  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - start_time > 5000 ) {
    get_hume();
    get_payload();
    send_payload();
    start_time = millis();
  }
}

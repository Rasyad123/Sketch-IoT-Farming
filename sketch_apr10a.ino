//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Final ESP32 MySQL Database
// RELAY LOGIKA TERBALIK 
//======================================== Including the libraries.
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "DHT.h"
//======================================== 

//======================================== DHT sensor settings (DHT11).
#define DHTPIN 15 //--> Defines the Digital Pin connected to the DHT11 sensor.
#define DHTTYPE DHT22 //--> Defines the type of DHT sensor used. Here used is the DHT11 sensor.
DHT dht11_sensor(DHTPIN, DHTTYPE); //--> Initialize DHT sensor.
//========================================

//======================================
#define sensorPinLDR 35 //LDR PIN
#define sensorPinSOIL 34 // SOIL PIN
//=====================================

// Defines the Digital Pin of the "On Board LED".
#define ON_Board_LED 2 

// Defines GPIO 18 as Relay Pompa.
#define LED_01 18 

// Defines GPIO 25 as Relay Kipas.
#define LED_02 25 

// DEfines GPIO 26 as Lamp
#define LED_03 26

//=====================(ini gua tau buat apa njir wkwkwwkwk)
long now = millis();
long lastMeasure = 0;
//===================(ku add soalnya project sebelumnya ada Aowkkwkwwkwkwk)

//======================================== SSID and Password of your WiFi router.
const char* ssid = "PE";
const char* password = "123456789";
//======================================== 

//======================================== Variables for HTTP POST request data.
String postData = ""; //--> Variables sent for HTTP POST request data.
String payload = "";  //--> Variable for receiving response from HTTP POST.
//======================================== 

//======================================== Variables for DHT11 sensor data.
float send_Temp;
int send_Humd;
String send_Status_Read_DHT11 = "";

int send_Soil;
String send_Status_Soil = "";
int send_LDR;
String send_Status_LDR = "";
 String LED_01_State = "";
    String LED_02_State = "";
    String LED_03_State = "";
//======================================== 

//________________________________________________________________________________ Subroutine to control LEDs after successfully fetching data from database.
void control_LEDs(JSONVar myObject) {
  Serial.println();
  Serial.println("---------------control_LEDs()");


  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    Serial.println("---------------");
    return;
  }

  if (myObject.hasOwnProperty("LED_01")) {
    Serial.print("myObject[\"LED_01\"] = ");
    Serial.println(myObject["LED_01"]);
  }

  if (myObject.hasOwnProperty("LED_02")) {
    Serial.print("myObject[\"LED_02\"] = ");
    Serial.println(myObject["LED_02"]);
  }

  if (myObject.hasOwnProperty("LED_03")) {
    Serial.print("myObject[\"LED_03\"] = ");
    Serial.println(myObject["LED_03"]);
  }

  if(strcmp(myObject["LED_01"], "ON") == 0)   {digitalWrite(LED_01, LOW);  Serial.println("Pompa ON"); }
  if(strcmp(myObject["LED_01"], "OFF") == 0)  {digitalWrite(LED_01, HIGH);   Serial.println("Pompa OFF");}
  if(strcmp(myObject["LED_02"], "ON") == 0)   {digitalWrite(LED_02, LOW);  Serial.println("Kipas ON"); }
  if(strcmp(myObject["LED_02"], "OFF") == 0)  {digitalWrite(LED_02, HIGH);   Serial.println("Kipas OFF");}
  if(strcmp(myObject["LED_03"], "ON") == 0)   {digitalWrite(LED_03, LOW);  Serial.println("Lamp ON"); }
  if(strcmp(myObject["LED_03"], "OFF") == 0)  {digitalWrite(LED_03, HIGH);   Serial.println("Lamp OFF");}


  Serial.println("---------------");
}
//________________________________________________________________________________ 

// ________________________________________________________________________________ Subroutine to read and get data from the DHT11 sensor.


void readsensor(){

  //------------------------------------------------------------------------ LDR MAIN
 Serial.println("-------------ldrtemp()");
 
 int valueldr = analogRead(sensorPinLDR);
  send_LDR = 100 - ((valueldr / 4095.00) * 100); // Persentase cahaya

  Serial.print("Light = ");
  Serial.print(valueldr);
  Serial.println(" Lux");
  Serial.print("Light = ");
  Serial.print(send_LDR);
  Serial.println("%");

  if (send_LDR > 40 && send_LDR < 90) {
    send_Status_LDR = "ENOUGH";
    Serial.println("cukup terang");
  } else {
    send_Status_LDR = "DARK";
    Serial.println("gelap");

  }

  Serial.println("--------------------------------------------------------");

    //--------------------------------------------------------------------------- SOIL MAIN

 Serial.println("-------------soiltemp()");
  int valuesoil = analogRead(sensorPinSOIL);
  send_Soil = 100 - ((valuesoil / 4095.00) * 100);  // Persentase kelembaban tanah

  Serial.println("--------------------------------------------------------");
  Serial.println("Result SOIL ");
  Serial.print("Moisture = ");
  Serial.print(send_Soil);
  Serial.println("%");

  if (send_Soil > 90) {
    send_Status_Soil = "PERFECT";
    Serial.println("The soil is PERFECT");
  } else if (send_Soil > 40 && send_Soil < 90) {
    send_Status_Soil = "WET";
    Serial.println("The soil is WET");
  } else {
    send_Status_Soil = "DRY"; 
    Serial.println("The soil is DRY");
  }
  Serial.println("--------------------------------------------------------");

 //================================================================== DHT GET MAIN
  Serial.println();
  Serial.println("-------------get_DHT11_sensor_data()");
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  
  // Read temperature as Celsius (the default)
  send_Temp = dht11_sensor.readTemperature();
  
  // Read Humidity
  send_Humd = dht11_sensor.readHumidity();
  
  // Read temperature as Fahrenheit (isFahrenheit = true)
  // float ft = dht11_sensor.readTemperature(true);

  // Check if any reads failed.
  if (isnan(send_Temp) || isnan(send_Humd)) {
    Serial.println("Failed to read from DHT sensor!");
    send_Temp = 0.00;
    send_Humd = 0;
    send_Status_Read_DHT11 = "FAILED";
  } else {
    send_Status_Read_DHT11 = "SUCCEED";
  }

  Serial.printf("Temperature : %.2f °C\n", send_Temp);
  Serial.printf("Humidity : %d %%\n", send_Humd);
  Serial.printf("Status Read DHT11 Sensor : %s\n", send_Status_Read_DHT11);
  Serial.println("-------------");


}


void autoControl(){

  Serial.println("=============================== AUTOMATIC ================================");


   String LED_01_State = "";
    String LED_02_State = "";
    String LED_03_State = "";

  //=============================== SOIL MATIC
 

 if (send_Soil > 80) {
     send_Status_Soil = "WET";
     digitalWrite(LED_01, HIGH); //--> Turn off LED_01.
     LED_01_State = "OFF";
     Serial.println("POMPA OFF");
  } else {
    send_Status_Soil = "DRY"; 
    digitalWrite(LED_01, LOW); //--> Turn on LED_01.
    LED_01_State = "ON";
    Serial.println("POMPA ON");
  }

  //====================================


  //==================================== DHT MATIC
  
   if (send_Temp > 35.0) {
    digitalWrite(LED_02, LOW);  // Nyalakan relay kipas
    LED_02_State = "ON";
    Serial.println("KIPAS ON");
  } else {
    digitalWrite(LED_02, HIGH);   // Matikan relay kipas
    LED_02_State = "OFF";
    Serial.println("KIPAS OFF");
  }
  //====================================


  //=================================== LDR

   if (send_LDR > 40 && send_LDR < 90) {
    send_Status_LDR = "ENOUGH";
    digitalWrite(LED_03, HIGH); //--> Turn off LED_03.
    LED_03_State = "OFF";
    Serial.println("LAMP OFF");
  } else {
   digitalWrite(LED_03, LOW); //--> Turn on LED_03.
    LED_03_State = "ON";
    send_Status_LDR = "DARK";
    Serial.println("LMAP ON");
  }
  //==========================================

   Serial.println("==================================================================");

}

//________________________________________________________________________________ VOID SETUP()
void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200); //--> Initialize serial communications with the PC.

  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output.
  pinMode(LED_01,OUTPUT); //--> LED_01 port Direction output.
  pinMode(LED_02,OUTPUT); //--> LED_02 port Direction output.
  pinMode(LED_03,OUTPUT); //--> LED_03 port Direction output.
  
 

  

  //---------------------------------------- Make WiFi on ESP32 in "STA/Station" mode and start connecting to WiFi Router/Hotspot.
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //---------------------------------------- 
  
  Serial.println();
  Serial.println("-------------");
  Serial.print("Connecting");

  //---------------------------------------- The process of connecting the WiFi on the ESP32 to the WiFi Router/Hotspot.
  // The process timeout of connecting ESP32 with WiFi Hotspot / WiFi Router is 20 seconds.
  // If within 20 seconds the ESP32 has not been successfully connected to WiFi, the ESP32 will restart.
  // I made this condition because on my ESP32, there are times when it seems like it can't connect to WiFi, so it needs to be restarted to be able to connect to WiFi.

  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //........................................ Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    //........................................ 

    //........................................ Countdown "connecting_process_timed_out".
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
    //........................................ 
  }
  //---------------------------------------- 
  
  digitalWrite(ON_Board_LED, LOW); //--> Turn off the On Board LED when it is connected to the wifi router.
  
  //---------------------------------------- If successfully connected to the wifi router, the IP Address that will be visited is displayed in the serial monitor
  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());
  Serial.println("-------------");
  //---------------------------------------- 

  // Setting up the DHT sensor (DHT11).
  dht11_sensor.begin();

  delay(2000);
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP()
void loop() {
  // put your main code here, to run repeatedly



  

  //---------------------------------------- Check WiFi connection status.
  if(WiFi.status()== WL_CONNECTED) {
    HTTPClient http;  //--> Declare object of class HTTPClient.
    int httpCode;     //--> Variables for HTTP return code.
    
    //........................................ Process to get LEDs data from database to control LEDs.
    postData = "id=esp32_01";
    payload = "";
  
    digitalWrite(ON_Board_LED, HIGH);
    Serial.println();
    Serial.println("---------------getdata.php");
    // In this project I use local server or localhost with XAMPP application.
    // So make sure all PHP files are "placed" or "saved" or "run" in the "htdocs" folder.
    // I suggest that you create a new folder for this project in the "htdocs" folder.
    // The "htdocs" folder is in the "xampp" installation folder.
    // The order of the folders I recommend:
    // xampp\htdocs\your_project_folder_name\phpfile.php
    //
    // ESP32 accesses the data bases at this line of code: 
    // http.begin("http://REPLACE_WITH_YOUR_COMPUTER_IP_ADDRESS/REPLACE_WITH_PROJECT_FOLDER_NAME_IN_htdocs_FOLDER/getdata.php");
    // REPLACE_WITH_YOUR_COMPUTER_IP_ADDRESS = there are many ways to see the IP address, you can google it. 
    //                                         But make sure that the IP address used is "IPv4 address".
    // Example : http.begin("http://192.168.0.0/ESP32_MySQL_Database/Final/getdata.php");
    http.begin("https://rasyadnext.my.id/getdata.php");  //--> Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
   
    httpCode = http.POST(postData); //--> Send the request
    payload = http.getString();     //--> Get the response payload

  
    Serial.print("httpCode : ");
    Serial.println(httpCode); //--> Print HTTP return code
    Serial.print("payload  : ");
    Serial.println(payload);  //--> Print request response payload
    
    http.end();  //--> Close connection
    Serial.println("---------------");
    digitalWrite(ON_Board_LED, LOW);
    //........................................ ;



JSONVar j = JSON.parse(payload);
if (JSON.typeof(j) == "undefined") {
  Serial.println("Parsing JSON gagal!");
  return;
}


bool autoMode = (int)j["auto_mode"] == 1;

  if (autoMode) {
    autoControl();
  } else {
    control_LEDs(j);
  }

 delay(1000);


    readsensor();
    



  
   // Balik logika: HIGH → “OFF”, LOW → “ON”
if (digitalRead(LED_01) == 1) LED_01_State = "OFF";
if (digitalRead(LED_01) == 0) LED_01_State = "ON";

if (digitalRead(LED_02) == 1) LED_02_State = "OFF";
if (digitalRead(LED_02) == 0) LED_02_State = "ON";

if (digitalRead(LED_03) == 1) LED_03_State = "OFF";
if (digitalRead(LED_03) == 0) LED_03_State = "ON";


  
   
  
    //........................................ The process of sending the DHT11 sensor data and the state of the LEDs to the database.
  

    postData = "id=esp32_01";
    postData += "&temperature=" + String(send_Temp);
    postData += "&humidity=" + String(send_Humd);
    postData += "&status_read_sensor_dht11=" + send_Status_Read_DHT11;
    postData += "&soil_moisture=" + String(send_Soil);
    postData += "&status_soil=" + send_Status_Soil;
    postData += "&light_level=" + String(send_LDR);
    postData += "&status_light=" + send_Status_LDR;
    postData += "&led_01=" + LED_01_State;
    postData += "&led_02=" + LED_02_State;
    postData += "&led_03=" + LED_03_State;


    payload = "";
  
    digitalWrite(ON_Board_LED, HIGH);
    Serial.println();
    Serial.println("---------------updateDHT11data_and_recordtable.php");
    // Example : http.begin("http://192.168.0.0/ESP32_MySQL_Database/Final/updateDHT11data_and_recordtable.php");
    http.begin("https://rasyadnext.my.id/updateDHT11data_and_recordtable.php");  //--> Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //--> Specify content-type header
   
    httpCode = http.POST(postData); //--> Send the request
    payload = http.getString();  //--> Get the response payload
  
    Serial.print("httpCode : ");
    Serial.println(httpCode); //--> Print HTTP return code
    Serial.print("payload  : ");
    Serial.println(payload);  //--> Print request response payload
    
    http.end();  //Close connection
    Serial.println("---------------");
    digitalWrite(ON_Board_LED, LOW);
    //........................................ 
    
    delay(4000);
  }
  //---------------------------------------- 
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
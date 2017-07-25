//For this documentation i will be using thingspeak.com to connect to the internet
// for the example i will use the readings of a dht sensor but the readings can be used for any other sensor in use

//================================================================================ Initialization 
//Initializing elements of the DHT sensor and the ESP8266 module // the DHT sensor can be substituted with any other kind of sensor 
#include <DHT.h>                                                 //DHT library => got this from here: https://github.com/adafruit/Adafruit_Sensor
#define DHTPIN 6                                                 // Connect the signal pin of DHT11 sensor to digital pin 6
#define DHTTYPE DHT11                                            // gloabal declaration of the DHT11 module and type 
DHT dht(DHTPIN, DHTTYPE);                                       // initialization if the DHT pin and the type

String apiKey = "XXXXXXXXXXXX";                                   //Replace this with the channel ID of your thingspeak API write key

String ssid = "xxxxxxx";                                             //Replace this with the wifi SSID of the wifi network that you will connect to 
String password = "wifi3210";                                      // Replace this with the wifi password that you will connect to.

//================================================================================ Functions
// Function to connect to the thingspeak channel 
  boolean thingSpeakWrite(float value1, float value2) {                 //both value 1 and value 2 belong to the dht sensor readings, the temperature and the humidity 
   String cmd = "AT+CIPSTART=\"TCP\",\"";                             // TCP connection
       cmd += "184.106.153.149";                                      // compussory connection to api.thingspeak.com
       cmd += "\",80";                                                //port of connection is port 80
 Serial1.println(cmd);
    if (DEBUG) Serial.println(cmd);
    if (Serial1.find("Error")) {
    if (DEBUG) Serial.println("AT+CIPSTART error");                      // AT command to send data to thingspeak channel
    return false;
  }

  String getStr = "GET /update?api_key=";                               // the get string calls the GET function of the thingspeak channel with the values of the DHT sensor 
  getStr += apiKey;                                                     // It then combines the values with the api key as that is the format of updating the thingspeak channel
  getStr += "&field1=";                                                       // Temperature field of the DHT sensor 
  getStr += String(value1);                                                  //Convering temperature field to string
  getStr += "&field2=";                                                     // Humidity /field
  getStr += String(value2);                                                 // Coverting humidity to string
  //  getStr +="&field3=";                                                     // You can add any other field according to your sensor inputs
  //  getStr += String(value3);

  getStr += "\r\n\r\n\r";                                                      //combines al the getStr values from both the temperature and the humidity fields 

  //Sending actua data to the thingspeak Channel
 cmd = "AT+CIPSEND=";                                                           // send AT command to ESP8266 to upload the data to cloud
 cmd += String(getStr.length());                                                //..converts the getStr data from all the sensors to string

//================================================================================ setup
void setup() {  // code will only run once here
 DEBUG = true;         // enable debug serial
   Serial.begin(9600); 
   dht.begin();                                                               // Start DHT sensor
   Serial1.begin(115200);                                                     // enable software serial 
    showResponse(1000);   

  Serial1.println("AT+RST");                                                 // reset esp8266 wifi module
  showResponse(1000);

  Serial1.println("AT+CWMODE=1");                                             // set esp8266 wifi module as client from wifi router or AP, there are severall options to change config
  showResponse(1000);

  Serial1.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");          // sets your home router SSID and password, as declared from above 
  showResponse(5000);

  if (DEBUG)  Serial.println("Setup completed");
 

}

//================================================================================ Loop to run throughout the program
void loop() {
  // put your main code here, to run repeatedly:

   // Read sensor values
  float t = dht.readTemperature();                                                  //DHT functions from library DHT.h too read the temperature readings
  float h = dht.readHumidity();                                                     //DHT functions from library DHT.h too read the temperature readings
  if (isnan(t) || isnan(h)) {
    if (DEBUG) Serial.println("Failed to read from DHT Sensor");                    //exception handler in the event that the DHT sensor is faultily connected 
  }
  else {
    if (DEBUG)  Serial.println("Temp=" + String(t) + " *C");    
    if (DEBUG) Serial.println("Humidity=" + String(h) + " %");
      thingSpeakWrite(t, h);                                                                   // Write values to thingspeak, calls the Thingspeak write function,
                                                                                                //passing the arguements value 1 for temp adn value 2 for humidity  
  }


  delay(20000);     // thingspeak.com needs 15 sec delay between updates,
}

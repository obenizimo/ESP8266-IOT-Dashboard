// Importing necessary libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
//#include <Adafruit_BME280.h>
//#include <Adafruit_Sensor.h>

// Replace network credentials with your own WiFi details
const char *ssid = "FiberHGW_TP6AE4_2.4GHz";  // Wi-Fi ağınızın adı (SSID)
const char *password = "3nDTjvXh"; // Wi-Fi şifreniz

bool GPIO_State = 0;
const int Led_Pin = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

JSONVar readings;

// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 5000;


// Create a sensor object
//Adafruit_BME280 bme;         // BME280 connect to ESP32 I2C (GPIO 21 = SDA, GPIO 22 = SCL)

// Init BME280
/*void initBME(){
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}*/



const char html_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <title>ESP IOT DASHBOARD</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <link rel="icon" type="image/png" href="favicon.png">
        <link rel="stylesheet" type="text/css" href="style.css">
    </head>
    <body>
        <div class="topnav">
            <h1>SENSOR READINGS (WEBSOCKET)</h1>
        </div>
        <div class="content">
            <div class="card-grid">
                <div class="card">
                    <p class="card-title"><i class="fas fa-thermometer-threequarters" style="color:#059e8a;"></i> Temperature</p>
                    <p class="reading"><span id="temperature"></span> &deg;C</p>
                </div>
                <div class="card">
                    <p class="card-title"> Humidity</p>
                    <p class="reading"><span id="humidity"></span> &percnt;</p>
                </div>
                <div class="card">
                    <p class="card-title"> Pressure</p>
                    <p class="reading"><span id="pressure"></span> hpa</p>
                </div>
            </div>
        </div>
        <script src="script.js"></script>
    </body>
</html>
  <style>
html {
    font-family: Arial, Helvetica, sans-serif;
    display: inline-block;
    text-align: center;
}
h1 {
    font-size: 1.8rem;
    color: white;
}
.topnav {
    overflow: hidden;
    background-color: #0A1128;
}
body {
    margin: 0;
}
.content {
    padding: 50px;
}
.card-grid {
    max-width: 800px;
    margin: 0 auto;
    display: grid;
    grid-gap: 2rem;
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
}
.card {
    background-color: white;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
}
.card-title {
    font-size: 1.2rem;
    font-weight: bold;
    color: #034078
}
.reading {
    font-size: 1.2rem;
    color: #1282A2;
}
  </style>

<script>

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
// Init web socket when the page loads
window.addEventListener('load', onload);

function onload(event) {
    initWebSocket();
}

function getReadings(){
    websocket.send("getReadings");
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

// When websocket is established, call the getReadings() function
function onOpen(event) {
    console.log('Connection opened');
    getReadings();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

// Function that receives the message from the ESP32 with the readings
function onMessage(event) {
    console.log(event.data);
    var myObj = JSON.parse(event.data);
    var keys = Object.keys(myObj);

    for (var i = 0; i < keys.length; i++){
        var key = keys[i];
        document.getElementById(key).innerHTML = myObj[key];
    }
}
</script>
</body>
</html>
)rawliteral";

// Get Sensor Readings and return JSON object
/*String getSensorReadings(){
  readings["temperature"] = String(bme.readTemperature());
  readings["humidity"] =  String(bme.readHumidity());
  readings["pressure"] = String(bme.readPressure()/100.0F);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}*/

String getSensorReadings() {
  // Rastgele sıcaklık, nem ve basınç değerleri oluştur
  float randomTemperature = random(15, 30); // Örneğin, 15 ila 30 arasında rastgele bir sıcaklık
  float randomHumidity = random(30, 60);    // Örneğin, 30 ila 60 arasında rastgele bir nem
  float randomPressure = random(900, 1100); // Örneğin, 900 ila 1100 arasında rastgele bir basınç

  // Sensör verilerini JSON formatında oluştur ve döndür
  readings["temperature"] = String(randomTemperature);
  readings["humidity"] = String(randomHumidity);
  readings["pressure"] = String(randomPressure);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}


void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    //data[len] = 0;
    //String message = (char*)data;
    // Check if the message is "getReadings"
    //if (strcmp((char*)data, "getReadings") == 0) {
      //if it is, send current sensor readings
      String sensorReadings = getSensorReadings();
      Serial.print(sensorReadings);
      notifyClients(sensorReadings);
    //}
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (GPIO_State){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String(); // Diğer durumlarda boş bir String döndür
}


void setup(){
  // Serial port for debugging purposes
  Serial.begin(9600);
   //initBME();
  pinMode(Led_Pin, OUTPUT);
  digitalWrite(Led_Pin, LOW);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);   // Wi-Fi bağlantısını başlat

  while (WiFi.status() != WL_CONNECTED) { // Wi-Fi bağlantısını bekleyin
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP()); // Bağlantı başarılıysa, IP adresini

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", html_page, processor);
  });

  // Start server
  server.begin();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    String sensorReadings = getSensorReadings();
    Serial.print(sensorReadings);
    notifyClients(sensorReadings);

  lastTime = millis();

  }

  ws.cleanupClients();
}
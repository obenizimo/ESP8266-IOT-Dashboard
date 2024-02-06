# ESP8266-IOT-Dashboard
This Arduino code creates an ESP8266-based IoT control panel and facilitates real-time communication of sensor data via WebSocket.


/*
  This Arduino sketch creates an ESP8266-based IoT dashboard with WebSocket communication for real-time sensor readings.
  It uses the ESPAsyncTCP and ESPAsyncWebServer libraries for handling asynchronous TCP communication and serving web pages, 
  respectively. Additionally, it utilizes the Arduino_JSON library for parsing and creating JSON data.

  The sketch sets up a WebSocket server to communicate with the web client, which continuously receives sensor readings 
  (temperature, humidity, and pressure) from the ESP8266 device and updates the corresponding HTML elements dynamically 
  using JavaScript.

  The HTML template served by the ESP8266 contains a simple dashboard layout with cards displaying sensor readings. 
  The WebSocket connection is established when the web page loads, and sensor readings are sent to the client 
  periodically using WebSocket messages.

  For demonstration purposes, the sketch generates random sensor readings every 5 seconds, simulating data from 
  environmental sensors. These readings are then sent to connected clients via WebSocket messages.

  Author: Oben ÖZKAN
  Date: 6.02.2024
  
*/


#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>

// ---------- WiFi ----------
const char* ssid = "******";
const char* password = "******";

// ---------- CAMERA PINS (AI THINKER) ----------
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ---------- GPS ----------
#define GPS_RX_PIN 15
#define GPS_TX_PIN 14

HardwareSerial gpsSerial(1);
TinyGPSPlus gps;

WebServer server(80);

// ---------- STREAM ----------
static const char* _STREAM_CONTENT_TYPE =
"multipart/x-mixed-replace;boundary=frame";

static const char* _STREAM_BOUNDARY =
"\r\n--frame\r\n";

static const char* _STREAM_PART =
"Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

// ---------- HTML ----------
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>ESP32 CAM + GPS</title>
<style>
body{
text-align:center;
font-family:Arial;
}
img{
width:90%;
max-width:500px;
}
button{
padding:15px;
font-size:18px;
margin-top:20px;
}
#result{
margin-top:20px;
}
</style>
</head>

<body>

<h2>ESP32 CAM Live Stream</h2>

<img src="/stream">

<br>

<button onclick="getLocation()">
Show GPS Location
</button>

<div id="result"></div>

<script>

function getLocation(){

fetch('/location')

.then(r=>r.text())

.then(data=>{
document.getElementById("result").innerHTML=
data.replace(/\n/g,"<br>");
})

}

</script>

</body>
</html>
)rawliteral";

// ---------- STREAM ----------
void handleStream(){

WiFiClient client = server.client();

client.println("HTTP/1.1 200 OK");

client.println("Content-Type: multipart/x-mixed-replace; boundary=frame");

client.println();

while(client.connected()){

camera_fb_t *fb =
esp_camera_fb_get();

if(!fb)
continue;

client.printf(
_STREAM_PART,
fb->len
);

client.write(
fb->buf,
fb->len
);

client.print(
_STREAM_BOUNDARY
);

esp_camera_fb_return(fb);

delay(10);

}

}

// ---------- GPS ----------
void handleLocation(){

unsigned long start =
millis();

while(
millis()-start<1000
){

while(
gpsSerial.available()
){

gps.encode(
gpsSerial.read()
);

}

}

if(
gps.location.isValid()
){

float lat =
gps.location.lat();

float lng =
gps.location.lng();

String msg="Latitude: ";

msg+=String(lat,6);

msg+="\nLongitude: ";

msg+=String(lng,6);

msg+="\n\n";

msg+="<a href='https://maps.google.com/?q=";

msg+=String(lat,6);

msg+=",";

msg+=String(lng,6);

msg+="'>Open in Google Maps</a>";

server.send(
200,
"text/html",
msg
);

}
else{

server.send(
200,
"text/plain",
"GPS not fixed.\nGo outside and wait."
);

}

}

// ---------- HOME ----------
void handleRoot(){

server.send_P(
200,
"text/html",
INDEX_HTML
);

}

// ---------- SETUP ----------
void setup(){

Serial.begin(
115200
);

gpsSerial.begin(
9600,
SERIAL_8N1,
GPS_RX_PIN,
GPS_TX_PIN
);

// CAMERA

camera_config_t config;

config.ledc_channel=
LEDC_CHANNEL_0;

config.ledc_timer=
LEDC_TIMER_0;

config.pin_d0=
Y2_GPIO_NUM;

config.pin_d1=
Y3_GPIO_NUM;

config.pin_d2=
Y4_GPIO_NUM;

config.pin_d3=
Y5_GPIO_NUM;

config.pin_d4=
Y6_GPIO_NUM;

config.pin_d5=
Y7_GPIO_NUM;

config.pin_d6=
Y8_GPIO_NUM;

config.pin_d7=
Y9_GPIO_NUM;

config.pin_xclk=
XCLK_GPIO_NUM;

config.pin_pclk=
PCLK_GPIO_NUM;

config.pin_vsync=
VSYNC_GPIO_NUM;

config.pin_href=
HREF_GPIO_NUM;

config.pin_sscb_sda=
SIOD_GPIO_NUM;

config.pin_sscb_scl=
SIOC_GPIO_NUM;

config.pin_pwdn=
PWDN_GPIO_NUM;

config.pin_reset=
RESET_GPIO_NUM;

config.xclk_freq_hz=
20000000;

config.pixel_format=
PIXFORMAT_JPEG;

config.frame_size=
FRAMESIZE_VGA;

config.jpeg_quality=
12;

config.fb_count=
2;

if(
esp_camera_init(
&config
)!=ESP_OK
){

Serial.println(
"Camera Failed"
);

return;

}

// WIFI

WiFi.begin(
ssid,
password
);

while(
WiFi.status()
!=WL_CONNECTED
){

delay(500);

Serial.print(
"."
);

}

Serial.println();

Serial.println(
"Connected"
);

Serial.println(
WiFi.localIP()
);

// SERVER

server.on(
"/",
handleRoot
);

server.on(
"/stream",
handleStream
);

server.on(
"/location",
handleLocation
);

server.begin();

}

// ---------- LOOP ----------
void loop(){

server.handleClient();

while(
gpsSerial.available()
){

gps.encode(
gpsSerial.read()
);

}

}

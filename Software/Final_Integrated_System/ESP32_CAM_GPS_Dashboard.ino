#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"
#include <HardwareSerial.h>
#include <TinyGPS++.h>

// =====================================================
//                    WI-FI DETAILS
// =====================================================

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// =====================================================
//          AI THINKER ESP32-CAM CAMERA PINS
// =====================================================

#define PWDN_GPIO_NUM      32
#define RESET_GPIO_NUM     -1
#define XCLK_GPIO_NUM       0
#define SIOD_GPIO_NUM      26
#define SIOC_GPIO_NUM      27

#define Y9_GPIO_NUM        35
#define Y8_GPIO_NUM        34
#define Y7_GPIO_NUM        39
#define Y6_GPIO_NUM        36
#define Y5_GPIO_NUM        21
#define Y4_GPIO_NUM        19
#define Y3_GPIO_NUM        18
#define Y2_GPIO_NUM         5

#define VSYNC_GPIO_NUM     25
#define HREF_GPIO_NUM      23
#define PCLK_GPIO_NUM      22

// =====================================================
//                    NEO-6M GPS
// =====================================================

// NEO-6M TX -> ESP32-CAM GPIO 15
#define GPS_RX_PIN 15

// We only receive GPS data, so ESP32 TX is not required.
#define GPS_TX_PIN -1

HardwareSerial gpsSerial(1);
TinyGPSPlus gps;

// =====================================================
//                 HTTP SERVER HANDLES
// =====================================================

httpd_handle_t dashboardServer = NULL;
httpd_handle_t streamServer = NULL;

// =====================================================
//                    STREAM SETTINGS
// =====================================================

#define PART_BOUNDARY "123456789000000000000987654321"

static const char* STREAM_CONTENT_TYPE =
    "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;

static const char* STREAM_BOUNDARY =
    "\r\n--" PART_BOUNDARY "\r\n";

static const char* STREAM_PART =
    "Content-Type: image/jpeg\r\n"
    "Content-Length: %u\r\n\r\n";

// =====================================================
//                  DASHBOARD WEBPAGE
// =====================================================

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">

    <meta
        name="viewport"
        content="width=device-width, initial-scale=1.0"
    >

    <title>Drone Surveillance Dashboard</title>

    <style>

        * {
            box-sizing: border-box;
        }

        body {
            margin: 0;
            padding: 15px;
            background: #eef2f5;
            font-family: Arial, Helvetica, sans-serif;
            color: #17212b;
        }

        .container {
            width: 100%;
            max-width: 900px;
            margin: auto;
        }

        .header {
            background: white;
            border-radius: 14px;
            padding: 18px;
            margin-bottom: 15px;
            text-align: center;
            box-shadow: 0 3px 12px rgba(0,0,0,0.10);
        }

        .header h1 {
            margin: 0;
            font-size: 25px;
        }

        .header p {
            margin: 8px 0 0;
            color: #59636d;
        }

        .status-row {
            display: flex;
            justify-content: center;
            align-items: center;
            gap: 8px;
            margin-top: 12px;
        }

        .status-dot {
            width: 12px;
            height: 12px;
            border-radius: 50%;
            background: orange;
        }

        .video-box {
            background: black;
            border-radius: 14px;
            padding: 8px;
            margin-bottom: 15px;
            overflow: hidden;
            box-shadow: 0 3px 12px rgba(0,0,0,0.15);
        }

        #cameraStream {
            display: block;
            width: 100%;
            min-height: 220px;
            object-fit: contain;
            border-radius: 9px;
        }

        .section-title {
            margin: 22px 0 10px;
            font-size: 21px;
        }

        .grid {
            display: grid;
            grid-template-columns:
                repeat(auto-fit, minmax(155px, 1fr));
            gap: 12px;
        }

        .card {
            background: white;
            padding: 15px;
            border-radius: 12px;
            box-shadow: 0 3px 10px rgba(0,0,0,0.08);
        }

        .label {
            color: #68737d;
            font-size: 13px;
            margin-bottom: 7px;
        }

        .value {
            font-size: 20px;
            font-weight: bold;
            word-break: break-word;
        }

        .coordinates {
            background: white;
            margin-top: 15px;
            padding: 17px;
            border-radius: 12px;
            box-shadow: 0 3px 10px rgba(0,0,0,0.08);
        }

        .coordinate-line {
            margin: 9px 0;
            font-size: 18px;
        }

        .map-button {
            display: block;
            width: 100%;
            margin-top: 16px;
            padding: 14px;
            text-align: center;
            text-decoration: none;
            border-radius: 10px;
            background: #087f5b;
            color: white;
            font-size: 18px;
            font-weight: bold;
        }

        .map-button.disabled {
            background: #8b949c;
            pointer-events: none;
        }

        .footer {
            text-align: center;
            color: #69737c;
            font-size: 13px;
            margin-top: 18px;
            padding-bottom: 15px;
        }

        .good {
            color: green;
        }

        .warning {
            color: darkorange;
        }

        .bad {
            color: red;
        }

    </style>
</head>

<body>

<div class="container">

    <div class="header">

        <h1>Aerial Surveillance Drone</h1>

        <p>Live Camera and GPS Telemetry Dashboard</p>

        <div class="status-row">
            <div id="statusDot" class="status-dot"></div>
            <strong id="connectionStatus">
                Waiting for GPS data...
            </strong>
        </div>

    </div>

    <div class="video-box">

        <img
            id="cameraStream"
            alt="ESP32-CAM Live Surveillance Stream"
        >

    </div>

    <h2 class="section-title">Drone GPS Telemetry</h2>

    <div class="grid">

        <div class="card">
            <div class="label">GPS Fix</div>
            <div class="value" id="gpsFix">Waiting</div>
        </div>

        <div class="card">
            <div class="label">Satellites</div>
            <div class="value" id="satellites">--</div>
        </div>

        <div class="card">
            <div class="label">Altitude</div>
            <div class="value" id="altitude">--</div>
        </div>

        <div class="card">
            <div class="label">Ground Speed</div>
            <div class="value" id="speed">--</div>
        </div>

        <div class="card">
            <div class="label">Course / Heading</div>
            <div class="value" id="course">--</div>
        </div>

        <div class="card">
            <div class="label">HDOP</div>
            <div class="value" id="hdop">--</div>
        </div>

        <div class="card">
            <div class="label">GPS Date</div>
            <div class="value" id="gpsDate">--</div>
        </div>

        <div class="card">
            <div class="label">GPS Time</div>
            <div class="value" id="gpsTime">--</div>
        </div>

        <div class="card">
            <div class="label">Data Age</div>
            <div class="value" id="dataAge">--</div>
        </div>

    </div>

    <div class="coordinates">

        <h2 style="margin-top:0;">Current Drone Coordinates</h2>

        <div class="coordinate-line">
            <strong>Latitude:</strong>
            <span id="latitude">Waiting...</span>
        </div>

        <div class="coordinate-line">
            <strong>Longitude:</strong>
            <span id="longitude">Waiting...</span>
        </div>

        <a
            id="mapLink"
            class="map-button disabled"
            href="#"
            target="_blank"
            rel="noopener noreferrer"
        >
            Open Drone Location in Google Maps
        </a>

    </div>

    <div class="footer">
        ESP32-CAM + NEO-6M GPS | Live Drone Monitoring
    </div>

</div>

<script>

    // Camera stream runs on port 81.
    document.getElementById("cameraStream").src =
        "http://" + window.location.hostname + ":81/stream";

    async function updateGPS() {

        try {

            const response = await fetch(
                "/gps?time=" + new Date().getTime()
            );

            if (!response.ok) {
                throw new Error("GPS endpoint error");
            }

            const data = await response.json();

            document.getElementById("satellites").textContent =
                data.satellites;

            document.getElementById("altitude").textContent =
                data.altitude_valid
                ? data.altitude.toFixed(2) + " m"
                : "--";

            document.getElementById("speed").textContent =
                data.speed_valid
                ? data.speed.toFixed(2) + " km/h"
                : "--";

            document.getElementById("course").textContent =
                data.course_valid
                ? data.course.toFixed(2) + "°"
                : "--";

            document.getElementById("hdop").textContent =
                data.hdop_valid
                ? data.hdop.toFixed(2)
                : "--";

            document.getElementById("gpsDate").textContent =
                data.date_valid
                ? data.date
                : "--";

            document.getElementById("gpsTime").textContent =
                data.time_valid
                ? data.time + " UTC"
                : "--";

            document.getElementById("dataAge").textContent =
                data.location_valid
                ? data.age + " ms"
                : "--";

            const statusDot =
                document.getElementById("statusDot");

            const statusText =
                document.getElementById("connectionStatus");

            const fixText =
                document.getElementById("gpsFix");

            const mapLink =
                document.getElementById("mapLink");

            if (data.location_valid) {

                document.getElementById("latitude").textContent =
                    data.latitude.toFixed(6);

                document.getElementById("longitude").textContent =
                    data.longitude.toFixed(6);

                fixText.textContent = "FIX ACQUIRED";
                fixText.className = "value good";

                statusDot.style.background = "green";

                statusText.textContent =
                    "Live GPS position received";

                mapLink.href =
                    "https://maps.google.com/?q=" +
                    data.latitude.toFixed(6) +
                    "," +
                    data.longitude.toFixed(6);

                mapLink.classList.remove("disabled");

            } else {

                document.getElementById("latitude").textContent =
                    "No valid fix";

                document.getElementById("longitude").textContent =
                    "No valid fix";

                fixText.textContent = "SEARCHING";
                fixText.className = "value warning";

                statusDot.style.background = "orange";

                statusText.textContent =
                    "Searching for GPS satellites";

                mapLink.href = "#";
                mapLink.classList.add("disabled");

            }

        } catch (error) {

            document.getElementById("connectionStatus").textContent =
                "Unable to receive GPS data";

            document.getElementById("statusDot").style.background =
                "red";

            document.getElementById("gpsFix").textContent =
                "ERROR";

            document.getElementById("gpsFix").className =
                "value bad";

        }

    }

    // Immediately request GPS data.
    updateGPS();

    // Update telemetry automatically every 2 seconds.
    setInterval(updateGPS, 2000);

</script>

</body>
</html>
)rawliteral";

// =====================================================
//             CAMERA STREAM REQUEST HANDLER
// =====================================================

static esp_err_t streamHandler(httpd_req_t* request)
{
    camera_fb_t* frameBuffer = NULL;

    esp_err_t result =
        httpd_resp_set_type(
            request,
            STREAM_CONTENT_TYPE
        );

    if (result != ESP_OK)
    {
        return result;
    }

    httpd_resp_set_hdr(
        request,
        "Access-Control-Allow-Origin",
        "*"
    );

    while (true)
    {
        frameBuffer = esp_camera_fb_get();

        if (!frameBuffer)
        {
            Serial.println("Camera frame capture failed");
            result = ESP_FAIL;
        }
        else
        {
            char headerBuffer[128];

            size_t headerLength =
                snprintf(
                    headerBuffer,
                    sizeof(headerBuffer),
                    STREAM_PART,
                    frameBuffer->len
                );

            result =
                httpd_resp_send_chunk(
                    request,
                    STREAM_BOUNDARY,
                    strlen(STREAM_BOUNDARY)
                );

            if (result == ESP_OK)
            {
                result =
                    httpd_resp_send_chunk(
                        request,
                        headerBuffer,
                        headerLength
                    );
            }

            if (result == ESP_OK)
            {
                result =
                    httpd_resp_send_chunk(
                        request,
                        reinterpret_cast<const char*>(
                            frameBuffer->buf
                        ),
                        frameBuffer->len
                    );
            }

            esp_camera_fb_return(frameBuffer);
            frameBuffer = NULL;
        }

        if (result != ESP_OK)
        {
            break;
        }

        delay(10);
    }

    return result;
}

// =====================================================
//               MAIN PAGE REQUEST HANDLER
// =====================================================

static esp_err_t rootHandler(httpd_req_t* request)
{
    httpd_resp_set_type(
        request,
        "text/html"
    );

    httpd_resp_set_hdr(
        request,
        "Cache-Control",
        "no-store"
    );

    return httpd_resp_send(
        request,
        INDEX_HTML,
        HTTPD_RESP_USE_STRLEN
    );
}

// =====================================================
//                GPS JSON REQUEST HANDLER
// =====================================================

static esp_err_t gpsHandler(httpd_req_t* request)
{
    // Consider location live only when valid and recent.
    bool locationValid =
        gps.location.isValid() &&
        gps.location.age() < 5000;

    bool altitudeValid =
        gps.altitude.isValid();

    bool speedValid =
        gps.speed.isValid();

    bool courseValid =
        gps.course.isValid();

    bool hdopValid =
        gps.hdop.isValid();

    bool dateValid =
        gps.date.isValid();

    bool timeValid =
        gps.time.isValid();

    uint32_t satellites = 0;

    if (gps.satellites.isValid())
    {
        satellites = gps.satellites.value();
    }

    String gpsDate = "";

    if (dateValid)
    {
        char dateBuffer[20];

        snprintf(
            dateBuffer,
            sizeof(dateBuffer),
            "%02d/%02d/%04d",
            gps.date.day(),
            gps.date.month(),
            gps.date.year()
        );

        gpsDate = dateBuffer;
    }

    String gpsTime = "";

    if (timeValid)
    {
        char timeBuffer[20];

        snprintf(
            timeBuffer,
            sizeof(timeBuffer),
            "%02d:%02d:%02d",
            gps.time.hour(),
            gps.time.minute(),
            gps.time.second()
        );

        gpsTime = timeBuffer;
    }

    String json = "{";

    json += "\"location_valid\":";
    json += locationValid ? "true" : "false";

    json += ",\"latitude\":";
    json += locationValid
        ? String(gps.location.lat(), 6)
        : "0.0";

    json += ",\"longitude\":";
    json += locationValid
        ? String(gps.location.lng(), 6)
        : "0.0";

    json += ",\"satellites\":";
    json += String(satellites);

    json += ",\"altitude_valid\":";
    json += altitudeValid ? "true" : "false";

    json += ",\"altitude\":";
    json += altitudeValid
        ? String(gps.altitude.meters(), 2)
        : "0.0";

    json += ",\"speed_valid\":";
    json += speedValid ? "true" : "false";

    json += ",\"speed\":";
    json += speedValid
        ? String(gps.speed.kmph(), 2)
        : "0.0";

    json += ",\"course_valid\":";
    json += courseValid ? "true" : "false";

    json += ",\"course\":";
    json += courseValid
        ? String(gps.course.deg(), 2)
        : "0.0";

    json += ",\"hdop_valid\":";
    json += hdopValid ? "true" : "false";

    json += ",\"hdop\":";
    json += hdopValid
        ? String(gps.hdop.hdop(), 2)
        : "0.0";

    json += ",\"date_valid\":";
    json += dateValid ? "true" : "false";

    json += ",\"date\":\"";
    json += gpsDate;
    json += "\"";

    json += ",\"time_valid\":";
    json += timeValid ? "true" : "false";

    json += ",\"time\":\"";
    json += gpsTime;
    json += "\"";

    json += ",\"age\":";

    if (gps.location.isValid())
    {
        json += String(gps.location.age());
    }
    else
    {
        json += "0";
    }

    json += "}";

    httpd_resp_set_type(
        request,
        "application/json"
    );

    httpd_resp_set_hdr(
        request,
        "Cache-Control",
        "no-store, no-cache, must-revalidate"
    );

    httpd_resp_set_hdr(
        request,
        "Access-Control-Allow-Origin",
        "*"
    );

    return httpd_resp_send(
        request,
        json.c_str(),
        json.length()
    );
}

// =====================================================
//                   START WEB SERVERS
// =====================================================

void startWebServers()
{
    // ---------------- Dashboard server: port 80 ----------------

    httpd_config_t dashboardConfig =
        HTTPD_DEFAULT_CONFIG();

    dashboardConfig.server_port = 80;

    httpd_uri_t rootUri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = rootHandler,
        .user_ctx  = NULL
    };

    httpd_uri_t gpsUri = {
        .uri       = "/gps",
        .method    = HTTP_GET,
        .handler   = gpsHandler,
        .user_ctx  = NULL
    };

    if (
        httpd_start(
            &dashboardServer,
            &dashboardConfig
        ) == ESP_OK
    )
    {
        httpd_register_uri_handler(
            dashboardServer,
            &rootUri
        );

        httpd_register_uri_handler(
            dashboardServer,
            &gpsUri
        );

        Serial.println(
            "Dashboard server started on port 80"
        );
    }
    else
    {
        Serial.println(
            "Failed to start dashboard server"
        );
    }

    // ---------------- Camera stream server: port 81 ----------------

    httpd_config_t streamConfig =
        HTTPD_DEFAULT_CONFIG();

    streamConfig.server_port = 81;

    // Control port must be different from dashboard server.
    streamConfig.ctrl_port =
        dashboardConfig.ctrl_port + 1;

    httpd_uri_t streamUri = {
        .uri       = "/stream",
        .method    = HTTP_GET,
        .handler   = streamHandler,
        .user_ctx  = NULL
    };

    if (
        httpd_start(
            &streamServer,
            &streamConfig
        ) == ESP_OK
    )
    {
        httpd_register_uri_handler(
            streamServer,
            &streamUri
        );

        Serial.println(
            "Camera stream server started on port 81"
        );
    }
    else
    {
        Serial.println(
            "Failed to start camera stream server"
        );
    }
}

// =====================================================
//                    CAMERA SETUP
// =====================================================

bool initializeCamera()
{
    camera_config_t config;

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;

    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;

    config.pin_xclk  = XCLK_GPIO_NUM;
    config.pin_pclk  = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href  = HREF_GPIO_NUM;

    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;

    config.pin_pwdn  = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;

    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    if (psramFound())
    {
        config.frame_size   = FRAMESIZE_VGA;
        config.jpeg_quality = 12;
        config.fb_count     = 2;
        config.grab_mode    = CAMERA_GRAB_LATEST;
        config.fb_location  = CAMERA_FB_IN_PSRAM;
    }
    else
    {
        config.frame_size   = FRAMESIZE_QVGA;
        config.jpeg_quality = 15;
        config.fb_count     = 1;
        config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;
        config.fb_location  = CAMERA_FB_IN_DRAM;
    }

    esp_err_t cameraResult =
        esp_camera_init(&config);

    if (cameraResult != ESP_OK)
    {
        Serial.printf(
            "Camera initialization failed: 0x%x\n",
            cameraResult
        );

        return false;
    }

    return true;
}

// =====================================================
//                         SETUP
// =====================================================

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(false);

    delay(1000);

    Serial.println();
    Serial.println("==============================");
    Serial.println("Aerial Surveillance Drone");
    Serial.println("ESP32-CAM + NEO-6M GPS");
    Serial.println("==============================");

    // Start GPS UART.
    gpsSerial.begin(
        9600,
        SERIAL_8N1,
        GPS_RX_PIN,
        GPS_TX_PIN
    );

    Serial.println("GPS serial started at 9600 baud");

    // Initialize camera.
    if (!initializeCamera())
    {
        Serial.println(
            "System stopped because camera failed"
        );

        return;
    }

    Serial.println("Camera initialized successfully");

    // Connect to Wi-Fi.
    WiFi.mode(WIFI_STA);

    WiFi.begin(
        ssid,
        password
    );

    Serial.print("Connecting to Wi-Fi");

    unsigned long connectionStart = millis();

    while (
        WiFi.status() != WL_CONNECTED &&
        millis() - connectionStart < 30000
    )
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Wi-Fi connection failed");
        Serial.println(
            "Check Wi-Fi name, password and 2.4 GHz network"
        );

        return;
    }

    // Reduces unnecessary delays in camera streaming.
    WiFi.setSleep(false);

    Serial.println("Wi-Fi connected");

    Serial.print("Dashboard address: http://");
    Serial.println(WiFi.localIP());

    Serial.print("Direct stream: http://");
    Serial.print(WiFi.localIP());
    Serial.println(":81/stream");

    startWebServers();
}

// =====================================================
//                          LOOP
// =====================================================

void loop()
{
    // Continuously read NMEA data coming from the NEO-6M.
    while (gpsSerial.available() > 0)
    {
        char incomingCharacter =
            gpsSerial.read();

        gps.encode(incomingCharacter);
    }

    delay(2);
}

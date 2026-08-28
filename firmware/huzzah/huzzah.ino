/*
  Robot Car — Feather HUZZAH firmware
  ------------------------------------
  Runs on: Adafruit Feather HUZZAH (ESP8266)
  Role:    Creates its own WiFi access point, serves a control web page,
           and continuously streams a single-byte movement command to
           the Metro M0 over hardware Serial.

  IMPORTANT — power during development vs. normal use:
  The ESP8266 exposes only one hardware UART, which is shared with the
  USB-to-serial chip. While this board is connected to a computer via
  USB (e.g. during flashing or Serial Monitor debugging), USB traffic
  will appear on the TX pin alongside your motor commands. For real
  operation, power this board from a standalone source (battery via
  the Verter) with USB disconnected.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ============================================================
// CONFIGURATION — edit these
// ============================================================
const char* WIFI_SSID     = "RobotCar";     // name of the network THIS BOARD creates
const char* WIFI_PASSWORD = "robot1234";    // password for that network

// Command bytes — must match Metro M0 firmware exactly
#define CMD_FORWARD  0xFF
#define CMD_BACKWARD 0x01
#define CMD_RIGHT    0xAA
#define CMD_LEFT     0x55
#define CMD_STOP     0x00

ESP8266WebServer server(80);
byte currentCommand = CMD_STOP;

// ============================================================
// HTML / CSS / JS — edit freely, this is just presentation
// ============================================================
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Robot Car</title>
    <style>
        body {
            background: #1a1a1a;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            font-family: monospace;
            color: white;
        }
        .controls { text-align: center; }
        .btn {
            width: 80px;
            height: 80px;
            font-size: 24px;
            margin: 6px;
            cursor: pointer;
            background: #333;
            color: white;
            border: 2px solid #666;
            border-radius: 12px;
            user-select: none;
            -webkit-user-select: none;
        }
        .btn:active { background: #555; }
        .row { display: flex; justify-content: center; }
    </style>
</head>
<body>
    <div class="controls">
        <div class="row"><button class="btn" id="forward">▲</button></div>
        <div class="row">
            <button class="btn" id="left">◄</button>
            <button class="btn" id="backward">▼</button>
            <button class="btn" id="right">►</button>
        </div>
    </div>

    <script>
        const commands = {
            forward:  '/forward',
            backward: '/backward',
            left:     '/left',
            right:    '/right'
        };

        let activeKey = null;
        let sendInterval = null;

        function startCommand(direction) {
            if (activeKey === direction) return;
            stopCommand();
            activeKey = direction;
            sendRequest(direction);
            sendInterval = setInterval(() => sendRequest(direction), 100);
        }

        function stopCommand() {
            if (sendInterval) {
                clearInterval(sendInterval);
                sendInterval = null;
            }
            activeKey = null;
            sendRequest('stop');
        }

        function sendRequest(direction) {
            fetch(direction === 'stop' ? '/stop' : commands[direction])
                .catch(() => {});
        }

        document.querySelectorAll('.btn').forEach(btn => {
            btn.addEventListener('mousedown',  () => startCommand(btn.id));
            btn.addEventListener('touchstart', (e) => { e.preventDefault(); startCommand(btn.id); });
            btn.addEventListener('mouseup',    stopCommand);
            btn.addEventListener('touchend',   stopCommand);
            btn.addEventListener('mouseleave', stopCommand);
        });

        const keyMap = {
            'ArrowUp'   : 'forward',
            'ArrowDown' : 'backward',
            'ArrowLeft' : 'left',
            'ArrowRight': 'right'
        };

        document.addEventListener('keydown', (e) => {
            if (keyMap[e.key] && activeKey !== keyMap[e.key]) {
                e.preventDefault();
                startCommand(keyMap[e.key]);
            }
        });

        document.addEventListener('keyup', (e) => {
            if (keyMap[e.key]) stopCommand();
        });
    </script>
</body>
</html>
)rawliteral";

// ============================================================
// ROUTE HANDLERS
// ============================================================
void handleRoot()     { server.send(200, "text/html", INDEX_HTML); }
void handleForward()  { currentCommand = CMD_FORWARD;  server.send(200, "text/plain", "ok"); }
void handleBackward() { currentCommand = CMD_BACKWARD; server.send(200, "text/plain", "ok"); }
void handleRight()    { currentCommand = CMD_RIGHT;    server.send(200, "text/plain", "ok"); }
void handleLeft()     { currentCommand = CMD_LEFT;     server.send(200, "text/plain", "ok"); }
void handleStop()     { currentCommand = CMD_STOP;     server.send(200, "text/plain", "ok"); }

// ============================================================
// SETUP
// ============================================================
void setup() {
    Serial.begin(9600);

    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Access point started. IP: ");
    Serial.println(WiFi.softAPIP());   // typically 192.168.4.1

    server.on("/",         handleRoot);
    server.on("/forward",  handleForward);
    server.on("/backward", handleBackward);
    server.on("/right",    handleRight);
    server.on("/left",     handleLeft);
    server.on("/stop",     handleStop);

    server.begin();
    Serial.println("Web server started.");
}

// ============================================================
// LOOP
// ============================================================
void loop() {
    server.handleClient();
    Serial.write(currentCommand);
    delay(50);   // ~20 bytes/sec — plenty for the Metro M0's 500ms timeout,
                 // and avoids hammering the CPU/UART at full speed
}

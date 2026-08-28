#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ============================================================
// CONFIGURATION — edit these
// ============================================================
const char* WIFI_SSID     = "Robot Car";      // network name your phone connects to
const char* WIFI_PASSWORD = "Vroom123";      // password for that network

// Command bytes — must match Metro M0 definitions exactly
#define CMD_FORWARD  0xFF
#define CMD_BACKWARD 0x01
#define CMD_RIGHT    0xAA
#define CMD_LEFT     0x55
#define CMD_STOP     0x00

// ============================================================
// WEB SERVER
// ============================================================
ESP8266WebServer server(80);    // 80 is the standard HTTP port

// ============================================================
// CURRENT STATE
// ============================================================
byte currentCommand = CMD_STOP;

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
        .controls {
            text-align: center;
        }
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
        .btn:active {
            background: #555;
        }
        .row {
            display: flex;
            justify-content: center;
        }
        h1, h2, h3 {
            font-family: monospace, sans-serif;
            font-weight: 700;
            font-size: 100px;
            color: pink;
        }
    </style>
</head>
<body>
    <div class="controls">
        <div class="row">
            <h1>Robot Car</h1>
        </div>
        <div class="row">
            <button class="btn" id="forward">▲</button>
        </div>
        <div class="row">
            <button class="btn" id="left">◄</button>
            <button class="btn" id="backward">▼</button>
            <button class="btn" id="right">►</button>
        </div>
    </div>

    <script>
        // ── Command routing ──
        const commands = {
            forward:  '/forward',
            backward: '/backward',
            left:     '/left',
            right:    '/right'
        };

        let activeKey = null;
        let sendInterval = null;

        function startCommand(direction) {
            if (activeKey === direction) return;  // already active
            stopCommand();                         // cancel any previous
            activeKey = direction;
            sendRequest(direction);                // send immediately
            sendInterval = setInterval(() => sendRequest(direction), 100);  // then every 100ms
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
                .catch(() => {});   // silently ignore network errors
        }

        // ── Button events (mouse + touch) ──
        document.querySelectorAll('.btn').forEach(btn => {
            btn.addEventListener('mousedown',  () => startCommand(btn.id));
            btn.addEventListener('touchstart', (e) => { e.preventDefault(); startCommand(btn.id); });
            btn.addEventListener('mouseup',    stopCommand);
            btn.addEventListener('touchend',   stopCommand);
            btn.addEventListener('mouseleave', stopCommand);  // finger slides off button
        });

        // ── Keyboard events ──
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
void handleRoot() {
    server.send(200, "text/html", INDEX_HTML);
}

void handleForward() {
    currentCommand = CMD_FORWARD;
    server.send(200, "text/plain", "ok");
}

void handleBackward() {
    currentCommand = CMD_BACKWARD;
    server.send(200, "text/plain", "ok");
}

void handleRight() {
    currentCommand = CMD_RIGHT;
    server.send(200, "text/plain", "ok");
}

void handleLeft() {
    currentCommand = CMD_LEFT;
    server.send(200, "text/plain", "ok");
}

void handleStop() {
    currentCommand = CMD_STOP;
    server.send(200, "text/plain", "ok");
}

// ============================================================
// SETUP
// ============================================================
void setup() {
    Serial.begin(9600);

    // Create WiFi access point
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Access point started. IP: ");
    Serial.println(WiFi.softAPIP());   // usually 192.168.4.1

    // Register routes
    server.on("/",        handleRoot);
    server.on("/forward", handleForward);
    server.on("/backward",handleBackward);
    server.on("/right",   handleRight);
    server.on("/left",    handleLeft);
    server.on("/stop",    handleStop);

    server.begin();
    Serial.println("Web server started.");
}

// ============================================================
// LOOP
// ============================================================
void loop() {
    server.handleClient();          // process any incoming HTTP requests
    Serial.write(currentCommand);   // continuously send current state to Metro M0
    delay(50); // Breathing room for the Feather, so as to not overheat unnecessarily.
}
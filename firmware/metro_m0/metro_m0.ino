/*
  Robot Car — Metro M0 Express firmware
  --------------------------------------
  Runs on: Adafruit Metro M0 Express
  Role:    Receives single-byte movement commands over Serial1 (hardware
           UART, connected to the Feather HUZZAH's TX pin) and drives a
           TB6612FNG dual motor driver accordingly.

  Motor driver: DollaTek TB6612FNG
  Motors:       2x TT DC gear motors

  Pin mapping — VERIFY AGAINST YOUR OWN WIRING BEFORE FLASHING.
  AIN1/BIN1 pins carry over from the original single-direction-pin driver
  wiring; AIN2/BIN2 and STBY pin numbers below are placeholders and must
  be updated to match whichever free digital pins you actually wired on
  your Metro M0:
    Pin 4  -> AIN1  (Motor A / left  — direction)
    Pin 8  -> AIN2  (Motor A / left  — direction)   [placeholder, confirm]
    Pin 5  -> PWMA  (Motor A / left  — speed, must be PWM-capable)
    Pin 7  -> BIN1  (Motor B / right — direction)
    Pin 9  -> BIN2  (Motor B / right — direction)   [placeholder, confirm]
    Pin 6  -> PWMB  (Motor B / right — speed, must be PWM-capable)
    Pin 3  -> STBY  (standby — must be HIGH, wire to a spare digital
                      pin or permanently to 3.3V)   [placeholder, confirm]

  Safety: if no command byte arrives for TIMEOUT_MS, motors are stopped
  automatically. This turns the continuous byte stream from the HUZZAH
  into an implicit heartbeat — a dropped WiFi connection results in the
  robot stopping rather than continuing on its last command indefinitely.
*/

// ---- Motor A (left) ----
const int AIN1 = 4;
const int AIN2 = 8;
const int PWMA = 5;

// ---- Motor B (right) ----
const int BIN1 = 7;
const int BIN2 = 9;
const int PWMB = 6;

// ---- Shared ----
const int STBY = 3;

// ---- Command protocol — must match the HUZZAH firmware exactly ----
#define CMD_FORWARD  0xFF
#define CMD_BACKWARD 0x01
#define CMD_RIGHT    0xAA
#define CMD_LEFT     0x55
#define CMD_STOP     0x00

const int SPEED = 255;                  // 0-255, full speed
const unsigned long TIMEOUT_MS = 500;   // stop motors if no command in this window
unsigned long lastCommandTime = 0;

void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);   // take the driver out of standby
  stopMotors();

  Serial.begin(9600);         // USB — debugging only
  Serial1.begin(9600);        // hardware UART — HUZZAH link

  lastCommandTime = millis();
  Serial.println("Metro M0 ready.");
}

void loop() {
  if (Serial1.available() > 0) {
    byte command = Serial1.read();
    lastCommandTime = millis();

    switch (command) {
      case CMD_FORWARD:
        driveMotor(AIN1, AIN2, PWMA, true,  SPEED);
        driveMotor(BIN1, BIN2, PWMB, true,  SPEED);
        break;

      case CMD_BACKWARD:
        driveMotor(AIN1, AIN2, PWMA, false, SPEED);
        driveMotor(BIN1, BIN2, PWMB, false, SPEED);
        break;

      case CMD_RIGHT:
        driveMotor(AIN1, AIN2, PWMA, true,  SPEED);
        driveMotor(BIN1, BIN2, PWMB, false, SPEED);
        break;

      case CMD_LEFT:
        driveMotor(AIN1, AIN2, PWMA, false, SPEED);
        driveMotor(BIN1, BIN2, PWMB, true,  SPEED);
        break;

      case CMD_STOP:
      default:
        stopMotors();
        break;
    }
  }

  // Safety timeout — stop if the HUZZAH link goes quiet
  if (millis() - lastCommandTime > TIMEOUT_MS) {
    stopMotors();
  }
}

// Drives a single motor. forward=true spins toward AIN1/BIN1 HIGH.
void driveMotor(int in1, int in2, int pwmPin, bool forward, int speed) {
  digitalWrite(in1, forward ? HIGH : LOW);
  digitalWrite(in2, forward ? LOW  : HIGH);
  analogWrite(pwmPin, speed);
}

void stopMotors() {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}

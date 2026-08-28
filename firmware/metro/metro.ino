int AIN1  = 5;
int AIN2  = 6;
int PWMA = 9;
int BIN1 = 10;
int BIN2 = 11;
int PWMB = 12;
int STBY = 4;

#define CMD_FORWARD  0xFF
#define CMD_BACKWARD 0x01
#define CMD_RIGHT    0xAA
#define CMD_LEFT     0x55
#define CMD_STOP     0x00

const unsigned long TIMEOUT_MS = 500;
unsigned long lastCommandTime = 0;

void setup() {
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);

    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(PWMB, OUTPUT);

    pinMode(STBY, OUTPUT);
    digitalWrite(STBY, HIGH); // enable motors

    stopMotors();
    
    Serial.begin(9600);
    Serial1.begin(9600);
    
    lastCommandTime = millis();
    
    Serial.println("Metro M0 ready.");
}

void loop() {
    if (Serial1.available() > 0) {
        byte command = Serial1.read();
        lastCommandTime = millis();
        
        switch (command) {
            case CMD_FORWARD:
                digitalWrite(AIN1, LOW);
                digitalWrite(AIN2, HIGH);
                analogWrite(PWMA, 255);

                digitalWrite(BIN1, HIGH);
                digitalWrite(BIN2, LOW);
                analogWrite(PWMB, 255);
                Serial.println("Forward");
                break;
                
            case CMD_BACKWARD:
                digitalWrite(AIN1, HIGH);
                digitalWrite(AIN2, LOW);
                analogWrite(PWMA, 255);

                digitalWrite(BIN1, LOW);
                digitalWrite(BIN2, HIGH);
                analogWrite(PWMB, 255);
                Serial.println("Backward");
                break;
                
            case CMD_RIGHT:
                digitalWrite(AIN1, HIGH);
                digitalWrite(AIN2, LOW);
                analogWrite(PWMA, 255);

                digitalWrite(BIN1, HIGH);
                digitalWrite(BIN2, LOW);
                analogWrite(PWMB, 255);

                Serial.println("Right");
                break;
                
            case CMD_LEFT:
                digitalWrite(AIN1, LOW);
                digitalWrite(AIN2, HIGH);
                analogWrite(PWMA, 255);

                digitalWrite(BIN1, LOW);
                digitalWrite(BIN2, HIGH);
                analogWrite(PWMB, 255);
                Serial.println("Left");
                break;
                
            case CMD_STOP:
            default:
                stopMotors();
                Serial.println("Stop");
                break;
        }
    }
    
    if (millis() - lastCommandTime > TIMEOUT_MS) {
        stopMotors();
    }
}

void stopMotors() {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, 0);

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, 0);
}   
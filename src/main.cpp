#include <Arduino.h>
#include <XboxSeriesXControllerESP32_asukiaaa.hpp>
#include <ESP32Servo.h>

XboxSeriesXControllerESP32_asukiaaa::Core xboxController;
Servo K969Servo;

const int servoPin = 18;
const int LED_BUILTIN_PIN = 2;
const float MAX_STEERING_ANGLE = 160.0;
const float MIN_STEERING_ANGLE = 20.0;
const float CENTER_STEERING_ANGLE = 90.0;
const float CENTER_TOLERANCE = 3.0;

int flag = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting NimBLE Client");
    xboxController.begin();
    pinMode(LED_BUILTIN_PIN, OUTPUT);
    K969Servo.attach(servoPin);
    K969Servo.write(CENTER_STEERING_ANGLE);
}

void demoVibration() {
    XboxSeriesXHIDReportBuilder_asukiaaa::ReportBase repo;
    repo.v.select.center = true;
    repo.v.select.left = false;
    repo.v.select.right = false;
    repo.v.select.shake = false;
    repo.v.power.center = 80;  // 80% power
    repo.v.timeActive = 35;    // 0.35 second
    Serial.println("\nController Connected Successfully");
    xboxController.writeHIDReport(repo);
}

float calculateSteeringAngle(float joyLHoriValue) {
    float steeringAngle = (joyLHoriValue - 65535.0) / 65535.0 * (-180);
    
    if (steeringAngle <= MIN_STEERING_ANGLE) {
        return MIN_STEERING_ANGLE;
    } else if (steeringAngle >= MAX_STEERING_ANGLE) {
        return MAX_STEERING_ANGLE;
    } else if (abs(steeringAngle - CENTER_STEERING_ANGLE) <= CENTER_TOLERANCE) {
        return CENTER_STEERING_ANGLE;
    } else {
        return steeringAngle;
    }
}

void loop() {
    xboxController.onLoop();
    if (xboxController.isConnected()) {
        if (xboxController.isWaitingForFirstNotification()) {
            Serial.println("waiting for first notification");
        } else {
            if (flag == 0) {
                demoVibration();
                digitalWrite(LED_BUILTIN, HIGH);
                Serial.println("Address: " + xboxController.buildDeviceAddressStr());
                Serial.println("battery " + String(xboxController.battery) + "%");
                flag += 1;
            }
            
            float joyLHoriValue = (float)xboxController.xboxNotif.joyLHori;
            float steeringAngle = calculateSteeringAngle(joyLHoriValue);
            K969Servo.write(steeringAngle);
            
            Serial.print("trigRT value: ");
            Serial.print((float)xboxController.xboxNotif.trigRT);
            Serial.print("\ttrigLT value: ");
            Serial.print((float)xboxController.xboxNotif.trigLT);
            Serial.print("\tjoyLHori rate: ");
            Serial.print(joyLHoriValue);
            Serial.print("\t\tsteering angle: ");
            Serial.println(steeringAngle);
        }
    } else {
        if (xboxController.getCountFailedConnection() > 2) {
            ESP.restart();
        }
        if (flag >= 1){
            digitalWrite(LED_BUILTIN, LOW);
            Serial.println("not connected");
            flag = 0;
        }
    }
} 
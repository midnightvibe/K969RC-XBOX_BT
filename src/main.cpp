#include <Arduino.h>
#include <XboxSeriesXControllerESP32_asukiaaa.hpp>
#include <ESP32Servo.h>

// Required to replace with your xbox address
// XboxSeriesXControllerESP32_asukiaaa::Core
// xboxController("44:16:22:5e:b2:d4");

// any xbox controller
XboxSeriesXControllerESP32_asukiaaa::Core xboxController;
Servo K969Servo;
int servoPin = 18;
int flag = 0;
float steeringAngle = 0.00;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting NimBLE Client");
    xboxController.begin();
    K969Servo.attach(servoPin);
    K969Servo.write(90);
}

void demoVibration() {
    XboxSeriesXHIDReportBuilder_asukiaaa::ReportBase repo;

    repo.v.select.center = true;
    repo.v.select.left = false;
    repo.v.select.right = false;
    repo.v.select.shake = false;
    repo.v.power.center = 80;  // 60% power
    repo.v.timeActive = 35;    // 0.35 second
    Serial.println("\nController Connected Succesfully");
    xboxController.writeHIDReport(repo);
}

void loop() {
    xboxController.onLoop();
    if (xboxController.isConnected()) {
        if (xboxController.isWaitingForFirstNotification()) {
        Serial.println("waiting for first notification");
        } else {
            if (flag == 0) {
                demoVibration();
                Serial.println("Address: " + xboxController.buildDeviceAddressStr());
                Serial.println("battery " + String(xboxController.battery) + "%");
                flag +=1;
            }
            //uint16_t joystickMax = XboxControllerNotificationParser::maxJoy;
            Serial.print("trigRT value: ");
            Serial.print((float)xboxController.xboxNotif.trigRT);
            Serial.print("\ttrigLT value: ");
            Serial.print((float)xboxController.xboxNotif.trigLT);
            Serial.print("\tjoyLHori rate: ");
            Serial.print((float)xboxController.xboxNotif.joyLHori);

            // calculate and output steering angle from joyLHori Input
            steeringAngle = (((float)(xboxController.xboxNotif.joyLHori)-(float)(65535))/((float)(65535))*(-180)); // -35° da sonst 
            if (steeringAngle <= 20) {
                steeringAngle = 20;
                K969Servo.write(steeringAngle);
                Serial.print("\t    steering angle: ");
                Serial.println((float)steeringAngle);
            } else if (steeringAngle >= 160) {
                steeringAngle = 160;
                K969Servo.write(steeringAngle);
                Serial.print("\t    steering angle: ");
                Serial.println((float)steeringAngle);
            } else if (steeringAngle >= 87 && steeringAngle <= 93 ) {
                steeringAngle = 90;
                K969Servo.write(steeringAngle);
                Serial.print("\t    steering angle: ");
                Serial.println((float)steeringAngle);
            } else {
                K969Servo.write(steeringAngle);
                Serial.print("\t    steering angle: ");
                Serial.println((float)steeringAngle);
            }
        }
    } else {
        Serial.println("not connected");
        if (xboxController.getCountFailedConnection() > 2) {
        ESP.restart();
        }
    }
}
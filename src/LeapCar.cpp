
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include <ServoEasing.h>
#include <StreamUtils.h>

ServoEasing clamp_servo, arm_top_servo, arm_middle_servo, arm_bottom_servo;  // create servo object to control a servo
const int SERVOS_COUNT = 4;                                                  //舵机数4个
int value[SERVOS_COUNT], idle[SERVOS_COUNT], current_angle[SERVOS_COUNT], MIN[SERVOS_COUNT], MAX[SERVOS_COUNT], INITANGLE[SERVOS_COUNT], previous_angle[SERVOS_COUNT], ANA[SERVOS_COUNT];

#define SERVO_UPDATE_DELAY 20
#define MOTOR_VOLTAGE_PIN_CNT 4
#define MOTOR_VOLTAGE_DIGIT 3

int servo_rotation_delta = 5;         // 舵机转动幅度
int servo_bottom_rotation_delta = 2;  // 底座舵机转动幅度
char command;                         // read the char
DynamicJsonDocument message(512);     // json message
// ReadBufferingStream buffer(Serial, 64);
char str[256];
int voltage[MOTOR_VOLTAGE_PIN_CNT] = {0, 0, 0, 0};
int prev_volt[MOTOR_VOLTAGE_PIN_CNT] = {0, 0, 0, 0};
int voltage_pins[] = {8, 9, 11, 10};

const char* READY_FOR_MSG = "OK";

//---------------------------------手爪函数定义---------------------------------------
void clamp_open()  //手爪打开
{
    clamp_servo.write(MAX[0]);
    delay(300);
}

void clamp_close()
{
    clamp_servo.write(MIN[0]);  //手爪闭合
    delay(300);
}
void bottom_rotate_left()  // 底座左转
{
    if (current_angle[3] + servo_bottom_rotation_delta < MAX[3]) {
        current_angle[3] += servo_bottom_rotation_delta;
    }
    arm_bottom_servo.write(current_angle[3]);
}

void bottom_rotate_right()  // 底座右转
{
    if (current_angle[3] - servo_bottom_rotation_delta > MIN[3])
        current_angle[3] -= servo_bottom_rotation_delta;
    arm_bottom_servo.write(current_angle[3]);
}

void arm_top_up()  //上臂舵机向上
{
    if (current_angle[1] + servo_rotation_delta < MAX[1])
        current_angle[1] += servo_rotation_delta;
    arm_top_servo.write(current_angle[1]);
}

void arm_top_down()  //上臂舵机向下
{
    if (current_angle[1] - servo_rotation_delta > MIN[1])
        current_angle[1] -= servo_rotation_delta;
    arm_top_servo.write(current_angle[1]);
}

void arm_middle_up()  //下臂舵机上升
{
    if (current_angle[2] - servo_rotation_delta > MIN[2])
        current_angle[2] -= servo_rotation_delta;
    arm_middle_servo.write(current_angle[2]);
}

void arm_middle_down()  //下臂舵机下降
{
    if (current_angle[2] + servo_rotation_delta < MAX[2])
        current_angle[2] += servo_rotation_delta;
    arm_middle_servo.write(current_angle[2]);
}
void arm_stop_all()  //停止所有舵机
{
    clamp_servo.write(current_angle[0]);
    arm_top_servo.write(current_angle[1]);
    arm_middle_servo.write(current_angle[2]);
    arm_bottom_servo.write(current_angle[3]);
}
//---------------------------------运动函数定义---------------------------------------
void motor_forve_vector(const int* vector)
{
    // Serial.print("Analog writing: ");
    // for (int i = 0; i < MOTOR_VOLTAGE_PIN_CNT; i++) {
    //     Serial.print(vector[i]);
    //     Serial.print(", ");
    // }
    // Serial.print("\n");

    for (int i = 0; i < MOTOR_VOLTAGE_PIN_CNT; i++) {
        int pin = voltage_pins[i];
        // int prev_v = prev_volt[i];
        int v = voltage[i];
        // if (prev_v != v) {
        //     analogWrite(pin, v);
        //     prev_volt[i] = v;
        // }

        analogWrite(pin, v);
    }
    // analogWrite(8, vector[0]);
    // digitalWrite(8, HIGH);
    // analogWrite(9, vector[1]);

    // analogWrite(11, vector[2]);
    // digitalWrite(11, HIGH);
    // analogWrite(10, vector[3]);
    // for (int i = 0; i < 255; i++) {
    //     analogWrite(9, i);
    //     analogWrite(10, i);
    //     delay(20);
    // }
    // for (int i = 255; i >= 0; i--) {
    //     analogWrite(9, i);
    //     analogWrite(10, i);
    //     delay(20);
    // }
}

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);  //The monitor UART
    while (!Serial)
        continue;
    Serial.println("Serial setup");
    Serial.println("Setup begin...");

    //-----电机IO口定-
    for (auto pin : voltage_pins) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    Serial.println("Motor done...");

    //机械手爪定义端口
    clamp_servo.attach(2);        //手爪电机
    arm_top_servo.attach(7);      //上臂电机
    arm_middle_servo.attach(12);  //下臂电机
    arm_bottom_servo.attach(13);  //底座电机

    Serial.println("Clamp done...");

    //手爪 Servo
    MIN[0] = 10;
    MAX[0] = 50;
    INITANGLE[0] = 30;

    //上臂电机
    MIN[1] = 10;  // This should bring the lever to just below 90deg to ground
    MAX[1] = 140;
    INITANGLE[1] = 90;  // This should bring the lever parallel with the ground

    //下臂电机
    MIN[2] = 40;
    MAX[2] = 170;
    INITANGLE[2] = 90;

    //底座电机
    MIN[3] = 0;
    MAX[3] = 170;
    INITANGLE[3] = 90;

    //初始化电机
    clamp_servo.write(INITANGLE[0]);
    arm_top_servo.write(INITANGLE[1]);
    arm_middle_servo.write(INITANGLE[2]);
    arm_bottom_servo.write(INITANGLE[3]);

    Serial.println("Clamp init done...");

    current_angle[0] = INITANGLE[0];
    current_angle[1] = INITANGLE[1];
    current_angle[2] = INITANGLE[2];
    current_angle[3] = INITANGLE[3];

    Serial.println(READY_FOR_MSG);
}

void loop()
{
    // put your main code here, to run repeatedly:
    // Serial.println("Loop begin...");

    unsigned long start = micros();
    if (Serial.available() > 0) {
        // String str = Serial.readStringUntil('\n');

        Serial.readBytes(str, MOTOR_VOLTAGE_PIN_CNT);

        // Serial.print("Serial Echo: ");
        // Serial.print(str);
        // Serial.print("\n");
        // Serial.println("Deserialization began...");

        // ReadLoggingStream loggingStream(Serial, Serial);
        // DeSerializationError error = deSerializeJson(message, Serial);
        // DeserializationError error = deserializeJson(message, str);
        // DeserializationError error;

        // deserializeJson(message, str);

        // Serial.println("Deserialization ended...");
        // Test if parsing succeeds.

        // unsigned long de_end = micros();

        // Serial.print("Deserialization took: ");
        // Serial.print((de_end - start) / 1000000.0, 6);
        // Serial.print("\n");

        // if (error) {
        //     // Serial.print("deserializeJson() failed: ");

        //     // Serial.println(error.f_str());
        //     // Serial.println(READY_FOR_MSG);

        //     for (int i = 0; i < MOTOR_VOLTAGE_PIN_CNT; i++) {
        //         voltage[i] = 0;
        //     }
        //     motor_forve_vector(voltage);
        //     return;
        // }
        // for (int i = 0; i < MOTOR_VOLTAGE_PIN_CNT; i++) {
        //     voltage[i] = int((message["voltage"][i]));
        // }
        // //! assuming a line structure as: 000255000255 for voltage
        // //! removing trailing linebreak
        // Serial.println("Length confirmed");
        for (int i = 0; i < MOTOR_VOLTAGE_PIN_CNT; i++) {
            voltage[i] = str[i];
        }
        //  else {
        //     Serial.print("Length Error: ");
        //     Serial.print(str.length());
        //     Serial.print("\n");
        // }
        motor_forve_vector(voltage);

        // unsigned long mo_end = micros();

        // Serial.print("Motor took: ");
        // Serial.print((mo_end - start) / 1000000.0, 6);
        // Serial.print("\n");
    } else {
        Serial.println(READY_FOR_MSG);
    }

    // if (Serial.available()) {
    //     // Serial.println("Serial busy...");
    //     // while (Serial.available() > 0)
    //     //     Serial.read();
    // } else {
    //     Serial.println(READY_FOR_MSG);

    // }
    // Serial.println(READY_FOR_MSG);
    unsigned long end = micros();
    double time = (end - start) / 1000000.0;
    // Serial.print("Loop time: ");
    // Serial.print(time, 6);
    // Serial.print("\n");
    Serial.print("FPS:");
    Serial.print(1 / time, 4);
    Serial.print("\n");

    return;

    // TODO: Map this too...
    // switch (command) {
    // case '0':
    //     Serial.println("Servo all stop");
    //     delay(SERVO_UPDATE_DELAY);
    //     break;
    // case '1':
    //     Serial.println("MeArm turn Left");
    //     bottom_rotate_left();
    //     delay(SERVO_UPDATE_DELAY);
    //     break;
    // case '2':
    //     Serial.println("MeArm turn Right");
    //     bottom_rotate_right();
    //     delay(SERVO_UPDATE_DELAY);
    //     break;
    // case '3':
    //     Serial.println("Arm A Up");
    //     arm_top_up();
    //     delay(SERVO_UPDATE_DELAY);
    //     break;
    // case '4':
    //     Serial.println("Arm A Down");
    //     arm_top_down();
    //     delay(SERVO_UPDATE_DELAY);
    //     break;
    // case '5':
    //     Serial.println("Arm B Up");
    //     arm_middle_up();
    //     delay(SERVO_UPDATE_DELAY);
    //     break;
    // case '6':
    //     Serial.println("Arm B Down");
    //     arm_middle_down();
    //     delay(SERVO_UPDATE_DELAY);
    //     break;
    // case '7':
    //     Serial.println("Clamp Open");
    //     clamp_open();  //打开手爪
    //     break;
    // case '8':
    //     Serial.println("Clamp Close");
    //     clamp_close();  // 闭合手爪
    //     break;
    // }
}
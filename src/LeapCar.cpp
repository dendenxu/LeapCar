#include <Arduino.h>
#include <ArduinoJson.h>
// #include <Servo.h>
#include <ServoTimer2.h>
#include <StreamUtils.h>

ServoTimer2 clamp_servo, arm_top_servo, arm_middle_servo, arm_bottom_servo;  // create servo object to control a servo
const int SERVOS_COUNT = 4;                                            //舵机数4个
int value[SERVOS_COUNT], idle[SERVOS_COUNT], current_angle[SERVOS_COUNT], MIN[SERVOS_COUNT], MAX[SERVOS_COUNT], INITANGLE[SERVOS_COUNT], previous_angle[SERVOS_COUNT], ANA[SERVOS_COUNT];

#define SERVO_UPDATE_DELAY 50
#define MOTOR_VOLTAGE_PIN_CNT 4

int servo_rotation_delta = 5;         // 舵机转动幅度
int servo_bottom_rotation_delta = 2;  // 底座舵机转动幅度
char command;                         // read the char
DynamicJsonDocument message(1024);    // json message
double voltage[MOTOR_VOLTAGE_PIN_CNT];

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
void motor_forve_vector(const double *vector)
{
    Serial.print("Analog writing: ");
    for (int i = 0; i < MOTOR_VOLTAGE_PIN_CNT; i++) {
        Serial.print(vector[i], 2);
        Serial.print(", ");
    }
    Serial.print("\n");
    analogWrite(8, vector[0]);
    // digitalWrite(8, HIGH);
    analogWrite(9, vector[1]);

    analogWrite(11, vector[2]);
    // digitalWrite(11, HIGH);
    analogWrite(10, vector[3]);
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
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);

    // Analog
    // pinMode(14, OUTPUT);
    // pinMode(15, OUTPUT);
    // pinMode(16, OUTPUT);
    // pinMode(17, OUTPUT);
    // pinMode(18, OUTPUT);
    // pinMode(19, OUTPUT);

    // analogWrite(14, 32);
    // analogWrite(15, 32);
    // analogWrite(16, 32);
    // analogWrite(17, 32);
    // analogWrite(18, 32);
    // analogWrite(19, 32);
    pinMode(17, OUTPUT);

    //机械手爪定义端口
    clamp_servo.attach(2);        //手爪电机
    arm_top_servo.attach(7);      //上臂电机
    arm_middle_servo.attach(12);  //下臂电机
    arm_bottom_servo.attach(13);  //底座电机

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

    current_angle[0] = INITANGLE[0];
    current_angle[1] = INITANGLE[1];
    current_angle[2] = INITANGLE[2];
    current_angle[3] = INITANGLE[3];

    Serial.println("Setup finished...");
}

void loop()
{
    for (int i = 0; i < 255; i++) {
        analogWrite(17, i);
        delay(20);
    }
    for (int i = 255; i >= 0; i--) {
        analogWrite(17, i);
        delay(20);
    }
    // int val9 = analogRead(9);
    // int val10 = analogRead(10);
    // int val14 = analogRead(14);
    // int val15 = analogRead(15);

    // Serial.print(val9);
    // Serial.print(", ");
    // Serial.print(val10);
    // Serial.print(", ");
    // Serial.print(val14);
    // Serial.print(", ");
    // Serial.print(val15);
    // Serial.print("\n");
    // put your main code here, to run repeatedly:
    // Serial.println("Loop begin...");
    // ReadLoggingStream loggingStream(Serial, Serial);
    DeserializationError error = deserializeJson(message, Serial);
    // Test if parsing succeeds.
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.f_str());
        return;
    }
    for (int i = 0; i < MOTOR_VOLTAGE_PIN_CNT; i++) {
        voltage[i] = message["voltage"][i];
    }
    motor_forve_vector(voltage);

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
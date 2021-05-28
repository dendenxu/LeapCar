
#include <Arduino.h>
#include <Servo.h>
#include <ServoEasing.h>

ServoEasing clamp_servo, arm_top_servo, arm_middle_servo, arm_bottom_servo;  // create servo object to control a servo
const int SERVOS_COUNT = 4;                                                  //舵机数4个
int value[SERVOS_COUNT], idle[SERVOS_COUNT], current_angle[SERVOS_COUNT], MIN[SERVOS_COUNT], MAX[SERVOS_COUNT], INITANGLE[SERVOS_COUNT], previous_angle[SERVOS_COUNT], ANA[SERVOS_COUNT];

#define SERVO_UPDATE_DELAY 20
#define SERVO_PIN_CNT 4
#define MOTOR_VOLTAGE_PIN_CNT 4
#define MOTOR_VOLTAGE_DIGIT 3

int servo_rotation_delta = 5;         // 舵机转动幅度
int servo_bottom_rotation_delta = 2;  // 底座舵机转动幅度
unsigned char str[256];
// int buffer[MOTOR_VOLTAGE_PIN_CNT] = {0, 0, 0, 0};
// int angle[SERVO_PIN_CNT] = {0, 0};
int voltage_pins[] = {8, 9, 11, 10};

const char* READY_FOR_MSG = "OK";
//---------------------------------运动函数定义---------------------------------------
void motor_forve_vector(const unsigned char* vector)
{
    for (int i = 0; i < MOTOR_VOLTAGE_PIN_CNT; i++) {
        int pin = voltage_pins[i];
        int v = vector[i];
        analogWrite(pin, v);
    }
}

void servo_angle_vector(const unsigned char* angle)
{
    /**********
    if (angle[0] == 63)
        bottom_rotate_right();
    if (angle[0] == 127)
        bottom_rotate_left();
    *******/
    arm_bottom_servo.startEaseTo(angle[0]);
    arm_middle_servo.startEaseTo(angle[1]);
    arm_top_servo.startEaseTo(angle[2]);
    clamp_servo.startEaseTo(angle[3]);
}

void setup()
{
    // put your setup code here, to run once:
    Serial2.begin(9600);  //The monitor UART
    while (!Serial2)
        continue;
    Serial2.println("Serial2 setup");
    Serial2.println("Setup begin...");

    //-----电机IO口定-
    for (auto pin : voltage_pins) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    Serial2.println("Motor done...");

    //机械手爪定义端口
    clamp_servo.attach(2);        //手爪电机
    arm_top_servo.attach(7);      //上臂电机
    arm_middle_servo.attach(12);  //下臂电机
    arm_bottom_servo.attach(13);  //底座电机

    Serial2.println("Clamp done...");

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
    clamp_servo.setSpeed(360);
    clamp_servo.write(INITANGLE[0]);
    clamp_servo.setEasingType(EASE_CUBIC_IN_OUT);
    arm_top_servo.setSpeed(360);
    arm_top_servo.write(INITANGLE[1]);
    arm_top_servo.setEasingType(EASE_CUBIC_IN_OUT);
    arm_middle_servo.setSpeed(360);
    arm_middle_servo.write(INITANGLE[2]);
    arm_middle_servo.setEasingType(EASE_CUBIC_IN_OUT);
    arm_bottom_servo.setSpeed(360);
    arm_bottom_servo.write(INITANGLE[3]);
    arm_bottom_servo.setEasingType(EASE_CUBIC_IN_OUT);

    Serial2.println("Clamp init done...");

    current_angle[0] = INITANGLE[0];
    current_angle[1] = INITANGLE[1];
    current_angle[2] = INITANGLE[2];
    current_angle[3] = INITANGLE[3];

    Serial2.println(READY_FOR_MSG);
}

void loop()
{
    // put your main code here, to run repeatedly:
    // Serial2.println("Loop begin...");

    unsigned long start = micros();
    if (Serial2.available() > 0) {
        Serial2.readBytes(str, MOTOR_VOLTAGE_PIN_CNT + SERVO_PIN_CNT);
        motor_forve_vector(str);
        servo_angle_vector(str + MOTOR_VOLTAGE_PIN_CNT);
    } else {
        Serial2.println(READY_FOR_MSG);
    }

    unsigned long end = micros();
    double time = (end - start) / 1000000.0;
    Serial2.print("FPS:");
    Serial2.print(1 / time, 4);
    Serial2.print("\n");

    return;
}

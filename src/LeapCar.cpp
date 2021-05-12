#include <Arduino.h>
#include <Servo.h>

Servo myservo1, myservo2, myservo3, myservo4;  // create servo object to control a servo
const int SERVOS = 4;                          //舵机数4个
int value[SERVOS], idle[SERVOS], currentAngle[SERVOS], MIN[SERVOS], MAX[SERVOS], INITANGLE[SERVOS], previousAngle[SERVOS], ANA[SERVOS];

char command;  //read the char
#define ServoDelayTime 50

int delta = 5;         //舵机转动幅度
int delta_bottom = 2;  //底座舵机转动幅度

//---------------------------------手爪函数定义---------------------------------------
void ClampOpen()  //手爪打开
{
    myservo1.write(MAX[0]);
    delay(300);
}

void ClampClose()
{
    myservo1.write(MIN[0]);  //手爪闭合
    delay(300);
}
void BottomLeft()  // 底座左转
{
    if (currentAngle[3] + delta_bottom < MAX[3]) {
        currentAngle[3] += delta_bottom;
    }
    myservo4.write(currentAngle[3]);
}

void BottomRight()  // 底座右转
{
    if (currentAngle[3] - delta_bottom > MIN[3])
        currentAngle[3] -= delta_bottom;
    myservo4.write(currentAngle[3]);
}

void Arm_A_Up()  //上臂舵机向上
{
    if (currentAngle[1] + delta < MAX[1])
        currentAngle[1] += delta;
    myservo2.write(currentAngle[1]);
}

void Arm_A_Down()  //上臂舵机向下
{
    if (currentAngle[1] - delta > MIN[1])
        currentAngle[1] -= delta;
    myservo2.write(currentAngle[1]);
}

void Arm_B_Up()  //下臂舵机上升
{
    if (currentAngle[2] - delta > MIN[2])
        currentAngle[2] -= delta;
    myservo3.write(currentAngle[2]);
}

void Arm_B_Down()  //下臂舵机下降
{
    if (currentAngle[2] + delta < MAX[2])
        currentAngle[2] += delta;
    myservo3.write(currentAngle[2]);
}
void Servo_stop()  //停止所有舵机
{
    myservo1.write(currentAngle[0]);
    myservo2.write(currentAngle[1]);
    myservo3.write(currentAngle[2]);
    myservo4.write(currentAngle[3]);
}
//---------------------------------运动函数定义---------------------------------------
void forward()
{
    digitalWrite(8, LOW);
    digitalWrite(9, HIGH);

    digitalWrite(11, LOW);
    digitalWrite(10, HIGH);
}

void right()
{
    digitalWrite(8, LOW);
    digitalWrite(9, HIGH);

    digitalWrite(11, LOW);
    digitalWrite(10, LOW);
}

void back()
{
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);

    digitalWrite(11, HIGH);
    digitalWrite(10, HIGH);
}

void left()
{
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);

    digitalWrite(11, LOW);
    digitalWrite(10, HIGH);
}

void stop()
{
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);

    digitalWrite(11, LOW);
    digitalWrite(10, LOW);
}

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);  //The monitor UART
    //-----电机IO口定-
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);

    //机械手爪定义端口
    myservo1.attach(2);   //手爪电机
    myservo2.attach(7);   //上臂电机
    myservo3.attach(12);  //下臂电机
    myservo4.attach(13);  //底座电机

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
    myservo1.write(INITANGLE[0]);
    myservo2.write(INITANGLE[1]);
    myservo3.write(INITANGLE[2]);
    myservo4.write(INITANGLE[3]);

    currentAngle[0] = INITANGLE[0];
    currentAngle[1] = INITANGLE[1];
    currentAngle[2] = INITANGLE[2];
    currentAngle[3] = INITANGLE[3];
}

void loop()
{
    // put your main code here, to run repeatedly:
    if (Serial.available() > 0) {
        command = Serial.read();
        Serial.println(command);
    }
    switch (command) {
    case 'F':
        forward();  //机器人前进
        break;
    case 'B':
        back();  //机器人后退
        break;
    case 'R':
        right();  //机器人右转
        break;
    case 'L':
        left();  //机器人左转
        break;
    case 'S':
        stop();  //机器人停止
        Serial.println("S");
        break;
    case '0':
        Serial.println("Servo all stop");
        delay(ServoDelayTime);
        break;
    case '1':
        Serial.println("MeArm turn Left");
        BottomLeft();
        delay(ServoDelayTime);
        break;
    case '2':
        Serial.println("MeArm turn Right");
        BottomRight();
        delay(ServoDelayTime);
        break;
    case '3':
        Serial.println("Arm A Up");
        Arm_A_Up();
        delay(ServoDelayTime);
        break;
    case '4':
        Serial.println("Arm A Down");
        Arm_A_Down();
        delay(ServoDelayTime);
        break;
    case '5':
        Serial.println("Arm B Up");
        Arm_B_Up();
        delay(ServoDelayTime);
        break;
    case '6':
        Serial.println("Arm B Down");
        Arm_B_Down();
        delay(ServoDelayTime);
        break;
    case '7':
        Serial.println("Clamp Open");
        ClampOpen();  //打开手爪
        break;
    case '8':
        Serial.println("Clamp Close");
        ClampClose();  // 闭合手爪
        break;
    }
}
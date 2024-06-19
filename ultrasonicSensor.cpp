#include <iostream>
#include <string>
#include <utility>
#include "ev3dev.h"


using namespace std;
using namespace ev3dev;

    motor sensorMotor = motor(OUTPUT_D, motor::motor_medium);
    ultrasonic_sensor usensor = ultrasonic_sensor(INPUT_4);

    motor rightMotor = motor(OUTPUT_B,motor::motor_large);
    motor leftMotor = motor(OUTPUT_A, motor::motor_large);
    int sensorpos = sensorMotor.set_position(0).position();
    int prev_angle = 180;

pair<int,int> ultrasonic(){

    int distance_cur = usensor.distance_centimeters();
    int target_angle = 0;
    int target_distance = distance_cur;
    sensorMotor.set_position_sp(sensorpos+=10);
    sensorMotor.set_speed_sp(500);
    sensorMotor.run_to_abs_pos();

    
    while (sensorpos < 180)
    {
        
        distance_cur = usensor.distance_centimeters();
        cout << "distance_cur = " << distance_cur << endl;
        sensorpos = sensorMotor.position();
        cout << "sensor angle = " << sensorpos << endl;
        if(distance_cur < target_distance){
            target_angle = sensorpos;
            target_distance = distance_cur;
        }
        sensorMotor.set_position_sp(sensorpos+=10);
        sensorMotor.run_to_abs_pos();
    }

    //sensorMotor.set_position_sp(target_angle);
    //sensorMotor.run_to_abs_pos();
    cout << "Target angle = " << target_angle << endl;
    cout << "Target Distance = " << target_distance << endl;
    /* sensorMotor.set_position_sp(0);
    //sensorMotor.set_speed_sp(-100);
    sensorMotor.run_to_abs_pos();
    cout << "zurück zur Anfangsposition " << sensorMotor.position()<< endl; */
    while(sensorMotor.position() > 0){
        sensorMotor.set_position_sp(0);
        //sensorMotor.set_speed_sp(-100);
        sensorMotor.run_to_abs_pos();
        cout << "zurück zur Anfangsposition " << sensorMotor.position()<< endl;
    }
    sensorpos = 0;
    
    

    return make_pair(target_angle,target_distance);
}


void motor_speed(int target){

    
    
    int rightpos = rightMotor.set_position(0).position();
    int leftpos = leftMotor.set_position(0).position();
    int turn = 90 - target;
    if(abs(target -  prev_angle) < 15){
        leftMotor.set_position_sp(90);
        leftMotor.set_speed_sp(120);
        leftMotor.run_to_abs_pos();
        rightMotor.set_position_sp(90);
        rightMotor.set_speed_sp(120);
        rightMotor.run_to_abs_pos();
        
    }else{
        if(turn < 0){
        leftMotor.set_position_sp(-4*turn);
        leftMotor.set_speed_sp(85);
        leftMotor.run_to_abs_pos();
        }else{
        rightMotor.set_position_sp(4*turn);
        rightMotor.set_speed_sp(85);
        rightMotor.run_to_abs_pos();
        }
     } 
    
    prev_angle = target;
    
    
    rightpos = rightMotor.position();
    cout << "right motor " << rightpos << endl;
    leftpos = leftMotor.position();
    cout << "left motor " << leftpos << endl;

}

int main(){

    
   
while (true)
{
    auto target = ultrasonic();
    if(target.second <=5 || target.second >=255){
        continue;
    }
    motor_speed(target.first);
}



    return 0;
}
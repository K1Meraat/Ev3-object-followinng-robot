#include <iostream>
#include <string>
#include <utility>
#include "ev3dev.h"

using namespace std;
using namespace ev3dev;

// Initialize the medium motor controlling the ultrasonic sensor's rotation
motor sensorMotor = motor(OUTPUT_D, motor::motor_medium);

// Initialize the ultrasonic sensor on port 4
ultrasonic_sensor usensor = ultrasonic_sensor(INPUT_4);

// Initialize the two large motors for robot movement
motor rightMotor = motor(OUTPUT_B, motor::motor_large);
motor leftMotor = motor(OUTPUT_A, motor::motor_large);

// Set initial position for the sensor motor and record it
int sensorpos = sensorMotor.set_position(0).position();

// Variable to remember the last angle the robot turned to
int prev_angle = 180;

/**
 * Function to rotate the ultrasonic sensor from 0 to 180 degrees,
 * scanning for the closest object. Returns the angle and distance
 * of the nearest object found.
 */
pair<int, int> ultrasonic() {
    // Get initial distance
    int distance_cur = usensor.distance_centimeters();
    int target_angle = 0;
    int target_distance = distance_cur;

    // Start scanning from 0 to 180 degrees in 10-degree steps
    sensorMotor.set_position_sp(sensorpos += 10);
    sensorMotor.set_speed_sp(500);
    sensorMotor.run_to_abs_pos();

    while (sensorpos < 180) {
        // Read current distance and update sensor position
        distance_cur = usensor.distance_centimeters();
        cout << "distance_cur = " << distance_cur << endl;
        sensorpos = sensorMotor.position();
        cout << "sensor angle = " << sensorpos << endl;

        // Track the smallest distance (closest object)
        if (distance_cur < target_distance) {
            target_angle = sensorpos;
            target_distance = distance_cur;
        }

        // Move sensor further by 10 degrees
        sensorMotor.set_position_sp(sensorpos += 10);
        sensorMotor.run_to_abs_pos();
    }

    // Print the angle and distance of the nearest object
    cout << "Target angle = " << target_angle << endl;
    cout << "Target Distance = " << target_distance << endl;

    // Return sensor to original position (0 degrees)
    while (sensorMotor.position() > 0) {
        sensorMotor.set_position_sp(0);
        sensorMotor.run_to_abs_pos();
        cout << "Back to initial position " << sensorMotor.position() << endl;
    }

    sensorpos = 0;

    return make_pair(target_angle, target_distance);
}

/**
 * Function to control the movement of the robot based on the angle
 * of the detected object. If the target is straight ahead or within
 * 15 degrees of the previous direction, it moves forward.
 * Otherwise, it turns toward the object.
 */
void motor_speed(int target) {
    // Reset and read current motor positions
    int rightpos = rightMotor.set_position(0).position();
    int leftpos = leftMotor.set_position(0).position();

    // Calculate how much the robot needs to turn
    int turn = 90 - target;

    // If the target is almost straight ahead, move forward
    if (abs(target - prev_angle) < 15) {
        leftMotor.set_position_sp(90);
        leftMotor.set_speed_sp(120);
        leftMotor.run_to_abs_pos();

        rightMotor.set_position_sp(90);
        rightMotor.set_speed_sp(120);
        rightMotor.run_to_abs_pos();
    } else {
        // If the target is to the left, turn left
        if (turn < 0) {
            leftMotor.set_position_sp(-4 * turn);
            leftMotor.set_speed_sp(85);
            leftMotor.run_to_abs_pos();
        } else {
            // If the target is to the right, turn right
            rightMotor.set_position_sp(4 * turn);
            rightMotor.set_speed_sp(85);
            rightMotor.run_to_abs_pos();
        }
    }

    // Update the previous angle
    prev_angle = target;

    // Debug output of motor positions
    rightpos = rightMotor.position();
    cout << "right motor " << rightpos << endl;
    leftpos = leftMotor.position();
    cout << "left motor " << leftpos << endl;
}

/**
 * Main control loop: repeatedly scan for the closest object,
 * and move the robot toward it unless the distance is too close or invalid.
 */
int main() {
    while (true) {
        auto target = ultrasonic();

        // If the detected object is too close (<5 cm) or too far (>255 cm), skip
        if (target.second <= 5 || target.second >= 255) {
            continue;
        }

        // Move robot toward the target
        motor_speed(target.first);
    }

    return 0;
}

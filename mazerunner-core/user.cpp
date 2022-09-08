/*
 * File: user.cpp
 * Project: mazerunner
 * File Created: Wednesday, 24th March 2021 2:10:17 pm
 * Author: Peter Harrison
 * -----
 * Last Modified: Thursday, 6th May 2021 12:00:14 pm
 * Modified By: Peter Harrison
 * -----
 * MIT License
 *
 * Copyright (c) 2021 Peter Harrison
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "user.h"
#include "config.h"
#include "mouse.h"
#include "reports.h"
#include "src/encoders.h"
#include "src/maze.h"
#include "src/motion.h"
#include "src/motors.h"
#include "src/profile.h"
#include "src/sensors.h"
#include <Arduino.h>

// to avoid conflicts with other code, you might want to name all the functions
// in this file starting with user_
// and yes, I know there are more canonical ways to do that :)

// for example:
void user_follow_wall() {
  // This is just an example and not expected to do anything
}

void user_log_front_sensor() {
  sensors.enable_sensors();
  motion.reset_drive_system();
  motors.enable_motor_controllers();
  report_front_sensor_track_header();
  forward.start(-200, 100, 0, 500);
  while (not forward.is_finished()) {
    report_front_sensor_track();
  }
  motion.reset_drive_system();
  sensors.disable_sensors();
}

void run_mouse(int function) {
  // NOTE: will start on button click
  switch (function) {
    case 0:
      Serial.println(F("OK"));
      break;
    case 1:
      report_sensor_calibration();
      break;
    case 2:
      mouse.search_maze();
      break;
    case 3:
      mouse.follow_to(maze.maze_goal());
      break;
    case 4:
      test_SS90E();
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    case 8:
      // test_edge_detection();
      break;
    case 9:
      // test_sensor_spin_calibrate();
      break;
    case 10:
      break;
    case 11:
      break;
    case 12:
      break;
    case 13:
      break;
    case 14:
      break;
    case 15:

      break;
    default:
      sensors.disable_sensors();
      motion.reset_drive_system();
      break;
  }
};

/**
 * By turning in place through 360 degrees, it should be possible to get a
 * sensor calibration for all sensors?
 *
 * At the least, it will tell you about the range of values reported and help
 * with alignment, You should be able to see clear maxima 180 degrees apart as
 * well as the left and right values crossing when the robot is parallel to
 * walls either side.
 *
 * Use either the normal report_sensor_track() for the normalised readings
 * or report_sensor_track_raw() for the readings straight off the sensor.
 *
 * Sensor sensitivity should be set so that the peaks from raw readings do
 * not exceed about 700-800 so that there is enough headroom to cope with
 * high ambient light levels.
 *
 * @brief turn in place while streaming sensors
 */

void test_sensor_spin_calibrate() {
  sensors.enable_sensors();
  delay(100);
  motion.reset_drive_system();
  motors.enable_motor_controllers();
  sensors.set_steering_mode(STEERING_OFF);
  report_sensor_track_header();
  rotation.start(360, 180, 0, 1800);
  while (not rotation.is_finished()) {
    report_sensor_track(true);
  }
  motion.reset_drive_system();
  sensors.disable_sensors();
  delay(100);
}

//***************************************************************************//
/**
 * Edge detection test displays the position at which an edge is found when
 * the robot is travelling down a straight.
 *
 * Start with the robot backed up to a wall.
 * Runs forward for 150mm and records the robot position when the trailing
 * edge of the adjacent wall(s) is found.
 *
 * The value is only recorded to the nearest millimeter to avoid any
 * suggestion of better accuracy than that being available.
 *
 * Note that UKMARSBOT, with its back to a wall, has its wheels 43mm from
 * the cell boundary.
 *
 * This value can be used to permit forward error correction of the robot
 * position while exploring.
 *
 * @brief find sensor wall edge detection positions
 */

void test_edge_detection() {
  bool left_edge_found = false;
  bool right_edge_found = false;
  int left_edge_position = 0;
  int right_edge_position = 0;
  int left_max = 0;
  int right_max = 0;
  sensors.enable_sensors();
  delay(100);
  motion.reset_drive_system();
  motors.enable_motor_controllers();
  sensors.set_steering_mode(STEERING_OFF);
  Serial.println(F("Edge positions:"));
  forward.start(FULL_CELL - 30.0, 100, 0, 1000);
  while (not forward.is_finished()) {
    if (sensors.lss.value > left_max) {
      left_max = sensors.lss.value;
    }

    if (sensors.rss.value > right_max) {
      right_max = sensors.rss.value;
    }

    if (not left_edge_found) {
      if (sensors.lss.value < left_max / 2) {
        left_edge_position = int(0.5 + forward.position());
        left_edge_found = true;
      }
    }
    if (not right_edge_found) {
      if (sensors.rss.value < right_max / 2) {
        right_edge_position = int(0.5 + forward.position());
        right_edge_found = true;
      }
    }
    delay(5);
  }
  Serial.print(F("Left: "));
  if (left_edge_found) {
    Serial.print(BACK_WALL_TO_CENTER + left_edge_position);
  } else {
    Serial.print('-');
  }

  Serial.print(F("  Right: "));
  if (right_edge_found) {
    Serial.print(BACK_WALL_TO_CENTER + right_edge_position);
  } else {
    Serial.print('-');
  }
  Serial.println();

  motion.reset_drive_system();
  sensors.disable_sensors();
  delay(100);
}

void test_SS90E() {
  // note that changes to the speeds are likely to affect
  // the other turn parameters
  uint8_t side = sensors.wait_for_user_start();
  motion.reset_drive_system();
  motors.enable_motor_controllers();
  sensors.set_steering_mode(STEERING_OFF);
  // move to the boundary with the next cell
  float distance = BACK_WALL_TO_CENTER + HALF_CELL;
  forward.start(distance, DEFAULT_TURN_SPEED, DEFAULT_TURN_SPEED, SEARCH_ACCELERATION);
  while (not forward.is_finished()) {
    delay(2);
  }
  forward.set_position(FULL_CELL);

  if (side == RIGHT_START) {
    mouse.turn_smooth(SS90ER);
  } else {
    mouse.turn_smooth(SS90EL);
  }
  // after the turn, estimate the angle error by looking for
  // changes in the side sensor readings
  int sensor_left = sensors.lss.value;
  int sensor_right = sensors.rss.value;
  // move two cells. The resting position of the mouse have the
  // same offset as the turn ending
  forward.start(2 * FULL_CELL, DEFAULT_TURN_SPEED, 0, SEARCH_ACCELERATION);
  while (not forward.is_finished()) {
    delay(2);
  }
  sensor_left -= sensors.lss.value;
  sensor_right -= sensors.rss.value;
  print_justified(sensor_left, 5);
  print_justified(sensor_right, 5);
  motion.reset_drive_system();
}

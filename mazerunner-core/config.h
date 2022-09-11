/*
 * File: config.h
 * Project: mazerunner
 * File Created: Monday, 29th March 2021 11:04:59 pm
 * Author: Peter Harrison
 * -----
 * Last Modified: Thursday, 6th May 2021 9:17:32 am
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

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

/***
 * The config.h file determines the actual robot variant that
 * will be the target for the build.
 *
 * This files lets you pick a specific robot that has its unique
 * configuration stored in that file. In this way, you can write
 * generic code that will work with a variety of different actual
 * robots. there are a number of example robot files in the project.
 * You should pick one that is closest to your setup, copy it and
 * then amend the details in that copy. Finally, add or modify
 * the selection settings below to use your new robot configuration.
 *
 */

/***
 * Start with the pinouts for the robot. These are the pin
 * definitions for the UKMARSBOT V1.x mainboard and should be
 * suitable for a number of derivatives.
 */

const int SERIAL_TX = 0;
const int SERIAL_RX = 1;
const int ENCODER_LEFT_CLK = 2;
const int ENCODER_RIGHT_CLK = 3;
const int ENCODER_LEFT_B = 4;
const int ENCODER_RIGHT_B = 5;
const int USER_IO_6 = 6;
const int MOTOR_LEFT_DIR = 7;
const int MOTOR_RIGHT_DIR = 8;
const int MOTOR_LEFT_PWM = 9;
const int MOTOR_RIGHT_PWM = 10;
const int USER_IO_11 = 11;
const int USER_IO_12 = 12;
const int SENSOR0 = A0;
const int SENSOR1 = A1;
const int SENSOR2 = A2;
const int SENSOR3 = A3;
const int SENSOR4 = A4;
const int SENSOR5 = A5;
const int FUNCTION_PIN = A6;
const int BATTERY_VOLTS = A7;
const int SWITCHES_CHANNEL = 6;
const int BATTERY_CHANNEL = 7;

/*************************************************************************/
/***
 * Structure definitions used in the software. Declared here for lack of a
 * better place.
 * Robot specific instances and values are in the robot config file.
 */
struct TurnParameters {
  int speed;
  int run_in;  // mm
  int run_out; // mm
  int angle;   // deg
  int omega;   // deg/s
  int alpha;   // deg/s/s
  int trigger; // sensor value
};

/*************************************************************************/
/***
 * It is possible that you might want to run the robot in a number of
 * different mazes with different calibration values. The config file
 * can have different sensor defaults for each of these environments
 * so here you can define which set will be used.
 */

#define EVENT_HOME 1
#define EVENT_UK 2
#define EVENT_PORTUGAL 3

// choose the one you will be using
#define EVENT EVENT_UK

/*************************************************************************/
/***
 * Since you may build for different physical robots, their characteristics
 * are kept in their own config files. Add you robot to the list and create
 * a corresponding config file with its custom values.
 *
 * If you have only one robot then you can reduce this section to a single
 * include line.
 */
#define ROBOT_DOROTHY 4
#define ROBOT_EMILY 5
#define ROBOT_FRANK 6

#define ROBOT_NAME ROBOT_EMILY

#if ROBOT_NAME == ROBOT_DOROTHY
#include "config-dorothy.h"
#elif ROBOT_NAME == ROBOT_EMILY
#include "config-emily.h"
#elif ROBOT_NAME == ROBOT_FRANK
#include "config-frank.h"
#else
#error "NO ROBOT DEFINED"
#endif

/*************************************************************************/
/*************************************************************************/

/***
 * these are the defaults for some system-wide settings regardless of the robot
 * or environment. It would be best not to mess with these without good reason.
 * Sometimes the controller needs the interval, sometimes the frequency
 * define one and pre-calculate the other. The compiler does the work and no flash or
 * RAM storage is used. Constants are used for better type checking and traceability.
 */

const float LOOP_FREQUENCY = 500.0f;
const float LOOP_INTERVAL = (1.0f / LOOP_FREQUENCY);

//***************************************************************************//

// This is the size fo each cell in the maze. Normally 180mm for a classic maze
const float FULL_CELL = 180.0f;
const float HALF_CELL = FULL_CELL / 2.0f;

//***************************************************************************//
/***
 * Use the physical constants from the robot config file to pre-calculate
 * some essential scaling factors. In that robot specific config file you
 * must provide:
 *      - ROTATION_BIAS
 *      - WHEEL_DIAMETER
 *      - ENCODER_PULSES
 *      - GEAR_RATIO
 *      - MOUSE_RADIUS
 */
const float MM_PER_COUNT_LEFT = (1 - ROTATION_BIAS) * PI * WHEEL_DIAMETER / (ENCODER_PULSES * GEAR_RATIO);
const float MM_PER_COUNT_RIGHT = (1 + ROTATION_BIAS) * PI * WHEEL_DIAMETER / (ENCODER_PULSES * GEAR_RATIO);
const float DEG_PER_MM_DIFFERENCE = (180.0 / (2 * MOUSE_RADIUS * PI));

//***************************************************************************//
/***
 * This piece of magic lets you define a variable, such as the maze, that can
 * survive a processor reset. The downside is that you MUST take care to
 * properly initialise it when necessary. If you just turn on the robot for
 * example, the maze will have random data in it.
 *
 * CLEAR THE MAZE BEFORE EVERY CONTEST
 *
 * The mazerunner code clears the maze if the user button is held down during
 * a reset.
 */
#define PERSISTENT __attribute__((section(".noinit")))

#endif

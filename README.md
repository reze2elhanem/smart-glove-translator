# Smart Glove Translator

## Overview
The Smart Glove Translator is a wearable device that converts hand gestures into text using **flex sensors** and an **MPU6050 motion sensor**. Each finger has a flex sensor that detects its bending angle, while the MPU6050 measures the rotation of the hand. The data is processed by an **Arduino**, and the recognized gestures are displayed on an **LCD screen**.

## Goal
To create an assistive device that helps people with speech or hearing disabilities communicate easily using gestures.

## Components
- Arduino Uno
- 10 Flex Sensors (one for each finger)
-  2 MPU6050 Sensor
- 16x2 LCD Display
- Jumper Wires
- Power Source (battery or USB)

## How It Works
1. Each flex sensor measures how much each finger is bent.
2. The MPU6050 reads the hand’s motion and rotation.
3. The Arduino processes the readings and compares them to pre-defined gesture values.
4. The detected gesture is shown on the LCD screen.
5. Later versions will include a voice module to speak the translated words.

## Future Plans
- Add a voice output module
- Create a 3D printed glove design
- Train a machine learning model for dynamic gestures

## Made by
**Joudy Walid**

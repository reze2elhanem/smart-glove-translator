Overview

The Smart Glove Translator is basically a glove I’m building that can translate hand gestures into text.
I’m using flex sensors on the fingers to detect how much they bend, and an MPU6050 sensor to track the hand’s movement and rotation.
All the data goes into an Arduino, which then figures out which gesture I’m making and shows the result on an LCD screen.

It’s like a small translator that helps people who can’t speak or hear communicate just by moving their hands.


 Goal

The main goal is to make communication easier for people with speech or hearing disabilities —
so they can “talk” using gestures and have their message displayed as text (and later even as voice).


 Components

Here’s what I’m using in my setup:

Arduino Uno

10 Flex Sensors (one for each finger)

2 MPU6050 Sensors

16x2 LCD Display

Jumper Wires

Power Source (battery or USB)


How It Works

The flex sensors measure how much each finger bends.

The MPU6050 reads the hand’s rotation and motion.

The Arduino compares those readings with pre-set gesture values.

When it matches a gesture, it shows the corresponding word on the LCD screen.

In the future, I’m planning to add a module that can actually say the words out loud.


 Future Plans

This is just my first version, but I’m planning to:

Add a voice output module

Create a 3D printed glove design

Try training a machine learning model to recognize more dynamic gestures

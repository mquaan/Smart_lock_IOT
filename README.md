# Smart Lock Project

## Overview
This project involves the design and implementation of a smart lock system, which can be controlled using various methods such as fingerprint recognition, keypad input, and remote access via a web interface.

## Team Members
- **Doãn Anh Khoa** - 21127076
- **Nguyễn Minh Quân (Team Leader)** - 21127143
- **Phạm Sĩ Phú** - 21127537

## Table of Contents
1. [Project Overview](#overview)
2. [Features](#features)
3. [System Architecture](#system-architecture)
4. [3D Design](#3d-design)
5. [Web Interface](#web-interface)
6. [NodeRED Flow Explanation](#nodered-flow-explanation)
7. [References](#references)

## Features
### Device Features
- **Manual Unlocking**: Unlock the door from inside using a button.
- **LCD Display**: Display messages and status information.
- **Fingerprint Sensor**: Unlock the door using registered fingerprints.
- **Keypad**: Unlock the door using a numeric password.
- **Admin Functions**: Add or remove passwords and fingerprints through the keypad.
- **Security Alerts**: Sound an alarm after 5 failed access attempts and lock the system for 2 minutes.
- **Emergency Notification**: Send emergency notifications to the owner.

### Web and Cloud Features
- **User Authentication**: Login to the web interface with predefined credentials.
- **Device Status**: Display device status and recent access logs.
- **Dynamic Password**: Generate temporary passwords for guest access.
- **Remote Unlock**: Unlock the door remotely through the web interface.
- **Data Storage**: Store access logs and user data on the cloud.

## System Architecture
![Data Flow Diagram](link-to-diagram)

### Data Flow
1. **Buzzer**: Receives digital signals from the Wemos board.
2. **Relay**: Controls the electronic lock based on signals from the Wemos board.
3. **LCD Screen**: Displays status messages and is controlled by the Arduino Uno.
4. **Keypad**: Sends character inputs to the Arduino Uno.
5. **Fingerprint Sensor**: Communicates with the Arduino Uno for fingerprint recognition.
6. **Wemos Board**: Connects to Wi-Fi and communicates with the MQTT broker and web server.
7. **MQTT Broker**: Facilitates communication between the Wemos board and the web server.
8. **NodeRED**: Manages data flows and interacts with IFTTT for notifications.

## 3D Design
![3D Design](link-to-3d-design-image)
- **Part A**: External unit with fingerprint sensor and keypad.
- **Part B**: Internal unit with manual unlock button and LCD display.

## Web Interface
### Sign In and Logout
- **Sign In**: Allows administrators to log in to the web interface.
- **Logout**: Securely logs out the user from the web interface.

### Home Tab
- Displays basic device information and the most recent access method.

### History Tab
- Shows the access history, including methods and timestamps.

### Security Tab
- **Dynamic Password**: Generate a temporary password for guest access.
- **Remote Unlock**: Unlock the door remotely by entering the admin password.

## NodeRED Flow Explanation
1. **Login to Website**: Flow for user authentication.
2. **Device Information**: Flow to display device status.
3. **Access History**: Flow to show access logs.
4. **Dynamic Password Creation**: Flow to generate and manage temporary passwords.
5. **Remote Unlock**: Flow to unlock the door remotely.

## References
- Project documentation and implementation details are available in the project report.
- For more information on specific components and libraries used, refer to the source code and related documentation.

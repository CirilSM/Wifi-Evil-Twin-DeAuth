# ESP8266 Captive Portal Lab

## Overview
The **ESP8266 Captive Portal Lab** is an educational project designed for use within controlled, isolated security laboratory environments. It demonstrates the mechanics of a "Captive Portal" attack (often referred to as an "Evil Twin" simulation). 

The device functions as an Access Point (AP) that intercepts DNS requests, redirects users to a localized web interface, and simulates credential validation by attempting to connect to an target network using captured input.

> **⚠️ WARNING: LEGAL & ETHICAL NOTICE**
> This project is strictly for **educational and research purposes only** in a controlled lab environment. The techniques demonstrated here can be used for unauthorized credential harvesting if deployed against networks without express written permission. **Unauthorized use is illegal and unethical.** The developer assumes no liability for misuse of this code.

## Features
* **DNS Redirection:** Uses `DNSServer` to catch all traffic and force users to the local portal.
* **Credential Harvesting:** Captures user input (password) via a custom HTML interface.
* **Automated Validation:** Uses `WIFI_AP_STA` mode to verify captured credentials against the target network in the background.
* **OLED Feedback:** Real-time visual status updates using an SSD1306 OLED display.
* **Admin Interface:** Includes a secure-ish (simulated) admin mode to manage the device.

## Hardware Requirements
* **Microcontroller:** ESP8266 (e.g., NodeMCU, Wemos D1 Mini).
* **Display:** SSD1306 I2C OLED Display (128x64).
* **Power:** Micro-USB cable or external power supply.

## Dependencies
This project requires the following Arduino libraries:
* `ESP8266WiFi` (Built-in)
* `ESP8266WebServer` (Built-in)
* `DNSServer` (Built-in)
* `Adafruit_GFX`
* `Adafruit_SSD1306`
* `Wire`

## How it Works
1. **Scanning:** Upon startup, the device scans for available networks.
2. **Setup:** The administrator selects an SSID from the scan list via the Admin interface.
3. **Redirection:** The device broadcasts an open AP with the chosen name. Any user joining this AP is redirected to the login portal.
4. **Capture:** When a user submits a password, the ESP8266 transitions to Station (STA) mode to verify the password.
5. **Feedback:** If the connection succeeds, the password is saved, and the result is displayed on the OLED screen.

## Installation
1. Install the required libraries via the Arduino Library Manager.
2. Update the `ADMIN_SSID` and `ADMIN_PASS` in the source code if necessary.
3. Upload the sketch to your ESP8266.
4. Connect to the `Security_Lab_Node` WiFi to access the configuration panel.

## Security Context
This project is intended to help students and researchers understand:
* How Captive Portals work.
* Why public "Open" WiFi networks are inherently insecure.
* The importance of not submitting sensitive data to untrusted network portals.

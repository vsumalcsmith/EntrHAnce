/*   
 * EntrHAnce
 * Licensed under the MIT License, Copyright (c) 2017 marthoc
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

// Mapping NodeMCU Ports to Arduino GPIO Pins
// Allows use of NodeMCU Port nomenclature in config.h
#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12 
#define D7 13
#define D8 15

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const boolean static_ip = STATIC_IP;
IPAddress ip(IP);
IPAddress gateway(GATEWAY);
IPAddress subnet(SUBNET);

const char* mqtt_broker = MQTT_BROKER;
const char* mqtt_clientId = MQTT_CLIENTID;
const char* mqtt_username = MQTT_USERNAME;
const char* mqtt_password = MQTT_PASSWORD;
String mqtt_topicbase = MQTT_TOPICBASE;

const bool entry1_enabled = ENTRY1_ENABLED;
const bool entry2_enabled = ENTRY2_ENABLED;
const bool entry3_enabled = ENTRY3_ENABLED;
const bool entry4_enabled = ENTRY4_ENABLED;
const bool entry5_enabled = ENTRY5_ENABLED;
const bool entry6_enabled = ENTRY6_ENABLED;
const bool entry7_enabled = ENTRY7_ENABLED;
const bool entry8_enabled = ENTRY8_ENABLED;

const char* entry1_alias = ENTRY1_ALIAS;
const char* entry2_alias = ENTRY2_ALIAS;
const char* entry3_alias = ENTRY3_ALIAS;
const char* entry4_alias = ENTRY4_ALIAS;
const char* entry5_alias = ENTRY5_ALIAS;
const char* entry6_alias = ENTRY6_ALIAS;
const char* entry7_alias = ENTRY7_ALIAS;
const char* entry8_alias = ENTRY8_ALIAS;

String entry1_topicStr = mqtt_topicbase + "/1";
String entry2_topicStr = mqtt_topicbase + "/2";
String entry3_topicStr = mqtt_topicbase + "/3";
String entry4_topicStr = mqtt_topicbase + "/4";
String entry5_topicStr = mqtt_topicbase + "/5";
String entry6_topicStr = mqtt_topicbase + "/6";
String entry7_topicStr = mqtt_topicbase + "/7";
String entry8_topicStr = mqtt_topicbase + "/8";

const char* entry1_topic = entry1_topicStr.c_str();
const char* entry2_topic = entry2_topicStr.c_str();
const char* entry3_topic = entry3_topicStr.c_str();
const char* entry4_topic = entry4_topicStr.c_str();
const char* entry5_topic = entry5_topicStr.c_str();
const char* entry6_topic = entry6_topicStr.c_str();
const char* entry7_topic = entry7_topicStr.c_str();
const char* entry8_topic = entry8_topicStr.c_str();

const int entry1_pin = D1;
const int entry2_pin = D2;
const int entry3_pin = D3;
const int entry4_pin = D4;
const int entry5_pin = D5;
const int entry6_pin = D6;
const int entry7_pin = D7;
const int entry8_pin = D8;

const boolean entry1_switch_no = ENTRY1_SWITCH_NO;
const boolean entry2_switch_no = ENTRY2_SWITCH_NO;
const boolean entry3_switch_no = ENTRY3_SWITCH_NO;
const boolean entry4_switch_no = ENTRY4_SWITCH_NO;
const boolean entry5_switch_no = ENTRY5_SWITCH_NO;
const boolean entry6_switch_no = ENTRY6_SWITCH_NO;
const boolean entry7_switch_no = ENTRY7_SWITCH_NO;
const boolean entry8_switch_no = ENTRY8_SWITCH_NO;

int entry1_lastStatusValue = -1;
int entry2_lastStatusValue = -1;
int entry3_lastStatusValue = -1;
int entry4_lastStatusValue = -1;
int entry5_lastStatusValue = -1;
int entry6_lastStatusValue = -1;
int entry7_lastStatusValue = -1;
int entry8_lastStatusValue = -1;

String entry1_statusString = "";
String entry2_statusString = "";
String entry3_statusString = "";
String entry4_statusString = "";
String entry5_statusString = "";
String entry6_statusString = "";
String entry7_statusString = "";
String entry8_statusString = "";

unsigned long entry1_lastSwitchTime = 0;
unsigned long entry2_lastSwitchTime = 0;
unsigned long entry3_lastSwitchTime = 0;
unsigned long entry4_lastSwitchTime = 0;
unsigned long entry5_lastSwitchTime = 0;
unsigned long entry6_lastSwitchTime = 0;
unsigned long entry7_lastSwitchTime = 0;
unsigned long entry8_lastSwitchTime = 0;

int debounceTime = 2000;

String availabilityBase = MQTT_CLIENTID;
String availabilityTopicStr = availabilityBase + "/availability";
const char* availabilityTopic = availabilityTopicStr.c_str();
const char* birthMessage = "online";
const char* lwtMessage = "offline";

String commandTopicStr = availabilityBase + "/command";
const char* commandTopic = commandTopicStr.c_str();

WiFiClient espClient;
PubSubClient client(espClient);

// Wifi setup function

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  if (static_ip) {
    WiFi.config(ip, gateway, subnet);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print(" WiFi connected - IP address: ");
  Serial.println(WiFi.localIP());
}

// Callback when MQTT message is received

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  
  Serial.println();

  String topicToProcess = topic;
  payload[length] = '\0';
  String payloadToProcess = (char*)payload;
  processReceivedMessage(topicToProcess, payloadToProcess);
}

// Functions that check entry status and publish an update when called

void publish_entry1_status() {
  if (digitalRead(entry1_pin) == LOW) {
    if (entry1_switch_no) {
      Serial.print(entry1_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry1_topic);
      Serial.println("...");
      client.publish(entry1_topic, "closed", true);
      entry1_statusString = "closed";
    }
    else {
      Serial.print(entry1_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry1_topic);
      Serial.println("...");
      client.publish(entry1_topic, "open", true);
      entry1_statusString = "open";      
    }
  }
  else {
    if (entry1_switch_no) {
      Serial.print(entry1_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry1_topic);
      Serial.println("...");
      client.publish(entry1_topic, "open", true);
      entry1_statusString = "open";
    }
    else {
      Serial.print(entry1_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry1_topic);
      Serial.println("...");
      client.publish(entry1_topic, "closed", true);
      entry1_statusString = "closed";      
    }
  }
}

void publish_entry2_status() {
  if (digitalRead(entry2_pin) == LOW) {
    if (entry2_switch_no) {
      Serial.print(entry2_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry2_topic);
      Serial.println("...");
      client.publish(entry2_topic, "closed", true);
      entry2_statusString = "closed";
    }
    else {
      Serial.print(entry2_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry2_topic);
      Serial.println("...");
      client.publish(entry2_topic, "open", true);
      entry2_statusString = "open";      
    }
  }
  else {
    if (entry2_switch_no) {
      Serial.print(entry2_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry2_topic);
      Serial.println("...");
      client.publish(entry2_topic, "open", true);
      entry2_statusString = "open";
    }
    else {
      Serial.print(entry2_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry2_topic);
      Serial.println("...");
      client.publish(entry2_topic, "closed", true);
      entry2_statusString = "closed";      
    }
  }
}

void publish_entry3_status() {
  if (digitalRead(entry3_pin) == LOW) {
    if (entry3_switch_no) {
      Serial.print(entry3_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry3_topic);
      Serial.println("...");
      client.publish(entry3_topic, "closed", true);
      entry3_statusString = "closed";
    }
    else {
      Serial.print(entry3_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry3_topic);
      Serial.println("...");
      client.publish(entry3_topic, "open", true);
      entry3_statusString = "open";      
    }
  }
  else {
    if (entry3_switch_no) {
      Serial.print(entry3_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry3_topic);
      Serial.println("...");
      client.publish(entry3_topic, "open", true);
      entry3_statusString = "open";
    }
    else {
      Serial.print(entry3_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry3_topic);
      Serial.println("...");
      client.publish(entry3_topic, "closed", true);
      entry3_statusString = "closed";      
    }
  }
}

void publish_entry4_status() {
  if (digitalRead(entry4_pin) == LOW) {
    if (entry4_switch_no) {
      Serial.print(entry4_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry4_topic);
      Serial.println("...");
      client.publish(entry4_topic, "closed", true);
      entry4_statusString = "closed";
    }
    else {
      Serial.print(entry4_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry4_topic);
      Serial.println("...");
      client.publish(entry4_topic, "open", true);
      entry4_statusString = "open";      
    }
  }
  else {
    if (entry4_switch_no) {
      Serial.print(entry4_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry4_topic);
      Serial.println("...");
      client.publish(entry4_topic, "open", true);
      entry4_statusString = "open";
    }
    else {
      Serial.print(entry4_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry4_topic);
      Serial.println("...");
      client.publish(entry4_topic, "closed", true);
      entry4_statusString = "closed";      
    }
  }
}

void publish_entry5_status() {
  if (digitalRead(entry5_pin) == LOW) {
    if (entry5_switch_no) {
      Serial.print(entry5_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry5_topic);
      Serial.println("...");
      client.publish(entry5_topic, "closed", true);
      entry5_statusString = "closed";
    }
    else {
      Serial.print(entry5_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry5_topic);
      Serial.println("...");
      client.publish(entry5_topic, "open", true);
      entry5_statusString = "open";      
    }
  }
  else {
    if (entry5_switch_no) {
      Serial.print(entry5_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry5_topic);
      Serial.println("...");
      client.publish(entry5_topic, "open", true);
      entry5_statusString = "open";
    }
    else {
      Serial.print(entry5_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry5_topic);
      Serial.println("...");
      client.publish(entry5_topic, "closed", true);
      entry5_statusString = "closed";      
    }
  }
}

void publish_entry6_status() {
  if (digitalRead(entry6_pin) == LOW) {
    if (entry6_switch_no) {
      Serial.print(entry6_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry6_topic);
      Serial.println("...");
      client.publish(entry6_topic, "closed", true);
      entry6_statusString = "closed";
    }
    else {
      Serial.print(entry6_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry6_topic);
      Serial.println("...");
      client.publish(entry6_topic, "open", true);
      entry6_statusString = "open";      
    }
  }
  else {
    if (entry6_switch_no) {
      Serial.print(entry6_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry6_topic);
      Serial.println("...");
      client.publish(entry6_topic, "open", true);
      entry6_statusString = "open";
    }
    else {
      Serial.print(entry6_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry6_topic);
      Serial.println("...");
      client.publish(entry6_topic, "closed", true);
      entry6_statusString = "closed";      
    }
  }
}

void publish_entry7_status() {
  if (digitalRead(entry7_pin) == LOW) {
    if (entry7_switch_no) {
      Serial.print(entry7_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry7_topic);
      Serial.println("...");
      client.publish(entry7_topic, "closed", true);
      entry7_statusString = "closed";
    }
    else {
      Serial.print(entry7_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry7_topic);
      Serial.println("...");
      client.publish(entry7_topic, "open", true);
      entry7_statusString = "open";      
    }
  }
  else {
    if (entry7_switch_no) {
      Serial.print(entry7_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry7_topic);
      Serial.println("...");
      client.publish(entry7_topic, "open", true);
      entry7_statusString = "open";
    }
    else {
      Serial.print(entry7_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry7_topic);
      Serial.println("...");
      client.publish(entry7_topic, "closed", true);
      entry7_statusString = "closed";      
    }
  }
}

void publish_entry8_status() {
  if (digitalRead(entry8_pin) == LOW) {
    if (entry8_switch_no) {
      Serial.print(entry8_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry8_topic);
      Serial.println("...");
      client.publish(entry8_topic, "closed", true);
      entry8_statusString = "closed";
    }
    else {
      Serial.print(entry8_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry8_topic);
      Serial.println("...");
      client.publish(entry8_topic, "open", true);
      entry8_statusString = "open";      
    }
  }
  else {
    if (entry8_switch_no) {
      Serial.print(entry8_alias);
      Serial.print(" open! Publishing to ");
      Serial.print(entry8_topic);
      Serial.println("...");
      client.publish(entry8_topic, "open", true);
      entry8_statusString = "open";
    }
    else {
      Serial.print(entry8_alias);
      Serial.print(" closed! Publishing to ");
      Serial.print(entry8_topic);
      Serial.println("...");
      client.publish(entry8_topic, "closed", true);
      entry8_statusString = "closed";      
    }
  }
}

// Functions that run in loop() to check each loop if entry status (open/closed) has changed and call publish_entryX_status() to publish any change if so

void check_entry1_status() {
  int currentStatusValue = digitalRead(entry1_pin);
  if (currentStatusValue != entry1_lastStatusValue) {
    unsigned long currentTime = millis();
    if (currentTime - entry1_lastSwitchTime >= debounceTime) {
      publish_entry1_status();
      entry1_lastStatusValue = currentStatusValue;
      entry1_lastSwitchTime = currentTime;
    }
  }
}

void check_entry2_status() {
  int currentStatusValue = digitalRead(entry2_pin);
  if (currentStatusValue != entry2_lastStatusValue) {
    unsigned long currentTime = millis();
    if (currentTime - entry2_lastSwitchTime >= debounceTime) {
      publish_entry2_status();
      entry2_lastStatusValue = currentStatusValue;
      entry2_lastSwitchTime = currentTime;
    }
  }
}

void check_entry3_status() {
  int currentStatusValue = digitalRead(entry3_pin);
  if (currentStatusValue != entry3_lastStatusValue) {
    unsigned long currentTime = millis();
    if (currentTime - entry3_lastSwitchTime >= debounceTime) {
      publish_entry3_status();
      entry3_lastStatusValue = currentStatusValue;
      entry3_lastSwitchTime = currentTime;
    }
  }
}

void check_entry4_status() {
  int currentStatusValue = digitalRead(entry4_pin);
  if (currentStatusValue != entry4_lastStatusValue) {
    unsigned long currentTime = millis();
    if (currentTime - entry4_lastSwitchTime >= debounceTime) {
      publish_entry4_status();
      entry4_lastStatusValue = currentStatusValue;
      entry4_lastSwitchTime = currentTime;
    }
  }
}

void check_entry5_status() {
  int currentStatusValue = digitalRead(entry5_pin);
  if (currentStatusValue != entry5_lastStatusValue) {
    unsigned long currentTime = millis();
    if (currentTime - entry5_lastSwitchTime >= debounceTime) {
      publish_entry5_status();
      entry5_lastStatusValue = currentStatusValue;
      entry5_lastSwitchTime = currentTime;
    }
  }
}

void check_entry6_status() {
  int currentStatusValue = digitalRead(entry6_pin);
  if (currentStatusValue != entry6_lastStatusValue) {
    unsigned long currentTime = millis();
    if (currentTime - entry6_lastSwitchTime >= debounceTime) {
      publish_entry6_status();
      entry6_lastStatusValue = currentStatusValue;
      entry6_lastSwitchTime = currentTime;
    }
  }
}

void check_entry7_status() {
  int currentStatusValue = digitalRead(entry7_pin);
  if (currentStatusValue != entry7_lastStatusValue) {
    unsigned long currentTime = millis();
    if (currentTime - entry7_lastSwitchTime >= debounceTime) {
      publish_entry7_status();
      entry7_lastStatusValue = currentStatusValue;
      entry7_lastSwitchTime = currentTime;
    }
  }
}

void check_entry8_status() {
  int currentStatusValue = digitalRead(entry8_pin);
  if (currentStatusValue != entry8_lastStatusValue) {
    unsigned long currentTime = millis();
    if (currentTime - entry8_lastSwitchTime >= debounceTime) {
      publish_entry8_status();
      entry8_lastStatusValue = currentStatusValue;
      entry8_lastSwitchTime = currentTime;
    }
  }
}

// Function that publishes birthMessage

void publish_birth_message() {
  // Publish the birthMessage
  Serial.print("Publishing birth message \"");
  Serial.print(birthMessage);
  Serial.print("\" to ");
  Serial.print(availabilityTopic);
  Serial.println("...");
  client.publish(availabilityTopic, birthMessage, true);
}

// Function called by callback() when a message is received 
// Passes the message topic as the "requestedDoor" parameter and the message payload as the "requestedAction" parameter
// Calls doorX_sanityCheck() to verify that the door is in a different state than requested before taking action, else trigger a status update

void processReceivedMessage(String topic, String payload) {
  if (topic == commandTopicStr && payload == "STATUS") {
    publish_entry1_status;
    publish_entry2_status;
    publish_entry3_status;
    publish_entry4_status;
    publish_entry5_status;
    publish_entry6_status;
    publish_entry7_status;
    publish_entry8_status;
  }
  else { 
    Serial.println("Unrecognized topic or payload... doing nothing!");
  }
}

// Function that runs in loop() to connect/reconnect to the MQTT broker, and publish the current door statuses on connect

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_clientId, mqtt_username, mqtt_password, availabilityTopic, 0, true, lwtMessage)) {
      Serial.println("Connected!");

      // Publish the birth message on connect/reconnect
      publish_birth_message();

      // Subscribe to the command topic
      Serial.print("Subscribing to ");
      Serial.print(commandTopic);
      Serial.println("...");
      client.subscribe(commandTopic);
     
      // Publish the current entry status on connect/reconnect to ensure status is synced with whatever happened while disconnected
      if (entry1_enabled) { publish_entry1_status();
      }
      if (entry2_enabled) { publish_entry2_status();
      }
      if (entry3_enabled) { publish_entry3_status();
      }
      if (entry4_enabled) { publish_entry4_status();
      }
      if (entry5_enabled) { publish_entry5_status();
      }
      if (entry6_enabled) { publish_entry6_status();
      }
      if (entry7_enabled) { publish_entry7_status();
      }
      if (entry8_enabled) { publish_entry8_status();
      }

    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // Setup the pins used in the sketch
  // Set the lastStatusValue variables to the state of the pins at setup time

  if (entry1_enabled) {
    pinMode(entry1_pin, INPUT_PULLUP);
    entry1_lastStatusValue = digitalRead(entry1_pin);
  }
  if (entry2_enabled) {
    pinMode(entry2_pin, INPUT_PULLUP);
    entry2_lastStatusValue = digitalRead(entry2_pin);
  }
  if (entry3_enabled) {
    pinMode(entry3_pin, INPUT_PULLUP);
    entry3_lastStatusValue = digitalRead(entry3_pin);
  }
  if (entry4_enabled) {
    pinMode(entry4_pin, INPUT_PULLUP);
    entry4_lastStatusValue = digitalRead(entry4_pin);
  }
  if (entry5_enabled) {
    pinMode(entry5_pin, INPUT_PULLUP);
    entry5_lastStatusValue = digitalRead(entry5_pin);
  }
  if (entry6_enabled) {
    pinMode(entry6_pin, INPUT_PULLUP);
    entry6_lastStatusValue = digitalRead(entry6_pin);
  }
  if (entry7_enabled) {
    pinMode(entry7_pin, INPUT_PULLUP);
    entry7_lastStatusValue = digitalRead(entry7_pin);
  }
  if (entry8_enabled) {
    pinMode(entry8_pin, INPUT_PULLUP);
    entry8_lastStatusValue = digitalRead(entry8_pin);
  }

  // Setup serial output, connect to wifi, connect to MQTT broker, set MQTT message callback
  Serial.begin(115200);
  Serial.println("Starting EntrHAnce...");
  
  setup_wifi();
  client.setServer(mqtt_broker, 1883);
  client.setCallback(callback);
}

void loop() {
  // Connect/reconnect to the MQTT broker and listen for messages
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Check entry open/closed status each loop and publish changes
  if (entry1_enabled) { check_entry1_status();
  }
  if (entry2_enabled) { check_entry2_status();
  }
  if (entry3_enabled) { check_entry3_status();
  }
  if (entry4_enabled) { check_entry4_status();
  }
  if (entry5_enabled) { check_entry5_status();
  }
  if (entry6_enabled) { check_entry6_status();
  }
  if (entry7_enabled) { check_entry7_status();
  }
  if (entry8_enabled) { check_entry8_status();
  }
  
}

# EntrHAnce
Retrofit an existing wired alarm system into your home automation system using MQTT.

## Overview
EntrHAnce monitors reed/magnetic switches connected to NodeMCU pins D1-D8 and publishes status changes over MQTT.

**Topic**: MQTT_TOPICBASE/Pin# (_by default:_ **entry/1** to **entry/8**)  
**Payload**: "open" or "closed"

## Basic Setup Instructions

### Hardware
1. NodeMCU
2. Breadboard
3. Power supply (5v USB or breadboard power supply)
4. Dupont wires

Connect one leg of each reed/magnetic switch to a monitored pin (D1-D8) and the other leg to ground.

### Software
1. Set configuration parameters in config.h
2. Upload to NodeMCU

### Home Assistant
Create entries in `configuration.yaml` for each enabled entry like so:
```yaml
binary_sensor:
  - platform: mqtt
    name: "Entry 1"
    state_topic: "entry/1"
    payload_on: "open"
    payload_off: "closed"
    availability_topic: "EntrHAnce/availability"
    device_class: opening    
```

### Request Status Update
A status update for each enabled entry can be requested by publishing the payload "STATUS" to the topic MQTT_CLIENTID/command.

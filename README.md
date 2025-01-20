# LooksPlusIO

IoT Device Development SDK for atCloud365.com

## Concept

By generalizing IoT devices, all devices are managed as a single object.<br>
The Object internal methods(funections) are all same but data is different.

## Tested B/D (Modules)

### ESP32

- OLIMAX-ESP32-GATEWAY B/D

### ESP8266

- ESP8266 ( not yet verified)

```text
https://arduino.esp8266.com/stable/package_esp8266com_index.json
https://arduino.esp8266.com/stable/package_esp8266com_index.json,https://espressif.github.io/arduino-esp32/package_esp32_index.json
```

## External Libs

- List of libs ( external libs)
  
  ```text
  ArduinoJson
  DHTesp
  NTPClient
  WebSockets_Generic
  ```

## Live Demo

https://atcloud365.com

## Steps for Using SDKs

- This is for developer testing and developing IoT device
- For the M/P, contact alex@happyelec.com

### Device SN(Serial Number)

Using SDKs require "Device SN", Please follow-up below process

- Access https://damosys.com:9000/mfg
- Signup and Login
- Define Your Equipment
- Generate SN for your Device
- Details with Pictures

![image](./images/mfg_signup.png)
![image](./images/mfg_add_equipment.png)
![image](./images/mfg_add_equipment_dlg.png)
![image](./images/mfg_add_equipment_dlg2.png)
![image](./images/mfg_list_of_equipments_table.png)
![image](./images/mfg_equipments_gen_sn.png)
![image](./images/mfg_equipments_gen_sn_confirm.png)

![image](./images/mfg_sn_export.png)
![image](./images/mfg_sn_in_sketch.png)
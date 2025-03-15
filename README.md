# EEPROM I2C Manager

EEPROM I2C Manager is a **C++ command-line tool** for reading, writing, and managing **I2C EEPROM devices using GPIO**.  
It supports various EEPROM chip sizes and provides essential functionalities like dumping EEPROM contents, writing random data, flashing firmware, and more.

---

## 📌 **Features**

- 🏷 **EEPROM Detection** – Scan the I2C bus for connected EEPROM chips.
- 📝 **Read Data** – Dump EEPROM contents in hexadecimal and ASCII format.
- 🔄 **Write Data** – Write random data, firmware, or erase EEPROM.
- 🔥 **Flash Firmware** – Upload binary firmware to the EEPROM.
- 💾 **Backup Firmware** – Save EEPROM contents to a file.

Supported EEPROM types:
```
24C02, 24C04, 24C08, 24C16, 24C32, 24C64, 24C128, 24C256, 24C512, 24C1024
```
___

## 🔌 **How to Connect EEPROM to Raspberry Pi (or Linux-based SBC)**

1. **Identify I2C Pins on Your Board**  
   - Raspberry Pi:
     ```
     SDA → GPIO2 (Pin 3)
     SCL → GPIO3 (Pin 5)
     GND → Any GND Pin
     VCC → 3.3V or 5V (Depends on EEPROM)
     ```

2. **Wiring Diagram**
   ```
   Raspberry Pi         EEPROM (24CXX)
   -------------------------------------
   GPIO2 (SDA)  <-->   SDA
   GPIO3 (SCL)  <-->   SCL
   GND         <-->   GND
   3.3V or 5V  <-->   VCC
   ```

3. **Enable I2C on Raspberry Pi**
   ```bash
   sudo raspi-config   # Enable I2C in "Interfacing Options"
   ```

4. **Check if EEPROM is Detected**
   ```bash
   sudo i2cdetect -y 1
   ```
   If the EEPROM is connected, you should see an address like `0x50`.

---

## 🛠 **Prerequisites**

Before running **EEPROM I2C Manager**, ensure you have:

### **1. C++ Compiler**
Install **g++** (if not already installed):
```bash
sudo apt install g++
```

### **2. I2C Tools & Headers**
```bash
sudo apt install i2c-tools libi2c-dev
```

---

## 🚀 **Installation & Compilation**

### **1. Clone the Repository**
```bash
git clone https://github.com/Electro-Gamma/gpio-i2c-eeprom.git
cd gpio-i2c-eeprom
```

### **2. Compile the Program**
```bash
make
```

### **3. Run the Program**
```bash
sudo ./i2ceeprom --bus 1 --address 0x50 --size 24C256 --read
```

---

## 🎯 **Usage Guide**

### **1. Detect I2C Devices**
Scan for connected I2C EEPROM chips:
```bash
sudo ./i2ceeprom --bus 1 --detect
```

### **2. Read EEPROM Contents**
Dump EEPROM data to the console:
```bash
sudo ./i2ceeprom --bus 1 --address 0x50 --size 24C256 --read
```

Example Output:
```
EEPROM DUMP 0x0000 0x4000
       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F      ASCII DATA
0x0000 | 45 78 61 6D 70 6C 65 20 64 61 74 61 00 00 00 00 | Example data....
```

### **3. Write Random Data**
```bash
sudo ./i2ceeprom --bus 1 --address 0x50 --size 24C256 --random
```

### **4. Erase (Blank) EEPROM**
```bash
sudo ./i2ceeprom --bus 1 --address 0x50 --size 24C256 --blank
```

### **5. Flash Firmware to EEPROM**
```bash
sudo ./i2ceeprom --bus 1 --address 0x50 --size 24C256 --write-firmware firmware.bin
```

### **6. Backup EEPROM Firmware**
```bash
sudo ./i2ceeprom --bus 1 --address 0x50 --size 24C256 --save-firmware backup.bin
```

---
## EEPROM Table

| Type    | Returns (Bytes) | Memory   | Page Size (Bytes) |
|:--------|:---------------:|:--------:|:-----------------:|
| 24C1024 | 131072          | 128 KB   | 128               |
| 24C512  | 65536           | 64 KB    | 128               |
| 24C256  | 32768           | 32 KB    | 64                |
| 24C128  | 16384           | 16 KB    | 64                |
| 24C64   | 8192            | 8 KB     | 32                |
| 24C32   | 4096            | 4 KB     | 32                |
| 24C16   | 2048            | 2 KB     | 16                |
| 24C08   | 1024            | 1 KB     | 16                |
| 24C04   | 512             | 512 B    | 16                |
| 24C02   | 256             | 256 B    | 8                 |
| 24C01   | 128             | 128 B    | 8                 |
___

## 📜 **License**
This project is licensed under the **MIT License**.  
See the [LICENSE](LICENSE) file for details.


### 🎉 Enjoy managing your I2C EEPROM devices with EEPROM I2C Manager!

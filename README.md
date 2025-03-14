# EEPROM I2C Manager

EEPROM I2C Manager is a **C++ command-line tool** for reading, writing, and managing **I2C EEPROM devices**.  
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

### **3. Enable I2C on Linux**
Enable I2C on a **Raspberry Pi** or other Linux systems:
```bash
sudo raspi-config   # Enable I2C in "Interfacing Options"
```

Check available I2C buses:
```bash
ls /dev/i2c-*
```

---

## 🚀 **Installation & Compilation**

### **1. Clone the Repository**
```bash
git clone https://github.com/Electro-Gamma/eeprom-manager.git
cd eeprom_manager
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


## 🔧 **Troubleshooting & Common Issues**

### ❓ **EEPROM Not Detected**
**Solution:**
1. Check if I2C is enabled:
   ```bash
   sudo i2cdetect -y 1
   ```
2. Ensure correct **I2C address** and **bus number** are used.

### ❓ **Cannot Access I2C Device**
**Solution:**
- Run the program with **sudo** to access I2C devices.

### ❓ **Wrong EEPROM Size Detected**
**Solution:**
- Manually specify EEPROM type using `--size 24C256`.

### ❓ **Firmware Flashing Fails**
**Solution:**
- Ensure the **firmware file size** does not exceed EEPROM capacity.


---

## 📜 **License**
This project is licensed under the **MIT License**.  
See the [LICENSE](LICENSE) file for details.


### 🎉 Enjoy managing your I2C EEPROM devices with EEPROM I2C Manager!

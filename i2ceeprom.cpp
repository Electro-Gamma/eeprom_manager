#include <iostream>
#include <fstream>
#include <map>
#include <random>
#include <iomanip>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <vector>
#include <glob.h>
#include <algorithm> // For std::min
#include <errno.h>   // For errno

// Map of EEPROM sizes
std::map<std::string, int> eeprom_sizes = {
    {"24C02", 256},    // 2 Kb -> 256 bytes
    {"24C04", 512},    // 4 Kb -> 512 bytes
    {"24C08", 1024},   // 8 Kb -> 1024 bytes
    {"24C16", 2048},   // 16 Kb -> 2048 bytes
    {"24C32", 4096},   // 32 Kb -> 4096 bytes
    {"24C64", 8192},   // 64 Kb -> 8192 bytes
    {"24C128", 16384}, // 128 Kb -> 16384 bytes
    {"24C256", 32768}, // 256 Kb -> 32768 bytes
    {"24C512", 65536}, // 512 Kb -> 65536 bytes
    {"24C1024", 131072} // 1024 Kb -> 131072 bytes
};

// Function to open the I2C bus
int open_i2c_bus(const std::string& bus_path) {
    int fd = open(bus_path.c_str(), O_RDWR);
    if (fd < 0) {
        std::cerr << "Error: Could not open I2C bus " << bus_path << std::endl;
        return -1;
    }
    return fd;
}

// Function to detect I2C devices
void detect_i2c_devices(int fd) {
    std::cout << "Detected I2C devices:" << std::endl;
    for (int addr = 0x03; addr <= 0x77; addr++) {
        if (ioctl(fd, I2C_SLAVE, addr) >= 0) {
            // Attempt to read a byte from the device
            uint8_t buffer;
            if (read(fd, &buffer, 1) == 1) {
                std::cout << "0x" << std::hex << addr << std::endl;
            } else if (errno == EIO) {
                // No device at this address
                continue;
            }
        }
    }
}

// Function to write a page to EEPROM
void eeprom_write_page(int fd, uint8_t device_address, uint16_t eeaddr, const uint8_t* data, size_t data_size) {
    uint8_t devaddr = device_address | ((eeaddr >> 8) & 0x07);
    uint8_t addr = eeaddr & 0xFF;

    uint8_t buffer[data_size + 1];
    buffer[0] = addr;
    std::memcpy(buffer + 1, data, data_size);

    if (ioctl(fd, I2C_SLAVE, devaddr) < 0) {
        std::cerr << "Error: Failed to set I2C slave address." << std::endl;
        return;
    }

    if (write(fd, buffer, data_size + 1) != static_cast<ssize_t>(data_size + 1)) {
        std::cerr << "Error: Failed to write to EEPROM." << std::endl;
    }
    usleep(10000); // 10 ms delay for EEPROM write cycle
}

// Function to read a page from EEPROM
void eeprom_read_page(int fd, uint8_t device_address, uint16_t eeaddr, uint8_t* data, size_t data_size) {
    uint8_t devaddr = device_address | ((eeaddr >> 8) & 0x07);
    uint8_t addr = eeaddr & 0xFF;

    if (ioctl(fd, I2C_SLAVE, devaddr) < 0) {
        std::cerr << "Error: Failed to set I2C slave address." << std::endl;
        return;
    }

    if (write(fd, &addr, 1) != 1) {
        std::cerr << "Error: Failed to write EEPROM address." << std::endl;
        return;
    }

    if (read(fd, data, data_size) != static_cast<ssize_t>(data_size)) {
        std::cerr << "Error: Failed to read from EEPROM." << std::endl;
    }
}

// Function to dump EEPROM data
void dump_eeprom(int fd, uint8_t device_address, uint16_t start_addr, size_t n_bytes) {
    std::cout << "EEPROM DUMP 0x" << std::hex << start_addr << " 0x" << n_bytes << std::endl;
    std::cout << "       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F      ASCII DATA" << std::endl;

    const size_t page_size = 16;
    uint8_t page_data[page_size];

    for (size_t p = 0; p < n_bytes; p += page_size) {
        uint16_t page_addr = start_addr + p;
        eeprom_read_page(fd, device_address, page_addr, page_data, page_size);

        std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << page_addr << " | ";
        for (size_t c = 0; c < page_size; c++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(page_data[c]) << " ";
        }
        std::cout << "| ";

        for (size_t c = 0; c < page_size; c++) {
            if (page_data[c] >= 0x20 && page_data[c] < 0x7F) {
                std::cout << static_cast<char>(page_data[c]);
            } else {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
}

// Function to write random data to EEPROM
void eeprom_write_random_data(int fd, uint8_t device_address, size_t eeprom_size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    const size_t page_size = 16;
    uint8_t page_data[page_size];

    for (size_t addr = 0; addr < eeprom_size; addr += page_size) {
        size_t bytes_to_write = std::min(page_size, eeprom_size - addr);
        for (size_t i = 0; i < bytes_to_write; i++) {
            page_data[i] = static_cast<uint8_t>(dis(gen));
        }
        eeprom_write_page(fd, device_address, addr, page_data, bytes_to_write);
    }

    std::cout << "Random data written to EEPROM." << std::endl;
}

// Function to blank the EEPROM (write 0xFF to all bytes)
void eeprom_blank(int fd, uint8_t device_address, size_t eeprom_size) {
    const size_t page_size = 16;
    uint8_t blank_data[page_size];
    std::fill(blank_data, blank_data + page_size, 0xFF);

    for (size_t addr = 0; addr < eeprom_size; addr += page_size) {
        size_t bytes_to_write = std::min(page_size, eeprom_size - addr);
        eeprom_write_page(fd, device_address, addr, blank_data, bytes_to_write);
    }

    std::cout << "EEPROM blanked (all bytes set to 0xFF)." << std::endl;
}

// Function to write firmware to EEPROM from a file
void write_firmware_to_eeprom(int fd, uint8_t device_address, const std::string& file_path, size_t eeprom_size) {
    std::ifstream firmware_file(file_path, std::ios::binary | std::ios::ate);
    if (!firmware_file) {
        std::cerr << "Error: Could not open file " << file_path << " for reading." << std::endl;
        return;
    }

    size_t file_size = firmware_file.tellg();
    firmware_file.seekg(0, std::ios::beg);

    if (file_size > eeprom_size) {
        std::cerr << "Error: Firmware file size exceeds EEPROM size." << std::endl;
        return;
    }

    const size_t page_size = 16;
    uint8_t page_data[page_size];

    for (size_t addr = 0; addr < file_size; addr += page_size) {
        size_t bytes_to_read = std::min(page_size, file_size - addr);
        firmware_file.read(reinterpret_cast<char*>(page_data), bytes_to_read);
        eeprom_write_page(fd, device_address, addr, page_data, bytes_to_read);
    }

    std::cout << "Firmware written to EEPROM." << std::endl;
}

// Function to save firmware from EEPROM to a file
void save_firmware_to_file(int fd, uint8_t device_address, size_t eeprom_size, const std::string& file_path) {
    std::ofstream firmware_file(file_path, std::ios::binary);
    if (!firmware_file) {
        std::cerr << "Error: Could not open file " << file_path << " for writing." << std::endl;
        return;
    }

    const size_t page_size = 16;
    uint8_t page_data[page_size];

    for (size_t addr = 0; addr < eeprom_size; addr += page_size) {
        eeprom_read_page(fd, device_address, addr, page_data, page_size);
        firmware_file.write(reinterpret_cast<char*>(page_data), page_size);
    }

    std::cout << "Firmware data saved to " << file_path << std::endl;
}

// Function to print usage information
void print_usage(const std::string& program_name) {
    std::cerr << "Usage: " << program_name << " --bus <bus> [--address <address> --size <size>] [options]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  --detect             Detect I2C devices on the bus (requires --bus)" << std::endl;
    std::cerr << "  --read               Read EEPROM data and display it (requires --bus, --address, --size)" << std::endl;
    std::cerr << "  --random             Write random data to EEPROM (requires --bus, --address, --size)" << std::endl;
    std::cerr << "  --blank              Blank the EEPROM (write 0xFF to all bytes) (requires --bus, --address, --size)" << std::endl;
    std::cerr << "  --write-firmware <file> Write firmware to EEPROM from a file (requires --bus, --address, --size)" << std::endl;
    std::cerr << "  --save-firmware <file> Save firmware from EEPROM to a file (requires --bus, --address, --size)" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    std::string bus_number;
    std::string device_address_str;
    std::string eeprom_size;
    std::string write_firmware_path;
    std::string save_firmware_path;
    bool detect = false;
    bool read = false;
    bool random = false;
    bool blank = false;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--bus" && i + 1 < argc) {
            bus_number = argv[++i];
        } else if (arg == "--address" && i + 1 < argc) {
            device_address_str = argv[++i];
        } else if (arg == "--size" && i + 1 < argc) {
            eeprom_size = argv[++i];
        } else if (arg == "--write-firmware" && i + 1 < argc) {
            write_firmware_path = argv[++i];
        } else if (arg == "--save-firmware" && i + 1 < argc) {
            save_firmware_path = argv[++i];
        } else if (arg == "--detect") {
            detect = true;
        } else if (arg == "--read") {
            read = true;
        } else if (arg == "--random") {
            random = true;
        } else if (arg == "--blank") {
            blank = true;
        } else {
            std::cerr << "Error: Unknown argument " << arg << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }

    // Validate required arguments based on the action
    if (bus_number.empty()) {
        std::cerr << "Error: --bus argument is required." << std::endl;
        print_usage(argv[0]);
        return 1;
    }

    if ((read || random || blank || !write_firmware_path.empty() || !save_firmware_path.empty()) &&
        (device_address_str.empty() || eeprom_size.empty())) {
        std::cerr << "Error: --address and --size arguments are required for this action." << std::endl;
        print_usage(argv[0]);
        return 1;
    }

    // Convert device address to integer
    uint8_t device_address = 0;
    if (!device_address_str.empty()) {
        device_address = static_cast<uint8_t>(std::stoi(device_address_str, nullptr, 16));
    }

    // Validate EEPROM size
    size_t eeprom_bytes = 0;
    if (!eeprom_size.empty()) {
        if (eeprom_sizes.find(eeprom_size) == eeprom_sizes.end()) {
            std::cerr << "Error: Invalid EEPROM size." << std::endl;
            return 1;
        }
        eeprom_bytes = eeprom_sizes[eeprom_size];
    }

    // Construct the I2C bus path
    std::string bus_path = "/dev/i2c-" + bus_number;

    // Open the I2C bus
    int fd = open_i2c_bus(bus_path);
    if (fd < 0) {
        return 1;
    }

    // Perform actions based on command-line arguments
    if (detect) {
        detect_i2c_devices(fd);
    }

    if (read) {
        dump_eeprom(fd, device_address, 0x0000, eeprom_bytes);
    }

    if (random) {
        eeprom_write_random_data(fd, device_address, eeprom_bytes);
    }

    if (blank) {
        eeprom_blank(fd, device_address, eeprom_bytes);
    }

    if (!write_firmware_path.empty()) {
        write_firmware_to_eeprom(fd, device_address, write_firmware_path, eeprom_bytes);
    }

    if (!save_firmware_path.empty()) {
        save_firmware_to_file(fd, device_address, eeprom_bytes, save_firmware_path);
    }

    close(fd);
    return 0;
}
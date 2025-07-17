/*
 * GetDeviceInfoDetail.c
 * C equivalent of the Python GetDeviceInfoDetail.py example
 * 
 * This program demonstrates how to:
 * 1. Use FT_CreateDeviceInfoList to get the number of FTDI devices
 * 2. Use FT_GetDeviceInfoDetail to get detailed information about each device
 * 3. Display the device information in a formatted manner
 */

#include <stdio.h>
#include <stdlib.h>

#include "ftd2xx_labview.h"
#ifdef _WIN32
    #pragma comment(lib, "ftd2xx.lib")
#endif

// Error names for status codes (matches Python version)
const char* error_names[] = {
    "FT_OK",
    "FT_INVALID_HANDLE",
    "FT_DEVICE_NOT_FOUND",
    "FT_DEVICE_NOT_OPENED",
    "FT_IO_ERROR",
    "FT_INSUFFICIENT_RESOURCES",
    "FT_INVALID_PARAMETER",
    "FT_INVALID_BAUD_RATE",
    "FT_DEVICE_NOT_OPENED_FOR_ERASE",
    "FT_DEVICE_NOT_OPENED_FOR_WRITE",
    "FT_FAILED_TO_WRITE_DEVICE",
    "FT_EEPROM_READ_FAILED",
    "FT_EEPROM_WRITE_FAILED",
    "FT_EEPROM_ERASE_FAILED",
    "FT_EEPROM_NOT_PRESENT",
    "FT_EEPROM_NOT_PROGRAMMED",
    "FT_INVALID_ARGS",
    "FT_NOT_SUPPORTED",
    "FT_OTHER_ERROR"
};

// Function to check status and handle errors (equivalent to Python check() function)
void check_status(FT_STATUS status) {
    if (status != FT_OK) {
        if (status < sizeof(error_names)/sizeof(error_names[0])) {
            fprintf(stderr, "Error: (status %d: %s)\n", status, error_names[status]);
        } else {
            fprintf(stderr, "Error: (status %d: Unknown error)\n", status);
        }
        exit(1);
    }
}

// Function to print device details (equivalent to Python printDetails() method)
void print_device_details(DWORD dev, DWORD flags, DWORD type, DWORD id, 
                         DWORD locId, char* serialNumber, char* description, 
                         FT_HANDLE ftHandle) {
    printf("Dev: %lu\n", dev);
    printf(" Flags=0x%lx\n", flags);
    printf(" Type=0x%lx\n", type);
    printf(" ID=0x%lx\n", id);
    printf(" LocId=0x%lx\n", locId);
    printf(" SerialNumber=%s\n", serialNumber);
    printf(" Description=%s\n", description);
    printf(" ftHandle=0x%p\n", (void*)ftHandle);
    printf("\n");
}

int main() {
    FT_STATUS status;
    DWORD numDevices;
    DWORD flags, type, id, locId;
    char serialNumber[64];
    char description[64];
    FT_HANDLE ftHandleTemp;
    DWORD i;
    
    printf("===== C D2XX Get Device Info Detail =====\n\n");
    printf("D2XX library loaded OK\n\n");
    
    // Create device info list and get the number of devices
    status = FT_CreateDeviceInfoList(&numDevices);
    check_status(status);
    
    printf("Number of devices: %lu\n\n", numDevices);
    
    if (numDevices > 0) {
        // Loop through each device and get detailed information
        for (i = 0; i < numDevices; i++) {
            // Get device info detail for device i
            status = FT_GetDeviceInfoDetail(i, &flags, &type, &id, &locId, 
                                          serialNumber, description, &ftHandleTemp);
            check_status(status);
            
            // Print the device details
            print_device_details(i, flags, type, id, locId, 
                               serialNumber, description, ftHandleTemp);
        }
    } else {
        printf("No FTDI devices found.\n");
    }
    
    return 0;
}

/*
 * Compilation instructions:
 * 
 * Windows (Visual Studio):
 * cl GetDeviceInfoDetail.c /I"path\to\ftd2xx\headers"
 * 
 * Windows (MinGW):
 * gcc -o GetDeviceInfoDetail.exe GetDeviceInfoDetail.c -I"path/to/ftd2xx_labview/headers" -L"path/to/ftd2xx/libs" -lftd2xx
 * 
 * Linux:
 * gcc -o GetDeviceInfoDetail GetDeviceInfoDetail.c -I"path/to/ftd2xx_labview/headers" -lftd2xx
 * 
 * Note: Make sure ftd2xx_labview.h and the ftd2xx library are properly installed
 * and accessible in your development environment.
 * 
 * Expected output format (example):
 * ===== C D2XX Get Device Info Detail =====
 * 
 * D2XX library loaded OK
 * 
 * Number of devices: 1
 * 
 * Dev: 0
 *  Flags=0x2
 *  Type=0x4
 *  ID=0x4036001
 *  LocId=0x14
 *  SerialNumber=FT1234567
 *  Description=USB Serial Converter
 *  ftHandle=0x0
 */
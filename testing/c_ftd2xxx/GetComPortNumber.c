/*
 * GetComPortNumber.c
 * C equivalent of the Python GetComPortNumber.py example
 * 
 * This program demonstrates how to:
 * 1. Load and use the ftd2xx library
 * 2. Open the first FTDI device found
 * 3. Get the COM port number using FT_GetComPortNumber
 * 4. Close the device
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

int main() {
    FT_HANDLE ftHandle;
    FT_STATUS status;
    LONG comPortNumber;
    
    printf("===== C D2XX Get Com Port =====\n");
    printf("D2XX library loaded OK\n\n");
    
    // Open the first device on the system (device index 0)
    status = FT_Open(0, &ftHandle);
    check_status(status);
    
    // Get the COM port number
    status = FT_GetComPortNumber(ftHandle, &comPortNumber);
    check_status(status);
    
    // Display the result
    if (comPortNumber == -1) {
        printf("No Com Port Assigned\n");
    } else {
        printf("Com Port Number: %ld\n", comPortNumber);
    }
    
    // Close the device
    status = FT_Close(ftHandle);
    check_status(status);
    
    return 0;
}

/*
 * Compilation instructions:
 * 
 * Windows (Visual Studio):
 * cl GetComPortNumber.c /I"path\to\ftd2xx\headers"
 * 
 * Windows (MinGW):
 * gcc -o GetComPortNumber.exe GetComPortNumber.c -I"path/to/ftd2xx_labview/headers" -L"path/to/ftd2xx/libs" -lftd2xx
 * 
 * Linux:
 * gcc -o GetComPortNumber GetComPortNumber.c -I"path/to/ftd2xx_labview/headers" -lftd2xx
 * 
 * Note: Make sure ftd2xx_labview.h and the ftd2xx library are properly installed
 * and accessible in your development environment.
 */
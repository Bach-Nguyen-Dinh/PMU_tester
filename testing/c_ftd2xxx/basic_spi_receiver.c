#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "ftd2xx.h"

#define DATA_COUNT 10000
#define BITS_PER_DATA 160
#define BYTES_PER_DATA 20  // 160 bits = 20 bytes
#define OUTPUT_FILE "SPIBin.txt"

// MPSSE Commands for SPI Mode 2 (CPOL=1, CPHA=0)
#define MPSSE_WRITE_NEG_EDGE    0x01   // Write on negative edge
#define MPSSE_READ_POS_EDGE     0x20   // Read on positive edge (for Mode 2)
#define MPSSE_DO_READ           0x20   // Read data
#define MPSSE_BITMODE           0x02   // Bit mode flag

// Function to convert byte array to binary string
void bytes_to_binary_string(unsigned char* bytes, int num_bytes, char* binary_str) {
    int bit_index = 0;
    for (int i = 0; i < num_bytes; i++) {
        for (int j = 7; j >= 0; j--) {
            binary_str[bit_index++] = ((bytes[i] >> j) & 1) ? '1' : '0';
        }
    }
    binary_str[bit_index] = '\0';
}

// Send command and wait for execution
FT_STATUS send_command(FT_HANDLE ftHandle, unsigned char* buffer, int length) {
    DWORD bytesWritten;
    FT_STATUS ftStatus = FT_Write(ftHandle, buffer, length, &bytesWritten);
    if (ftStatus != FT_OK || bytesWritten != length) {
        return FT_OTHER_ERROR;
    }
    return FT_OK;
}

// Initialize MPSSE mode for SPI
FT_STATUS initialize_mpsse(FT_HANDLE ftHandle) {
    FT_STATUS ftStatus;
    DWORD bytesWritten, bytesRead;
    unsigned char buffer[16];
    
    // Reset MPSSE
    ftStatus = FT_SetBitMode(ftHandle, 0x00, 0x00);
    if (ftStatus != FT_OK) return ftStatus;
    
    // Enable MPSSE mode
    ftStatus = FT_SetBitMode(ftHandle, 0x00, 0x02);
    if (ftStatus != FT_OK) return ftStatus;
    
    Sleep(50); // Allow time for MPSSE to initialize
    
    // Synchronize MPSSE by sending bad commands
    buffer[0] = 0xAA;  // Bad command
    ftStatus = FT_Write(ftHandle, buffer, 1, &bytesWritten);
    if (ftStatus != FT_OK) return ftStatus;
    
    buffer[0] = 0xAB;  // Bad command  
    ftStatus = FT_Write(ftHandle, buffer, 1, &bytesWritten);
    if (ftStatus != FT_OK) return ftStatus;
    
    // Read and discard synchronization response
    Sleep(20);
    DWORD bytesAvailable;
    FT_GetQueueStatus(ftHandle, &bytesAvailable);
    if (bytesAvailable > 0) {
        unsigned char dummy[64];
        FT_Read(ftHandle, dummy, bytesAvailable > 64 ? 64 : bytesAvailable, &bytesRead);
    }
    
    // Configure clock divisor for 6MHz
    // Clock = 60MHz / ((1 + divisor) * 2)
    // For 6MHz: divisor = 4 (60MHz / ((1+4)*2) = 6MHz)
    buffer[0] = 0x86;  // Set clock divisor command
    buffer[1] = 0x04;  // Divisor low byte (4)
    buffer[2] = 0x00;  // Divisor high byte
    ftStatus = send_command(ftHandle, buffer, 3);
    if (ftStatus != FT_OK) return ftStatus;
    
    // Configure I/O pins for SPI Mode 2 with active-high CS
    // ADBUS0 = SCLK (output, idle HIGH for CPOL=1), ADBUS1 = MOSI (output), 
    // ADBUS2 = MISO (input), ADBUS3 = CS (output, start HIGH for active-high)
    buffer[0] = 0x80;  // Set low byte pins command
    buffer[1] = 0x09;  // Value: SCLK=HIGH(bit0=1), CS=HIGH(bit3=1) = 0x09
    buffer[2] = 0x0B;  // Direction: SCLK(0), MOSI(1), CS(3) as outputs = 0x0B
    ftStatus = send_command(ftHandle, buffer, 3);
    
    return ftStatus;
}

// Perform SPI read transaction
FT_STATUS spi_read_data(FT_HANDLE ftHandle, unsigned char* data, int length) {
    FT_STATUS ftStatus;
    DWORD bytesWritten, bytesRead;
    unsigned char buffer[32];
    int bufferIndex = 0;
    
    // Assert CS (set LOW for active-high operation)
    buffer[bufferIndex++] = 0x80;  // Set low byte pins
    buffer[bufferIndex++] = 0x01;  // CS=LOW(bit3=0), SCLK=HIGH(bit0=1) = 0x01
    buffer[bufferIndex++] = 0x0B;  // Direction
    
    // SPI Mode 2: CPOL=1, CPHA=0 - Read on falling edge, clock idles high
    buffer[bufferIndex++] = 0x20;  // Read bytes on positive clock edge
    buffer[bufferIndex++] = (length - 1) & 0xFF;        // Length low byte
    buffer[bufferIndex++] = ((length - 1) >> 8) & 0xFF; // Length high byte
    
    // Deassert CS (set HIGH)
    buffer[bufferIndex++] = 0x80;  // Set low byte pins
    buffer[bufferIndex++] = 0x09;  // CS=HIGH(bit3=1), SCLK=HIGH(bit0=1) = 0x09
    buffer[bufferIndex++] = 0x0B;  // Direction
    
    // Send immediate command to execute
    buffer[bufferIndex++] = 0x87;
    
    // Send command sequence
    ftStatus = FT_Write(ftHandle, buffer, bufferIndex, &bytesWritten);
    if (ftStatus != FT_OK) return ftStatus;
    
    // Wait for data and read response
    Sleep(2); // Small delay for data to be available
    ftStatus = FT_Read(ftHandle, data, length, &bytesRead);
    if (ftStatus != FT_OK) return ftStatus;
    
    if (bytesRead != length) {
        printf("Warning: Expected %d bytes, got %d bytes\n", length, bytesRead);
        return FT_OTHER_ERROR;
    }
    
    return FT_OK;
}

int main() {
    FT_HANDLE ftHandle;
    FT_STATUS ftStatus;
    unsigned char spi_data[BYTES_PER_DATA];
    char binary_string[BITS_PER_DATA + 1];
    FILE* output_file;
    int i;
    DWORD numDevices;
    
    printf("FT232H SPI Master Receiver Program (Direct FTD2XX)\n");
    printf("==================================================\n");
    printf("Receiving %d packets of %d bits each\n", DATA_COUNT, BITS_PER_DATA);
    printf("SPI Mode: 2, CS Active High, Clock Rate: 6 MHz\n\n");
    
    // Check for FTDI devices
    ftStatus = FT_CreateDeviceInfoList(&numDevices);
    if (ftStatus != FT_OK || numDevices == 0) {
        printf("Error: No FTDI devices found (error code: %d)\n", ftStatus);
        return -1;
    }
    
    printf("Found %d FTDI device(s)\n", numDevices);
    
    // Open the FT232H device (not the USB-Blaster)
    // Look for FT232H specifically
    int ft232h_index = -1;
    for (DWORD i = 0; i < numDevices; i++) {
        char description[256];
        FT_STATUS status = FT_GetDeviceInfoDetail(i, NULL, NULL, NULL, NULL, NULL, description, NULL);
        if (status == FT_OK && strstr(description, "FT232H") != NULL) {
            ft232h_index = i;
            printf("Found FT232H at device index %d\n", i);
            break;
        }
    }
    
    if (ft232h_index == -1) {
        printf("Error: FT232H device not found\n");
        return -1;
    }
    
    ftStatus = FT_Open(ft232h_index, &ftHandle);
    if (ftStatus != FT_OK) {
        printf("Error: Failed to open FTDI device (error code: %d)\n", ftStatus);
        return -1;
    }
    
    printf("FTDI device opened successfully\n");
    
    // Configure device timeouts and latency
    ftStatus = FT_SetLatencyTimer(ftHandle, 1);  // 1ms latency
    if (ftStatus != FT_OK) {
        printf("Error: Failed to set latency timer\n");
        FT_Close(ftHandle);
        return -1;
    }
    
    ftStatus = FT_SetTimeouts(ftHandle, 5000, 5000);  // 5 second timeouts
    if (ftStatus != FT_OK) {
        printf("Error: Failed to set timeouts\n");
        FT_Close(ftHandle);
        return -1;
    }
    
    // Purge buffers
    FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX);
    
    // Initialize MPSSE for SPI
    ftStatus = initialize_mpsse(ftHandle);
    if (ftStatus != FT_OK) {
        printf("Error: Failed to initialize MPSSE (error code: %d)\n", ftStatus);
        FT_Close(ftHandle);
        return -1;
    }
    
    printf("MPSSE initialized for SPI Mode 2\n");
    
    // Open output file
    output_file = fopen(OUTPUT_FILE, "w");
    if (!output_file) {
        printf("Error: Cannot create output file %s\n", OUTPUT_FILE);
        FT_Close(ftHandle);
        return -1;
    }
    
    printf("Output file opened: %s\n", OUTPUT_FILE);
    printf("Starting data reception...\n\n");
    
    // Main data reception loop
    for (i = 0; i < DATA_COUNT; i++) {
        // Clear the data buffer
        memset(spi_data, 0, BYTES_PER_DATA);
        
        // Perform SPI read operation
        ftStatus = spi_read_data(ftHandle, spi_data, BYTES_PER_DATA);
        
        if (ftStatus != FT_OK) {
            printf("Error: SPI read failed (packet %d, error code: %d)\n", i + 1, ftStatus);
            break;
        }
        
        // Convert bytes to binary string
        bytes_to_binary_string(spi_data, BYTES_PER_DATA, binary_string);
        
        // Write to file
        fprintf(output_file, "%s", binary_string);
        if (i < DATA_COUNT - 1) {
            fprintf(output_file, "\n");  // Space separator except for last entry
        }
        
        // Progress indicator
        if ((i + 1) % 1000 == 0) {
            printf("Received %d/%d packets (%.1f%%)\n", i + 1, DATA_COUNT, 
                   ((float)(i + 1) / DATA_COUNT) * 100.0);
        }
        
        // Small delay to match slave transmission rate (10kHz = 100µs period)
        Sleep(1);  // 1ms delay
    }
    
    printf("\nData reception completed!\n");
    printf("Total packets received: %d\n", i);
    
    // Cleanup
    fclose(output_file);
    FT_Close(ftHandle);
    
    printf("Program finished successfully. Data saved to %s\n", OUTPUT_FILE);
    
    return 0;
}
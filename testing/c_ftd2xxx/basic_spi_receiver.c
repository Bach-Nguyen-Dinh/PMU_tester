#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "ftd2xx.h"

#define DATA_COUNT 10000
#define BITS_PER_DATA 160
#define BYTES_PER_DATA 20  // 160 bits = 20 bytes
#define OUTPUT_FILE "SPIBin.txt"

// Optimize buffer sizes for high-speed operation
#define USB_BUFFER_SIZE 65536  // 64KB USB buffer
#define BATCH_SIZE 100         // Read multiple packets in one operation

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

// Send command without error checking for speed
FT_STATUS send_command_fast(FT_HANDLE ftHandle, unsigned char* buffer, int length) {
    DWORD bytesWritten;
    return FT_Write(ftHandle, buffer, length, &bytesWritten);
}

// Initialize MPSSE mode for SPI - optimized version
FT_STATUS initialize_mpsse_fast(FT_HANDLE ftHandle) {
    FT_STATUS ftStatus;
    DWORD bytesWritten, bytesRead;
    unsigned char buffer[16];
    
    // Reset and enable MPSSE mode
    FT_SetBitMode(ftHandle, 0x00, 0x00);
    FT_SetBitMode(ftHandle, 0x00, 0x02);
    Sleep(50);
    
    // Synchronize MPSSE
    buffer[0] = 0xAA;
    FT_Write(ftHandle, buffer, 1, &bytesWritten);
    buffer[0] = 0xAB;
    FT_Write(ftHandle, buffer, 1, &bytesWritten);
    
    Sleep(10);
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
    ftStatus = send_command_fast(ftHandle, buffer, 3);
    if (ftStatus != FT_OK) return ftStatus;
    
    // Configure I/O pins for SPI Mode 2 with active-high CS
    // ADBUS0 = SCLK (output, idle HIGH for CPOL=1), ADBUS1 = MOSI (output), 
    // ADBUS2 = MISO (input), ADBUS3 = CS (output, start HIGH for active-high)
    buffer[0] = 0x80;  // Set low byte pins command
    buffer[1] = 0x09;  // Value: SCLK=HIGH(bit0=1), CS=HIGH(bit3=1) = 0x09
    buffer[2] = 0x0B;  // Direction: SCLK(0), MOSI(1), CS(3) as outputs = 0x0B
    ftStatus = send_command_fast(ftHandle, buffer, 3);
    
    return FT_OK;
}

// High-speed batch SPI read operation
FT_STATUS spi_read_batch(FT_HANDLE ftHandle, unsigned char* data, int num_packets) {
    FT_STATUS ftStatus;
    DWORD bytesWritten, bytesRead;
    unsigned char commandBuffer[1024];  // Large command buffer
    int cmdIndex = 0;
    int totalBytes = num_packets * BYTES_PER_DATA;
    
    // Build batch command for multiple packets
    for (int i = 0; i < num_packets; i++) {
        // Assert CS (active high - set CS LOW to activate)
        commandBuffer[cmdIndex++] = 0x80;  // Set pins
        commandBuffer[cmdIndex++] = 0x01;  // CS=LOW, SCLK=HIGH
        commandBuffer[cmdIndex++] = 0x0B;  // Direction
        
        // Read command for SPI Mode 2 (falling edge)
        commandBuffer[cmdIndex++] = 0x24;  // Read on falling edge
        commandBuffer[cmdIndex++] = (BYTES_PER_DATA - 1) & 0xFF;
        commandBuffer[cmdIndex++] = ((BYTES_PER_DATA - 1) >> 8) & 0xFF;
        
        // Deassert CS (set CS HIGH to deactivate)
        commandBuffer[cmdIndex++] = 0x80;  // Set pins
        commandBuffer[cmdIndex++] = 0x09;  // CS=HIGH, SCLK=HIGH  
        commandBuffer[cmdIndex++] = 0x0B;  // Direction
    }
    
    // Send immediate command
    commandBuffer[cmdIndex++] = 0x87;
    
    // Send all commands at once
    ftStatus = FT_Write(ftHandle, commandBuffer, cmdIndex, &bytesWritten);
    if (ftStatus != FT_OK) return ftStatus;
    
    // Read all data at once
    ftStatus = FT_Read(ftHandle, data, totalBytes, &bytesRead);
    if (ftStatus != FT_OK) return ftStatus;
    
    if (bytesRead != totalBytes) {
        printf("Warning: Expected %d bytes, got %d bytes\n", totalBytes, bytesRead);
        return FT_OTHER_ERROR;
    }
    
    return FT_OK;
}

int main() {
    FT_HANDLE ftHandle;
    FT_STATUS ftStatus;
    FILE* output_file;
    DWORD numDevices;
    
    // Allocate large buffer for all data in memory
    unsigned char* all_data = malloc(DATA_COUNT * BYTES_PER_DATA);
    char* all_binary_strings = malloc(DATA_COUNT * (BITS_PER_DATA + 2)); // +2 for newline and null
    
    if (!all_data || !all_binary_strings) {
        printf("Error: Failed to allocate memory\n");
        return -1;
    }

    printf("SPI Mode: 2, CS Active High, Clock Rate: 6 MHz\n");
    
    // Find FTDI devices
    ftStatus = FT_CreateDeviceInfoList(&numDevices);
    if (ftStatus != FT_OK || numDevices == 0) {
        printf("Error: No FTDI devices found\n");
        free(all_data);
        free(all_binary_strings);
        return -1;
    }
    
    // Find FT232H device
    int ft232h_index = -1;
    for (DWORD i = 0; i < numDevices; i++) {
        char description[256];
        if (FT_GetDeviceInfoDetail(i, NULL, NULL, NULL, NULL, NULL, description, NULL) == FT_OK) {
            if (strstr(description, "FT232H") != NULL) {
                ft232h_index = i;
                printf("Found FT232H at device index %d\n", i);
                break;
            }
        }
    }
    
    if (ft232h_index == -1) {
        printf("Error: FT232H device not found\n");
        free(all_data);
        free(all_binary_strings);
        return -1;
    }
    
    // Open device
    ftStatus = FT_Open(ft232h_index, &ftHandle);
    if (ftStatus != FT_OK) {
        printf("Error: Failed to open FT232H device\n");
        free(all_data);
        free(all_binary_strings);
        return -1;
    }
    
    printf("FT232H opened successfully\n");
    
    // Optimize device settings for high speed
    FT_SetUSBParameters(ftHandle, USB_BUFFER_SIZE, USB_BUFFER_SIZE);  // Large USB buffers
    FT_SetLatencyTimer(ftHandle, 1);        // Minimum latency (1ms)
    FT_SetTimeouts(ftHandle, 1000, 1000);   // Reasonable timeouts
    FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX);
    
    // Initialize MPSSE
    ftStatus = initialize_mpsse_fast(ftHandle);
    if (ftStatus != FT_OK) {
        printf("Error: Failed to initialize MPSSE\n");
        FT_Close(ftHandle);
        free(all_data);
        free(all_binary_strings);
        return -1;
    }
    
    printf("MPSSE initialized for high-speed operation\n");
    printf("Starting high-speed data reception...\n\n");
    
    // Record start time
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    
    int packets_received = 0;
    int batch_size = BATCH_SIZE;
    
    // Main high-speed reception loop
    while (packets_received < DATA_COUNT) {
        // Adjust batch size for remaining packets
        if (packets_received + batch_size > DATA_COUNT) {
            batch_size = DATA_COUNT - packets_received;
        }
        
        // Read batch of packets
        ftStatus = spi_read_batch(ftHandle, 
                                 all_data + (packets_received * BYTES_PER_DATA), 
                                 batch_size);
        
        if (ftStatus != FT_OK) {
            printf("Error: Batch read failed at packet %d\n", packets_received + 1);
            break;
        }
        
        packets_received += batch_size;
        
        // Progress indicator (less frequent to avoid slowing down)
        if (packets_received % 1000 == 0) {
            printf("Received %d/%d packets (%.1f%%)\n", 
                   packets_received, DATA_COUNT, 
                   ((float)packets_received / DATA_COUNT) * 100.0);
        }
        
        // No sleep - go as fast as possible!
    }
    
    // Record end time
    QueryPerformanceCounter(&end);
    double elapsed = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    
    printf("\nData reception completed!\n");
    printf("Total packets received: %d\n", packets_received);
    printf("Reception time: %.3f seconds\n", elapsed);
    printf("Effective rate: %.1f packets/second\n", packets_received / elapsed);
    
    // Now convert all data to binary strings in memory
    printf("Converting data to binary format...\n");
    
    QueryPerformanceCounter(&start);
    
    char* string_ptr = all_binary_strings;
    for (int i = 0; i < packets_received; i++) {
        bytes_to_binary_string(all_data + (i * BYTES_PER_DATA), BYTES_PER_DATA, string_ptr);
        string_ptr += BITS_PER_DATA;
        *string_ptr++ = '\n';  // Add newline
    }
    *(string_ptr - 1) = '\0';  // Replace last newline with null terminator
    
    QueryPerformanceCounter(&end);
    double conversion_time = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("Conversion time: %.3f seconds\n", conversion_time);
    
    // Write all data to file at once
    printf("Writing data to file...\n");
    
    QueryPerformanceCounter(&start);
    
    output_file = fopen(OUTPUT_FILE, "w");
    if (!output_file) {
        printf("Error: Cannot create output file\n");
        FT_Close(ftHandle);
        free(all_data);
        free(all_binary_strings);
        return -1;
    }
    
    fwrite(all_binary_strings, 1, strlen(all_binary_strings), output_file);
    fclose(output_file);
    
    QueryPerformanceCounter(&end);
    double write_time = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("File write time: %.3f seconds\n", write_time);
    
    // Cleanup
    FT_Close(ftHandle);
    free(all_data);
    free(all_binary_strings);
    
    printf("\n=== Performance Summary ===\n");
    printf("Total time: %.3f seconds\n", elapsed + conversion_time + write_time);
    printf("Reception: %.3f seconds (%.1f%%)\n", elapsed, (elapsed / (elapsed + conversion_time + write_time)) * 100);
    printf("Conversion: %.3f seconds (%.1f%%)\n", conversion_time, (conversion_time / (elapsed + conversion_time + write_time)) * 100);
    printf("File write: %.3f seconds (%.1f%%)\n", write_time, (write_time / (elapsed + conversion_time + write_time)) * 100);
    printf("Data saved to %s\n", OUTPUT_FILE);
    
    return 0;
}
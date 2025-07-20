/*
 * High-Performance FT232H SPI Reader
 * Optimized for maximum throughput using FTDI D2XX library
 * 
 * Compile with: gcc -o ft232h_spi_reader ft232h_spi_reader.c ftd2xx.dll (.dll file must be in the same directory)
 * Windows: Link with ftd2xx.lib
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#ifdef _WIN32
    #include <windows.h>
    #include "ftd2xx.h"
    #define SLEEP_MS(ms) Sleep(ms)
    #define GET_TIME() GetTickCount()
#else
    #include <unistd.h>
    #include <ftd2xx.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
    #define GET_TIME() (clock() * 1000 / CLOCKS_PER_SEC)
#endif

// MPSSE Commands for SPI
#define MSB_RISING_EDGE_CLOCK_BYTE_OUT  0x10
#define MSB_FALLING_EDGE_CLOCK_BYTE_OUT 0x11
#define MSB_RISING_EDGE_CLOCK_BIT_OUT   0x12
#define MSB_FALLING_EDGE_CLOCK_BIT_OUT  0x13
#define MSB_RISING_EDGE_CLOCK_BYTE_IN   0x20
#define MSB_RISING_EDGE_CLOCK_BIT_IN    0x22
#define MSB_FALLING_EDGE_CLOCK_BYTE_IN  0x24
#define MSB_FALLING_EDGE_CLOCK_BIT_IN   0x26

// Configuration
#define CLOCK_DIVISOR           4       // For 6MHz: 60/((1+4)*2) = 6MHz
#define BYTES_PER_SAMPLE        20      // 160 bits = 20 bytes
#define USB_BUFFER_SIZE         65536   // Maximum USB buffer size
#define CMD_BUFFER_SIZE         32768   // Command buffer size
#define DATA_BUFFER_SIZE        131072  // Data buffer size

// Calculate optimal batch size based on USB buffer limits
#define BYTES_PER_CMD           3       // Each read command is 3 bytes
#define MAX_BATCH_SIZE          ((USB_BUFFER_SIZE - 1024) / (BYTES_PER_CMD + BYTES_PER_SAMPLE))
#define OPTIMAL_BATCH_SIZE      2000    // Conservative batch size

// Global variables
static FT_HANDLE ftHandle = NULL;
static UCHAR OutputBuffer[CMD_BUFFER_SIZE];
static UCHAR InputBuffer[DATA_BUFFER_SIZE];

// Function prototypes
bool SPI_Initialize(void);
bool SPI_SynchronizeMPSSE(void);
bool SPI_ConfigureSPI(void);
int SPI_ReceiveBatch(int numSamples, UCHAR* dataBuffer, int bufferSize);
void SPI_Close(void);
void PrintBinaryData(const UCHAR* data, int length, FILE* file);
double GetElapsedTime(DWORD startTime);

int main(int argc, char* argv[])
{
    printf("High-Performance FT232H SPI Reader (C Implementation)\n");
    printf("=====================================================\n");
    
    // Parse command line arguments
    int totalSamples = 10000;
    int batchSize = OPTIMAL_BATCH_SIZE;
    
    if (argc > 1) {
        totalSamples = atoi(argv[1]);
        if (totalSamples <= 0) totalSamples = 10000;
    }
    
    if (argc > 2) {
        batchSize = atoi(argv[2]);
        if (batchSize <= 0 || batchSize > MAX_BATCH_SIZE) {
            printf("Warning: Invalid batch size %d, using %d\n", batchSize, OPTIMAL_BATCH_SIZE);
            batchSize = OPTIMAL_BATCH_SIZE;
        }
    }
    
    printf("Configuration:\n");
    printf("  Total samples: %d\n", totalSamples);
    printf("  Batch size: %d\n", batchSize);
    printf("  Bytes per sample: %d\n", BYTES_PER_SAMPLE);
    printf("  SPI clock: 6 MHz\n");
    printf("  Mode: Half-duplex receive only\n\n");
    
    // Initialize SPI interface
    if (!SPI_Initialize()) {
        printf("Failed to initialize SPI interface\n");
        return 1;
    }
    
    printf("SPI interface initialized successfully\n");
    
    // Open output files
    FILE* outputFile = fopen("output.txt", "w");
    FILE* counterFile = fopen("CounterOutput.txt", "w");
    
    if (!outputFile || !counterFile) {
        printf("Failed to open output files\n");
        SPI_Close();
        return 1;
    }
    
    // Write headers
    fprintf(outputFile, "Binary Data (160 bits per sample)\n");
    fprintf(counterFile, "Counter Data (bits 124-147)\n");
    
    // Allocate data buffer
    UCHAR* dataBuffer = (UCHAR*)malloc(batchSize * BYTES_PER_SAMPLE);
    if (!dataBuffer) {
        printf("Failed to allocate data buffer\n");
        fclose(outputFile);
        fclose(counterFile);
        SPI_Close();
        return 1;
    }
    
    printf("Starting high-speed data collection...\n");
    printf("(Press Ctrl+C to stop)\n\n");
    
    // Performance tracking
    DWORD startTime = GET_TIME();
    int totalSamplesCollected = 0;
    int batchCount = 0;
    
    // Main data collection loop
    while (totalSamplesCollected < totalSamples) {
        // Calculate samples for this batch
        int samplesThisBatch = (totalSamples - totalSamplesCollected);
        if (samplesThisBatch > batchSize) {
            samplesThisBatch = batchSize;
        }
        
        // Perform batch read
        DWORD batchStartTime = GET_TIME();
        int samplesReceived = SPI_ReceiveBatch(samplesThisBatch, dataBuffer, 
                                             samplesThisBatch * BYTES_PER_SAMPLE);
        DWORD batchTime = GET_TIME() - batchStartTime;
        
        if (samplesReceived <= 0) {
            printf("Error: Failed to receive data in batch %d\n", batchCount + 1);
            break;
        }
        
        // Process and write data
        for (int i = 0; i < samplesReceived; i++) {
            const UCHAR* sampleData = &dataBuffer[i * BYTES_PER_SAMPLE];
            
            // Write full binary data to output file
            PrintBinaryData(sampleData, BYTES_PER_SAMPLE, outputFile);
            
            // Extract bits 124-147 (24 bits) for counter output
            // Bit 124 is in byte 15, bit 4 (124 = 15*8 + 4)
            // Bit 147 is in byte 18, bit 3 (147 = 18*8 + 3)
            if (BYTES_PER_SAMPLE >= 19) { // Make sure we have enough bytes
                // Extract 24 bits starting from bit 124
                UCHAR counterBytes[3] = {0};
                
                // Extract bits carefully - this is byte/bit manipulation
                int startBit = 124;
                for (int bit = 0; bit < 24; bit++) {
                    int globalBit = startBit + bit;
                    int byteIndex = globalBit / 8;
                    int bitIndex = globalBit % 8;
                    
                    if (byteIndex < BYTES_PER_SAMPLE) {
                        int counterByteIndex = bit / 8;
                        int counterBitIndex = bit % 8;
                        
                        if (sampleData[byteIndex] & (1 << bitIndex)) {
                            counterBytes[counterByteIndex] |= (1 << counterBitIndex);
                        }
                    }
                }
                
                // Write 24-bit counter data
                PrintBinaryData(counterBytes, 3, counterFile);
            }
        }
        
        totalSamplesCollected += samplesReceived;
        batchCount++;
        
        // Progress reporting
        double elapsed = GetElapsedTime(startTime);
        double samplesPerSec = totalSamplesCollected / elapsed;
        
        printf("Batch %d: %d samples, %lu ms, Progress: %d/%d (%.1f%%), Speed: %.0f smp/s\n",
               batchCount, samplesReceived, batchTime,
               totalSamplesCollected, totalSamples,
               (double)totalSamplesCollected / totalSamples * 100.0,
               samplesPerSec);
        
        // Small delay to prevent overwhelming the system
        if (batchCount % 10 == 0) {
            SLEEP_MS(1);
        }
    }
    
    // Calculate final performance
    double totalTime = GetElapsedTime(startTime);
    double avgSamplesPerSec = totalSamplesCollected / totalTime;
    double dataRateMBps = (totalSamplesCollected * BYTES_PER_SAMPLE) / (totalTime * 1024 * 1024);
    
    printf("\n=== PERFORMANCE RESULTS ===\n");
    printf("Total samples collected: %d\n", totalSamplesCollected);
    printf("Total time: %.3f seconds\n", totalTime);
    printf("Average speed: %.0f samples/second\n", avgSamplesPerSec);
    printf("Data rate: %.2f MB/s\n", dataRateMBps);
    printf("Total batches: %d\n", batchCount);
    printf("USB transactions: %d\n", batchCount);
    printf("\nData written to output.txt and CounterOutput.txt\n");
    
    // Cleanup
    free(dataBuffer);
    fclose(outputFile);
    fclose(counterFile);
    SPI_Close();
    
    return 0;
}

bool SPI_Initialize(void)
{
    FT_STATUS ftStatus;
    DWORD numDevs;
    
    // Check for FTDI devices
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (ftStatus != FT_OK || numDevs == 0) {
        printf("Error: No FTDI devices found\n");
        return false;
    }
    
    printf("Found %lu FTDI device(s)\n", numDevs);
    
    // Open first device
    ftStatus = FT_Open(0, &ftHandle);
    if (ftStatus != FT_OK) {
        printf("Error: Failed to open FTDI device\n");
        return false;
    }
    
    // Reset device
    ftStatus = FT_ResetDevice(ftHandle);
    if (ftStatus != FT_OK) {
        printf("Error: Failed to reset device\n");
        return false;
    }
    
    // Purge buffers
    DWORD bytesInQueue;
    ftStatus = FT_GetQueueStatus(ftHandle, &bytesInQueue);
    if (ftStatus == FT_OK && bytesInQueue > 0) {
        DWORD bytesRead;
        FT_Read(ftHandle, InputBuffer, bytesInQueue, &bytesRead);
    }
    
    // Set USB parameters for high performance
    ftStatus = FT_SetUSBParameters(ftHandle, USB_BUFFER_SIZE, USB_BUFFER_SIZE);
    if (ftStatus != FT_OK) {
        printf("Warning: Failed to set USB parameters\n");
    }
    
    // Disable event and error characters
    ftStatus = FT_SetChars(ftHandle, 0, false, 0, false);
    if (ftStatus != FT_OK) {
        printf("Warning: Failed to set characters\n");
    }
    
    // Set timeouts (5 seconds for large batches)
    ftStatus = FT_SetTimeouts(ftHandle, 5000, 5000);
    if (ftStatus != FT_OK) {
        printf("Warning: Failed to set timeouts\n");
    }
    
    // Set latency timer to minimum
    ftStatus = FT_SetLatencyTimer(ftHandle, 2);
    if (ftStatus != FT_OK) {
        printf("Warning: Failed to set latency timer\n");
    }
    
    // Reset controller
    ftStatus = FT_SetBitMode(ftHandle, 0x00, 0x00);
    if (ftStatus != FT_OK) {
        printf("Error: Failed to reset bit mode\n");
        return false;
    }
    
    // Enable MPSSE mode
    ftStatus = FT_SetBitMode(ftHandle, 0x00, 0x02);
    if (ftStatus != FT_OK) {
        printf("Error: Failed to enable MPSSE mode\n");
        return false;
    }
    
    // Wait for MPSSE to initialize
    SLEEP_MS(50);
    
    // Synchronize MPSSE interface
    if (!SPI_SynchronizeMPSSE()) {
        printf("Error: Failed to synchronize MPSSE\n");
        return false;
    }
    
    // Configure SPI interface
    if (!SPI_ConfigureSPI()) {
        printf("Error: Failed to configure SPI\n");
        return false;
    }
    
    return true;
}

bool SPI_SynchronizeMPSSE(void)
{
    FT_STATUS ftStatus;
    DWORD bytesWritten, bytesRead, bytesInQueue;
    
    // Send bad command 0xAA
    OutputBuffer[0] = 0xAA;
    ftStatus = FT_Write(ftHandle, OutputBuffer, 1, &bytesWritten);
    if (ftStatus != FT_OK) return false;
    
    SLEEP_MS(10);
    
    // Read response
    ftStatus = FT_GetQueueStatus(ftHandle, &bytesInQueue);
    if (ftStatus == FT_OK && bytesInQueue > 0) {
        FT_Read(ftHandle, InputBuffer, bytesInQueue, &bytesRead);
    }
    
    // Send bad command 0xAB
    OutputBuffer[0] = 0xAB;
    ftStatus = FT_Write(ftHandle, OutputBuffer, 1, &bytesWritten);
    if (ftStatus != FT_OK) return false;
    
    SLEEP_MS(10);
    
    // Read response
    ftStatus = FT_GetQueueStatus(ftHandle, &bytesInQueue);
    if (ftStatus == FT_OK && bytesInQueue > 0) {
        FT_Read(ftHandle, InputBuffer, bytesInQueue, &bytesRead);
    }
    
    return true;
}

bool SPI_ConfigureSPI(void)
{
    FT_STATUS ftStatus;
    DWORD bytesWritten;
    int bufferIndex = 0;
    
    // Disable clock divide by 5
    OutputBuffer[bufferIndex++] = 0x8A;
    
    // Turn off adaptive clocking
    OutputBuffer[bufferIndex++] = 0x97;
    
    // Disable 3-phase data clock
    OutputBuffer[bufferIndex++] = 0x8D;
    
    ftStatus = FT_Write(ftHandle, OutputBuffer, bufferIndex, &bytesWritten);
    if (ftStatus != FT_OK) return false;
    
    bufferIndex = 0;
    
    // Set clock divisor for 6MHz
    OutputBuffer[bufferIndex++] = 0x86; // Set clock divisor command
    OutputBuffer[bufferIndex++] = CLOCK_DIVISOR & 0xFF;        // Low byte
    OutputBuffer[bufferIndex++] = (CLOCK_DIVISOR >> 8) & 0xFF; // High byte
    
    // Configure GPIO pins for SPI (no CS toggling for continuous operation)
    OutputBuffer[bufferIndex++] = 0x80; // Set data bits low byte
    OutputBuffer[bufferIndex++] = 0x00; // Value: CS low for continuous operation
    OutputBuffer[bufferIndex++] = 0x0B; // Direction: MOSI(0), SCK(1) = output, MISO(2) = input, CS(3) = output
    
    // Set high byte pins
    OutputBuffer[bufferIndex++] = 0x82; // Set data bits high byte
    OutputBuffer[bufferIndex++] = 0x00; // Value
    OutputBuffer[bufferIndex++] = 0x00; // Direction
    
    ftStatus = FT_Write(ftHandle, OutputBuffer, bufferIndex, &bytesWritten);
    if (ftStatus != FT_OK) return false;
    
    SLEEP_MS(20);
    
    // Turn off loopback
    bufferIndex = 0;
    OutputBuffer[bufferIndex++] = 0x85;
    ftStatus = FT_Write(ftHandle, OutputBuffer, bufferIndex, &bytesWritten);
    if (ftStatus != FT_OK) return false;
    
    SLEEP_MS(30);
    
    return true;
}

int SPI_ReceiveBatch(int numSamples, UCHAR* dataBuffer, int bufferSize)
{
    FT_STATUS ftStatus;
    DWORD bytesWritten, bytesRead, bytesInQueue;
    int bufferIndex = 0;
    int expectedBytes = numSamples * BYTES_PER_SAMPLE;
    
    // Build command buffer for batch read
    for (int i = 0; i < numSamples; i++) {
        if (bufferIndex + 3 >= CMD_BUFFER_SIZE) {
            printf("Error: Command buffer overflow\n");
            return -1;
        }
        
        OutputBuffer[bufferIndex++] = MSB_RISING_EDGE_CLOCK_BYTE_IN;
        OutputBuffer[bufferIndex++] = (BYTES_PER_SAMPLE - 1) & 0xFF;        // Low byte of length
        OutputBuffer[bufferIndex++] = ((BYTES_PER_SAMPLE - 1) >> 8) & 0xFF; // High byte of length
    }
    
    // Send all commands at once
    ftStatus = FT_Write(ftHandle, OutputBuffer, bufferIndex, &bytesWritten);
    if (ftStatus != FT_OK) {
        printf("Error: Failed to write commands\n");
        return -1;
    }
    
    // Wait for data with adaptive timing
    int baseWait = 5; // 5ms base wait
    int scaledWait = numSamples / 100; // Scale with batch size
    SLEEP_MS(baseWait + scaledWait);
    
    // Read data with retry mechanism
    int totalBytesRead = 0;
    int retries = 0;
    const int maxRetries = 10;
    
    while (totalBytesRead < expectedBytes && retries < maxRetries) {
        ftStatus = FT_GetQueueStatus(ftHandle, &bytesInQueue);
        if (ftStatus != FT_OK) {
            printf("Error: Failed to get queue status\n");
            return -1;
        }
        
        if (bytesInQueue > 0) {
            DWORD bytesToRead = bytesInQueue;
            if (totalBytesRead + bytesToRead > bufferSize) {
                bytesToRead = bufferSize - totalBytesRead;
            }
            
            ftStatus = FT_Read(ftHandle, dataBuffer + totalBytesRead, bytesToRead, &bytesRead);
            if (ftStatus != FT_OK) {
                printf("Error: Failed to read data\n");
                return -1;
            }
            
            totalBytesRead += bytesRead;
        } else {
            // No data available, wait a bit and retry
            SLEEP_MS(1);
            retries++;
        }
    }
    
    // Calculate number of complete samples received
    int samplesReceived = totalBytesRead / BYTES_PER_SAMPLE;
    
    if (samplesReceived < numSamples) {
        printf("Warning: Expected %d samples (%d bytes), got %d samples (%d bytes)\n",
               numSamples, expectedBytes, samplesReceived, totalBytesRead);
    }
    
    return samplesReceived;
}

void SPI_Close(void)
{
    if (ftHandle != NULL) {
        FT_Close(ftHandle);
        ftHandle = NULL;
    }
}

void PrintBinaryData(const UCHAR* data, int length, FILE* file)
{
    for (int i = 0; i < length; i++) {
        for (int bit = 7; bit >= 0; bit--) {
            fprintf(file, "%d", (data[i] >> bit) & 1);
        }
    }
    fprintf(file, "\n");
}

double GetElapsedTime(DWORD startTime)
{
    DWORD currentTime = GET_TIME();
    return (double)(currentTime - startTime) / 1000.0; // Convert to seconds
}
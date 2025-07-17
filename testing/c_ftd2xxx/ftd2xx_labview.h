/*
 * ftd2xx_complete_labview.h
 * Complete FTDI D2XX header file optimized for LabVIEW
 * Based on original ftd2xx.h with simplified types for LabVIEW compatibility
 */

#ifndef FTD2XX_COMPLETE_LABVIEW_H
#define FTD2XX_COMPLETE_LABVIEW_H

// Basic type definitions for LabVIEW compatibility
typedef void* FT_HANDLE;
typedef unsigned long FT_STATUS;
typedef unsigned long DWORD;
typedef unsigned long* LPDWORD;
typedef void* LPVOID;
typedef unsigned char UCHAR;
typedef unsigned char* PUCHAR;
typedef char* PCHAR;
typedef unsigned short WORD;
typedef unsigned short* LPWORD;
typedef int BOOL;
typedef char CHAR;
typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef unsigned long* PULONG;
typedef void* PVOID;
typedef long LONG;
typedef long* LPLONG;
typedef const void* LPCVOID;
typedef char* LPCTSTR;
typedef void* LPSECURITY_ATTRIBUTES;
typedef void* LPOVERLAPPED;
typedef void* HANDLE;

// FT_DEVICE type
typedef ULONG FT_DEVICE;

// Status codes
#define FT_OK 0
#define FT_INVALID_HANDLE 1
#define FT_DEVICE_NOT_FOUND 2
#define FT_DEVICE_NOT_OPENED 3
#define FT_IO_ERROR 4
#define FT_INSUFFICIENT_RESOURCES 5
#define FT_INVALID_PARAMETER 6
#define FT_INVALID_BAUD_RATE 7
#define FT_DEVICE_NOT_OPENED_FOR_ERASE 8
#define FT_DEVICE_NOT_OPENED_FOR_WRITE 9
#define FT_FAILED_TO_WRITE_DEVICE 10
#define FT_EEPROM_READ_FAILED 11
#define FT_EEPROM_WRITE_FAILED 12
#define FT_EEPROM_ERASE_FAILED 13
#define FT_EEPROM_NOT_PRESENT 14
#define FT_EEPROM_NOT_PROGRAMMED 15
#define FT_INVALID_ARGS 16
#define FT_NOT_SUPPORTED 17
#define FT_OTHER_ERROR 18
#define FT_DEVICE_LIST_NOT_READY 19

// Success macro
#define FT_SUCCESS(status) ((status) == FT_OK)

// FT_OpenEx Flags
#define FT_OPEN_BY_SERIAL_NUMBER 1
#define FT_OPEN_BY_DESCRIPTION 2
#define FT_OPEN_BY_LOCATION 4
#define FT_OPEN_MASK (FT_OPEN_BY_SERIAL_NUMBER | FT_OPEN_BY_DESCRIPTION | FT_OPEN_BY_LOCATION)

// FT_ListDevices Flags
#define FT_LIST_NUMBER_ONLY 0x80000000
#define FT_LIST_BY_INDEX 0x40000000
#define FT_LIST_ALL 0x20000000
#define FT_LIST_MASK (FT_LIST_NUMBER_ONLY|FT_LIST_BY_INDEX|FT_LIST_ALL)

// Baud Rates
#define FT_BAUD_300 300
#define FT_BAUD_600 600
#define FT_BAUD_1200 1200
#define FT_BAUD_2400 2400
#define FT_BAUD_4800 4800
#define FT_BAUD_9600 9600
#define FT_BAUD_14400 14400
#define FT_BAUD_19200 19200
#define FT_BAUD_38400 38400
#define FT_BAUD_57600 57600
#define FT_BAUD_115200 115200
#define FT_BAUD_230400 230400
#define FT_BAUD_460800 460800
#define FT_BAUD_921600 921600

// Word Lengths
#define FT_BITS_8 8
#define FT_BITS_7 7

// Stop Bits
#define FT_STOP_BITS_1 0
#define FT_STOP_BITS_2 2

// Parity
#define FT_PARITY_NONE 0
#define FT_PARITY_ODD 1
#define FT_PARITY_EVEN 2
#define FT_PARITY_MARK 3
#define FT_PARITY_SPACE 4

// Flow Control
#define FT_FLOW_NONE 0x0000
#define FT_FLOW_RTS_CTS 0x0100
#define FT_FLOW_DTR_DSR 0x0200
#define FT_FLOW_XON_XOFF 0x0400

// Purge rx and tx buffers
#define FT_PURGE_RX 1
#define FT_PURGE_TX 2

// Events
#define FT_EVENT_RXCHAR 1
#define FT_EVENT_MODEM_STATUS 2
#define FT_EVENT_LINE_STATUS 4

// Timeouts
#define FT_DEFAULT_RX_TIMEOUT 300
#define FT_DEFAULT_TX_TIMEOUT 300

// Device Types
#define FT_DEVICE_BM 0
#define FT_DEVICE_AM 1
#define FT_DEVICE_100AX 2
#define FT_DEVICE_UNKNOWN 3
#define FT_DEVICE_2232C 4
#define FT_DEVICE_232R 5
#define FT_DEVICE_2232H 6
#define FT_DEVICE_4232H 7
#define FT_DEVICE_232H 8
#define FT_DEVICE_X_SERIES 9
#define FT_DEVICE_4222H_0 10
#define FT_DEVICE_4222H_1_2 11
#define FT_DEVICE_4222H_3 12
#define FT_DEVICE_4222_PROG 13
#define FT_DEVICE_900 14
#define FT_DEVICE_930 15
#define FT_DEVICE_UMFTPD3A 16
#define FT_DEVICE_2233HP 17
#define FT_DEVICE_4233HP 18
#define FT_DEVICE_2232HP 19
#define FT_DEVICE_4232HP 20
#define FT_DEVICE_233HP 21
#define FT_DEVICE_232HP 22
#define FT_DEVICE_2232HA 23
#define FT_DEVICE_4232HA 24
#define FT_DEVICE_232RN 25
#define FT_DEVICE_2233HPN 26
#define FT_DEVICE_4233HPN 27
#define FT_DEVICE_2232HPN 28
#define FT_DEVICE_4232HPN 29
#define FT_DEVICE_233HPN 30
#define FT_DEVICE_232HPN 31
#define FT_DEVICE_BM_A 32

// Bit Modes
#define FT_BITMODE_RESET 0x00
#define FT_BITMODE_ASYNC_BITBANG 0x01
#define FT_BITMODE_MPSSE 0x02
#define FT_BITMODE_SYNC_BITBANG 0x04
#define FT_BITMODE_MCU_HOST 0x08
#define FT_BITMODE_FAST_SERIAL 0x10
#define FT_BITMODE_CBUS_BITBANG 0x20
#define FT_BITMODE_SYNC_FIFO 0x40

// Driver Types
#define FT_DRIVER_TYPE_D2XX 0
#define FT_DRIVER_TYPE_VCP 1

// Device Information Flags
#define FT_FLAGS_OPENED 1
#define FT_FLAGS_HISPEED 2

// Device List Info Node Structure
typedef struct _ft_device_list_info_node {
    ULONG Flags;
    ULONG Type;
    ULONG ID;
    DWORD LocId;
    char SerialNumber[16];
    char Description[64];
    FT_HANDLE ftHandle;
} FT_DEVICE_LIST_INFO_NODE;

// Win32 API compatible structures
typedef struct _FTCOMSTAT {
    DWORD fCtsHold : 1;
    DWORD fDsrHold : 1;
    DWORD fRlsdHold : 1;
    DWORD fXoffHold : 1;
    DWORD fXoffSent : 1;
    DWORD fEof : 1;
    DWORD fTxim : 1;
    DWORD fReserved : 25;
    DWORD cbInQue;
    DWORD cbOutQue;
} FTCOMSTAT, *LPFTCOMSTAT;

typedef struct _FTDCB {
    DWORD DCBlength;
    DWORD BaudRate;
    DWORD fBinary : 1;
    DWORD fParity : 1;
    DWORD fOutxCtsFlow : 1;
    DWORD fOutxDsrFlow : 1;
    DWORD fDtrControl : 2;
    DWORD fDsrSensitivity : 1;
    DWORD fTXContinueOnXoff : 1;
    DWORD fOutX : 1;
    DWORD fInX : 1;
    DWORD fErrorChar : 1;
    DWORD fNull : 1;
    DWORD fRtsControl : 2;
    DWORD fAbortOnError : 1;
    DWORD fDummy2 : 17;
    WORD wReserved;
    WORD XonLim;
    WORD XoffLim;
    UCHAR ByteSize;
    UCHAR Parity;
    UCHAR StopBits;
    char XonChar;
    char XoffChar;
    char ErrorChar;
    char EofChar;
    char EvtChar;
    WORD wReserved1;
} FTDCB, *LPFTDCB;

typedef struct _FTTIMEOUTS {
    DWORD ReadIntervalTimeout;
    DWORD ReadTotalTimeoutMultiplier;
    DWORD ReadTotalTimeoutConstant;
    DWORD WriteTotalTimeoutMultiplier;
    DWORD WriteTotalTimeoutConstant;
} FTTIMEOUTS, *LPFTTIMEOUTS;

// =============================================================================
// FUNCTION DECLARATIONS
// =============================================================================

// D2XX Classic Functions
__declspec(dllimport) FT_STATUS __stdcall FT_CreateDeviceInfoList(LPDWORD lpdwNumDevs);
__declspec(dllimport) FT_STATUS __stdcall FT_GetDeviceInfoList(FT_DEVICE_LIST_INFO_NODE *pDest, LPDWORD lpdwNumDevs);
__declspec(dllimport) FT_STATUS __stdcall FT_GetDeviceInfoDetail(DWORD dwIndex, LPDWORD lpdwFlags, LPDWORD lpdwType, LPDWORD lpdwID, LPDWORD lpdwLocId, LPVOID lpSerialNumber, LPVOID lpDescription, FT_HANDLE *pftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_ListDevices(PVOID pvArg1, PVOID pvArg2, DWORD dwFlags);
__declspec(dllimport) FT_STATUS __stdcall FT_Open(int deviceNumber, FT_HANDLE *pHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_OpenEx(PVOID pvArg1, DWORD dwFlags, FT_HANDLE *pHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_Close(FT_HANDLE ftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_Read(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToRead, LPDWORD lpdwBytesReturned);
__declspec(dllimport) FT_STATUS __stdcall FT_Write(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToWrite, LPDWORD lpdwBytesWritten);
__declspec(dllimport) FT_STATUS __stdcall FT_SetBaudRate(FT_HANDLE ftHandle, ULONG dwBaudRate);
__declspec(dllimport) FT_STATUS __stdcall FT_SetDivisor(FT_HANDLE ftHandle, USHORT usDivisor);
__declspec(dllimport) FT_STATUS __stdcall FT_SetDataCharacteristics(FT_HANDLE ftHandle, UCHAR uWordLength, UCHAR uStopBits, UCHAR uParity);
__declspec(dllimport) FT_STATUS __stdcall FT_SetTimeouts(FT_HANDLE ftHandle, ULONG dwReadTimeout, ULONG dwWriteTimeout);
__declspec(dllimport) FT_STATUS __stdcall FT_SetFlowControl(FT_HANDLE ftHandle, USHORT usFlowControl, UCHAR uXonChar, UCHAR uXoffChar);
__declspec(dllimport) FT_STATUS __stdcall FT_SetDtr(FT_HANDLE ftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_ClrDtr(FT_HANDLE ftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_SetRts(FT_HANDLE ftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_ClrRts(FT_HANDLE ftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_GetModemStatus(FT_HANDLE ftHandle, ULONG *lpdwModemStatus);
__declspec(dllimport) FT_STATUS __stdcall FT_GetQueueStatus(FT_HANDLE ftHandle, DWORD *lpdwAmountInRxQueue);
__declspec(dllimport) FT_STATUS __stdcall FT_GetDeviceInfo(FT_HANDLE ftHandle, FT_DEVICE *lpftDevice, LPDWORD lpdwID, PCHAR pcSerialNumber, PCHAR pcDescription, LPVOID pvDummy);
__declspec(dllimport) FT_STATUS __stdcall FT_GetDriverVersion(FT_HANDLE ftHandle, LPDWORD lpdwDriverVersion);
__declspec(dllimport) FT_STATUS __stdcall FT_GetLibraryVersion(LPDWORD lpdwDLLVersion);
__declspec(dllimport) FT_STATUS __stdcall FT_GetComPortNumber(FT_HANDLE ftHandle, LPLONG lplComPortNumber);
__declspec(dllimport) FT_STATUS __stdcall FT_GetStatus(FT_HANDLE ftHandle, DWORD *lpdwAmountInRxQueue, DWORD *lpdwAmountInTxQueue, DWORD *lpdwEventStatus);
__declspec(dllimport) FT_STATUS __stdcall FT_SetEventNotification(FT_HANDLE ftHandle, DWORD dwEventMask, PVOID pvArg);
__declspec(dllimport) FT_STATUS __stdcall FT_SetChars(FT_HANDLE ftHandle, UCHAR uEventChar, UCHAR uEventCharEnabled, UCHAR uErrorChar, UCHAR uErrorCharEnabled);
__declspec(dllimport) FT_STATUS __stdcall FT_SetBreakOn(FT_HANDLE ftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_SetBreakOff(FT_HANDLE ftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_Purge(FT_HANDLE ftHandle, ULONG ulMask);
__declspec(dllimport) FT_STATUS __stdcall FT_ResetDevice(FT_HANDLE ftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_ResetPort(FT_HANDLE ftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_CyclePort(FT_HANDLE ftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_Rescan(void);
__declspec(dllimport) FT_STATUS __stdcall FT_Reload(WORD wVID, WORD wPID);
__declspec(dllimport) FT_STATUS __stdcall FT_SetResetPipeRetryCount(FT_HANDLE ftHandle, DWORD dwCount);
__declspec(dllimport) FT_STATUS __stdcall FT_StopInTask(FT_HANDLE ftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_RestartInTask(FT_HANDLE ftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_SetDeadmanTimeout(FT_HANDLE ftHandle, ULONG ulDeadmanTimeout);
__declspec(dllimport) FT_STATUS __stdcall FT_IoCtl(FT_HANDLE ftHandle, DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
__declspec(dllimport) FT_STATUS __stdcall FT_SetWaitMask(FT_HANDLE ftHandle, DWORD Mask);
__declspec(dllimport) FT_STATUS __stdcall FT_WaitOnMask(FT_HANDLE ftHandle, DWORD *Mask);
__declspec(dllimport) FT_STATUS __stdcall FT_GetEventStatus(FT_HANDLE ftHandle, DWORD *dwEventDWord);

// Static library functions
__declspec(dllimport) FT_STATUS __stdcall FT_Initialise(void);
__declspec(dllimport) void __stdcall FT_Finalise(void);

// EEPROM Programming Functions
__declspec(dllimport) FT_STATUS __stdcall FT_ReadEE(FT_HANDLE ftHandle, DWORD dwWordOffset, LPWORD lpwValue);
__declspec(dllimport) FT_STATUS __stdcall FT_WriteEE(FT_HANDLE ftHandle, DWORD dwWordOffset, WORD wValue);
__declspec(dllimport) FT_STATUS __stdcall FT_EraseEE(FT_HANDLE ftHandle);

// EEPROM Program Data Structure (simplified for LabVIEW)
typedef struct ft_program_data {
    DWORD Signature1;
    DWORD Signature2;
    DWORD Version;
    WORD VendorId;
    WORD ProductId;
    char *Manufacturer;
    char *ManufacturerId;
    char *Description;
    char *SerialNumber;
    WORD MaxPower;
    WORD PnP;
    WORD SelfPowered;
    WORD RemoteWakeup;
    // Add other fields as needed
} FT_PROGRAM_DATA, *PFT_PROGRAM_DATA;

// EEPROM Header Structure
typedef struct ft_eeprom_header {
    FT_DEVICE deviceType;
    WORD VendorId;
    WORD ProductId;
    UCHAR SerNumEnable;
    WORD MaxPower;
    UCHAR SelfPowered;
    UCHAR RemoteWakeup;
    UCHAR PullDownEnable;
} FT_EEPROM_HEADER, *PFT_EEPROM_HEADER;

// Additional EEPROM functions
__declspec(dllimport) FT_STATUS __stdcall FT_EE_Read(FT_HANDLE ftHandle, PFT_PROGRAM_DATA pData);
__declspec(dllimport) FT_STATUS __stdcall FT_EE_ReadEx(FT_HANDLE ftHandle, PFT_PROGRAM_DATA pData, char *Manufacturer, char *ManufacturerId, char *Description, char *SerialNumber);
__declspec(dllimport) FT_STATUS __stdcall FT_EE_Program(FT_HANDLE ftHandle, PFT_PROGRAM_DATA pData);
__declspec(dllimport) FT_STATUS __stdcall FT_EE_ProgramEx(FT_HANDLE ftHandle, PFT_PROGRAM_DATA pData, char *Manufacturer, char *ManufacturerId, char *Description, char *SerialNumber);
__declspec(dllimport) FT_STATUS __stdcall FT_EE_UASize(FT_HANDLE ftHandle, LPDWORD lpdwSize);
__declspec(dllimport) FT_STATUS __stdcall FT_EE_UARead(FT_HANDLE ftHandle, PUCHAR pucData, DWORD dwDataLen, LPDWORD lpdwBytesRead);
__declspec(dllimport) FT_STATUS __stdcall FT_EE_UAWrite(FT_HANDLE ftHandle, PUCHAR pucData, DWORD dwDataLen);
__declspec(dllimport) FT_STATUS __stdcall FT_EEPROM_Read(FT_HANDLE ftHandle, void *eepromData, DWORD eepromDataSize, char *Manufacturer, char *ManufacturerId, char *Description, char *SerialNumber);
__declspec(dllimport) FT_STATUS __stdcall FT_EEPROM_Program(FT_HANDLE ftHandle, void *eepromData, DWORD eepromDataSize, char *Manufacturer, char *ManufacturerId, char *Description, char *SerialNumber);

// Additional functions
__declspec(dllimport) FT_STATUS __stdcall FT_EE_ReadConfig(FT_HANDLE ftHandle, UCHAR ucAddress, PUCHAR pucValue);
__declspec(dllimport) FT_STATUS __stdcall FT_EE_WriteConfig(FT_HANDLE ftHandle, UCHAR ucAddress, UCHAR ucValue);
__declspec(dllimport) FT_STATUS __stdcall FT_EE_ReadECC(FT_HANDLE ftHandle, UCHAR ucOption, LPWORD lpwValue);
__declspec(dllimport) FT_STATUS __stdcall FT_GetQueueStatusEx(FT_HANDLE ftHandle, DWORD *dwRxBytes);
__declspec(dllimport) FT_STATUS __stdcall FT_ComPortIdle(FT_HANDLE ftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_ComPortCancelIdle(FT_HANDLE ftHandle);
__declspec(dllimport) FT_STATUS __stdcall FT_VendorCmdGet(FT_HANDLE ftHandle, UCHAR Request, UCHAR *Buf, USHORT Len);
__declspec(dllimport) FT_STATUS __stdcall FT_VendorCmdSet(FT_HANDLE ftHandle, UCHAR Request, UCHAR *Buf, USHORT Len);
__declspec(dllimport) FT_STATUS __stdcall FT_VendorCmdGetEx(FT_HANDLE ftHandle, USHORT wValue, UCHAR *Buf, USHORT Len);
__declspec(dllimport) FT_STATUS __stdcall FT_VendorCmdSetEx(FT_HANDLE ftHandle, USHORT wValue, UCHAR *Buf, USHORT Len);

// Non-Windows platform functions
__declspec(dllimport) FT_STATUS __stdcall FT_SetVIDPID(DWORD dwVID, DWORD dwPID);
__declspec(dllimport) FT_STATUS __stdcall FT_GetVIDPID(DWORD *pdwVID, DWORD *pdwPID);
__declspec(dllimport) FT_STATUS __stdcall FT_GetDeviceLocId(FT_HANDLE ftHandle, LPDWORD lpdwLocId);

// Extended API Functions
__declspec(dllimport) FT_STATUS __stdcall FT_SetLatencyTimer(FT_HANDLE ftHandle, UCHAR ucLatency);
__declspec(dllimport) FT_STATUS __stdcall FT_GetLatencyTimer(FT_HANDLE ftHandle, PUCHAR pucLatency);
__declspec(dllimport) FT_STATUS __stdcall FT_SetBitMode(FT_HANDLE ftHandle, UCHAR ucMask, UCHAR ucEnable);
__declspec(dllimport) FT_STATUS __stdcall FT_GetBitMode(FT_HANDLE ftHandle, PUCHAR pucMode);
__declspec(dllimport) FT_STATUS __stdcall FT_SetUSBParameters(FT_HANDLE ftHandle, ULONG ulInTransferSize, ULONG ulOutTransferSize);

// Win32 API Functions
__declspec(dllimport) FT_HANDLE __stdcall FT_W32_CreateFile(LPCTSTR lpszName, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreate, DWORD dwAttrsAndFlags, HANDLE hTemplate);
__declspec(dllimport) BOOL __stdcall FT_W32_CloseHandle(FT_HANDLE ftHandle);
__declspec(dllimport) BOOL __stdcall FT_W32_ReadFile(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpdwBytesReturned, LPOVERLAPPED lpOverlapped);
__declspec(dllimport) BOOL __stdcall FT_W32_WriteFile(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpdwBytesWritten, LPOVERLAPPED lpOverlapped);
__declspec(dllimport) BOOL __stdcall FT_W32_GetOverlappedResult(FT_HANDLE ftHandle, LPOVERLAPPED lpOverlapped, LPDWORD lpdwBytesTransferred, BOOL bWait);
__declspec(dllimport) BOOL __stdcall FT_W32_EscapeCommFunction(FT_HANDLE ftHandle, DWORD dwFunc);
__declspec(dllimport) BOOL __stdcall FT_W32_GetCommModemStatus(FT_HANDLE ftHandle, LPDWORD lpdwModemStatus);
__declspec(dllimport) BOOL __stdcall FT_W32_SetupComm(FT_HANDLE ftHandle, DWORD dwReadBufferSize, DWORD dwWriteBufferSize);
__declspec(dllimport) BOOL __stdcall FT_W32_SetCommState(FT_HANDLE ftHandle, LPFTDCB lpftDcb);
__declspec(dllimport) BOOL __stdcall FT_W32_GetCommState(FT_HANDLE ftHandle, LPFTDCB lpftDcb);
__declspec(dllimport) BOOL __stdcall FT_W32_SetCommTimeouts(FT_HANDLE ftHandle, FTTIMEOUTS *pftTimeouts);
__declspec(dllimport) BOOL __stdcall FT_W32_GetCommTimeouts(FT_HANDLE ftHandle, FTTIMEOUTS *pftTimeouts);
__declspec(dllimport) BOOL __stdcall FT_W32_SetCommBreak(FT_HANDLE ftHandle);
__declspec(dllimport) BOOL __stdcall FT_W32_ClearCommBreak(FT_HANDLE ftHandle);
__declspec(dllimport) BOOL __stdcall FT_W32_SetCommMask(FT_HANDLE ftHandle, ULONG ulEventMask);
__declspec(dllimport) BOOL __stdcall FT_W32_GetCommMask(FT_HANDLE ftHandle, LPDWORD lpdwEventMask);
__declspec(dllimport) BOOL __stdcall FT_W32_WaitCommEvent(FT_HANDLE ftHandle, PULONG pulEvent, LPOVERLAPPED lpOverlapped);
__declspec(dllimport) BOOL __stdcall FT_W32_PurgeComm(FT_HANDLE ftHandle, DWORD dwMask);
__declspec(dllimport) DWORD __stdcall FT_W32_GetLastError(FT_HANDLE ftHandle);
__declspec(dllimport) BOOL __stdcall FT_W32_ClearCommError(FT_HANDLE ftHandle, LPDWORD lpdwErrors, LPFTCOMSTAT lpftComstat);
__declspec(dllimport) BOOL __stdcall FT_W32_CancelIo(FT_HANDLE ftHandle);

#endif // FTD2XX_COMPLETE_LABVIEW_H
import serial

ser = serial.Serial('COM11', 115200, timeout=1)

while True:
    data = ser.readline()
    if data:
        print(f"Received: {data.decode().strip()}")

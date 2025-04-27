import serial
import time

def receive_serial_data(comPort, baudRate):
    try:
        ser = serial.Serial(comPort, baudRate, timeout=1)
        time.sleep(2)  # Allow port to settle
        print(f"Listening on {comPort} at {baudRate} baud...")
        
        while True:
            data = ser.readline().decode().strip()
            if data:
                print(f"Received: {data}")
                # Echo back the received data
                ser.write((data + '\n').encode())
    except Exception as e:
        print(f"Error: {e}")
    finally:
        if ser and ser.is_open:
            ser.close()

if __name__ == "__main__":
    comPort = "COM11"  # Receiver's COM port
    baudRate = 115200
    receive_serial_data(comPort, baudRate)

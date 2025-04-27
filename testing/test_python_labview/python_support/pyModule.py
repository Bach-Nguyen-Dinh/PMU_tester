import serial
import time

def add_num(a, b):
    # This function adds two numbers and returns the result
    return a + b

class SerialManager:
    def __init__(self, comPort, baudRate):
        self.ser = serial.Serial(comPort, baudRate, timeout=1)
        time.sleep(2)  # Allow port to stabilize

    def send(self, data):
        if self.ser and self.ser.is_open:
            self.ser.write(data.encode() + b'\n')
        else:
            return "Serial port not open!"

    def receive(self):
        if self.ser and self.ser.is_open:
            response = self.ser.readline().decode().strip()
            return response
        return "No response from receiver!"

    def close(self):
        if self.ser:
            self.ser.close()

def send_serial_data(comPort, baudRate, data, max_retries=5):
    sm = SerialManager(comPort, baudRate)

    for attempt in range(max_retries):
        # print(f"Attempt {attempt + 1}: Sending data...")
        sm.send(data)

        time.sleep(0.5)  # Give receiver a moment to respond

        ack = sm.receive()
        # print(f"Received acknowledgment: {ack}")

        if ack == data:
            return "Acknowledgment matched. Transmission successful!"
            # break
        else:
            # print("Acknowledgment mismatch. Retrying...")
            time.sleep(1)  # Wait before retrying
    else:
        sm.close()
        return "Failed to transmit after retries."

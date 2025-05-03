import os
os.environ["BLINKA_FT232H"] = "1"

import board
import busio
import digitalio
import time
from datetime import datetime
import keyboard

# Initialization SPI
spi = busio.SPI(board.SCK, board.MOSI, board.MISO)
cs = digitalio.DigitalInOut(board.D4)
cs.direction = digitalio.Direction.OUTPUT
cs.value = True  # keep CS low
#改 原False

# Get SPI lock
while not spi.try_lock():
    pass

# configure SPI - Mode 0


spi.configure(
    baudrate=100000,    
    # 100kHz - 改 10k
    phase=0,           # CPHA = 0 - 改
    #改
    polarity=0,        # CPOL = 0
    #cs_delay=0.0001    # CS延迟 - 加
)

# def transfer_data(data):
    # """send and receive data"""
    # write_buffer = bytearray(data)
    # read_buffer = bytearray(3)
    # spi.write_readinto(write_buffer, read_buffer)
    # return read_buffer

def transfer_data(data):
    cs.value = False
    time.sleep(0.00001)  # 稍微等一下稳定
    write_buffer = bytearray(data)
    read_buffer = bytearray(3)
    spi.write_readinto(write_buffer, read_buffer)
    cs.value = True
    return read_buffer #别的地方也要跟着改 还没改



def receive_data():
    """receive-only mode, sending dummy data"""
    dummy_data = bytearray([0xFF, 0xFF, 0xFF])  # send all 1's as dummy data
    read_buffer = bytearray(3)
    spi.write_readinto(dummy_data, read_buffer)
    return read_buffer

def format_data(data):
    """Formatted Data Display"""
    hex_str = "".join([f"{x:02X}" for x in data])
    int_val = int.from_bytes(data, 'big')
    bin_str = format(int_val, '024b')
    #改
    return f"HEX: 0x{hex_str}, DEC: {int_val}, BIN: {bin_str}"

try:
    print("SPI 1Hz Sending and receiving procedures initiated")
    print("configure:")
    print("- Mode 0 (CPOL=0, CPHA=0)")
    print("- 100kHz SPI clk")
    print("- every 1 second")
    print("- 24-bit data")
    print("\n1. Continuous sending incrementing data")
    print("2. Continuous sending fixed data")
    print("3. Single transmission of customized data")
    print("4. Receive-only mode (monitor MISO)")
    print("5. Exit program")

    counter = 0
    while True:
        choice = input("\nplease choose mode (1-5): ")
        
        if choice == '1':
            print("\nstart sending incrementing data...")
            try:
                while True:
                    # Check if 'r' key is pressed
                    if keyboard.is_pressed('r'):
                        counter = 0
                        print("\nCounter reset to 0")
                        time.sleep(0.5)  # Small delay to avoid multiple resets on long press


                    # send
                    send_data = counter.to_bytes(3, 'big')
                    counter = (counter + 1) & 0xFFFFFF  # keep within 24 bits
                    
                    # send and receive data
                    current_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
                    received = transfer_data(send_data)
                    
                    print(f"\n[{current_time}]")
                    print(f"send: {format_data(send_data)}")
                    print(f"receive: {format_data(received)}")
                    
                    received_value = int.from_bytes(received, byteorder='big')
                    if received_value == 0xAAAAAA:
                        print("\n✅ Acknowledgment received! Stopping the counter...")
                        break
                    # Instead of sleeping 0.1 directly, split it into 10x 0.01
                    total_sleep = 0.5  # Total sleep time
                    split = 10         # Number of small sleeps
                    for _ in range(split):
                        time.sleep(total_sleep / split)
                        if keyboard.is_pressed('r'):
                            counter = 0
                            print("\nCounter reset to 0")
                            time.sleep(0.5)  # again small wait to avoid bouncing                  
                   
            except KeyboardInterrupt:
                print("\nstop sending")
                
        elif choice == '2':
            hex_input = input("\nplease enter 24-bit hexadecimal data to send (e.g. ABCDEF): ")
            try:
                value = int(hex_input, 16)
                if value > 0xFFFFFF:
                    print("fault: data exceeds 24 bits")
                    continue
                    
                print("\nstart sending fixed data...")
                data = value.to_bytes(3, 'big')
                try:
                    while True:
                        current_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
                        received = transfer_data(data)
                        
                        print(f"\n[{current_time}]")
                        print(f"send: {format_data(data)}")
                        print(f"receive: {format_data(received)}")
                        
                        time.sleep(0.2)  # 1 second interval
                        
                except KeyboardInterrupt:
                    print("\nstopped sending")
                    
            except ValueError:
                print("fault: invalid hexadecimal input")
                
        elif choice == '3':
            hex_input = input("\nplease enter 24-bit hexadecimal data to send (e.g. ABCDEF): ")
            try:
                value = int(hex_input, 16)
                if value > 0xFFFFFF:
                    print("fault: data exceeds 24 bits")
                    continue
                    
                data = value.to_bytes(3, 'big')

                current_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
                received = transfer_data(data)
                
                print(f"\n[{current_time}]")
                print(f"send: {format_data(data)}")
                print(f"receive: {format_data(received)}")
                
            except ValueError:
                print("fault: invalid hexadecimal input")
        
        elif choice == '4':
            print("\nstarting receive-only mode...")
            print("(Press Ctrl+C to stop)")
            output_path = r"P:\1. University Of Adelaide\Courses\S3\Project\Software Files\FPGA\working1\output.txt" #新加

            with open(output_path, 'w') as file: # 新加
                file.write("Timestamp, HEX, BIN\n")  # 写入表头 title


            try:
                last_data = None
                while True:
                    time.sleep(0.45)  # Check more frequently than the sending rate
                    #I used want do offset the data shifting
                    current_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
                    received = receive_data()
                    
                    # Only print when data changes
                    if received != last_data:
                        print(f"\n[{current_time}]")
                        print(f"receive: {format_data(received)}")
                        last_data = bytearray(received)  # Make a copy


                        with open(output_path, 'a') as file:
                              # 用 'a' 追加模式，不覆盖已有数据
                            #not sure what is a for now 3.13.25
                            hex_data = received.hex().upper()  # HEX 格式 data in hex upper
                            int_val = int.from_bytes(received, 'big')  # 转换为整数
                            bin_data = format(int_val, f'0{8 * len(received)}b')  # BIN 格式
                            
                            file.write(f"[{current_time}], HEX: 0x{hex_data}, BIN: {bin_data}\n")  # 写入一行
                        #新加

                    time.sleep(0.45)
                    #time.sleep(0.05)  # Check more frequently than the sending rate
                    
            except KeyboardInterrupt:
                print("\nstopped receiving")
                
        elif choice == '5':
            print("\nexit program")
            break
            
        else:
            print("\ninvalid input, please try again")

except KeyboardInterrupt:
    print("\nexit program")
finally:
    spi.unlock()
    cs.deinit()
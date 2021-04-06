import serial
import sys
import os
BAUDRATE = 38400
ser = None
DELIMITERV = "aaaa"
DELIMITERG = "bbbb"

'''
run script by either: python3 bms_serial_measurement.py
or: python3 bms_serial_measurement.py PORT
where port is the com port for the BMS
'''



def init(first_time):
    if first_time and len(sys.argv) > 1:
        com_port = sys.argv[1]
    else:    
        print("Input serial port(ttyUSB0, COM0):")
        com_port = input()
        
    if "tty" in com_port and not "dev" in com_port:
        com_port = f"/dev/{com_port}"
        
    return com_port
    

if __name__ == "__main__":
    com_port = init(first_time = True)
    while(com_port == ""):
        com_port = init(first_time = False)
    
    try:
        ser = serial.Serial(com_port, BAUDRATE, timeout=20)
        while(1):
            data = int.from_bytes(ser.read(1), "big")
            print(data)
            if hex(data)[2:] == "aa":
                data = int.from_bytes(ser.read(1), "big")
                if hex(data)[2:] == "aa":
                    print("Found starting point")
                    break
        voltages = []
        while(1):
            data = int.from_bytes(ser.read(2), "big")
            if hex(data)[2:] ==  DELIMITERV:
                # print("Delimiter found, printing voltages:")
                for count, volt in enumerate(voltages):
                    # print(f"Voltage {count+1} - {volt:.4f}V")
                    pass
                # print("-"*20)
                voltages = []
            elif hex(data)[2:] ==  DELIMITERG:
                print("Delimiter found, printing Temperatures:")
                for count, volt in enumerate(voltages):
                    
                    print(f"Temperature {count+1} - {volt:.6f}*C")
                    pass
                print("-"*20)
                voltages = []
            else:
                voltages.append(data * 100e-6) #convert bits to voltage. 100uV/bit
    except Exception as e:
        print(e)
        ser.close()


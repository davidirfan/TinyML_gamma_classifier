import serial
import time
import csv

# Adjust this to match your ESP32's COM port
ser = serial.Serial('COM3', 115200)                             # Open serial port at 115200 baud rate
time.sleep(2)                                                   # Allow ESP32 to reset

# Open and read the CSV file
with open('new_cs137_21_60s.csv', 'r') as file:        # Open the CSV file in read mode
    reader = csv.reader(file)                                   # Create a CSV reader object    
    for row in reader:                                          # Iterate through each row in the CSV file
        if len(row) != 2:                                       # Check if the row has exactly 3 columns
            print(f"Skipping invalid row: {row}")               # Skip rows that don't have 3 columns
            continue                                            # Skip rows that don't have 3 columns
        # Construct the line to send to ESP32
        line = f"{row[0]},{row[1]}\n"                  # Create a string from the row data
        ser.write(line.encode())                                # Send the line to ESP32
        time.sleep(0.05)                                        # small delay to let ESP32 handle input

# Read and print ESP32 response
time.sleep(0.5)                                                 # Wait for ESP32 to process the command
while ser.in_waiting:                                           # Check if there is data in the serial buffer
    print(ser.readline().decode().strip())                      # Read and print the response from ESP32

time.sleep(1)  

# Tell ESP32 to show stored data
ser.write(b'RUN\n')                                            # Send command to ESP32 to show stored data

# Read and print ESP32 response
time.sleep(0.5)                                                 # Wait for ESP32 to process the command
while ser.in_waiting:                                           # Check if there is data in the serial buffer
    print(ser.readline().decode().strip())     

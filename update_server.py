import socket
import struct
import csv

HOST = '127.0.0.1'  # Hostname of the server
PORT = 8080  # Port to listen on

# Define the struct format
fmt = 'Iiiiiiiiiiiffffffffff'
fmt2 = 'iiiiiiiiiiiiiiiiiiii' 

def receive_data(connection):
    data = connection.recv(struct.calcsize(fmt))
    if not data:
        return None

    training_info = struct.unpack(fmt, data)
    print(f'Received from client: {training_info}')
    return training_info

def write_to_csv(training_info):
    with open('data.csv', 'a', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(training_info)

def send_feedback(connection, feedback_array):
    feedback_data = struct.pack(fmt2, *feedback_array)
    connection.sendall(feedback_data)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    # Bind the socket to a specific address and port
    s.bind((HOST, PORT))

    # Listen for incoming connections
    s.listen()
    print(f'Server is listening on {HOST}:{PORT}, waiting for client')

    # Accept a client connection
    conn, addr = s.accept()
    print('Connected by', addr)

    while True:
        print("Server is waiting for client's data...")
        training_info = receive_data(conn)
        if training_info is None:
            break

        write_to_csv(training_info)

        feedback_array = [i * 2 for i in range(1, 22)]
        #send_feedback(conn, feedback_array)

    # Close the connection
    conn.close()


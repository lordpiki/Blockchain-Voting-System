import socket
import os
import sys

sys.path.append(sys.path[0][:-len("User/")])
from envLoader import get_envLoader

class Communicator:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        print(f'Connecting to CPP {host}:{port}')
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def connect(self):
        self.socket.connect((self.host, self.port))

    def disconnect(self):
        self.socket.close()

    def get_message(self):

        sizeBuffer = self.socket.recv(4)
        messageSize = int.from_bytes(sizeBuffer, byteorder='little', signed=False)
        print(f"getting msg from cpp size:{ messageSize}")

        # Receive the actual message
        message = self.socket.recv(messageSize)
        print("msg code:", message[0])
        try:
            print("msg:", message.decode('utf-8'))
        except:
            print("Failed to decode message")
            print("msg not decoded:", message)
        
        return message

    def send_message(self, message, to_encode = True):
        
        message_size = len(message)
        print(f'Sending to CPP message of size {message_size}')

        # Convert the size to a 4-byte binary string
        size_buffer = message_size.to_bytes(4, byteorder='big')
        print(f'message to CPP is : {message}')

        # Send the size first
        self.socket.sendall(size_buffer)

        # Send the actual message
        if to_encode:
            self.socket.sendall(message.encode('utf-8'))
            return
        self.socket.sendall(message)

        
    def test_run(self):
        self.connect()
        message = self.get_message()
        print(f'Received message: {message}')
        if (message != 'Hello from client'):
            return False
        
        message_to_send = 'Hello from Python!'
        self.send_message(message_to_send)
        print(f'Sent message: {message_to_send}')

        self.disconnect()
        return True

# Usage example:
if __name__ == '__main__':
    env = get_envLoader().getEnv()
    communicator = Communicator(env['COMM_IP'], int(env['COMM_PORT']))
    communicator.test_run()

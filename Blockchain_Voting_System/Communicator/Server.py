import socket
import threading
import time 
from Colors import print_colored_text, Colors
import random

class Server:
    
    @staticmethod
    def recieve(connection, size):
        data = connection.recv(size)
        if not data:
            raise "connection disconnected"
        return data
    
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client_sockets = {}
        
        self.to_finish = False
        
        self.edge_set = set()
        
        self.client_sockets_lock = threading.Lock()
        self.edge_set_lock = threading.Lock()
        
    def get_edge_set(self):
        with self.edge_set_lock:
            return self.edge_set.copy()

    def run(self):
        try:
            self.server_socket.bind((self.host, self.port))
            self.server_socket.listen(1)

            print_colored_text(f"Server listening on {self.host}:{self.port}", "blue")

            while not self.to_finish:
                client_socket, client_address = self.server_socket.accept()
                
                # getting actual ip, port from the client
                msg = self.recieve(client_socket, 20).decode()

                client_address, client_port = msg.split(":")
                print_colored_text(f"Accepted connection from {client_address}", "green")
                
                client_socket.sendall("ok".encode())
                client_thread = threading.Thread(target=self.handle_client, args=(client_socket,))
                client_thread.start()
                
                with self.client_sockets_lock:
                    self.client_sockets[client_socket] = client_address + ":" + str(client_port)

        except Exception as e:
            print_colored_text(f"Error in run: {e}", "red")
        finally:
            self.server_socket.close()

             
    # function to be overloaded by child class
    def handle_client(self, client_socket):
        pass

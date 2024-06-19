import socket
import threading
import struct
import time
import random
import sys
import os
import datetime

sys.path.append(sys.path[0][:-len("User")])

from User.Communicator import Communicator
from Colors import print_colored_text, Colors
from envLoader import get_envLoader

conections_mtx = threading.Lock()

env = get_envLoader().getEnv()

DNS_IP = env['DNS_IP']
DNS_PORT = int(env['DNS_PORT'])

print("dns ip and port are: ", DNS_IP, DNS_PORT)

GOV_IP = env['GOV_IP']
GOV_PORT = int(env['GOV_PORT'])

SELF_IP = socket.gethostbyname(socket.gethostname())
SELF_PORT = int(env['SELF_PORT'])

MIN_CONNECTIONS = 5
MAX_CONNECTIONS = MIN_CONNECTIONS * 2

BROAD_CAST_CODE = '1'

INIT_TTL = 20


class TempCommunicator:
    def get_message(self):
        while True:
            pass
        return ""
    
    def send_message(self, message):
        pass

    def connect(self):
        pass

    def disconnect(self):
        pass
    
    
class Peer:
    @staticmethod
    def recieve(connection, size):
        try:
            data = connection.recv(size)
            return data
        except ConnectionResetError:
             print_colored_text("connection disconnected", "red")
        
    def __init__(self):
        # Communicator to the crypto (CPP) part
        self.communicator = Communicator('127.0.0.1', int(sys.argv[1]))
        # self.communicator = TempCommunicator()

        # List of connections to other pears
        self.connections = {}

        self.to_finish = False
                
        # Server socket to listen to new connections
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Create mutex for connections management.
        self.conections_mtx = threading.Lock()
        self.dns_mtx = threading.Lock()
        self.msg_set = set()
    
    def connect_to_dns(self):
        # Socket with the DNS server
        print("Connectiong to dns with IP,PORT" + DNS_IP + ":" + str(DNS_PORT))
        self.dns_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.dns_socket.connect((DNS_IP, DNS_PORT))
        
        with self.dns_mtx:
            self.dns_socket.sendall(str(SELF_IP + ":" + str(SELF_PORT)).encode())
            self.recieve(self.dns_socket, 2)


    def send_broadcast(self, ttl, data, connection_to_ignore = None):
            # Construct message
            ttl_bytes = struct.pack('!I', ttl)
            size_bytes = struct.pack('!I', len(data))
            message = BROAD_CAST_CODE.encode() + ttl_bytes + size_bytes + (data if type(data) == bytes else data.encode())
            current_time = datetime.datetime.now().strftime("%H:%M:%S")
            print_colored_text(f"Sending broadcast at {current_time}, code:{message[0]}, msg: {message}", "yellow")

            # Send message to all connections
            self.conections_mtx.acquire()
            for connection in self.connections.keys():
                if not connection == connection_to_ignore:
                    try:
                        connection.sendall(message)
                    except:
                        connection.close()
            self.conections_mtx.release()

    def handle_connection(self, connection: socket.socket):
        if len(sys.argv) > 1:
            print_colored_text("New connection created", "green")
        try:
            while not self.to_finish:
                # recive code
                code = Peer.recieve(connection, 1)
                if code == -999:
                    self.connections.pop(connection)
                    return
                
                # code not valid, close connection
                if not code.decode() == BROAD_CAST_CODE:
                    if len(sys.argv) > 1:
                        print("invalid code", code.decode())
                    raise "Invalid code"

                # Recive full message from connection
                ttl = struct.unpack('!I', Peer.recieve(connection, 4))[0]
                size = struct.unpack('!I', Peer.recieve(connection, 4))[0]
                data = Peer.recieve(connection, size)

                print_colored_text(f"Recived data from connection. Code: {data[0]}. Size: {len(data)}", "blue")
                # If ttl zero ignore message
                if ttl == 0:
                    continue
                
                has_seen = any(hash(data) == i[0] and time.time() - i[1] < 20 for i in self.msg_set)
                
                if has_seen:
                    print_colored_text(f"Seen message", "magenta")
                    continue
                
                self.msg_set.add((hash(data), time.time()))
                self.communicator.send_message(data, False)
                self.send_broadcast(ttl - 1, data, connection)
        except:
            connection.close()
            try:
                # trying to pop connection if not already popped
                self.conections_mtx.acquire()
                self.connections.pop(connection)
                self.conections_mtx.release()
            except:
                pass
        if len(sys.argv) > 1:
            print_colored_text("Connection closed", "red")


    def keepAlive(self):
        # This function make sure there is always a correct number of connections
        
        def combine(connection):
            return self.connections[connection]
        
        def get_dns_ips():
            try:
                get_ip_msg = chr(40) + str(MAX_CONNECTIONS -len(self.connections)) + str(len(self.connections)) + ','.join(map(combine, self.connections))

                self.dns_socket.sendall(get_ip_msg.encode())
                
                msg = self.recieve(self.dns_socket, 1024).decode()
                
                if msg == None:
                    return []
                num_of_ips = int(msg[0])
                
                if num_of_ips > 0:
                    ip_list = msg[1:].split(',')
                    return ip_list
                return []
            except Exception as e:
                print_colored_text(f"Error in get_dns_ips: {e}", "red")
                exit(-1)
        
        def update_dns():
            get_ip_msg = chr(50) + ','.join(map(combine, self.connections))
            self.dns_socket.sendall(get_ip_msg.encode())
            self.recieve(self.dns_socket, 2)
               
        def fill_connections():
            # Get list of ips from DNS
            ip_list = get_dns_ips()
            for ip in ip_list:
                try:
                    # Create connection
                    connection = createConnection((ip.split(':')[0], int(ip.split(':')[1])))
                    # Append connection to list
                    if connection is not None:
                        self.connections[connection] = ip
            
                except Exception as e:
                    print_colored_text(f"Error in fill_connections: {e}", "red")
            # uppdate dns for visualisation
            update_dns()
            
        def createConnection(addr):
            if ":".join(map(str, addr)) in list(self.connections.values()):
                return None
            
            print_colored_text(f"Creating connection to {addr}", "yellow")
            # Create socket
            connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            connection.connect(addr)
            
            connection.sendall(str(SELF_PORT).encode())

            # Create thread that handles the new connection
            client_thread = threading.Thread(target=self.handle_connection, args=(connection, ))
            client_thread.start()
            update_dns()
            return connection
        
        try:
            while not self.to_finish:
                # If there are too many connections close one
                if len(self.connections) > MAX_CONNECTIONS:
                    
                    # removing 2nd to last element
                    socket_key, socket_value = self.connections.popitem()

                    socket_key2, socket_value2 = self.connections.popitem()

                    self.connections[socket_key] = socket_value

                    # Close the socket
                    socket_key2.close()
                    print(len(self.connections))
                    
                # If there are too few create a new connection
                elif len(self.connections) < MIN_CONNECTIONS:
                    fill_connections()
                update_dns()
                time.sleep(5)
        except Exception as e:
            print_colored_text(f"Error in keepAlive: {e}", "red")
            self.to_finish = True
            exit()

    def bind_and_listen(self):
        # Listen with server socket
        self.server_socket.bind((SELF_IP, SELF_PORT))
        self.server_socket.listen()

        while not self.to_finish:
            # Accept client - new connection
            connection, addr = self.server_socket.accept()
            
            # recieve actual port
            port = self.recieve(connection, 1024).decode()
            

            # Create a thread that handles the connection
            client_thread = threading.Thread(target=self.handle_connection, args=(connection,))
            client_thread.start()
            
            # Append connection to list
            self.connections[connection] = addr[0] + ':' + port
            
    def handle_communicator(self):
        try:
            self.communicator.connect()

            while not self.to_finish:
                # Get message from communicator
                data = self.communicator.get_message()
                if not data:
                    continue
                print(f"got message from cpp: {data}")

                if data[0] == 60:
                    self.handle_signed_pk(data[1:])
                    continue
                
                # Broadcast message
                print("sending broadcast")
                self.send_broadcast(INIT_TTL, data, None)

            self.communicator.disconnect()

        except ConnectionResetError:
            print_colored_text("Communicator connection was closed.", "red")
        except Exception as e:
            print_colored_text(f"Error in handle_communicator: {e}", "red")
            exit(-1)
        finally:
            self.to_finish = True

    def handle_signed_pk(self, data):
        try:
            govAddr = (GOV_IP, GOV_PORT)
            #connect  to government
            govConnection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            govConnection.connect(govAddr)
            
            
            govConnection.sendall(str(SELF_IP + ":" + str(SELF_PORT)).encode())
            self.recieve(govConnection, 2)
            
            govConnection.sendall(data)

            signed_pk = self.recieve(govConnection, 1024)
            
            self.communicator.send_message(signed_pk, False)
            
            govConnection.close()
        except Exception as e:
            print_colored_text(f"Error in handle_signed_pk: {e}", "red")
            
            exit(-1)
    
    def handle_dns(self):
        
        self.get_dns_ips()
        while not self.to_finish:
            msg = self.dns_socket.recv(1)

            if msg == chr(50):
                self.dns_socket.sendall(chr(51).encode())

    def run(self):
        # run all threads
        keepAlive_thread = threading.Thread(target=self.keepAlive, args=(), daemon=True)
        communicator_thread = threading.Thread(target=self.handle_communicator, args=(), daemon=True)
        listener_thread = threading.Thread(target=self.bind_and_listen, args=(), daemon=True)
        self.connect_to_dns()

        # Start threads.
        keepAlive_thread.start()
        communicator_thread.start()
        listener_thread.start()
        
        try:
            while not self.to_finish:
                pass
        except KeyboardInterrupt:
            self.to_finish = True
            exit(0)

if __name__ == '__main__':
    peer = Peer()
    peer.run()

import threading
import sys  
sys.path.append(sys.path[0][:-len("Government/")])
from Server import Server
from Colors import print_colored_text, Colors
import subprocess



from envLoader import get_envLoader


class Government(Server):
    def __init__(self, host, port):
        super().__init__(host, port)
                
            
    def encrypt(self, data):
        govSign = subprocess.Popen(['../x64/Debug/GovSign.exe' , data], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        govSign_output, _ = govSign.communicate()
        return govSign_output
        
                    
    def handle_client(self, client_socket):
        try:
            # getting data from client
            data = self.recieve(client_socket, 1024)
            
            print_colored_text(f"Received data: {data.decode('utf-8')}", "yellow")
            # encrypting data and sending it back
            client_socket.sendall(self.encrypt(data))
                
        except Exception as e:
            print_colored_text(f"Error handling client: {e}", "red")
            exit()
        finally:
            client_socket.close()


if __name__ == "__main__":
    env = get_envLoader().getEnv()
    host = env['GOV_IP']
    port = int(env['GOV_PORT'])

    server = Government(host, port)
    server_thread = threading.Thread(target=server.run)
    server_thread.start()
        
    while not server.to_finish:
        try:
            server.to_finish = input("To exit at any point type 'exit'\n") == 'exit'
            if server.to_finish:
                exit()
            pass
        except:
            server.to_finish = True
            exit()  
        

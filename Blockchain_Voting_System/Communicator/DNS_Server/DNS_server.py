import threading
import sys  

sys.path.append(sys.path[0][:-len("DNS_Server/")])

# from ..Utilities.Server import Server
from Colors import print_colored_text, Colors
from Server import Server
from envLoader import get_envLoader
import random
import networkx as nx
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

plt.rcParams['toolbar'] = 'None'

def visualize(server):
    # Create an empty graph
    G = nx.Graph()

    # Create a figure with black background
    fig = plt.figure(facecolor='black', figsize=(8, 6))

    # List to store the graph objects for animation
    graph_objects = []

    # Function to update the graph during animation
    def update(frame):
        nonlocal graph_objects
        count = 0
        ips = server.get_edge_set()
        with server.edge_set_lock:
            count = len(server.edge_set)
        if count != len(G.edges):
            # Clear the figure
            fig.clear()

            with server.client_sockets_lock:
                G.add_nodes_from(server.client_sockets.values())
            G.add_edges_from(ips)

            # Add the amount of edges from each node as labels
            node_labels = {node: node + " - " + str(len(G.edges(node))) for node in G.nodes}

            # Draw the graph with node labels
            pos = nx.spring_layout(G)  # You can use other layouts as well

            # Draw the graph and labels
            graph_objects = []
            graph_objects.append(nx.draw(G, pos, with_labels=False, font_weight='bold', node_color='green', edge_color='green'))
            graph_objects.extend(nx.draw_networkx_labels(G, pos, labels=node_labels, font_color='white'))
            
            # Draw smaller black nodes inside each node
            node_size = 130
            node_color = 'black'
            for node in G.nodes:
                nx.draw_networkx_nodes(G, pos, nodelist=[node], node_size=node_size, node_color=node_color)

            # Set the background color of the figure to black
            fig.set_facecolor('black')

    # Set up the animation
    ani = FuncAnimation(fig, update, interval=1000, cache_frame_data=False)

    # Show the plot
    plt.show()



class DNS_Server(Server):
    def __init__(self, host, port):
        super().__init__(host, port)
        self.func_table = {40: self.handle_ip_request, 50: self.handle_update}

    def update_edge_set(self):
        with self.client_sockets_lock, self.edge_set_lock:
            self.edge_set = [edge for edge in self.edge_set if edge[0] in self.client_sockets.values() and edge[1] in self.client_sockets.values()]


    def handle_update(self, data, client_socket):
        connection_list = data.split(',')
        if '' in connection_list:
            connection_list.remove('')
        
        with self.edge_set_lock and self.client_sockets_lock:
            client_ip = self.client_sockets[client_socket]
            print_colored_text(f'Handling update request from {client_ip}', 'blue')
            self.edge_set = [edge for edge in self.edge_set if not client_ip in edge]

            print_colored_text(f"connection_list: {connection_list}", 'blue')

            for ip in connection_list:
                self.edge_set += [(client_ip, ip)]

            print_colored_text(f'Updated edge set: {self.edge_set}', 'green')
                
        client_socket.sendall("ok".encode())
    
    def handle_ip_request(self, data, client_socket):
        try:
            # Extract the number of IP addresses and ignored IP addresses from the data
            num_of_ip = int(data[0])
            num_of_ignored_ip = int(data[1])

            # The ignored IP addresses are initialized with the IP address of the client
            with self.client_sockets_lock:
                ignored_ip_list = [self.client_sockets[client_socket]]
                print_colored_text(f'Handling IP request from {self.client_sockets[client_socket]}', 'blue')
                print_colored_text(f"Known clients: {self.client_sockets.values()}", 'blue')
            
            # Extract the ignored IP addresses from the data
            if num_of_ignored_ip > 0:
                ignored_ip_list += data[2:].split(',')[:num_of_ignored_ip]

            with self.client_sockets_lock:
                ips = [ip for ip in self.client_sockets.values() if ip not in ignored_ip_list]
            random.shuffle(ips)

            ips = ips[:num_of_ip if num_of_ip < len(ips) else None]
            msg = str(len(ips)) + ','.join(ips)

            print_colored_text(f"sending ips: {msg}", 'blue')

            # Send the IP addresses to the client
            client_socket.sendall(msg.encode()) 
        except Exception as e:
            print_colored_text(f"Error sending IP addresses: {e}", "red")
        
    
    def handle_client(self, client_socket):
        try:
            while not self.to_finish:

                data = self.recieve(client_socket, 1024)
                
                print_colored_text(f"Received data: {data.decode('utf-8')}", "green")
                
                code = int(data[0])
                
                # getting msg code and calling matching function
                self.func_table[code](data.decode()[1:], client_socket)
                            
                
        except Exception as e:
            print_colored_text(f"Error handling client: {e}", "red")
            exit()
        finally:
            
            with self.edge_set_lock and self.client_sockets_lock:
                self.edge_set = {edge for edge in self.edge_set if self.client_sockets[client_socket] not in edge}
                size = len(self.client_sockets)
                self.client_sockets.pop(client_socket)
                print_colored_text(f"Disconnected client_socket {size - len(self.client_sockets)}", "red")
                client_socket.close()
        

if __name__ == "__main__":
    env = get_envLoader().getEnv()
    host = env['DNS_IP']  # Change this to your desired host
    port = int(env['DNS_PORT'])  # Change this to your desired port

    server = DNS_Server(host, port)
    server_thread = threading.Thread(target=server.run)
    server_thread.start()
    
    visualize(server)
    
    while not server.to_finish:
        try:
            server.to_finish = input("To exit at any point type 'exit'\n") == 'exit'
            if server.to_finish:
                Server.to_finish = True
                exit()
            pass
        except:
            server.to_finish = True
            exit()

    
    
    
    
    
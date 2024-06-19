import subprocess
import random
import sys

instance = None
sys.path.append(sys.path[0][:-len("GUI/")] + 'Blockchain')

class singleton(type):
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            instance = super().__call__(*args, **kwargs)
            cls._instances[cls] = instance
        return cls._instances[cls]

class ProcessManager(metaclass=singleton):
    
    def __init__(self) -> None:
        self.cpp_port = random.randint(5001, 5999)
        self.processes = []
        # Run government and DNS servers
        # self.gov = subprocess.Popen(['python', '../Communicator/Government/Government.py'])
        # self.dns = subprocess.Popen(['python', '../Communicator/DNS_Server/DNS_Server.py'])

    def clear(self):
        # Kill old CPP and Peer processes
        for p in self.processes:
            p.kill()
        self.processes = []

    def append(self, process):
        self.processes.append(process)

    def run_peer(self, client_type):
        self.clear()

        # Run CPP
        self.append(subprocess.Popen(['../x64/Debug/Blockchain.exe', str(self.cpp_port)], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE))

        output = self.processes[0].stdout.readlines(6) 
        print(output)

        # Choose client type
        self.processes[0].stdin.write(f'{client_type}\n'.encode())
        self.processes[0].stdin.flush()

        # Run peer
        self.append(subprocess.Popen(['python', '../Communicator/User/peer.py', str(self.cpp_port)]))


    def get_cpp(self):
        if len(self.processes) >= 1:
            return self.processes[0]
        else:
            raise "Cpp process isn't open"
        
    def kill(self):
        # Terminate all processes
        self.clear()
        # self.gov.kill()
        # self.dns.kill()
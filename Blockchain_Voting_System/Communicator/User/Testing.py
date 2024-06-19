# from Communicator import Communicator
from peer import Peer
import subprocess
import threading
from signal import SIGINT
import time

class Testing:
    def run_all_tests(self):
        # self.test_communicator()
        self.test_peer_connection()
        self.test_message_broadcast()

    def test_communicator(self):
        # Create an instance of the Communicator class
        # communicator = Communicator()
        # status = communicator.test_run()
        # return status
        pass

    def test_peer_connection(self):
        print("Test peer connection: ", end="")

        # Lines that indecates successful connection
        CONNECTION_CREATED = "New connection created"
        CONNECTION_CLOSED = "Connection closed"

        # Ports for both peers
        port1 = "1234"
        port2 = "2345"
        cmd = ["python", "peer.py"]

        # Run peers using subprocess
        p1 = subprocess.Popen(cmd + [port1],stdout=subprocess.PIPE,stdin=subprocess.PIPE)
        p2 = subprocess.Popen(cmd + [port2],stdout=subprocess.PIPE,stdin=subprocess.PIPE)

        time.sleep(5)

        # Get the output from the peers' processes.
        p1.kill()
        time.sleep(1)
        p2.kill()

        output1 = p1.communicate()[0].decode()
        output2 = p2.communicate()[0].decode()

        # Check if the peers had connected successfully.
        if CONNECTION_CREATED in output1 and CONNECTION_CREATED in output2 and CONNECTION_CLOSED in output2:
            print("test passed.")
            return True
        
        # If test faild print the peers' output.
        print("test failed.")
        print("output1:", output1)
        print("output2:", output2)

        return False

    def test_message_broadcast(self):
        print("Test message broadcast: ", end="")
        TEXT = "Hello from peer 5"

        port1 = "1234"
        port2 = "2345"
        port3 = "3456"
        port4 = "4567"
        cmd = ["python", "peer.py"]

        p1 = subprocess.Popen(cmd + [port1],stdout=subprocess.PIPE,stdin=subprocess.PIPE)
        p2 = subprocess.Popen(cmd + [port2],stdout=subprocess.PIPE,stdin=subprocess.PIPE)
        p3 = subprocess.Popen(cmd + [port3],stdout=subprocess.PIPE,stdin=subprocess.PIPE)
        p4 = subprocess.Popen(cmd + [port4],stdout=subprocess.PIPE,stdin=subprocess.PIPE)

        # This peer would run with the default port
        peer = Peer()
        t = threading.Thread(target=peer.run, args=(), daemon=True)
        t.start()

        # The peers should connect automaticly to each other.

        # Send broadcast message from peer 5
        time.sleep(10)
        peer.send_broadcast(5, TEXT)
        time.sleep(10)

        p1.kill()
        p2.kill()
        p3.kill()
        p4.kill()

        # Check if you got the message from the other peer
        outputs = []
        outputs += [p1.communicate()[0].decode()]
        outputs += [p2.communicate()[0].decode()]
        outputs += [p3.communicate()[0].decode()]
        outputs += [p4.communicate()[0].decode()]

        for output in outputs:
            if TEXT not in output:
                print("Test failed.")
                print(outputs)
                return False
        print("Test passed.")
        return True


        
# Instantiate the Testing class and call the test_communicator function
testing = Testing()
print("run")
testing.run_all_tests()

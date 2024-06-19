from flask import Flask, render_template, request, jsonify
from process_manager import ProcessManager 
import random

p_manager = ProcessManager()

processes = []

app = Flask(__name__, template_folder='templates', static_folder='static')

from miner import miner_blueprint
app.register_blueprint(miner_blueprint)

from voter import   voter_blueprint
app.register_blueprint(voter_blueprint)

@app.route('/')
def main():
    return render_template('index.html')

def updateEnv(server_ip):
    env_path = '../Communicator/.env'
    if server_ip:
        with open(env_path, 'r') as f:
            lines = f.readlines()

        with open(env_path, 'w') as f:
            for line in lines:
                if line.startswith('DNS_IP='):
                    f.write(f'DNS_IP={server_ip}\n')
                elif line.startswith('GOV_IP='):
                    f.write(f'GOV_IP={server_ip}\n')
                else:
                    f.write(line)

        print(f"Updated DNS_IP and GOV_IP to {server_ip}")
    else:
        print("No server IP found in localStorage.")

@app.route('/set_ip', methods=['POST'])
def set_ip():
    data = request.get_json()
    ip = data.get('ip')
    updateEnv(ip)

    # Handle the IP as needed
    print(f'Received IP: {ip}')

    return jsonify({'message': 'IP received'}), 200

if __name__ == '__main__':
    print("run")
    num = random.randint(5001, 5999)
    print(num)
    app.run(debug=True, port=num)

    p_manager.kill()
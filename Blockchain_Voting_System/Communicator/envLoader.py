import random
import os
import socket
_instance = None

def get_envLoader():
    global _instance
    if _instance is None:
        env_file = '.env'
        current_dir = os.path.dirname(os.path.abspath(__file__))
        env_path = os.path.join(current_dir, env_file)
        _instance = envLoader(env_path)
    return _instance


class envLoader:
    def __init__(self, envPath):
        self.envPath = envPath
        self.env = {}
        self.loadEnv()
        self.setup()

    # trash code, ignore it it works
    def loadEnv(self):
        with open(self.envPath, 'r') as file:
            for line in file:
                if not line.startswith('#') and not line.startswith('\n'):
                    key, value = line.strip().split('=')
                    if key == 'SELF_PORT':
                        self.env[key] = random.randrange(5001, 5999)
                        continue
                    self.env[key] = value

    def getEnv(self):
        return self.env
    
    def setup(self):
        pass

if __name__ == "__main__":
    env = get_envLoader().getEnv()
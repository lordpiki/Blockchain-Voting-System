from flask import Blueprint, render_template
import subprocess
from process_manager import ProcessManager
import time
p_manager = ProcessManager()

miner_blueprint = Blueprint("miner", __name__)
cpp_p = None
@miner_blueprint.route('/miner')
def miner():
    global cpp_p
    print("Running miner")
    p_manager.run_peer(2)

    cpp_p = p_manager.get_cpp()
    return render_template('miner.html', stats=getMinerStats(cpp_p))

    
@miner_blueprint.route('/stats')
def getStats():
    global cpp_p
    return getMinerStats(cpp_p)

def getMinerStats(process):

    process.stdin.write(b'1\n')
    process.stdin.flush()
    output = process.stdout.readlines(1)
    
    generalStats = dict()
    
    while not b"blockCount" in output[0]:
        output = process.stdout.readlines(1)
            
    minerStats = dict()
    
    for i in output[0].decode('utf-8').split(';'):
        print(i)
        key, value = i.split(':')
        minerStats[key] = value
        
    generalStats['Miner Info'] = minerStats
    
    blockStats = dict()
    
    while not b"end" in output[0]:
        stats = dict()
        output = process.stdout.readlines(1)
        if b"end" in output[0]:
            break
        for i in output[0].decode('utf-8').split(';'):
            print(i)
            try:
                key, value = i.split(':')
                stats[key] = value
            except:
                pass
        print("printing stats:", stats)
        try:
            blockStats[stats['Block']] = stats
        except:
            pass
        
    generalStats['Blocks Info'] = blockStats    
    print("general stats:", generalStats)
    return generalStats

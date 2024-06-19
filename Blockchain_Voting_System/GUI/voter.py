from flask import Blueprint, render_template, request
from process_manager import ProcessManager
import json

voter_blueprint = Blueprint('voter',__name__)
p_manager = ProcessManager()

@voter_blueprint.route('/voter', methods=['GET'])
def vote_screen():
    print("Running voter")
    p_manager.run_peer(1)

    return render_template("voter.html")

@voter_blueprint.route('/view_votes', methods=['GET'])
def view_votes():
    return render_template("view_votes.html")

@voter_blueprint.route('/fetch_votes')
def get_votes():
    print("getting votes")
    cpp = p_manager.get_cpp()

    # Ask cpp for votes count
    try:
        cpp.stdin.write(b'1\n')
        cpp.stdin.flush()
    except:
        print("cpp process crushed: ", cpp.communicate())
        return {"candidates": [], "votes": []}

    # Read votes count
    line = ''
    while '{' not in line:
        try:
            line = cpp.stdout.readline()
            line = line.decode()
        except:
            print(f"Cannot decode line {line}")
            line = ''
    print(f'line: {line}')
    output = json.loads(line)
    print(output)

    # Convert dict to arrays
    candidates = list(output.keys())
    votes = list(output.values())
    
    candidates_list = [1, 2, 3, 4, 5, 6, 7, 8, 9]
    votes_list = [0,0,0,0,0,0,0,0,0]
    for i in range(len(candidates)):
        i=int(i)
        votes_list[int(candidates[i])-1] = votes[i]

    return {"candidates": candidates_list, "votes": votes_list}

@voter_blueprint.route('/send_vote', methods=['POST'])
def vote():
    cpp = p_manager.get_cpp()
    data = request.get_json()

        # Retrieve the candidate value from the JSON data
    candidate = data.get('candidate')
    
    print(f"Voting for candidate {candidate}")
    # Vote for the wanted candidate
    cpp.stdin.write(f'2 {candidate}'.encode())
    cpp.stdin.flush()
    
    # Wait for confirmation
    while not b'OK' in cpp.stdout.readline():
        pass

    print("Vote sent")
    return 'OK'

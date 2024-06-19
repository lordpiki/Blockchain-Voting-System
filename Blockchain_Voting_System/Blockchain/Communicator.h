#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "ICommunicator.h"
#include "Singleton.h"
#include <string>
#include "Node.h"

using std::mutex;
using std::string;

// singeltone class
class Communicator : public ICommunicator, public Singleton<Communicator>
{
public:
	static void setPort(int port) { _port = port; }

	// delete copy constuctor and assign operator
	Communicator(Communicator&) = delete;
	void operator=(const Communicator&) = delete;
	// destructor
	~Communicator() { Node::log("Communicator destroyed", "communicator.txt"); }
	void run();
	

	// methods
	void sendMessage(const string& str);
	void sendMessage(const BYTES& bytes) override;
	BYTES getNextMessage() override;
	string getPort () { return std::to_string(_port); }

private:
	Communicator();
	// variables
	mutex _connectionMtx;
	// sockets
	SOCKET _clientSocket;
	
	// methods
	

	// connection methods
	int bindAndListen(const SOCKET& serverSocket);
	int acceptClient(const SOCKET& serverSocket);

	static int _port;

	friend class Singleton<Communicator>;
};
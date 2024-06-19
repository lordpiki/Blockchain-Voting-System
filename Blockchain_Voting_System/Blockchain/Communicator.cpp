#include "Communicator.h"
#include <iostream>
#include "Node.h"

int Communicator::_port = 5100;

Communicator::Communicator()
{
	run();
}

// TO DO (if the message is empty ignore it)
void Communicator::sendMessage(const BYTES& bytes)
{
	if (bytes.empty())
		return;

	int messageSize = bytes.size();

	// Add message size to buffer.
	char* messageBuffer = new char[messageSize + sizeof(int)];
	memcpy(messageBuffer, &messageSize, sizeof(int));

	// Add message bytes to buffer.
	memcpy(&messageBuffer[sizeof(int)], bytes.data(), messageSize);

	// Send the messgae.
	{
		std::lock_guard<mutex> lock(_connectionMtx);
		send(_clientSocket, messageBuffer, messageSize + sizeof(int), 0);
	}
	std::cout << "Message sent (size: " << messageSize << "):" << string(bytes.begin(), bytes.end()) << std::endl;
}

void Communicator::run()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed" << std::endl;
		throw("WSAStartup failed");
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Failed to create socket" << std::endl;
		throw("Failed to create socket");
	}

	bindAndListen(serverSocket);
}

void Communicator::sendMessage(const string& msg)
{
	sendMessage(BYTES(msg.begin(), msg.end()));

}

BYTES Communicator::getNextMessage()
{
    char sizeBuffer[4];
    recv(_clientSocket, sizeBuffer, sizeof(int), 0);

    // Convert the size from a 4-byte binary string to an integer
    int messageSize = ntohl(*reinterpret_cast<int*>(sizeBuffer));

    // Receive the actual message
    char* messageBuffer = new char[messageSize];
    recv(_clientSocket, messageBuffer, messageSize, 0);

    BYTES receivedMessage(messageBuffer, messageBuffer + messageSize);

    // Cleanup dynamically allocated buffer
    delete[] messageBuffer;

    return receivedMessage;
}

int Communicator::bindAndListen(const SOCKET& serverSocket)
{
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(_port);

	std::cout << "Binding to port " << _port << std::endl;

	if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Bind failed" << std::endl;
		closesocket(serverSocket);
		return 1;
	}

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen failed" << std::endl;
		closesocket(serverSocket);
		return 1;
	}

	std::cout << "Waiting for a connection..." << std::endl;
	acceptClient(serverSocket);
}

int Communicator::acceptClient(const SOCKET& serverSocket)
{
	_clientSocket = accept(serverSocket, NULL, NULL);
	if (_clientSocket == INVALID_SOCKET) {
		std::cerr << "Accept failed" << std::endl;
		closesocket(serverSocket);
		return 1;
	}
	std::cout << "Accepted client" << std::endl;
}
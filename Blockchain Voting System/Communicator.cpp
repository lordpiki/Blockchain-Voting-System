#include "Communicator.h"
#include <iostream>
#pragma comment(lib, "ws2_32.lib")



using std::cout;
using std::endl;
#define PORT 8876

Communicator::Communicator(int port)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed" << std::endl;
		throw("WSAStartup failed");
	}

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Failed to create socket" << std::endl;
		throw("Failed to create socket");
	}

	bindAndListen();
}

Communicator::~Communicator()
{
	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();

}

void Communicator::sendMsg(const string& message)
{
	send(clientSocket, message.c_str(), message.length(), 0);
	cout << "Sent msg: " << message << endl;
}

string Communicator::recieveMsg()
{
	char buffer[1024];
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (bytesRead > 0)
	{
		buffer[bytesRead] = '\0';
		return string(buffer);
	}
}

int Communicator::bindAndListen()
{
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(PORT);

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
	acceptClient();
}

int Communicator::acceptClient()
{
	clientSocket = accept(serverSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Accept failed" << std::endl;
		closesocket(serverSocket);
		return 1;
	}
}
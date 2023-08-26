#pragma once

#include <WinSock2.h>
#include <map>
#include <vector>
#include <condition_variable>
#include <mutex>


#include <Windows.h>

using std::string;

class Communicator
{
public:
    Communicator(int port);
    ~Communicator();

    void sendMsg(const string& message);
    string recieveMsg();


private:

    int bindAndListen();
    int acceptClient();

    SOCKET serverSocket;
    SOCKET clientSocket;

};
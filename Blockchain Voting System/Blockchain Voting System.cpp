#include <iostream>
#include <WinSock2.h>
#include "Communicator.h"

#include <iostream>

struct sockaddr_in pcAddr, laptopAddr;

using std::cout;
using std::endl;

#define PORT 8876
#define PC_IP_ADDRESS 192.168.1.86
#define LAPTOP_IP_ADDRESS 192.168.63.1

int main()
{
    Communicator com(PORT);
    com.sendMsg("PC");
    cout << com.recieveMsg() << endl;
}


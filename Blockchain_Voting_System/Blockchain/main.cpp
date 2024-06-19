#include "User.h"
#include "Communicator.h"
#include <iostream>


int main(int argc, char** argv)
{
    CryptoHelper::init();

    if (argc < 2)
        exit(1);
    Communicator::setPort(std::stoi(argv[1]));

    User user;
    return 0;
}

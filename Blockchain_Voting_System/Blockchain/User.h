#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "Testing.h"
#include "Node.h"
#include "Voter.h"
#include "Miner.h"

class User
{
public:
	User();
	User(int choice);
	void run(int choice);
private:

	void printVotes(const std::map<char, unsigned int>& votes);
	void printHelloMsg();
	int printMenu();
	int safeInput();

	void runVoter();
	void runMiner();

	Blockchain _blockchain;
	Testing _testing;
};


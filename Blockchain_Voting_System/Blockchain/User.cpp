#include "User.h"
#include <iostream>

#define BE_VOTER 1
#define BE_MINER 2
#define RUN_TESTS 3
#define EXIT 4
#define STATS 1
#define COUNT_VOTES 1
#define VOTE 2

using std::cout;
using std::endl;
using std::cin;

User::User()
{
	printHelloMsg();
	run(printMenu());
}

User::User(int choice)
{
	run(choice);
}

void User::run(int choice)
{
	switch (choice)
	{
		case BE_VOTER:
			runVoter();
			break;
		case BE_MINER:
			runMiner();
			break;
		case RUN_TESTS:
			_testing.runTests();
			break;
		case EXIT:
			exit(0);
			break;
	default:
		break;
	}
}

void User::runVoter()
{
	Voter voter;
	cout << "1. count votes, 2. vote, else. exit" << endl;

	int input = 0;
	char candidate = 0;

	voter.run();
	while (input != EXIT)
	{
		cin >> input;
		switch (input)
		{
		case COUNT_VOTES:
			printVotes(voter.countVotes());
			break;
		case VOTE:
			cin >> candidate;
			cout << "voting for candidate: " << candidate << endl;
			voter.vote(candidate);
			voter.log("Voted for candidate: " + candidate, "log.txt");
			cout << "OK" << endl;
			break;
		default:
			input = EXIT;
			voter.log("Exiting voter", "log.txt");
			break;
		}
	}
}

void User::runMiner()
{
	Miner miner;
	miner.run();
	int input = 0;

	while (input != EXIT)
	{
		cout << "Press 1 to get stats:" << endl;
		cin >> input;
		cout << "input: " << input << endl;
		if (input == STATS)
			miner.getCurrStats();
	}
}

void User::printVotes(const std::map<char, unsigned int>& votes)
{
	cout << "{";
	for (auto it = votes.begin(); it != votes.end(); it++)
	{
		cout << "\"" << it->first << "\":" << it->second;
		auto next = it;
		if (++next != votes.end())
			cout << ", ";
	}
	cout << "}" << endl;
}

void User::printHelloMsg()
{
	cout << "Hello! Welcome to the blockchain system!" << endl;
	cout << "Please choose what you want to do!" << endl;
}


int User::printMenu()
{
	int choice = 0;
	cout << "1. Be a voter" << endl;
	cout << "2. Be a miner" << endl;
	cout << "3. Run tests" << endl;
	cout << "4. Exit" << endl;
	cin >> choice;
	cout << "You chose " << choice << endl;
	return choice;
}

int User::safeInput()
{
	int num = 0;
	cin >> num;
	std::cin.ignore(INT_MAX, '\n');
	cout << "input: " << num << endl;
	return num;
}


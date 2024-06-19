#pragma once

#include "Block.h"
#include "Vote.h"
#include "Blockchain.h"
#include <rsa.h>
#include <vector>

using std::vector;

class Testing
{
public:

	Testing();

	// function will run over all the tests and will return true if all of them were successfull, if not it will throw an error
	bool runTests();


private:
	// function pointers
	vector<std::function<bool(Testing&)>> _functionPointers;
	
	// Test functions
	bool testConnection() const;
	bool testVote() const;
	bool testBlock() const;
	bool testDeserializerAndSerializer() const;
	bool testBlockchain() const;
	bool testSingleton() const;
	bool testIBD() const;
	bool testVoterAndMiner() const;
	bool testPublishBlock() const;

	// helper functions
	template <typename T>
	bool printTestMessage(const string& message, const T& axcepteOutput, const T& realOutput) const;
	Vote createDefualtVote() const;
	Block createDefualtBlock() const;
	Block createGenesisBlock() const;
	void printVotes(const map<char, unsigned int>& votes) const;

	Blockchain createDefaultBlockchain() const;
};


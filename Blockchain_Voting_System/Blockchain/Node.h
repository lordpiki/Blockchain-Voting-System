#pragma once
#include "ICommunicator.h"
#include "Blockchain.h"
#include "TestCommunicator.h"
#include <chrono>

class Node
{
public:
	// ctor
	Node();
	Node(Blockchain& chain) : _blockchain(chain), _communicator(TestCommunicator::getInstance()), _identifier(rand()) {}
	~Node() { log("Node destroyed", "Node.txt"); }

	// methods
	virtual void run();
	map<char, unsigned int>  countVotes() { return _blockchain.countVotes(); }
	int getChainLength() { return _blockchain.getSize(); }
	list<Block> getBlocks() { return _blockchain.getBlocks(); }
	
	// test log function
	static void log(const string& msg, const string& fileName);


protected:
	// variables
	ICommunicator& _communicator;
	Blockchain _blockchain;
	std::mutex _blockchainMtx;
	std::mutex _timerMtx;
	std::chrono::time_point<std::chrono::system_clock> _timer;
	int _identifier;

	void listen();

	// handle general message
	void handleMessage(const BYTES& bytes);

	// specific message handlers
	BYTES handleIBDRequest(const BYTES& bytes);
	BYTES handlePublishBlock(const BYTES& bytes);
	BYTES handleIBDResponse(const BYTES& bytes);
	virtual BYTES handlePublishVote(const BYTES& bytes) { return BYTES(); }
	virtual BYTES handleSignedPk(const BYTES& bytes) { return BYTES(); }

	// IBD process monitor
	void monitorIBD();
	void resetTimer();
};
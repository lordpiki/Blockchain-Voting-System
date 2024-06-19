#pragma once
#include "ICommunicator.h"
#include "Singleton.h"
#include <queue>


using std::mutex;
using std::queue;

// singeltone class
class TestCommunicator : public ICommunicator, public Singleton<TestCommunicator>
{
public:
	// delete copy constuctor and assign operator
	TestCommunicator(TestCommunicator&) = delete;
	void operator=(const TestCommunicator&) = delete;

	// destructor
	~TestCommunicator() {}

	// methods
	void sendMessage(const BYTES& bytes) override;
	BYTES getNextMessage() override;

	// getters and setters
	void setTest(int value) { _test = value; }
	int getTest() const { return _test; }

private:
	// methods
	TestCommunicator();

	// variables
	// _connection;
	queue<BYTES> _messages;
	mutex _connectionMtx;
	int _test;

	friend class Singleton<TestCommunicator>;
};
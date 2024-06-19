#pragma once
#include <vector>
#include <mutex>

#define BYTES std::vector<char>
using std::mutex;

class ICommunicator
{
public:
	virtual void sendMessage(const BYTES& bytes) = 0;
	virtual BYTES getNextMessage() = 0;

protected:
	// _connection;
	mutex _connectionMtx;

};
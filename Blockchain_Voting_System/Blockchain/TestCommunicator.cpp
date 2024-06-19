#include "TestCommunicator.h"

std::condition_variable cv;

void TestCommunicator::sendMessage(const BYTES& bytes)
{
    // Add meseges to message queue

    if (bytes.size() == 0)
        return;
	{
		std::lock_guard<mutex> lock(_connectionMtx); 
		_messages.push(bytes);
	}
	cv.notify_one();
}
 
BYTES TestCommunicator::getNextMessage()
{
    // Read messages from queue

    BYTES bytes;

    std::unique_lock<std::mutex> ul(_connectionMtx);
    cv.wait(ul, [this] { return !_messages.empty(); });

    bytes = _messages.front();
    _messages.pop();

    cv.notify_one();

    return bytes;
}

TestCommunicator::TestCommunicator() : _test(0)
{
	 
}
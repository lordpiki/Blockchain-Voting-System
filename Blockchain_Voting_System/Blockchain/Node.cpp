#include "Node.h"
#include "Codes.h"
#include "Deserializer.h"
#include "Serializer.h"
#include "Communicator.h"
#include <thread>
#include <fstream>

#define CODE_INDEX 0
#define CODE_SIZE 1
#define IBD_TIME_SPENT 3   

Node::Node() : _communicator(Communicator::getInstance()), _identifier(rand())
{
}

void Node::run()
{
    {
        std::lock_guard<mutex> lock(_timerMtx);
        _timer = std::chrono::system_clock::now() + std::chrono::seconds(10);
    }

    std::thread listener(&Node::listen, this);
    listener.detach();

    std::thread IBDthread(&Node::monitorIBD, this);
    IBDthread.detach();
}

void Node::log(const string& msg, const string& fileName)
{
    std::ofstream file;
    string port = Communicator::getInstance().getPort();
    string portFileName = port + fileName;
    file.open(portFileName, std::ios::app);
    file << msg << std::endl;
    file.close();
}

void Node::listen()
{
    while (true)
    {
        // when receiving a message create a thread that handles it
        std::thread handler(&Node::handleMessage, this, _communicator.getNextMessage());
        handler.detach();
    }
}

void Node::handleMessage(const BYTES& bytes)
{
    BYTES response;

    try
    {
        // Remove the first byte from bytes
        switch (bytes[CODE_INDEX])
        {
        case Codes::IBD: response = handleIBDRequest(bytes); break;
        case Codes::IBD_RESPONSE: response = handleIBDResponse(bytes); break;
        case Codes::PUBLISH_BLOCK: response = handlePublishBlock(bytes); break;
        case Codes::PUBLISH_VOTE: response = handlePublishVote(bytes); break;
        case Codes::GET_SIGNED_PK_RESPONSE: response = handleSignedPk(bytes); break;
        default:
            // code is not supported
            break;
        }
        _communicator.sendMessage(response);
    }
    catch (const char* errorMessage)
    {
        std::cout << errorMessage << std::endl;
    }
}

BYTES Node::handleIBDRequest(const BYTES& bytes)
{
    // deserialize request
    std::pair<BYTE, string> pair = Deserializer::deserializeIBDRequest(bytes);

    // lock mutex
    std::lock_guard<mutex> lock(_blockchainMtx);

    // get blocks
    list<Block> blocks = _blockchain.getBlocksByHash(pair.second);

    return Serializer::serialize(pair.first, blocks);
}

BYTES Node::handlePublishBlock(const BYTES& bytes)
{
    list<Block> blocks = Deserializer::deserializePublishBlocks(bytes);

    // lock mutex
    std::lock_guard<mutex> lock(_blockchainMtx);

    _blockchain.insertBlocks(blocks);

    // reset IBD timer
    resetTimer();

    // return empty vector - No response
    return BYTES();
}

BYTES Node::handleIBDResponse(const BYTES& bytes)
{
    std::pair<BYTE, list<Block>> pair = Deserializer::deserializeIBDResponse(bytes);
    if (pair.first != _identifier)
        throw "identifier doesn't match";
    int size = 0;

    // lock mutex
    std::lock_guard<mutex> lock(_blockchainMtx);
    size = _blockchain.getSize();
    // insert new blocks to the chain
    _blockchain.insertBlocks(pair.second);

    if (_blockchain.getSize() == size)
    {
        throw "size bag";
    }
    // reset IBD timer
    resetTimer();

    // return request for the next blocks
    return Serializer::serialize(_identifier, _blockchain.getFirstBlock().getBlockHash());
}

void Node::monitorIBD()
{
    std::chrono::seconds spent(0);
    while (true)
    {
        std::this_thread::sleep_for(spent);
        {
            std::lock_guard<mutex> lock(_timerMtx);
            if (std::chrono::system_clock::now() < _timer)
            { 
                spent = std::chrono::duration_cast<std::chrono::seconds>(_timer - std::chrono::system_clock::now()) + std::chrono::seconds(1);
                log("sleeping for " + std::to_string(std::chrono::duration_cast<std::chrono::duration<float>>(spent).count() / 60) + " minutes.", "log.txt");
                continue;
            }
        }
        // create IBD request to download chain
        {
            std::lock_guard<mutex> lock(_blockchainMtx);
            _communicator.sendMessage(Serializer::serialize(_identifier, _blockchain.getFirstBlock().getBlockHash()));
        }
        log("IBD request sent", "log.txt");
        resetTimer();
    }
}

void Node::resetTimer()
{
    {
        std::lock_guard<mutex> lock(_timerMtx);
        _timer = std::chrono::system_clock::now() + std::chrono::minutes(IBD_TIME_SPENT);
    }
}
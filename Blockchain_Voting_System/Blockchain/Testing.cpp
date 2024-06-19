#include "Communicator.h"
#include "Testing.h"
#include "Serializer.h"
#include "Deserializer.h"
#include <iostream>
#include <cryptlib.h>
#include <osrng.h>
#include <base64.h>
#include <string>
#include "Vote.h"
#include "Serializer.h"
#include <cstdlib>
#include <ctime> 
#include "TestCommunicator.h"
#include "Node.h"
#include "Miner.h"
#include "Voter.h"


#define WAITING_TIME 3
using std::cout;
using std::endl;

Testing::Testing()
{
	// adding all the tests
	//_functionPointers.push_back(&Testing::testConnection);
    _functionPointers.push_back(&Testing::testIBD);
    _functionPointers.push_back(&Testing::testPublishBlock);
	_functionPointers.push_back(&Testing::testVote);
	_functionPointers.push_back(&Testing::testBlock);
	_functionPointers.push_back(&Testing::testDeserializerAndSerializer);
    _functionPointers.push_back(&Testing::testBlockchain);
    _functionPointers.push_back(&Testing::testSingleton);
}

bool Testing::runTests() 
{
	int functionNum = 0;
    bool generalStatus = true;
    clock_t start = clock(); // starting clock
    clock_t end;

    try
    {
        for (const auto& func : _functionPointers)
        {
            // running func
            generalStatus = generalStatus && func(*this);
            functionNum++;

        }
    }
    catch (const char* errorMessage) {
        std::cerr << "Caught an error: " << errorMessage << std::endl;
        generalStatus = false;
    }
	
    // ending clock
    end = clock(); 

    printTestMessage("All tests passed?", true, generalStatus);
    cout << "Time to run tests: " << (end - start) / (double)CLOCKS_PER_SEC << "s" << endl;
	return true;
}

bool Testing::testConnection() const
{
// testing connection

    Communicator& communicator = Communicator::getInstance();
    string message = "Hello from client";
    communicator.sendMessage(BYTES(message.begin(), message.end()));
    BYTES response = communicator.getNextMessage();
    
    
    bool status = printTestMessage("Testing connection", string(response.begin(), response.end()), string("Hello from Python!"));
	

    return status;
}

bool Testing::testVote() const
{
    char canditate = 7;
    int res = 0;
    bool status = true;

    RSA::PrivateKey sk = CryptoHelper::generatePrivateKey();
    RSA::PublicKey pk = CryptoHelper::generatePublicKey(sk);

    // sign client public key
    string signedPublicKey = CryptoHelper::signPK(pk);

    // test vote creation
    Vote vote(canditate, sk, pk, signedPublicKey);
    cout << "Vote:" << vote.toString() << endl;
    Deserializer::deserializePublishVote(Serializer::serialize(vote));
    res = vote.Verify(CryptoHelper::getGovPk());
    cout << "Gov pk: " << CryptoHelper::publicKeyToString(CryptoHelper::getGovPk()) << endl;
    status = status && printTestMessage("Testing Vote first constructor and verefication", 1, res);

    // test vote verefication
    string signature = vote.getSignature();
    Vote vote2(canditate, signature, pk, signedPublicKey);
    res = vote.Verify(CryptoHelper::getGovPk());
    status = status && printTestMessage("Testing Vote second constructor and verefication", 1, res);

	return status;
}

bool Testing::testBlock() const
{
    int res = 0;
   
    Block block = createDefualtBlock();
    bool status = printTestMessage("Testing false hash", false, block.Verify(CryptoHelper::getGovPk()));

    cout << endl << "Mining block ~1 min";
    block.Mine();
    status = status && printTestMessage("Testing verification after mining", true, block.Verify(CryptoHelper::getGovPk()));

	return status;
}

// Serializers test
// Object -> Serializer -> Bytes -> Deserializer -> object 
// // If original object = recieved object: test passed
bool Testing::testDeserializerAndSerializer() const
{
    // getting first vote & block
	Vote originalVote(createDefualtVote());
	Block originalBlock(createDefualtBlock());
    char originalCode = 0x01;

    // converting to bytes
	BYTES voteBytes = Serializer::serialize(originalVote);
	BYTES blockBytes = Serializer::serialize(originalBlock);
    cout << "voteBytes size: " << voteBytes.size() << endl;

    // converting back to blocks
	Vote recievedVote(Deserializer::deserializePublishVote(voteBytes));
	Block recievedBlock(Deserializer::deserializeBlock(blockBytes));

    // testing equality of vote & block
    bool status = printTestMessage("Testing original vote = recieved vote", true, recievedVote == originalVote);
    status = status && printTestMessage("Testing original block = recieved block", true, recievedBlock == originalBlock);

	return status;
}

bool Testing::testBlockchain() const
{
    Blockchain blockchainFull(createDefaultBlockchain());
    Blockchain blockchainHalfFull(blockchainFull);
    Blockchain blockchainEmpty(blockchainFull);
    blockchainHalfFull.removeBlocks(3);
    blockchainEmpty.removeBlocks(5);
 
    blockchainHalfFull.insertBlocks(blockchainFull.getBlocksByHash(blockchainHalfFull.getFirstBlock().getBlockHash()));
    bool status = printTestMessage("All sizes should be same", true, blockchainFull.getSize() == blockchainHalfFull.getSize());

    blockchainEmpty.insertBlocks(blockchainFull.getBlocksByHash(blockchainEmpty.getFirstBlock().getBlockHash()));
    status = status && printTestMessage("All sizes should be same", true, blockchainFull.getSize() == blockchainEmpty.getSize());


    // should be roughly disterbuted
    cout << endl;
    for (const auto& it : blockchainFull.countVotes())
    {
        cout << "Candidate number #" << it.first << " : " << it.second << endl;
    }



    return status;
}

bool Testing::testSingleton() const
{
    // Create refrences
    TestCommunicator& ref1 = TestCommunicator::getInstance();
    TestCommunicator& ref2 = TestCommunicator::getInstance();

    // Setting setting different test value to both refrences
    ref1.setTest(2);
    ref2.setTest(4);

    return printTestMessage("Checking if refrences point to the same object", true, ref1.getTest() == ref2.getTest());
}

bool Testing::testIBD() const 
{ 
    Blockchain chain = createDefaultBlockchain();
    Blockchain empty;
    bool status = true;
    bool test = true;

    list<Block> blocks = chain.getBlocksByHash(empty.getFirstBlock().getBlockHash());

    // Check chain download
    empty.insertBlocks(blocks);
    status = printTestMessage("Checking chain download", true, chain.getSize() == empty.getSize());

    try
    {
        // Try redownload the same blocks. Should raise an exception.
        empty.insertBlocks(blocks);
        test = false;
    }
    catch (const char* errorMessage) {}
    status = status && printTestMessage("Test redoawnloading chain:", true, test);

    test = true;
    blocks.pop_front();
    try 
    {
        // Try download the end of the chain. Should raise an exception.
        empty.insertBlocks(blocks);
        test = false;
    }
    catch (const char* errorMessage) {}
    status = status && printTestMessage("Test downloading empty chain:", true, test);

    return status; 
}
bool Testing::testVoterAndMiner() const 
{ 
    // Create empty blockchain: with only the genesis block
    Blockchain blockchain;

    Miner miner(blockchain);
    Voter voter1(blockchain);
    Voter voter2(blockchain);
    Voter voter3(blockchain);
    
    miner.run();
    
    voter1.vote((char)1);
    voter2.vote((char)2);
    voter3.vote((char)3);
    
    std::this_thread::sleep_for(std::chrono::minutes(WAITING_TIME));

    printVotes(miner.countVotes());
    return true; 
}

bool Testing::testPublishBlock() const
{
    // Create identical chains
    Blockchain sender(createDefaultBlockchain());
    Blockchain reciver(sender);

    // Insert additional blocks to the sender chain
    string privHash = sender.getFirstBlock().getBlockHash();
    list<Block> blocks;
    for (int i = 0; i < 3; i++)
    {
        Block block = createDefualtBlock();
        block.setPreviousHash(privHash);
        block.Mine();

        privHash = block.getBlockHash();
        blocks.push_front(block);
    }
    sender.insertBlocks(blocks);

    // Download the addtional blocks from the sender to the reciver
    reciver.insertBlocks(Deserializer::deserializePublishBlocks(Serializer::serialize(sender.getBlocksToPublish())));

    return printTestMessage("Test publish blocks", true, sender.getSize() == reciver.getSize());
}

template <typename T>
bool Testing::printTestMessage(const string& message, const T& expectedOutput, const T& realOutput) const
{
    // colors for testing
    const char* redColor = "\033[31m";
    const char* greenColor = "\033[32m";
    const char* resetColor = "\033[0m";
    bool status = expectedOutput == realOutput;

    cout << endl << message << endl;
    cout << "Expected output: " << expectedOutput << ", real output: " << realOutput << endl;
    cout << "status: " << ((status) ? greenColor : redColor ) << ((status) ? "success" : "failure") << resetColor << endl;
    return expectedOutput == realOutput;
}

Vote Testing::createDefualtVote() const
{
    srand(time(0));

    // get random candidate num from 1 - 10
    char candidate = (rand() % 10) + '0';

    RSA::PrivateKey sk = CryptoHelper::generatePrivateKey();
    RSA::PublicKey pk = CryptoHelper::generatePublicKey(sk);
    return Vote(candidate, sk, pk, CryptoHelper::signPK(pk));
}

Block Testing::createDefualtBlock() const 
{
    vector<Vote> votes;
    for (int i = 0; i < 5; i++)
        votes.push_back(createDefualtVote());

    Block tempBlock("TempHash", vector<Vote>(), 0);
    return Block(tempBlock.getBlockHash(), votes, 0);
}

Block Testing::createGenesisBlock() const
{
    // genesis's blocks hash is empty
    Block genesis("", vector<Vote>(), 0);
    genesis.Mine();
    return genesis;

}

Blockchain Testing::createDefaultBlockchain() const
{
    Blockchain blockchain;
    list<Block> blocks;

    
    cout << "Creating blockchain.. ~1 min" << endl;

    for (int i = 0; i < 5; i++)
    {
        Block block(createDefualtBlock());
        if (!blocks.empty())
            block.setPreviousHash(blocks.front().getBlockHash());
        else
            block.setPreviousHash(createGenesisBlock().getBlockHash());
        block.Mine();
        blocks.push_front(block);
    }
      blockchain.insertBlocks(blocks);

    return blockchain;
}

void Testing::printVotes(const map<char, unsigned int>& votes) const
{
    for (const auto& it : votes)
    {
        cout << "Candidate number #" << it.first << " : " << it.second << endl;
    }
}
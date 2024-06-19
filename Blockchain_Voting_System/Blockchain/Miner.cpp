#include "Miner.h"
#include "Codes.h"
#include "Deserializer.h"
#include <iostream>
#include <fstream>

#define MAX_VOTES 255
#define WAITING_TIME 15

using std::cout;
using std::endl;

Miner::Miner() : _isMining(false) {}
Miner::Miner(Blockchain& blockchain) : Node(blockchain), _isMining(false) {}

void Miner::run()
{
	Node::run();
	std::cout << "Miner is running" << std::endl;

	std::thread miner(&Miner::mine, this);
	miner.detach();
}

void Miner::getCurrStats()
{
	int counter = 0;
	cout << endl << "isMining:" << std::to_string(_isMining) << ";";
	cout << "votesInQueue:" << _votes.size() << ";";
	cout << "blockCount:" << _blockchain.getSize() << endl;

	cout << "Blockchain start with :" << _blockchain.getSize() << "Blocks" << endl;
	for (const Block& block : _blockchain.getBlocks())
	{
		counter++;
		cout << "Block: " << _blockchain.getSize() - counter << ";";
		cout << "Votes: " << block.getVotes().size() << ";";
		cout << "Hash: " << block.getBlockHash() << ";";
		cout << "PrevHash: " << (block.GetPreviousHash().size() == 0 ? "None" : block.GetPreviousHash()) << endl;
	}
	cout << "Blockchain end" << endl;
}

void Miner::mine()
{
	// Perm thread, running as long as the miner runs.
	while (true)
	{
		// Votes vector - contains all the votes that will be in the next block.
		vector<Vote> votes;
		// Wait for votes to be published
		std::this_thread::sleep_for(std::chrono::seconds(WAITING_TIME));

		if (_votes.size() == 0)
		{
			continue;
		}
		
		{
			// Insert votes to the vector.
			std::lock_guard<mutex> lock(_voteMtx);
			while (!_votes.empty() && votes.size() < MAX_VOTES)
			{
				std::lock_guard<mutex> blockchainLock(_blockchainMtx);

				//checking if user already voted
				if (!_blockchain.checkKeyStr(_votes.front().getPublicKeyString()))
					votes.push_back(_votes.front());

				_votes.pop();
			}
		}

		
		// Create the block and mine it.
		Block block(_blockchain.getFirstBlock().getBlockHash(), votes, 0);
		if (votes.size() == 0)
		{
			continue;
		}
		
		_isMining = true;
		block.Mine();
		_isMining = false;


		// Insert the new block into list object so it could be inserted to the chain.
		list<Block> list;
		list.push_back(block);

		try
		{
			// Try inserting the new block, if the insertion was successfull publish the block.
			_blockchain.insertBlocks(list);
			publishBlocks();
		}
		catch (const char* errMessage)
		{
			std::cout << errMessage << std::endl;
		}
	}
}

void Miner::publishBlocks()
{
	// Publish the newest block in the chain.
	_communicator.sendMessage(Serializer::serialize(_blockchain.getBlocksToPublish()));
}

BYTES Miner::handlePublishVote(const BYTES& bytes)
{

	Vote vote = Deserializer::deserializePublishVote(bytes);

	if (!vote.Verify(CryptoHelper::getGovPk()))
	{
		return BYTES();
	}

	{
		// _blockchainMtx scope
		// TODO: check if the key already exists in votes
		std::lock_guard<mutex> lock(_blockchainMtx);
		if (_blockchain.checkKeyStr(vote.getPublicKeyString()))
		{
			return BYTES();
		}

		if (!_votes.empty())
		{
			std::lock_guard<mutex> lock(_voteMtx);
			Vote firstVote = _votes.front();
			_votes.pop();
			while (!_votes.empty() && !(_votes.front() == firstVote))
			{
				Vote currentVote = _votes.front();
				_votes.pop();

				if (vote.getPublicKeyString() == currentVote.getPublicKeyString())
				{
					return BYTES();
				}
				_votes.push(currentVote);
			}
			_votes.push(firstVote);
		}
	}

	{
		// _voteMtx scope
		std::lock_guard<mutex> lock(_voteMtx);
		_votes.push(vote);

	}

	// return empty message - No response
	return BYTES();
}



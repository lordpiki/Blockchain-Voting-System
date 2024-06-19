#include "Blockchain.h"
#include "CryptoHelper.h"
#include <algorithm>
#include "Node.h"


#define GET_BLOCKS_AMOUNT 5

Blockchain::Blockchain()
{
	// Create Genesis block
	Block genesis("", vector<Vote>(), 0);
	genesis.Mine();

	_blockchain.push_back(genesis);
}

map<char, unsigned int> Blockchain::countVotes()
{
	// Map to store the count of votes for each candidate
	map<char, unsigned int> voteCountMap;

	// go over all blocks
	for (const Block& block : _blockchain)
	{
		// go over all votes in block
		for (const Vote& vote : block.getVotes())
		{
			// add count to candidate
			voteCountMap[vote.getCandidate()]++;
		}
	}

	return voteCountMap;
}

void Blockchain::insertBlocks(const list<Block>& blocks)
{
	int count = 0;
	
	// checking chain list
	if (!checkChain(blocks))
	{
		throw("Chain isn't valid");
	}

	// if blockchain is empty
	if (_blockchain.end() == _blockchain.begin())
	{
		for (auto block = blocks.begin(); block != blocks.end(); block++)
		{
			_blockchain.push_back(*block);
			updateHashSet(*block);
		}
		return;
	}
	// going over _blockchain to find matching hash
	for (auto it = _blockchain.begin() ; it != _blockchain.end(); it++)
	{
		if (count == GET_BLOCKS_AMOUNT || count > blocks.size())
			throw("Block isn't in the last 5 blocks");
		
		// if previousHash was found, remove all the blocks previous to that and add from the list
		if (it->getBlockHash() == blocks.back().GetPreviousHash())
		{
			// removing all the blocks from the beginning till it
			removeBlocks(it); 
			
			// adding all the blocks from recieved blocks to _blockchain
			for (auto tempIt = --blocks.end();; tempIt--)
			{
				_blockchain.push_front(*tempIt);
				// updating hashset 
				// (Possible bug to fix later: a person who was counted in a block that got removed, but isn't in the recieved block, won't get his hash removed from the set (Very unlikely scenario that it can happen cuz it requires impersination which is very difficult))
				updateHashSet(*tempIt);
				if (tempIt == blocks.begin())
					break;
			}
			return;
		}
		count++;
	}
	
}

list<Block> Blockchain::getBlocksByHash(const string& hash)
{
	// TODO: Make sure to never send the genesis block
	list<Block> blocks;

	for (auto it = _blockchain.begin(); it != _blockchain.end(); it++)
	{
		if (it->GetPreviousHash() == hash)
		{
			blocks.push_front(*(it));

			for (int i = 0; i < GET_BLOCKS_AMOUNT - 1 && it != _blockchain.begin(); i++)
				blocks.push_front(*(--it));

			return blocks;
		}
	}
	throw("Block not found");
}

void Blockchain::removeBlocks(int count)
{
	if (count > _blockchain.size() - 1)
	{
		throw("Amount of blocks to remove bigger than the amount of blocks");
	}

	
	list<Block>::iterator it = _blockchain.begin();
	for (int i = 0; i < count; i++)
	{
 		for (const Vote& vote : it->getVotes())
		{
			// calculate hash of each person's publicKey
			string hash(CryptoHelper::calcualteSHA256(vote.getPublicKeyString()));

			if (_hashSet.find(hash) != _hashSet.end())
				_hashSet.erase(hash);
		}
		auto tempIt = it;
		it++;
		_blockchain.erase(tempIt);
	}
}

list<Block> Blockchain::getBlocksToPublish()
{
	list<Block>::iterator it = _blockchain.begin();
	list<Block> blocks;
	for (int count = 0; count < GET_BLOCKS_AMOUNT; count++)
	{
		if (it == --_blockchain.end())
			break;
		blocks.push_back(*it);
		it++;
	}
	return blocks;
}

void Blockchain::removeBlocks(const list<Block>::iterator& it)
{
	removeBlocks(std::distance(_blockchain.begin(), it));
}

void Blockchain::updateHashSet(const Block& block)
{
	// going over all uniqe hashes in 
	for (const Vote& vote : block.getVotes())
	{
		// calculate hash of each person's publicKey
		string hash(CryptoHelper::calcualteSHA256(vote.getPublicKeyString()));

		if (_hashSet.find(hash) != _hashSet.end())
			throw("Voter already in the system");

		// inserting hash if person isn't in the system
		_hashSet.insert(hash);

	}
}

bool Blockchain::checkKeyStr(const string& pkStr) const
{
	return _hashSet.find(CryptoHelper::calcualteSHA256(pkStr)) != _hashSet.end();
}

bool Blockchain::checkChain(const list<Block>& list) const
{
	for (auto it = list.begin(); it != --list.end();)
	{
		if (!it->Verify(CryptoHelper::getGovPk()))
			return false;
		if (it->GetPreviousHash() == "" && it == --list.end())
		{
			return true;
		}

		if (it->GetPreviousHash() != (++it)->GetPreviousHash())
		{
			return false;
		}
	}
	return true;
}
#pragma once

#include <string>
#include <map>
#include <unordered_set>
#include <list>

#include "Block.h"

using std::string;
using std::map;
using std::unordered_set;
using std::list;

class Blockchain
{
public:

	Blockchain();

	// count all the votes in the blockchain and return a map of candidate : voteCount
	map<char, unsigned int> countVotes();

	// inserting a block to the list
	void insertBlocks(const list<Block>& blocks);

	// search blocks and return a the 5 blocks before it
	list<Block> getBlocksByHash(const string& hash);

	bool checkKeyStr(const string& pkStr) const;
	list<Block> getBlocks() const { return _blockchain; }
	unordered_set<string> getHashSet() const { return _hashSet; }

	// TEMP FUNCTION for testing blockchain
	Block getFirstBlock() const { return _blockchain.front(); }
	Block getLastBlock() const { return _blockchain.back(); }

	list<Block> getBlocksToPublish();
	void removeBlocks(int count);
	void removeBlocks(const list<Block>::iterator& it);
	int getSize() const { return _blockchain.size(); }

private:
	// data structures
	list<Block> _blockchain;
	unordered_set<string> _hashSet;

	// helper functions
	bool checkChain(const list<Block>& list) const;
	void updateHashSet(const Block& block);
};


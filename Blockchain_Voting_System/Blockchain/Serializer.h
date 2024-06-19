#pragma once
#include <vector>
#include <string>
#include "Block.h"
#include "Vote.h"
#include "Codes.h"
#include "Blockchain.h";

#define BYTES std::vector<char>

using std::string;
using std::list;

class Serializer 
{
public:
	// Serializer functions
	static BYTES serialize(int identifier, const string& prevHash); // ibd request
	static BYTES serialize(int identifier, const list<Block>& blockList); // ibd response
	static BYTES serialize(const list<Block>& blocks); // publish blocks
	static BYTES serialize(const Vote& vote); // publish vote
	static BYTES serialize(const RSA::PublicKey& pk); // for testing

	// for testing
	static BYTES serialize(const Block& block);


private:
	// object serializers
	static BYTES serializeVote(const Vote& vote);
	static BYTES serializeBlock(const Block& block);
	static BYTES serializeBlocklist(const list<Block> blockList);


	static void insertNumber(BYTES& bytes, int number); // insert number into bytes

	// function to insert a vector/str into another vector more easily
	static void insertIntoVector(BYTES& bytes, const string& str);
	static void insertIntoVector(BYTES& bytes, const BYTES& vector);

	// turns all votes of block into bytes
	static BYTES getVotesBytes(const Block& block);
	
};


#pragma once
#include "Vote.h"
#include "Block.h"
#include "Blockchain.h"
#include "Serializer.h"
#include <vector>
#include <utility>
#include <string>
#include "CryptoHelper.h"

#define BYTES std::vector<char>
using std::pair;

class Deserializer
{
public:
	// get the bytes without the codes
	static pair<int, string> deserializeIBDRequest(const BYTES& bytes); // when client wants to get block from other clients
	static pair<int ,list<Block>> deserializeIBDResponse(const BYTES& bytes); // when client gets block from other clients
	static list<Block> deserializePublishBlocks(const BYTES& bytes); // when miner finishes mining block, he sends it to everyone
	static Vote deserializePublishVote(const BYTES& bytes);
	static RSA::PublicKey deserializePublicKey(const BYTES& bytes);
	

	// helper functions & testing
	static Vote deserializeVote(const BYTES& bytes); // when client votes, he sends it to everyone
	static Block deserializeBlock(const BYTES& bytes);
	static list<Block> deserializeBlockList(const BYTES& bytes);

private: 
	// defult index for if the vector gets the number from the middle
	static int getNumber(const BYTES& bytes, unsigned int index = 0); // extract num from 4 bytes
};
#include "Deserializer.h"
#include "Node.h"

#define SIGNATURE_SIZE 256
#define GOV_SIGNATURE_SIZE 256
#define PUBLIC_KEY_SIZE 392
#define SIGNED_PUBLIC_KEY_SIZE GOV_SIGNATURE_SIZE
#define VOTE_SIZE (1 + SIGNATURE_SIZE + PUBLIC_KEY_SIZE + SIGNED_PUBLIC_KEY_SIZE)
#define HASH_SIZE 64
#define NONCE_SIZE 4
#define COUNT_SIZE 1
#define CODE_SIZE 1
#define MAX_BLOCK_SIZE (HASH_SIZE + 4 + 1 + VOTE_SIZE * 255)

Vote Deserializer::deserializeVote(const BYTES& bytes)
{
	int index = 0;

	// TODO: Remove comment
	if (bytes.size() != VOTE_SIZE)
	{
		throw("bytes length doesn't much required vote size");
	}

	// extract vote params from bytes
	char candidate = bytes[index++];

	string signature(bytes.begin() + index, bytes.begin() + index + SIGNATURE_SIZE);

	index += SIGNATURE_SIZE;

	string publicKeyString(bytes.begin() + index, bytes.begin() + index + PUBLIC_KEY_SIZE);
	index += PUBLIC_KEY_SIZE;

	string signedPublicKey(bytes.begin() + index, bytes.end());

	// convert string key to rsa object
	RSA::PublicKey pk(CryptoHelper::StringToPublicKey(publicKeyString));

	Vote vote(candidate, signature, pk, signedPublicKey);


	return vote;
}

Block Deserializer::deserializeBlock(const BYTES& bytes)
{
	int index = 0;
	vector<Vote> votes;

	// extract prev block's hash
	string prevHash(bytes.begin(), bytes.begin() + HASH_SIZE);
	index += HASH_SIZE;
	
	// extract nonce number from block
	unsigned int nonce = getNumber(bytes, index);
	index += NONCE_SIZE;

	// extract vote count from block
	// TODO: fix to just take the ascii value
	unsigned int voteCount = bytes[index];
	index += COUNT_SIZE;

	if (bytes.size() < index + voteCount * VOTE_SIZE)
		throw("bytes length doesn't much");

	for (unsigned int i = 0; i < voteCount; i++)
	{
		Vote vote(deserializeVote(BYTES(bytes.begin() + index, bytes.begin() + index + VOTE_SIZE))); // getting vote num i from block
		index += VOTE_SIZE; // upping index
		votes.push_back(vote); // inserting vote into votes

	}

	return Block(prevHash, votes, nonce);
}

pair<int, string> Deserializer::deserializeIBDRequest(const BYTES& bytes)
{
	// all the message should just be the hash
	int identifier = getNumber(bytes, CODE_SIZE);
	return { identifier, string(bytes.begin() + sizeof(int) + CODE_SIZE, bytes.end()) };
}

pair<int, list<Block>> Deserializer::deserializeIBDResponse(const BYTES& bytes)
{
	// getting identifier
	int identifier = getNumber(bytes, CODE_SIZE);
	// getting block list from bytes (removing identifier)
	return { identifier, deserializeBlockList({bytes.begin() + sizeof(int) + CODE_SIZE, bytes.end()}) };
}

list<Block> Deserializer::deserializePublishBlocks(const BYTES& bytes)
{
	return deserializeBlockList({ bytes.begin() + CODE_SIZE, bytes.end() });
}

Vote Deserializer::deserializePublishVote(const BYTES& bytes)
{
	return deserializeVote({ bytes.begin() + CODE_SIZE, bytes.end() });
}

RSA::PublicKey Deserializer::deserializePublicKey(const BYTES& bytes)
{
	return CryptoHelper::StringToPublicKey(string(bytes.begin() + CODE_SIZE, bytes.end()));
}


list<Block> Deserializer::deserializeBlockList(const BYTES& bytes)
{
	list<Block> blockList;
	int index = 0;

	// getting block count
	unsigned int blockCount = bytes[index];
	index += COUNT_SIZE;

	for (int i = 0; i < blockCount; i++)
	{
		// getting individual block to serialize 
		// Note, no reason to try and calculate the block size, because the deserializeBlock takes just the block part,
		BYTES blockBytes(bytes.begin() + index, bytes.end());
		Block block(deserializeBlock(blockBytes));

		// pushing back deserialized block
		blockList.push_back(block);
		// getting size of block to up index
		index += Serializer::serialize(block).size();
	}

	//TBH I am in shock that this for loop is working, but it is!

	return blockList;
}

int Deserializer::getNumber(const BYTES& bytes, unsigned int index)
{
	int num = 0;
	memcpy(&num, bytes.data() + index, sizeof(int));
	return num;
}


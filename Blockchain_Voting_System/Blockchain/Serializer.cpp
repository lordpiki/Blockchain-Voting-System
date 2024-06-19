#include "Serializer.h"
#include "Node.h"

// For testing
BYTES Serializer::serialize(const Block& block)
{
    return serializeBlock(block);
}

// ------------ public functions ---------------

// publish vote
BYTES Serializer::serialize(const Vote& vote)
{
    BYTES bytes = { Codes::PUBLISH_VOTE };

    // Serialize vote object
    BYTES voteBytes = serializeVote(vote);
    
    // Insert vote bytes to message
    insertIntoVector(bytes, voteBytes);


    return bytes;
}

BYTES Serializer::serialize(const RSA::PublicKey& pk)
{
    BYTES bytes = { Codes::GET_SIGNED_PK_REQUEST };

    // inserting public key into bytes
    insertIntoVector(bytes, CryptoHelper::publicKeyToString(pk));

    return bytes;
}


// IBD request
BYTES Serializer::serialize(int identifier, const string& prevHash)
{
    BYTES bytes = { Codes::IBD };

    // inserting at start identifier code
    insertNumber(bytes, identifier);

    // inserting hash
    insertIntoVector(bytes, prevHash);

    return bytes;
}


// IBD response
BYTES Serializer::serialize(int identifier, const list<Block>& blockList)
{
    BYTES bytes = { Codes::IBD_RESPONSE };

    // pushing back identifier code
    insertNumber(bytes, identifier);
    
    // Inserting blocklist bytes
    insertIntoVector(bytes, serializeBlocklist(blockList));
    
    return bytes;
}

// Publish blocks
BYTES Serializer::serialize(const list<Block>& blocks)
{
    BYTES bytes = { Codes::PUBLISH_BLOCK };

    insertIntoVector(bytes, serializeBlocklist(blocks));

    return bytes;
}

// ---------- private functions --------------

BYTES Serializer::serializeVote(const Vote& vote)
{
    BYTES bytes;

    // pushing candidate number
    bytes.push_back(vote.getCandidate());


    // inserting signature
    insertIntoVector(bytes, vote.getSignature());

    // inserting publicKey
    insertIntoVector(bytes, vote.getPublicKeyString());

    // inserting signedPublicKey
    insertIntoVector(bytes, vote.getSignedPublicKey());

    return bytes;
}
BYTES Serializer::serializeBlock(const Block& block)
{
    BYTES bytes;
    BYTES voteBytes = getVotesBytes(block);

    // inserting previous hash of block, to this block bytes
    insertIntoVector(bytes, block.GetPreviousHash());

    // getting nonce number into bytes
    unsigned int pow = block.GetPOW();

    bytes.resize(bytes.size() + 4);

    // inserting powBytes into vector
    memcpy(bytes.data() + block.GetPreviousHash().size(), &pow, 4);

    // insert amount of votes into bytes
    bytes.push_back((char)block.getVotes().size());

    // inserting all votes into bytes
    insertIntoVector(bytes, voteBytes);

    return bytes;
}
BYTES Serializer::serializeBlocklist(const list<Block> blockList)
{
    // Insert how many blocks are int the list
    BYTES bytes = { (char)blockList.size() };

    // Insert blocks' bytes
    for (const Block& block : blockList)
    {
        insertIntoVector(bytes, serializeBlock(block));
    }

    return bytes;
}

void Serializer::insertNumber(BYTES& bytes, int number)
{
	// inserting the number into the vector byte by byte
	bytes.insert(bytes.end(), (char*)&number, (char*)&number + sizeof(int));
}

void Serializer::insertIntoVector(BYTES& bytes, const std::string& str)
{
    // inserting the str into the vector byte by byte
    bytes.insert(bytes.end(), str.begin(), str.end());
}

void Serializer::insertIntoVector(BYTES& bytes, const BYTES& vector)
{
    // __||__
    bytes.insert(bytes.end(), vector.begin(), vector.end());
}

BYTES Serializer::getVotesBytes(const Block& block)
{
    BYTES votesBytes;
    BYTES tempBytes;

    for (auto& vote : block.getVotes())
    {
        // can be done easily with 1 line but prefered to do it in multiple to help with debugging later maybe
        tempBytes = serializeVote(vote); // getting serialized version of vote
        insertIntoVector(votesBytes, tempBytes); // inserting vote into votes
    }
    return votesBytes;
}

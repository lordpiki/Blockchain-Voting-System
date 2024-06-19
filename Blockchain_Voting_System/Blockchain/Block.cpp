#include <iostream>
#include "Block.h"


bool Block::Verify(const RSA::PublicKey& govPublicKey) const
{
    for (Vote v : _votes)
    {
        if (!v.Verify(CryptoHelper::getGovPk()))
            return false;
    }

    return validatePOW(_leadingZeros);
}

unsigned int Block::Mine()
{
    _nonce = 0; // Resetting nonce to start with 0

    while (!validatePOW(_leadingZeros)) // Iterate until the function finds a good nonce number
    {
        _nonce++; // Upping nonce by 1 to check
    }

    return _nonce;
}




bool Block::hasLeadingZeros(const string& input, unsigned int zeros) const
{
    for (unsigned int i = 0; i < zeros; ++i) // Going over the string and checking if it has the correct number of 0`s at the start
    {
        if (input[i] != '0') {
            return false;
        }
    }
    return true;
}

bool Block::validatePOW(const unsigned int zeros) const
{
    string hash = getBlockHash();
    return hasLeadingZeros(hash, zeros); // Checking and returning if the hash has the correct number of 0`s at the start
}

string Block::getVotesString() const
{
    string res;
    for (const Vote& v : _votes)
        res += v.toString();

    return res;
}

bool operator==(const Block& lhs, const Block& rhs)
{
    // checking if each member of Block is the same
    return lhs.GetPreviousHash() == rhs.GetPreviousHash() &&
        lhs.getVotes() == rhs.getVotes() &&
        lhs.GetPOW() == rhs.GetPOW();
}

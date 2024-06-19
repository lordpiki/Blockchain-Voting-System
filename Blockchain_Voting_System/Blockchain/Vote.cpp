#include "Vote.h"
#include <iostream>
#include <rsa.h>
#include <base64.h>
#include <files.h>
#include <osrng.h>
#include "Node.h"

using CryptoPP::byte;
using std::string;


Vote::Vote(const char& candidate, const RSA::PrivateKey& privateKey, const RSA::PublicKey& publicKey, const string& signedPublicKey) :
    _candidate(candidate), _publicKey(publicKey), _signedPublicKey(signedPublicKey)
{
    _signature = CryptoHelper::sign(std::to_string(_candidate), privateKey);
}


bool Vote::Verify(const CryptoPP::RSA::PublicKey& govPublicKey) const 
{
    string publicKeyStr = CryptoHelper::publicKeyToString(_publicKey);
    string voteData = std::to_string(_candidate);

    return verifySignature(publicKeyStr, _signedPublicKey, govPublicKey) &&
        verifySignature(voteData, _signature, _publicKey);
}

string Vote::toString() const
{
    return std::to_string(_candidate) + 
        _signature + 
        CryptoHelper::publicKeyToString(_publicKey) + 
        _signedPublicKey;
}


bool Vote::verifySignature(const string& message, const string& signature, const RSA::PublicKey& publicKey) const
{
    try {
        CryptoPP::RSASS< CryptoPP::PKCS1v15, CryptoPP::SHA256 >::Verifier verifier(publicKey);

        CryptoPP::StringSource ss(message+signature, true, new CryptoPP::SignatureVerificationFilter(
            verifier, nullptr,
            CryptoPP::SignatureVerificationFilter::THROW_EXCEPTION
        ));

        // If the verification process completes without throwing an exception, the signature is valid
        return true;
    }
    catch (const CryptoPP::Exception& e) {
        return false; // Verification failed
    }
}


bool operator==(const Vote& lhs, const Vote& rhs)
{
    // checking if each member of Vote is the same
    return lhs.getCandidate() == rhs.getCandidate() &&
        lhs.getSignature() == rhs.getSignature() &&
        lhs.getPublicKeyString() == rhs.getPublicKeyString() &&
        lhs.getSignedPublicKey() == rhs.getSignedPublicKey();
}

bool operator!=(const Vote& lhs, const Vote& rhs)
{
    return !(lhs==rhs);
}

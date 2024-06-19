#pragma once

#include <cryptlib.h>
#include <rsa.h>
#include <osrng.h>
#include <base64.h>
#include <string>
#include <pssr.h>
#include "CryptoHelper.h"

using std::string;

//#define char char


class Vote 
{
public:
    // Constructor for recived vote
    Vote(const char& candidate, const string& signature, const RSA::PublicKey& publicKey, const string& signedPublicKey) :
        _candidate(candidate), _signature(signature), _publicKey(publicKey), _signedPublicKey(signedPublicKey) {}

    //constructor for created vote
    Vote(const char& candidate, const RSA::PrivateKey& privateKey, const RSA::PublicKey& publicKey, const string& signedPublicKey);
    
    // Destructor
    virtual ~Vote() {}

    // methods
    bool Verify(const RSA::PublicKey& govPublicKey) const;
    string toString() const;

    // Equality operator overload
    friend bool operator==(const Vote& lhs, const Vote& rhs);
    friend bool operator!=(const Vote& lhs, const Vote& rhs);


    // getters
    char getCandidate() const { return _candidate; }
    string getSignature() const { return _signature; }
    string getPublicKeyString() const { return CryptoHelper::publicKeyToString(_publicKey); }
    string getSignedPublicKey() const { return _signedPublicKey; }

private:
    char _candidate;
    string _signature;
    RSA::PublicKey _publicKey;
    string _signedPublicKey;

    bool verifySignature(const string& message, const string& signature, const RSA::PublicKey& publicKey) const;
    
};

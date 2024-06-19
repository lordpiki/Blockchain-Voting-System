#include <chrono>
#include "Voter.h"
#include "Serializer.h"
#include "Deserializer.h"


Voter::Voter(Blockchain& blockchain) : Node(blockchain)
{
	init();
}

void Voter::vote(char candidate) const
{
	Vote myVote(candidate, _sk, _pk, _signedPublicKey);
	_communicator.sendMessage(Serializer::serialize(myVote));

	waitForConfermation(myVote);
}

void Voter::init()
{
	// Initialize rsa key and govermental signature
	_sk = CryptoHelper::generatePrivateKey();
	_pk = CryptoHelper::generatePublicKey(_sk);
	// get signed public key
	handleSignedPk();
}

void Voter::handleSignedPk()
{
	// Sign the public key
	_communicator.sendMessage(Serializer::serialize(_pk));
	vector<char> signedPk = _communicator.getNextMessage();
	string base64signedPK(signedPk.begin(), signedPk.end());
	string decodedSignature;
	CryptoPP::StringSource(base64signedPK, true,
		new CryptoPP::Base64Decoder(
			new CryptoPP::StringSink(_signedPublicKey)
		)
	);
}


void Voter::waitForConfermation(const Vote& myVote) const
{
    std::chrono::minutes waitDuration(5); // Set the duration to 10 minutes
	std::chrono::seconds checkDuration(10); // Set the duration to 1 minute
    auto start = std::chrono::steady_clock::now(); // Get the current time

    while (!_blockchain.checkKeyStr(CryptoHelper::publicKeyToString(_pk)))
    {
		unordered_set<string> hashSet = _blockchain.getHashSet();
		
        std::this_thread::sleep_for(checkDuration);
        
        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(end - start);
        if (elapsed >= waitDuration)
        {
			_communicator.sendMessage(Serializer::serialize(myVote));
        }
    }
	log("Voted for candidate", "log.txt");

}

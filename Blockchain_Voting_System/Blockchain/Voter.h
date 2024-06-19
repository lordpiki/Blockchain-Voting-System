#pragma once
#include "Node.h"

class Voter : public Node
{
public:
	Voter() { init(); }
	Voter(Blockchain& blockchain);
	~Voter()
	{
		log("Voter destroyed", "log.txt");
	}
	
	void vote(char candidate) const;
private:
	RSA::PublicKey _pk;
	RSA::PrivateKey _sk;
	string _signedPublicKey;

	void init();
	void handleSignedPk();
	void waitForConfermation(const Vote& myVote) const;
};
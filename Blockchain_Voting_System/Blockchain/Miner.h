#pragma once
#include "Node.h"
#include <queue>
#include <string>

using std::queue;
using std::string;

class Miner : public Node
{
public:
	Miner(Blockchain& blockchain);
	Miner();

	void run() override;
	void getCurrStats();


private:
	queue<Vote> _votes;
	mutex _voteMtx;

	// stats
	bool _isMining;

	void mine();

	void publishBlocks();

	// new handlers
	BYTES handlePublishVote(const BYTES& bytes) override;
};
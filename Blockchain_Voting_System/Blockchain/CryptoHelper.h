#pragma once

#include <cryptlib.h>
#include <osrng.h>
#include <base64.h>
#include <string>
#include <rsa.h>
#include <sha.h>
#include <cstring>
#include <hex.h>
#include <rsa.h>
#include <osrng.h>
#include <base64.h>
#include <string>
#include <pssr.h>

using std::string;
using CryptoPP::RSA;


class CryptoHelper
{

public:

	static void init();

	// Calcualte SHA256 hash
	static string calcualteSHA256(const string& input);

	// static publicKey <-> String functions
	static string publicKeyToString(const RSA::PublicKey& publicKey);
	static RSA::PublicKey StringToPublicKey(const std::string& publicKeyStr);

	// staic privateKey <-> String functions
	static string privateKeyToString(const RSA::PrivateKey& privateKey);
	static RSA::PrivateKey StringToPrivateKey(const std::string& privateKeyStr);

	static RSA::PublicKey generatePublicKey(const RSA::PrivateKey& privateKey);
	static RSA::PrivateKey generatePrivateKey();
	
	// gov functions
	static string signPK(const RSA::PublicKey& publicKey);
	static string sign(const string& text, const RSA::PrivateKey& privateKey);
	static RSA::PublicKey getGovPk() { return _govPk; }

	static bool verifySignature(const string& message, const string& signature, const RSA::PublicKey& publicKey);



	static void saveKeysToFiles();

private:

	// Load gov keys from file
	static void loadGovKeys();

	static CryptoPP::RSA::PrivateKey _govSk;
	static CryptoPP::RSA::PublicKey _govPk;

};


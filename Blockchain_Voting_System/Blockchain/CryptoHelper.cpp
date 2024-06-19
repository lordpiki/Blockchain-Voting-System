#include "CryptoHelper.h"

#include <iostream>
#include <fstream>
#include <files.h>

#include <rsa.h>
#include <osrng.h>
#include <files.h>
#include <hex.h>

using std::cout;
using std::endl;

CryptoPP::RSA::PrivateKey CryptoHelper::_govSk;
CryptoPP::RSA::PublicKey CryptoHelper::_govPk;

void CryptoHelper::init()
{
    loadGovKeys();
}

string CryptoHelper::calcualteSHA256(const string& input)
{
    CryptoPP::SHA256 hash;
    string output;

    // Generating hash using SHA256 algorithm
    CryptoPP::StringSource(input, true, new CryptoPP::HashFilter(hash, new CryptoPP::HexEncoder(new CryptoPP::StringSink(output))));

    return output;
}

string CryptoHelper::publicKeyToString(const RSA::PublicKey& publicKey)
{
    CryptoPP::ByteQueue queue;
    publicKey.Save(queue);

    std::string result;
    CryptoPP::Base64Encoder encoder(new CryptoPP::StringSink(result), false);
    queue.CopyTo(encoder);
    encoder.MessageEnd();

    return result;
}

RSA::PublicKey CryptoHelper::StringToPublicKey(const std::string& publicKeyStr)
{
    RSA::PublicKey publicKey;

    CryptoPP::ByteQueue queue;
    CryptoPP::StringSource source(publicKeyStr, true, new CryptoPP::Base64Decoder(new CryptoPP::Redirector(queue)));

    publicKey.Load(queue);

    return publicKey;
}

string CryptoHelper::privateKeyToString(const RSA::PrivateKey& privateKey)
{
    CryptoPP::ByteQueue queue;
    privateKey.Save(queue);

    std::string result;
    CryptoPP::Base64Encoder encoder(new CryptoPP::StringSink(result), false);
    queue.CopyTo(encoder);
    encoder.MessageEnd();

    return result;
}

RSA::PrivateKey CryptoHelper::StringToPrivateKey(const std::string& privateKeyStr)
{

    RSA::PrivateKey privateKey;

    CryptoPP::ByteQueue queue;
    CryptoPP::StringSource source(privateKeyStr, true, new CryptoPP::Base64Decoder(new CryptoPP::Redirector(queue)));

    privateKey.Load(queue);

    return privateKey;

}

RSA::PublicKey CryptoHelper::generatePublicKey(const RSA::PrivateKey& privateKey)
{
    return CryptoPP::RSA::PublicKey(privateKey);
}

RSA::PrivateKey CryptoHelper::generatePrivateKey()
{
    CryptoPP::AutoSeededRandomPool rng;
    RSA::PrivateKey sk;
    
    sk.GenerateRandomWithKeySize(rng, 2048);

    return sk;
}

string CryptoHelper::signPK(const RSA::PublicKey& publicKey)
{
    return sign(publicKeyToString(publicKey), _govSk);
}

string CryptoHelper::sign(const string& text, const RSA::PrivateKey& privateKey)
{

    string signedText;
    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::RSASS<CryptoPP::PKCS1v15, CryptoPP::SHA256>::Signer signer(privateKey);

    CryptoPP::StringSource(text, true,
        new CryptoPP::SignerFilter(rng, signer, new CryptoPP::StringSink(signedText)));

    return signedText;
}

bool CryptoHelper::verifySignature(const string& message, const string& signature, const RSA::PublicKey& publicKey)
{
    try {
        CryptoPP::RSASS< CryptoPP::PKCS1v15, CryptoPP::SHA256 >::Verifier verifier(publicKey);

        CryptoPP::StringSource ss(message + signature, true, new CryptoPP::SignatureVerificationFilter(
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

void CryptoHelper::saveKeysToFiles()
{


    // Save private key to file
    CryptoPP::FileSink privateKeyFile("private_key.der");
    _govSk.DEREncode(privateKeyFile);
    privateKeyFile.MessageEnd();

    // Save public key to file
    CryptoPP::FileSink publicKeyFile("public_key.der");
    _govPk.DEREncode(publicKeyFile);
    publicKeyFile.MessageEnd();

    std::cout << "Generated and saved RSA key pair." << std::endl;
}

void CryptoHelper::loadGovKeys()
{
    // Load private key from filep
    try {
        // Load the public key from the file
        CryptoPP::FileSource publicKeyFile("public_key.der", true);
        _govPk.BERDecode(publicKeyFile);

        // Load the private key from the file
        CryptoPP::FileSource privateKeyFile("private_key.der", true);
        _govSk.BERDecode(privateKeyFile);
    }
    catch (const std::exception& e)
    {
	    std::cerr << "Error loading private key from file: " << e.what() << endl;
        exit(-1);
	}

}

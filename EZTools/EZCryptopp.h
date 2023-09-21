/*
Copyright (c) 2017-2022, Michael Romans of Romans Audio
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the MRUtils project.
*/

#ifndef CRYPTOPPTEST_EZCRYPTOPP_H
#define CRYPTOPPTEST_EZCRYPTOPP_H

#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <cryptopp/config.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/filters.h>
#include <cryptopp/aes.h>
#include "EZTools.h"

using namespace std;
using namespace CryptoPP;

class EZCryptopp {
public:

    static bool isHex(string checkForHex) {
        size_t found;
        found = checkForHex.find_first_not_of("0123456789ABCDEF");
        if (found!=string::npos) {
            // This string has non-hex characters in it
            return false;
        } else {
            // only hex characters
            return true;
        }
    }

    static string HexEncode(string binaryString) {
        if (isHex(binaryString)) {
            // This string is already hex, don't convert it!
            return binaryString;;
        } else {
            string hexString;
            StringSource(binaryString, true, new HexEncoder(new StringSink(hexString)));
            return hexString;
        }
    }

    static string HexDecode(string hexString) {
        if (isHex(hexString)) {
            string binaryString;
            StringSource(hexString, true, new HexDecoder(new StringSink(binaryString)));
            return binaryString;
        } else {
            // it's already binary!
            return hexString;
        }
    }

    static EZTools::EZReturn<string> EncryptStringAES(string plainText, string key, string iv, bool &err, string &errMsg) {
        EZTools::EZReturn<string> res;
        // returns an encypted string in binary
        // This version uses the given iv and returns only the cipherText with no prepend
        try {
            // convert the iv if it is sent in hex
            iv = HexDecode(iv);

            // convert the key if it is sent in hex
            key = HexDecode(key);

            // setup the encyptor
#ifdef CENTOS7
            CBC_Mode< Rijndael >::Encryption e1( (byte *)key.c_str(), key.length(), (byte *)iv.c_str() );
#else
            CBC_Mode< Rijndael >::Encryption e1( (CryptoPP::byte *)key.c_str(), key.length(), (CryptoPP::byte *)iv.c_str() );
#endif
            // Encryption
            string cipherText;
            StringSource( plainText, true,
                    new StreamTransformationFilter( e1,
                            new CryptoPP::StringSink( cipherText )));

            // return the concatated iv and cipherText
            err = false;
            res.wasSuccessful(true);
            res.data = cipherText;
            return res;
        } catch (Exception& e) {
            res.wasSuccessful(false);
            res.message(e.GetWhat());
            return res;
        }

    }

    static EZTools::EZReturn<string> DecryptStringAES(string cipherText, string key, string iv, bool &err, string &errMsg){
        EZTools::EZReturn<string> res;
        // returns the decrypted string
        // This version uses the given iv
        // use DecryptStringAES(cipherText, key, err) if the iv is prepended to the cipherText
        // cipherText, key, and iv can be given as hex or binary
        try {
            // convert ivAndCipherText as neccessary
            cipherText = HexDecode(cipherText);

            // convert key if necessary
            key = HexDecode(key);

            // convert iv if necessary
            if (isHex(iv)) {
                iv = HexDecode(iv);
            }

            // set up decrypter
#ifdef CENTOS7
            CBC_Mode< Rijndael >::Decryption d((byte *)key.c_str(), key.length(), (byte *)iv.c_str());
#else
            CBC_Mode< Rijndael >::Decryption d((CryptoPP::byte *)key.c_str(), key.length(), (CryptoPP::byte *)iv.c_str());
#endif
            // decrypt
            string plainText;
            StringSource( cipherText, true, new StreamTransformationFilter( d, new StringSink( plainText)));
            err = false;
            res.wasSuccessful(true);
            res.data = plainText;
            return res;
        } catch (Exception& e) {
            res.wasSuccessful(false);
            string errText = "Bad decrypt";
            res.message(e.GetWhat());
            errMsg = e.GetWhat();
            err = true;
            return res;
        }
    }

    static void EncryptFileAES(istream& inFile, ostream& outFile, string key, string iv, bool &err, string &errMsg) {
        // will encrypt the file at filenameIn to filenameOut using AES
        // WARNING: The iv must be known and retained for decryption!!
        // key and iv can be hex or binary


        // convert the key and iv
        key = HexDecode(key);
        iv = HexDecode(iv);
        try {
            // Set up the encrypter
#ifdef CENTOS7
            CBC_Mode< Rijndael >::Encryption e1( (byte *)key.c_str(), key.length(), (byte *)iv.c_str() );
#else
            CBC_Mode< Rijndael >::Encryption e1( (CryptoPP::byte *)key.c_str(), key.length(), (CryptoPP::byte *)iv.c_str() );
#endif
            // encrypt
            //if (filenameOut == "cout")
            //      FileSource( filenameIn.c_str(), true, new StreamTransformationFilter( e1, new FileSink(cout)));
            //else
            FileSource( inFile, true, new StreamTransformationFilter( e1, new FileSink(outFile)));
            err= false;
            return;
        } catch (Exception& e) {
            errMsg = e.GetWhat();
            err = true;
            return;
        }
    }

    static void DecryptFileAES(istream& inFile, ostream& outFile, string key, string iv, bool &err, string &errMsg) {
        // will encrypt the file at filenameIn to filenameOut using AES
        // WARNING: The correct iv MUST be provided
        // key and iv can be hex or binary

        // convert the key and iv
        key = HexDecode(key);
        iv = HexDecode(iv);

        try {
            // Set up the encrypter
#ifdef CENTOS7
            CBC_Mode< Rijndael >::Decryption d ( (byte *)key.c_str(), key.length(), (byte *)iv.c_str() );
#else
            CBC_Mode< Rijndael >::Decryption d ( (CryptoPP::byte *)key.c_str(), key.length(), (CryptoPP::byte *)iv.c_str() );
#endif
            // encrypt
            //if (filenameOut == "cout")
            //      FileSource( filenameIn.c_str(), true, new StreamTransformationFilter( d, new FileSink(cout)));
            //else
            FileSource( inFile, true, new StreamTransformationFilter( d, new FileSink(outFile)));
            err= false;
            return;
        } catch (Exception& e) {
            errMsg = e.GetWhat();
            err = true;
            return;
        }
    }

    static string BitGen(int howManyBits) {
        // returns a new random key in binary of the given bit length
        int byteLength = howManyBits/8; // bitLengths always better be div 8!!
        return ByteGen(byteLength);
    }

    static string ByteGen(int howManyBytes) {
        // returns a new random key of the given byte length
        AutoSeededRandomPool rnd;
#ifdef CENTOS7
        byte block[howManyBytes];
#else
        CryptoPP::byte block[howManyBytes];
#endif
        rnd.GenerateBlock(block, howManyBytes);
        string blockString;
        blockString.assign((char *)block, sizeof(block));
        return blockString;
    }


    static string hashSHA256(string inputString) {
        // returns a SHA-256 encoded hash of the inputString in binary
        // always returns 256 bits
        CryptoPP::SHA256 hash;
#ifdef CENTOS7
        byte digest [ SHA256::DIGESTSIZE ];

        hash.CalculateDigest( digest, (byte *)inputString.c_str(), inputString.length() );
#else
        CryptoPP::byte digest [ SHA256::DIGESTSIZE ];

        hash.CalculateDigest( digest, (CryptoPP::byte *)inputString.c_str(), inputString.length() );
#endif
        string hashString;
        hashString.assign((char *)digest, sizeof(digest));

        return hashString;
    }

    static inline string generateAES256Key() {
        return HexEncode(EZCryptopp::BitGen(256));;
    }

    class AES256KeyPair {
    public:
        AES256KeyPair() = default;
        AES256KeyPair(EZTools::EZString PublicKey, EZTools::EZString PrivateKey) {
            if (PublicKey.regexMatch("^-")) {
                auto kk = PublicKey.split("\n");
                _key = kk.at(1);
            } else {
                _key = PublicKey;
            }
            if (PrivateKey.regexMatch("^-")) {
                auto kk = PrivateKey.split("\n");
                _vec = kk.at(1);
            } else {
                _vec = PrivateKey;
            }
        }
        ~AES256KeyPair() = default;
        EZTools::EZString publicKeyOutput() {
            std::stringstream ss;
            ss << "-----EZCryptopp Public Key------" << endl;
            ss << _key << endl;
            ss << "--------------------------------";
            return ss.str();
        }
        EZTools::EZString privateKeyOutput() {
            std::stringstream ss;
            ss << "-----EZCryptopp Private Key-----" << endl;
            ss << _vec << endl;
            ss << "--------------------------------";
            return ss.str();
        }
        EZTools::EZString publicKey() { return _key; }
        EZTools::EZString privateKey() { return _vec; }
    protected:
        EZTools::EZString _key = generateAES256Key();
        EZTools::EZString _vec = generateAES256Key();
    };

    // On the fly random encryption for in memory AES256 passwords
    // Object storage is AES256 so no memory snooping...
    class Password_t {
    public:
        Password_t() = default;
        Password_t(AES256KeyPair keys) {
            _keys = keys;
        }
        ~Password_t() = default;
        void setUnencryptedPassword(EZTools::EZString password) {
            auto r1 = EncryptStringAES(password, _keys.publicKey(),_keys.privateKey(),_error,_errorMessage);
            if (!r1.wasSuccessful()) {
                std::cout << r1.message() << endl;
                exit(EXIT_FAILURE);
            }
            _encPass = HexEncode(r1.data);
            _hash = HexEncode(hashSHA256(password));
        }
        EZTools::EZString decrypt() {
            auto r1 = DecryptStringAES(_encPass, _keys.publicKey(), _keys.privateKey(),
                                       _error,_errorMessage);
            if (!r1.wasSuccessful()) {
                std::cout << r1.message() << std::endl;
                exit(EXIT_FAILURE);
            }
            return r1.data;
        }
        void setEncryptedPassword(EZTools::EZString hexPass) {
            _encPass = hexPass;
            auto r1 = DecryptStringAES(_encPass,
                                       _keys.publicKey(),_keys.privateKey(), _error,_errorMessage);
            if (!r1.wasSuccessful()) {
                std::cout << r1.message() << std::endl;
                exit(EXIT_FAILURE);
            }
            _hash = HexEncode(hashSHA256(r1.data));
        }
        EZTools::EZString encryptedPassword() { return _encPass; }
        EZTools::EZString sha256Hash() { return _hash; }
        AES256KeyPair keys() { return _keys; }

    protected:
        bool _error = false;
        EZTools::EZString _hash;
        EZTools::EZString _errorMessage;
        EZTools::EZString _encPass;
        AES256KeyPair _keys;
    };

};

#endif //CRYPTOPPTEST_EZCRYPTOPP_H

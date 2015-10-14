//
//  client-key-exchange.hpp
//  browser-cc
//
//  Created by Nissassin Seventeen on 10/6/15.
//  Copyright © 2015 Nissassin Seventeen. All rights reserved.
//

#ifndef client_key_exchange_hpp
#define client_key_exchange_hpp

#include <stdio.h>

#include <cstdint>
#include <vector>

#include "Exportable.hpp"

class ASN1Cert;
class CipherSuite;
class ClientDiffieHellmanPublic;
class EncryptedPreMasterSecret;

class ClientKeyExchange: public Exportable {
public:
	ClientKeyExchange(const CipherSuite *cipherSuite, const ASN1Cert *asn1Cert);
	virtual vector<uint8_t> toData()const;
	virtual size_t size()const;
	~ClientKeyExchange();
private:
	const CipherSuite *cipherSuite;
	EncryptedPreMasterSecret *encryptedPreMasterSecret;
	ClientDiffieHellmanPublic *clientDiffieHellmanPublic;
};

#endif /* client_key_exchange_hpp */
//
//  SslWrapper.cpp
//  browser-cc
//
//  Created by Nissassin Seventeen on 10/2/15.
//  Copyright © 2015 Nissassin Seventeen. All rights reserved.
//

#include "SslWrapper.hpp"


#include <iostream>

#include "Connection.hpp"
#include "Handshake.hpp"
#include "Record.hpp"
#include "Util.hpp"

SslWrapper::SslWrapper(const Url* url) :
url(url){
	this->connection = new Connection(url->getHostname(), url->isUseSsl());
}

vector<uint8_t> SslWrapper::get() {
	if (this->url->isUseSsl()) {
		//prepare client hello
		Record *clientHello = new Record(Handshake::CLIENT_HELLO);

		vector<uint8_t> tosend(clientHello->toData());
		this->connection->send(tosend);
		cout << "Request:" << endl << Util::readableForm(tosend) << endl;

		vector<uint8_t> data = this->connection->receive();
		cout << "Response:" << endl << Util::readableForm(data) << endl; //this is server hello message

		//this must be a server hello handshake record
		size_t offset(0);
		Record *serverHello = new Record(data, offset);
		offset += serverHello->size();

		if (serverHello->getType() == Record::ALERT) {
			cerr << "Receive alert from server" << endl;
		} else if (serverHello->getType() == Record::HANDSHAKE) {
			vector<Record*> records;
            Record *record;
			do {
				records.push_back(
						new Record(data, offset, serverHello->getHandshake()));
				record = *(records.rbegin());
				offset += record->size();
//					break; //TLS_RSA_WITH_AES_128_CBC_SHA256
            } while(record->getHandshake()->getType()
                    != Handshake::SERVER_HELLO_DONE);
			if (records.size() == 2) {
				//one certificate and one hellodone

				vector<Record*> toSend;
				toSend.push_back(
						new Record(Handshake::CLIENT_KEY_EXCHANGE,
								serverHello->getHandshake(),
								records[0]->getHandshake()));
				toSend.push_back(new Record(Record::CHANGE_CIPHER_SPEC));
				toSend.push_back(new Record(Handshake::FINISHED));

			}
			for (int i = 0; i < records.size(); i++)
				delete records[i];

		} else
			cerr << "Unexpected response from server" << endl;

		delete clientHello;
		delete serverHello;

		//prepare finished message
//        Record finished(data.)

		return vector<uint8_t>();
	}
	this->connection->send(this->url->httpGetRequest());
	return this->connection->receive();
}

SslWrapper::~SslWrapper() {
	delete this->connection;
}
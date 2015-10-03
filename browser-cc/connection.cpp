//
//  connection.cpp
//  browser-cc
//
//  Created by Nissassin Seventeen on 10/2/15.
//  Copyright © 2015 Nissassin Seventeen. All rights reserved.
//

#include "connection.hpp"
#include "err.hpp"

using namespace std;

Connection::Connection(string hostname, bool isSsl){
    const int SSL_PORT = 443;
    const int HTTP_PORT = 80;
    
    this->isConnecting = false;
    this->activatingConnection = NULL;
    
    //get ip list
    vector<string> ipList = Connection::ipListFromHostname(hostname);
    for(int i = 0; i < ipList.size(); i++)
        this->subConnections.push_back(SubConnection(ipList[i], isSsl ? SSL_PORT : HTTP_PORT));
}

vector<string> Connection::ipListFromHostname(string hostname){
    vector<string> ipList;
    
    struct addrinfo hints, *server_info;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET6 for IPv6
    hints.ai_socktype = SOCK_STREAM;
    
    int er = getaddrinfo(hostname.c_str(), "http", &hints, &server_info);
    if (er != 0)
        throw Err(Err::CannotResolveHostname, gai_strerror(er));
    
    //iterate result
    //first loop to get number of result
    for(struct addrinfo *it = server_info; it != NULL; it = it->ai_next){
        struct sockaddr_in *host = (struct sockaddr_in*)it->ai_addr;
        char *sin = inet_ntoa(host->sin_addr);
        ipList.push_back(string(sin));
    }
    freeaddrinfo(server_info);

    return ipList;
}

void Connection::send(vector<uint8_t> request){
    if (this->isConnecting){
        if (this->activatingConnection == NULL)
            throw Err(Err::NoConnection);
        this->activatingConnection->doSend(request);
    }else{
        this->isConnecting = true;
        for(int i = 0; i < this->subConnections.size(); i++){
            try{
                this->subConnections[i].doConnect();
                this->subConnections[i].doSend(request);
                //wait for error until here
                this->activatingConnection = &this->subConnections[i];
                break;
            }catch (Err err){
            }
        }
    }
}

vector<uint8_t> Connection::receive(){
    if (!this->isConnecting)
        throw Err(Err::DontSendButReceive);
    if (this->activatingConnection == NULL)
        throw Err(Err::NoConnection);
    return this->activatingConnection->doReceive();
}
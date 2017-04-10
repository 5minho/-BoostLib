//
// Created by 오민호 on 2017. 4. 9..
//

#ifndef BOOSTPROJ_SESSION_H
#define BOOSTPROJ_SESSION_H

#include "../protocol/protocol.h"

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <deque>
#include "../protocol/protocol.h"



class Session {
    boost::asio::ip::tcp::socket sessionSocket;
    int sessionID;
    int roomID;
    std::string userId;
    std::string userPw;
    boost::array<char, protocol::MAX_PACKET_LENGTH> recvBuffer;
    std::deque< char* > sendDataQueue;
    //ChatServer* server;
    void handle_write(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_receive( const boost::system::error_code& error, size_t bytes_transferred );

public:
    Session(int sessionID, boost::asio::io_service& io_service, ChatServer* server);
    ~Session();
    void init();
    void postRecv();
    void postSend();
};

#endif //BOOSTPROJ_SESSION_H

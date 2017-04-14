//
// Created by 오민호 on 2017. 4. 14..
//

#include "Session.h"
#include "../ChatServer.h"

Session::Session(int sessionID, boost::asio::io_service &io_service, ChatServer *server)
:m_sessionSocket(io_service),m_sessionID(sessionID), m_pServer(server) , m_pos(pos::LOBBY) {
    m_recvBuffer.assign(0);
}

Session::~Session() {
    while(!m_sendDataQueue.empty()) {
        delete[] m_sendDataQueue.front();
        m_sendDataQueue.pop_front();
    }
}

void Session::init() {
    m_nPacketbufferMark = 0;
}

void Session::postRecv() {
    m_sessionSocket.async_read_some(boost::asio::buffer(m_recvBuffer),
                                    boost::bind(&Session::handle_receive,
                                                this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred)
    );
}

void Session::handle_receive(const boost::system::error_code &error, size_t bytes_transferred) {
    if(error) {

    } else {
        std::memcpy(&m_nPacketBuffer[m_nPacketbufferMark], m_recvBuffer.data(), bytes_transferred);

        size_t nPacketData = m_nPacketbufferMark + bytes_transferred;
        size_t nReadData = 0;

        while(nPacketData > 0) {
            if(nPacketData < sizeof(packet::Header)) break;

            packet::Header* pHeader = (packet::Header*)&m_nPacketBuffer[nReadData];
            if(pHeader->len <= nPacketData) {
                m_pServer->processPacket(this->m_sessionID, &m_nPacketBuffer[nReadData]);
                nPacketData -= pHeader->len;
                nReadData += pHeader->len;
            }

            else break;
        }

        if(nPacketData > 0) {
            char tempBuffer[m_nPacketBuffer.size()];
            memcpy(&tempBuffer[0], &m_nPacketBuffer[nReadData], nPacketData);
            memcpy(&m_nPacketBuffer[0], &tempBuffer[0], nPacketData);
        }

        m_nPacketbufferMark = nPacketData;

        postRecv();
    }
}

void Session::postSend(const bool bImmediately, const int nSize, char *pdata) {

}



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

void Session::postRecv() {// 클라이언트의 요청을 받는 함수
    //비동기로 함수 호출을 하고, 완료되면 handle_receive 함수를 호출한다.
    m_sessionSocket.async_read_some(boost::asio::buffer(m_recvBuffer),
                                    boost::bind(&Session::handle_receive,
                                                this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred)
    );
}

//데이터를 받고 패킷별로 나누는 함
void Session::handle_receive(const boost::system::error_code &error, size_t bytes_transferred) {
    if(error) {
        if(error == boost::asio::error::eof) {
            // 클라리언트와 연결이 끊어 진 경우
            // 로그에 남긴다.
            std::cout << "클라이 언트와 연결이 끊어 졌습니다." << std::endl;
        } else {
            //error 이유를 알아낸다.
            std::cout << "error No : " << error.value() << "error Message" << error.message() << std::endl;
        }
        //해당 세션 닫음
        m_pServer->closeSession(m_sessionID);

    } else {
        //버퍼에 있는 데이터를 패킷버퍼로 복사한다.
        std::memcpy(&m_nPacketBuffer[m_nPacketbufferMark], m_recvBuffer.data(), bytes_transferred);

        //버퍼에 데이터가 얼마나 있는지 표시한다.
        size_t nPacketData = m_nPacketbufferMark + bytes_transferred;
        size_t nReadData = 0;

        //받은 데이터를 모두 처리할 때 까지 반
        while(nPacketData > 0) {
            //남은 데이터가 패킷 헤더보다 작으면 중단
            if(nPacketData < sizeof(packet::Header)) break;
            packet::Header* pHeader = (packet::Header*)&m_nPacketBuffer[nReadData];

            //처리할 수 있을 만큼 데이터가 있으면 처리
            if(pHeader->len <= nPacketData) {
                m_pServer->processPacket(this->m_sessionID, &m_nPacketBuffer[nReadData]);
                nPacketData -= pHeader->len;
                nReadData += pHeader->len;
            }

            else break;
        }
        //처리하고 남은 데이터를 버퍼 앞쪽으로 민다.
        if(nPacketData > 0) {
            char tempBuffer[m_nPacketBuffer.size()];
            memcpy(&tempBuffer[0], &m_nPacketBuffer[nReadData], nPacketData);
            memcpy(&m_nPacketBuffer[0], &tempBuffer[0], nPacketData);
        }
        //남은 데이터양을 표시하고 다시 요청을 받는다.
        m_nPacketbufferMark = nPacketData;
        postRecv();
    }
}

//클라이언트에게 요청을 보낸다.
void Session::postSend(const bool bImmediately, const int nSize, char *pdata) {
    char * pSendData = nullptr;
    //보낼 데이터를 저장한다. (async_write가 완료 되어  handle_write 함수가 호풀 될때까지 데이터를 보존하기 위해)
    if(bImmediately == false) {
        pSendData = new char[nSize];
        memcpy(pSendData, pdata, nSize);
        m_sendDataQueue.push_back(pSendData);
    } else {
        //즉시 보내는 경우
        pSendData = pdata;
    }
    //전에 보내기 요청을 한 것이 아직 완료되지 않았을 경우 곧바로 보내지 않고 저장만 한다.
    if(!bImmediately && m_sendDataQueue.size() > 1) return;

    boost::asio::async_write(m_sessionSocket,
                             boost::asio::buffer(pSendData, nSize),
                             boost::bind(&Session::handle_write,
                                         this,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred)
    );
}

void Session::handle_write(const boost::system::error_code &error, size_t bytes_transferred) {
    delete[] m_sendDataQueue.front();
    m_sendDataQueue.pop_front();

    if(!m_sendDataQueue.empty()){
        char* pdata = m_sendDataQueue.front();
        packet::Header* header = (packet::Header*)&pdata[0];
        postSend(true, header->len, pdata);
    }
}


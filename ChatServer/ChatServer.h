//
// Created by 오민호 on 2017. 4. 11..
//

#ifndef BOOSTPROJ_CHATSERVER_H
#define BOOSTPROJ_CHATSERVER_H


#include "Protocol/Protocol.h"
#include "Session/Session.h"
#include <list>

const int PORT_NUMBER = 31400;


class ChatServer {
    int m_nSeqNumber;
    bool m_bIsAccepting;
    boost::asio::ip::tcp::acceptor m_acceptor;

    std::vector< Session* > m_sessionList;
    std::deque< int > m_sessionQueue;

    bool postAccept() {
        //해당할 session id가 없다면 종료
        if(m_sessionQueue.empty())
            return (m_bIsAccepting = false);

        //session id 할당
        int nSessionID = m_sessionQueue.front();
        m_sessionQueue.pop_front();
        //요청 수락
        m_acceptor.async_accept(m_sessionList[nSessionID]->getSocket(), boost::bind(&ChatServer::handle_accept, this, m_sessionList[nSessionID], boost::asio::placeholders::error));
        return (m_bIsAccepting = true);
    }

    void handle_accept(Session* psession, boost::system::error_code& error ) {
        if(!error) {
            // 서버 로그에 해당 세션이 접속 했다는 것을 알림
            // lobby의 사람들에게 해당 세션이 접속했다는 것을 알림
            psession->init(); // session 초기화
            psession->postRecv(); // session 메시지 받을 준비
        } else {
            //서버 로그에 에러에 대한 사항을 남김
            std::cout << "error ID : " << error.value() << " error Message : " << error.message();
        }
    }

public:
    ChatServer(boost::asio::io_service& io_service)
    : m_acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),PORT_NUMBER )){
        m_bIsAccepting = false;
    }
    void start() {
        //서버 시작
        postAccept();
    }
    //maxSessionCount 만큼 session을 미리 만들어 놓는다.
    void init(const int maxSessionCount) {
        for(int i = 0 ; i < maxSessionCount; ++i) {
            Session *pSession = new Session(i, m_acceptor.get_io_service(), this);
            m_sessionList.push_back(pSession);
            m_sessionQueue.push_back(i);
        }
    }

    void closeSession(const int nSessionID) {
        //해당 세션이 접속 종료 됐다는 것을 로그에 저장
        m_sessionList[nSessionID]->getSocket().close(); //session의 소켓을 종료
        m_sessionQueue.push_back(nSessionID);//session queue에 해당 id를 다시쓰기 위해 저장
        if(!m_bIsAccepting) postAccept(); //대기 상태로 돌아감
    }
};

#endif //BOOSTPROJ_CHATSERVER_H

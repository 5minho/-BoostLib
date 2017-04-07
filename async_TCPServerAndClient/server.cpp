//
// Created by 오민호 on 2017. 4. 3..
//

#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>

const unsigned short PORT_NUMBER = 31400;

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::asio::buffer;
using boost::asio::placeholders::error;
using boost::asio::placeholders::bytes_transferred;
using boost::system::error_code;
using boost::array;
using std::string;
using std::cout;
using std::endl;

class Session {
public:
    Session(io_service& io_service):m_Socket(io_service) { }

    tcp::socket& Socket() {
        return m_Socket;
    }

    void postReceive() {
        //입력버퍼 초기화
        m_ReceiveBuffer.assign(0);
        //소켓을 통해 비동기적으로 클라이언트의 메시지를 읽음
        m_Socket.async_read_some(buffer(m_ReceiveBuffer),
                                 boost::bind(&Session::handle_receive, // 완료하면 호출할 함수
                                             this, //위 함수를 멤버로 가지는 클래스의 인스턴스
                                             error, //위 함수에 전달할 첫번째 인자
                                             bytes_transferred) // 두번째 인자
        );
    }

private:
    void handle_write(const error_code& /*error*/, size_t /*bytes_transferred*/) {}
    void handle_receive(const error_code& error, size_t bytes_transferred) {
        if(error) { // 에러가 발생했을때
            if(error == boost::asio::error::eof)
                cout << "클라이언트와 연결이 끊어졌습니다" << endl;
            else
                cout << "error No: " << error.value() << "error Message: " <<error.message() << endl;
        } else { // 에러가 발생하지 않았다면
            cout << "클라이언트에서 받은 메시지: " << m_ReceiveBuffer.data() << ", 받은 크기: " << bytes_transferred << endl;
            //m_WrtieMessage에 적절히 포매팅을 해서 복사
            m_WriteMessge = boost::str(boost::format("Re:%s") % m_ReceiveBuffer.data());
            //클라이언트에 에코메시지 전송
            boost::asio::async_write(m_Socket, buffer(m_WriteMessge),
                                     boost::bind(&Session::handle_write,
                                                 this,
                                                 error,
                                                 bytes_transferred)
            );
            //메시지 전송이 완료되지 않아도 호출된다
            postReceive();
        }
    }

    tcp::socket m_Socket; // 클라이언트와 연결할 소켓
    string m_WriteMessge; // 클라이언트에게 보낼 에코 메시지
    array<char, 128> m_ReceiveBuffer; // 클라이언트가 보낸 메시지를 저장할 버퍼
};

class TCP_Server {
public:
    //
    TCP_Server(io_service& io_service)
            : m_acceptor(io_service, tcp::endpoint(tcp::v4(), PORT_NUMBER)) {
        m_pSession = nullptr;
        StartAccept();
    }

    ~TCP_Server() {
        if(m_pSession != nullptr)
            delete m_pSession;
    }

private:
    tcp::acceptor m_acceptor;
    Session* m_pSession;

    void StartAccept() {
        cout << "클라이언트 접속 대기......" << endl;
        m_pSession = new Session(m_acceptor.get_io_service());
        m_acceptor.async_accept(m_pSession->Socket(),
                                boost::bind(&TCP_Server::handle_accept, //완료하면 호출할 함수
                                            this, // 함수를 멤버로 가지는 클래스(Session)의 인스턴스
                                            m_pSession, // handle_accept 함수에 넘길 첫 번째 인자
                                            error)); // handle_accept 함수에 넘길 두 번째 인자
    }

    void handle_accept(Session* pSession, const error_code& error) {
        if(!error) {
            cout << "클라이언트 접속 성공" << endl;
            pSession->postReceive();
        }
    }
};

int main() {
    io_service io_service;
    TCP_Server server(io_service);
    io_service.run();

    cout << "네트워크 접속 종료" << endl;
    getchar();
    return 0;
}
//
// Created by 오민호 on 2017. 4. 4..
//

#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>

const char SERVER_IP[] = "127.0.0.1";
const unsigned short PORT_NUMBER = 31400;

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::asio::buffer;
using boost::asio::error::eof;
using boost::asio::placeholders::error;
using boost::asio::placeholders::bytes_transferred;
using boost::system::error_code;
using boost::array;
using std::string;
using std::cout;
using std::endl;


class TCP_Client {
public:
    TCP_Client(io_service& io_service):m_io_service(io_service),
                                       m_socket(io_service),
                                       m_nSeqNumber(0) { }

    void Connect(tcp::endpoint& endpoint) {
        m_socket.async_connect(endpoint,
                               boost::bind(&TCP_Client::handle_connect,
                                           this,
                                           error)
        );
    }

private:
    void PostWrite() {
        if(m_socket.is_open() == false) return;

        if(m_nSeqNumber > 7) {
            m_socket.close();
            return;
        }
        ++m_nSeqNumber;
        m_writeMessage = boost::str(boost::format("%d - Send Message") % m_nSeqNumber);
        boost::asio::async_write(m_socket, buffer(m_writeMessage),
                                 boost::bind(&TCP_Client::handle_write,
                                             this,
                                             boost::asio::placeholders::error,
                                             bytes_transferred
        ));
        PostReceive();
    }

    void PostReceive() {
        m_ReceiveBuffer.assign(0);
        m_socket.async_read_some(buffer(m_ReceiveBuffer),
                                 boost::bind(&TCP_Client::handle_receive,
                                                       this,
                                                       boost::asio::placeholders::error,
                                                       bytes_transferred));
    }

    void handle_connect(const error_code& error) {
        if(error) cout << "connect failed : " << error.message() << endl;
        else {
            cout << "connected" << endl;
            PostWrite();
        }
    }

    void handle_write(const error_code& /*error*/, size_t /*bytes_transferred*/) { }
    void handle_receive(const error_code& error, size_t bytes_transferred) {
        if(error) {
            if(error == eof) cout << "서버와 연결이 끊어졌습니다" << endl;
            else cout << "error No: " << error.value() << "error Message" << error.message() << endl;
        }
        else {
            const string strRecvMessage = m_ReceiveBuffer.data();
            cout << "서버에서 받은 메시지 : " << strRecvMessage << ", 받은 크기 " << bytes_transferred << endl;
            PostWrite();
        }
    }

    int m_nSeqNumber;
    io_service& m_io_service;
    tcp::socket m_socket;
    string m_writeMessage;
    array<char, 128> m_ReceiveBuffer;
};

int main() {
    io_service ioService;
    tcp::endpoint endpoint(boost::asio::ip::address::from_string(SERVER_IP), PORT_NUMBER);
    TCP_Client client(ioService);
    client.Connect(endpoint);
    ioService.run();
    cout << "네트워크 접속 종료" << endl;
    getchar();
    return 0;
}

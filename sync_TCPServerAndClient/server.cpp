#include <iostream>
#include <array>
#include <boost/asio.hpp>

const char SERVER_IP[] = "127.0.0.1";
const unsigned short PORT_NUMBER = 31400;


int main() {

    //커널에서 발생한 I/O 이벤트를 디스패치 해주는 클래스
    boost::asio::io_service io_service;
    //네트워크 주소를 설정하고, 이 주소로 클라이언트가 접속한다.
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), PORT_NUMBER);
    //클라이언트의 접속을 받아들이는 객체
    boost::asio::ip::tcp::acceptor acceptor(io_service, endpoint);
    //tcp 소켓 생성
    boost::asio::ip::tcp::socket socket(io_service);
    //클라이언트의 접속받을 준, 클라이언트의 접속이 완료될 때까지 대기
    acceptor.accept(socket);

    std::cout << "클라이언트 접속" << std::endl;
    for(;;) {
        //클라이언트가 보낸 메시지를 담을 버퍼 생성
        char buf[128] = {0, };
        //시스템에서 발생하는 에러 코드를 랩핑한 클래스 에러가 발생하면 에러 코드와 에러 메시지를 얻을 수 있다
        boost::system::error_code error;
        //클라이언트가 보낸 메시지를 받는다.
        size_t len = socket.read_some(boost::asio::buffer<char>(buf), error);
        //에러가 어떤 에러인지 확인 한다
        if(error) {
            //클라이언트의 접속이 끓기는 경우
            if(error == boost::asio::error::eof) std::cout << "클라이언트와 연결이 끊어졌습니다" << std::endl;
            //진짜 에러인 경우
            else std::cout << "error No : " << error.value() << " error Message: " << error.message() << std::endl;
            break;
        }
        std::cout << "클라이언트에서 받은 메시지" << &buf[0] << std::endl;
        //boost::array를 이용해 버퍼 생성
        boost::array<char, 128> szMessage = {0};
        sprintf(szMessage.begin(), "Re:%s",&buf[0]);
        int nMsgLen = szMessage.size();
        boost::system::error_code ignored_error;
        //클라이언트에게 Echo 메시지를 보낸다. 배열에 있는 데이터를 모두 보내지 않고 인자로 지정한 양만큼 보낸다.
        socket.write_some(boost::asio::buffer(szMessage, nMsgLen), ignored_error);
        std::cout << "클라이언트에 보낸 메시지: " << szMessage.begin() << std::endl;
    }
    getchar();
    return 0;
}
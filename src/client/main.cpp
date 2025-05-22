#include <iostream>
#include <thread>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

void read_from_server(tcp::socket& socket) {
    boost::asio::streambuf buf;
    while (true) {
        boost::system::error_code ec;
        std::size_t n = boost::asio::read_until(socket, buf, '\n', ec);
        if (ec) {
            std::cout << "[서버 연결 종료]" << std::endl;
            break;
        }
        std::istream is(&buf);
        std::string line;
        std::getline(is, line);
        std::cout << "[서버] " << line << std::endl;
    }
}

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        tcp::resolver resolver(io_context);

        // Docker 환경에서는 서버 이름을 'server'로 사용
        std::string server_ip = "server";
        unsigned short server_port = 8080;

        std::cout << "서버에 연결을 시도합니다... (" << server_ip << ":" << server_port << ")" << std::endl;
        
        boost::asio::connect(socket, resolver.resolve(server_ip, std::to_string(server_port)));
        std::cout << "서버에 연결되었습니다." << std::endl;

        // 서버로부터 메시지 읽기 스레드
        std::thread reader([&socket]() { read_from_server(socket); });

        // 사용자 입력 → 서버로 전송
        std::string msg;
        while (std::getline(std::cin, msg)) {
            if (msg == "/quit") break;
            msg += '\n';
            boost::asio::write(socket, boost::asio::buffer(msg));
        }
        socket.close();
        reader.join();
    } catch (std::exception& e) {
        std::cerr << "에러: " << e.what() << std::endl;
    }
    return 0;
} 
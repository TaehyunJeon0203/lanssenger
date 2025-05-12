#include "client/client.hpp"
#include <iostream>

// 클라이언트 객체 생성자
// TCP 소켓을 초기화하고 필요한 시그널/슬롯 연결을 설정
Client::Client() : connected_(false) {
    socket = std::make_unique<boost::asio::ip::tcp::socket>(io_context);
    resolver = std::make_unique<boost::asio::ip::tcp::resolver>(io_context);
}

// 클라이언트 객체 소멸자
Client::~Client() {
    disconnectFromServer();
}

// 서버에 연결을 시도
bool Client::connectToServer(const std::string& host, unsigned short port) {
    try {
        boost::asio::ip::tcp::resolver::results_type endpoints = 
            resolver->resolve(host, std::to_string(port));
        
        boost::asio::connect(*socket, endpoints);
        connected_ = true;
        
        // IO 스레드 시작
        io_thread = std::thread([this]() {
            try {
                io_context.run();
            } catch (const std::exception& e) {
                error(e.what());
            }
        });
        
        // 데이터 읽기 시작
        readData();
        
        connected();
        return true;
    } catch (const std::exception& e) {
        error(e.what());
        return false;
    }
}

// 서버와의 연결을 해제
void Client::disconnectFromServer() {
    if (connected_) {
        try {
            socket->close();
        } catch (...) {}
        
        if (io_thread.joinable()) {
            io_context.stop();
            io_thread.join();
        }
        
        connected_ = false;
        disconnected();
    }
}

// 서버로 메시지를 전송
bool Client::sendMessage(const std::string& message) {
    if (!connected_) {
        error("Not connected to server");
        return false;
    }

    try {
        boost::asio::write(*socket, boost::asio::buffer(message + "\n"));
        return true;
    } catch (const std::exception& e) {
        error(e.what());
        return false;
    }
}

// 현재 서버와의 연결 상태를 확인
bool Client::isConnected() const {
    return connected_;
}

// 소켓에서 데이터를 읽어오는 슬롯
void Client::readData() {
    auto buffer = std::make_shared<std::array<char, MAX_BUFFER_SIZE>>();
    
    socket->async_read_some(
        boost::asio::buffer(*buffer),
        [this, buffer](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                std::string message(buffer->data(), bytes_transferred);
                messageReceived(message);
                readData();  // 다음 데이터 읽기
            } else {
                handleError(error);
            }
        }
    );
}

// 소켓 오류를 처리하는 슬롯
void Client::handleError(const boost::system::error_code& error) {
    if (error == boost::asio::error::eof) {
        // 정상적인 연결 종료
        disconnectFromServer();
    } else {
        // 에러 발생
        this->error(error.message());
        disconnectFromServer();
    }
} 
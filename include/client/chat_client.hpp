#ifndef CHAT_CLIENT_HPP
#define CHAT_CLIENT_HPP

#include <string>
#include <memory>
#include <functional>
#include <thread>
#include <atomic>
#include <boost/asio.hpp>

class ChatClient {
public:
    using MessageCallback = std::function<void(const std::string&)>;

    ChatClient();
    ~ChatClient();

    bool connect(const std::string& host, unsigned short port);
    void setMessageCallback(MessageCallback callback);
    void start();
    void stop();
    void sendMessage(const std::string& message);

private:
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;
    std::thread receive_thread_;
    std::thread io_thread_;
    std::atomic<bool> running_;
    MessageCallback message_callback_;
};

#endif // CHAT_CLIENT_HPP 
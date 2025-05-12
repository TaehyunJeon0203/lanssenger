#pragma once

#include <memory>
#include <string>
#include <QTcpSocket>
#include <QObject>

// LAN 메신저의 클라이언트 클래스
// 서버와의 통신을 담당하며, 메시지 전송 및 수신 기능을 제공
class Client : public QObject {
    Q_OBJECT

public:
    // 클라이언트 객체 생성자
    explicit Client(QObject *parent = nullptr);
    
    // 클라이언트 객체 소멸자
    ~Client();

    // 서버에 연결을 시도
    bool connectToServer(const QString &host, quint16 port);

    // 서버와의 연결을 해제
    void disconnectFromServer();

    // 서버로 메시지를 전송
    bool sendMessage(const QString &message);

    // 현재 서버와의 연결 상태를 확인
    bool isConnected() const;

signals:
    // 메시지를 수신했을 때 발생하는 시그널
    void messageReceived(const QString &message);

    // 서버에 연결되었을 때 발생하는 시그널
    void connected();

    // 서버와의 연결이 해제되었을 때 발생하는 시그널
    void disconnected();

    // 오류가 발생했을 때 발생하는 시그널
    void error(const QString &errorString);

private slots:
    // 소켓에서 데이터를 읽어오는 슬롯
    void readData();

    // 소켓 오류를 처리하는 슬롯
    void handleError(QAbstractSocket::SocketError socketError);

private:
    std::unique_ptr<QTcpSocket> socket_; // TCP 소켓 객체
}; 
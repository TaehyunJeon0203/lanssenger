#pragma once

#include <memory>
#include <string>
#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QMap>

// LAN 메신저의 서버 클래스
// 클라이언트의 연결을 관리하고 메시지를 브로드캐스팅하는 기능을 제공
class Server : public QObject {
    Q_OBJECT

public:
    // 서버 객체 생성자
    explicit Server(QObject *parent = nullptr);
    
    // 서버 객체 소멸자
    ~Server();

    // 서버를 시작
    bool start(quint16 port);

    // 서버를 중지
    void stop();

    // 서버의 실행 상태를 확인
    bool isRunning() const;

signals:
    // 새로운 클라이언트가 연결되었을 때 발생하는 시그널
    void clientConnected(const QString &clientAddress);

    // 클라이언트가 연결을 해제했을 때 발생하는 시그널
    void clientDisconnected(const QString &clientAddress);

    // 클라이언트로부터 메시지를 수신했을 때 발생하는 시그널
    void messageReceived(const QString &clientAddress, const QString &message);

    // 오류가 발생했을 때 발생하는 시그널
    void error(const QString &errorString);

private slots:
    // 새로운 클라이언트 연결을 처리하는 슬롯
    void handleNewConnection();

    // 클라이언트 연결 해제를 처리하는 슬롯
    void handleClientDisconnection();

    // 클라이언트로부터의 데이터를 처리하는 슬롯
    void handleClientData();

private:
    std::unique_ptr<QTcpServer> server_; // TCP 서버 객체
    QMap<QTcpSocket*, QString> clients_; // 연결된 클라이언트 목록 (소켓 -> IP 주소)
}; 
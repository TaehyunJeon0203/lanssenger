#pragma once

#include <memory>
#include <string>
#include <QTcpSocket>
#include <QObject>

/**
 * @brief LAN 메신저의 클라이언트 클래스
 * 
 * 이 클래스는 서버와의 통신을 담당하며, 메시지 전송 및 수신 기능을 제공합니다.
 * Qt의 시그널/슬롯 메커니즘을 사용하여 이벤트를 처리합니다.
 */
class Client : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 클라이언트 객체 생성자
     * @param parent 부모 QObject (기본값: nullptr)
     */
    explicit Client(QObject *parent = nullptr);
    
    /**
     * @brief 클라이언트 객체 소멸자
     */
    ~Client();

    /**
     * @brief 서버에 연결을 시도합니다.
     * @param host 서버의 호스트 주소
     * @param port 서버의 포트 번호
     * @return 연결 성공 여부
     */
    bool connectToServer(const QString &host, quint16 port);

    /**
     * @brief 서버와의 연결을 해제합니다.
     */
    void disconnectFromServer();

    /**
     * @brief 서버로 메시지를 전송합니다.
     * @param message 전송할 메시지
     * @return 전송 성공 여부
     */
    bool sendMessage(const QString &message);

    /**
     * @brief 현재 서버와의 연결 상태를 확인합니다.
     * @return 연결 상태 (true: 연결됨, false: 연결되지 않음)
     */
    bool isConnected() const;

signals:
    /**
     * @brief 메시지를 수신했을 때 발생하는 시그널
     * @param message 수신된 메시지
     */
    void messageReceived(const QString &message);

    /**
     * @brief 서버에 연결되었을 때 발생하는 시그널
     */
    void connected();

    /**
     * @brief 서버와의 연결이 해제되었을 때 발생하는 시그널
     */
    void disconnected();

    /**
     * @brief 오류가 발생했을 때 발생하는 시그널
     * @param errorString 오류 메시지
     */
    void error(const QString &errorString);

private slots:
    /**
     * @brief 소켓에서 데이터를 읽어오는 슬롯
     */
    void readData();

    /**
     * @brief 소켓 오류를 처리하는 슬롯
     * @param socketError 발생한 소켓 오류
     */
    void handleError(QAbstractSocket::SocketError socketError);

private:
    std::unique_ptr<QTcpSocket> socket_; ///< TCP 소켓 객체
}; 
#pragma once

#include <memory>
#include <string>
#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QMap>

/**
 * @brief LAN 메신저의 서버 클래스
 * 
 * 이 클래스는 클라이언트의 연결을 관리하고 메시지를 브로드캐스팅하는 기능을 제공합니다.
 * Qt의 시그널/슬롯 메커니즘을 사용하여 이벤트를 처리합니다.
 */
class Server : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 서버 객체 생성자
     * @param parent 부모 QObject (기본값: nullptr)
     */
    explicit Server(QObject *parent = nullptr);
    
    /**
     * @brief 서버 객체 소멸자
     */
    ~Server();

    /**
     * @brief 서버를 시작합니다.
     * @param port 서버가 리스닝할 포트 번호
     * @return 서버 시작 성공 여부
     */
    bool start(quint16 port);

    /**
     * @brief 서버를 중지합니다.
     */
    void stop();

    /**
     * @brief 서버의 실행 상태를 확인합니다.
     * @return 서버 실행 상태 (true: 실행 중, false: 중지됨)
     */
    bool isRunning() const;

signals:
    /**
     * @brief 새로운 클라이언트가 연결되었을 때 발생하는 시그널
     * @param clientAddress 클라이언트의 IP 주소
     */
    void clientConnected(const QString &clientAddress);

    /**
     * @brief 클라이언트가 연결을 해제했을 때 발생하는 시그널
     * @param clientAddress 연결 해제된 클라이언트의 IP 주소
     */
    void clientDisconnected(const QString &clientAddress);

    /**
     * @brief 클라이언트로부터 메시지를 수신했을 때 발생하는 시그널
     * @param clientAddress 메시지를 보낸 클라이언트의 IP 주소
     * @param message 수신된 메시지
     */
    void messageReceived(const QString &clientAddress, const QString &message);

    /**
     * @brief 오류가 발생했을 때 발생하는 시그널
     * @param errorString 오류 메시지
     */
    void error(const QString &errorString);

private slots:
    /**
     * @brief 새로운 클라이언트 연결을 처리하는 슬롯
     */
    void handleNewConnection();

    /**
     * @brief 클라이언트 연결 해제를 처리하는 슬롯
     */
    void handleClientDisconnection();

    /**
     * @brief 클라이언트로부터의 데이터를 처리하는 슬롯
     */
    void handleClientData();

private:
    std::unique_ptr<QTcpServer> server_; ///< TCP 서버 객체
    QMap<QTcpSocket*, QString> clients_; ///< 연결된 클라이언트 목록 (소켓 -> IP 주소)
}; 
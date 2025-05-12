#include "client/client.hpp"
#include <QDebug>

/**
 * @brief 클라이언트 객체 생성자
 * 
 * TCP 소켓을 초기화하고 필요한 시그널/슬롯 연결을 설정합니다.
 * 
 * @param parent 부모 QObject
 */
Client::Client(QObject *parent)
    : QObject(parent)
    , socket_(std::make_unique<QTcpSocket>(this))
{
    // 소켓의 readyRead 시그널을 readData 슬롯에 연결
    connect(socket_.get(), &QTcpSocket::readyRead, this, &Client::readData);
    // 소켓의 connected 시그널을 클라이언트의 connected 시그널에 연결
    connect(socket_.get(), &QTcpSocket::connected, this, &Client::connected);
    // 소켓의 disconnected 시그널을 클라이언트의 disconnected 시그널에 연결
    connect(socket_.get(), &QTcpSocket::disconnected, this, &Client::disconnected);
    // 소켓의 error 시그널을 handleError 슬롯에 연결
    connect(socket_.get(), QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, &Client::handleError);
}

/**
 * @brief 클라이언트 객체 소멸자
 */
Client::~Client() = default;

/**
 * @brief 서버에 연결을 시도합니다.
 * 
 * @param host 서버의 호스트 주소
 * @param port 서버의 포트 번호
 * @return 연결 성공 여부
 */
bool Client::connectToServer(const QString &host, quint16 port)
{
    socket_->connectToHost(host, port);
    return socket_->waitForConnected(5000); // 5초 동안 연결 대기
}

/**
 * @brief 서버와의 연결을 해제합니다.
 */
void Client::disconnectFromServer()
{
    if (socket_->state() == QAbstractSocket::ConnectedState) {
        socket_->disconnectFromHost();
    }
}

/**
 * @brief 서버로 메시지를 전송합니다.
 * 
 * @param message 전송할 메시지
 * @return 전송 성공 여부
 */
bool Client::sendMessage(const QString &message)
{
    if (socket_->state() == QAbstractSocket::ConnectedState) {
        return socket_->write(message.toUtf8()) != -1;
    }
    return false;
}

/**
 * @brief 현재 서버와의 연결 상태를 확인합니다.
 * 
 * @return 연결 상태 (true: 연결됨, false: 연결되지 않음)
 */
bool Client::isConnected() const
{
    return socket_->state() == QAbstractSocket::ConnectedState;
}

/**
 * @brief 소켓에서 데이터를 읽어오는 슬롯
 * 
 * 소켓에서 데이터를 읽어와 messageReceived 시그널을 발생시킵니다.
 */
void Client::readData()
{
    QByteArray data = socket_->readAll();
    emit messageReceived(QString::fromUtf8(data));
}

/**
 * @brief 소켓 오류를 처리하는 슬롯
 * 
 * @param socketError 발생한 소켓 오류
 */
void Client::handleError(QAbstractSocket::SocketError socketError)
{
    emit error(socket_->errorString());
} 
#include "server/server.hpp"
#include <QDebug>

// 서버 객체 생성자
// TCP 서버를 초기화하고 새로운 연결을 처리하기 위한 시그널/슬롯 연결을 설정
Server::Server(QObject *parent)
    : QObject(parent)
    , server_(std::make_unique<QTcpServer>(this))
{
    // 새로운 연결이 들어올 때 handleNewConnection 슬롯을 호출
    connect(server_.get(), &QTcpServer::newConnection, this, &Server::handleNewConnection);
}

// 서버 객체 소멸자
// 서버를 중지하고 모든 클라이언트 연결을 정리
Server::~Server()
{
    stop();
}

// 서버를 시작
// 지정된 포트에서 클라이언트의 연결을 수락하기 시작
bool Server::start(quint16 port)
{
    if (!server_->listen(QHostAddress::Any, port)) {
        emit error(server_->errorString());
        return false;
    }
    return true;
}

// 서버를 중지
// 서버를 중지하고 모든 클라이언트 연결을 종료
void Server::stop()
{
    if (server_->isListening()) {
        server_->close();
        // 모든 클라이언트 연결 종료
        for (auto it = clients_.begin(); it != clients_.end(); ++it) {
            it.key()->disconnectFromHost();
        }
        clients_.clear();
    }
}

// 서버의 실행 상태를 확인
bool Server::isRunning() const
{
    return server_->isListening();
}

// 새로운 클라이언트 연결을 처리하는 슬롯
// 새로운 클라이언트가 연결되면 필요한 시그널/슬롯 연결을 설정하고 클라이언트 목록에 추가
void Server::handleNewConnection()
{
    QTcpSocket *clientSocket = server_->nextPendingConnection();
    QString clientAddress = clientSocket->peerAddress().toString();

    // 클라이언트 소켓의 시그널을 적절한 슬롯에 연결
    connect(clientSocket, &QTcpSocket::disconnected, this, &Server::handleClientDisconnection);
    connect(clientSocket, &QTcpSocket::readyRead, this, &Server::handleClientData);

    // 클라이언트 목록에 추가
    clients_[clientSocket] = clientAddress;
    emit clientConnected(clientAddress);
}

// 클라이언트 연결 해제를 처리하는 슬롯
// 클라이언트가 연결을 해제하면 클라이언트 목록에서 제거하고 관련 리소스를 정리
void Server::handleClientDisconnection()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        QString clientAddress = clients_[clientSocket];
        clients_.remove(clientSocket);
        clientSocket->deleteLater();
        emit clientDisconnected(clientAddress);
    }
}

// 클라이언트로부터의 데이터를 처리하는 슬롯
// 클라이언트로부터 메시지를 수신하면 다른 모든 클라이언트에게 브로드캐스팅
void Server::handleClientData()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        QByteArray data = clientSocket->readAll();
        QString message = QString::fromUtf8(data);
        emit messageReceived(clients_[clientSocket], message);

        // 다른 모든 클라이언트에게 메시지 전달
        for (auto it = clients_.begin(); it != clients_.end(); ++it) {
            if (it.key() != clientSocket) {
                it.key()->write(data);
            }
        }
    }
} 
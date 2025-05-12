#include "server/server.hpp"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>

/**
 * @brief LAN 메신저 서버의 메인 함수
 * 
 * 명령줄 인터페이스를 통해 서버를 시작하고 클라이언트의 연결을 관리합니다.
 * 
 * @param argc 명령줄 인자의 개수
 * @param argv 명령줄 인자 배열
 * @return 프로그램 종료 코드
 */
int main(int argc, char *argv[])
{
    // Qt 애플리케이션 초기화
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("LANssenger Server");
    QCoreApplication::setApplicationVersion("1.0");

    // 명령줄 파서 설정
    QCommandLineParser parser;
    parser.setApplicationDescription("LANssenger Server");
    parser.addHelpOption();
    parser.addVersionOption();

    // 포트 번호 옵션 추가
    QCommandLineOption portOption(QStringList() << "p" << "port",
                                "Port number to listen on", "port", "8080");

    parser.addOption(portOption);
    parser.process(app);

    // 서버 객체 생성
    Server server;
    QTextStream out(stdout);

    // 클라이언트 연결 시그널 처리
    QObject::connect(&server, &Server::clientConnected, [&](const QString &clientAddress) {
        out << "Client connected: " << clientAddress << Qt::endl;
    });

    // 클라이언트 연결 해제 시그널 처리
    QObject::connect(&server, &Server::clientDisconnected, [&](const QString &clientAddress) {
        out << "Client disconnected: " << clientAddress << Qt::endl;
    });

    // 메시지 수신 시그널 처리
    QObject::connect(&server, &Server::messageReceived, [&](const QString &clientAddress, const QString &message) {
        out << "Message from " << clientAddress << ": " << message << Qt::endl;
    });

    // 오류 시그널 처리
    QObject::connect(&server, &Server::error, [&](const QString &errorString) {
        out << "Error: " << errorString << Qt::endl;
    });

    // 서버 시작
    quint16 port = parser.value(portOption).toUShort();

    if (!server.start(port)) {
        out << "Failed to start server" << Qt::endl;
        return 1;
    }

    out << "Server started on port " << port << Qt::endl;
    out << "Press Ctrl+C to stop the server" << Qt::endl;

    return app.exec();
} 
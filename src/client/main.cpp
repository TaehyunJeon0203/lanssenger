#include "client/client.hpp"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>
#include <iostream>

/**
 * @brief LAN 메신저 클라이언트의 메인 함수
 * 
 * 명령줄 인터페이스를 통해 서버에 연결하고 메시지를 주고받는 기능을 제공합니다.
 * 
 * @param argc 명령줄 인자의 개수
 * @param argv 명령줄 인자 배열
 * @return 프로그램 종료 코드
 */
int main(int argc, char *argv[])
{
    // Qt 애플리케이션 초기화
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("LANssenger Client");
    QCoreApplication::setApplicationVersion("1.0");

    // 명령줄 파서 설정
    QCommandLineParser parser;
    parser.setApplicationDescription("LANssenger Client");
    parser.addHelpOption();
    parser.addVersionOption();

    // 호스트 주소 옵션 추가
    QCommandLineOption hostOption(QStringList() << "h" << "host",
                                "Server host address", "host", "localhost");
    // 포트 번호 옵션 추가
    QCommandLineOption portOption(QStringList() << "p" << "port",
                                "Server port number", "port", "8080");

    parser.addOption(hostOption);
    parser.addOption(portOption);
    parser.process(app);

    // 클라이언트 객체 생성
    Client client;
    QTextStream out(stdout);
    QTextStream in(stdin);

    // 메시지 수신 시그널 처리
    QObject::connect(&client, &Client::messageReceived, [&](const QString &message) {
        out << "Received: " << message << Qt::endl;
    });

    // 연결 성공 시그널 처리
    QObject::connect(&client, &Client::connected, [&]() {
        out << "Connected to server" << Qt::endl;
    });

    // 연결 해제 시그널 처리
    QObject::connect(&client, &Client::disconnected, [&]() {
        out << "Disconnected from server" << Qt::endl;
    });

    // 오류 시그널 처리
    QObject::connect(&client, &Client::error, [&](const QString &errorString) {
        out << "Error: " << errorString << Qt::endl;
    });

    // 서버 연결 시도
    QString host = parser.value(hostOption);
    quint16 port = parser.value(portOption).toUShort();

    if (!client.connectToServer(host, port)) {
        out << "Failed to connect to server" << Qt::endl;
        return 1;
    }

    // 메시지 입력 루프
    out << "Enter messages (type 'quit' to exit):" << Qt::endl;

    while (true) {
        QString message = in.readLine();
        if (message == "quit") {
            break;
        }
        if (!client.sendMessage(message)) {
            out << "Failed to send message" << Qt::endl;
        }
    }

    return 0;
} 
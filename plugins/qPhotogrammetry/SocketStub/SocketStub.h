#ifndef SOCKETSTUB_H
#define SOCKETSTUB_H
#include <QJsonObject>
#include <QJsonDocument>
#include <QPlainTextEdit>
#include <string>
#include <cstring>      // Needed for memset
#include <netdb.h>      // Needed for the socket functions
#include <sys/socket.h> // Needed for the socket functions
class SocketStub
{
public:
    SocketStub( );
    SocketStub(std::string domain, std::string port);

    QString send_command(QJsonObject jsonObject);
    char* send_f(QByteArray file, QString suffix);
    QString send_files(QString files);
    void close_socket();

    int status;
    int socketfd ; // The socket descripter
    struct addrinfo host_info;       // The struct that getaddrinfo() fills up with data.
    struct addrinfo *host_info_list; // Pointer to the to the linked list of host_info's.

};

#endif // SOCKETSTUB_H

#include "SocketStub.h"
#include <string>
#include <QJsonObject>
#include <QJsonDocument>

SocketStub::SocketStub()
{}
SocketStub::SocketStub( std::string domain, std::string port)
{
    // The MAN page of getaddrinfo() states "All  the other fields in the structure pointed
    // to by hints must contain either 0 or a null pointer, as appropriate." When a struct
    // is created in c++, it will be given a block of memory. This memory is not nessesary
    // empty. Therefor we use the memset function to make sure all fields are NULL.
    memset(&host_info, 0, sizeof host_info);

    //ui->plainTextEdit->insertPlainText(QString("Setting up the structs..."));

    host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
    host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.

    // Now fill up the linked list of host_info structs with google's address information.
    status = getaddrinfo(domain.c_str(), port.c_str(), &host_info, &host_info_list);
    // getaddrinfo returns 0 on succes, or some other value when an error occured.
    // (translated into human readable text by the gai_gai_strerror function).
    //if (status != 0)  ui->plainTextEdit->insertPlainText(QString( "getaddrinfo error") + QString(gai_strerror(status)));

    //ui->plainTextEdit->insertPlainText(QString("Creating a socket..."));
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    //if (socketfd == -1)  ui->plainTextEdit->insertPlainText(QString("socket error ")) ;

    //ui->plainTextEdit->insertPlainText(QString("Connect()ing..."));

    status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    //if (status == -1)  ui->plainTextEdit->insertPlainText(QString( "connect error" ));

}

char * SocketStub::send_command(QJsonObject jsonObject, int buffLen)
{
    QJsonDocument jsonDoc(jsonObject);
    QString strJson(jsonDoc.toJson());
    std::string str = strJson.toStdString();

    char * msg = new char[str.size() + 1];
    std::copy(str.begin(), str.end(), msg);
    msg[str.size()] = '\0'; // don't forget the terminating 0

    //char * msg = strJson.toStdString().c_str();
    int len;
    ssize_t bytes_sent;
    len = strlen(msg);
    bytes_sent = send(socketfd, msg, len, 0);

    // don't forget to free the string after finished using it
    delete[] msg;

    //ui->plainTextEdit->insertPlainText(QString( "Waiting to recieve data..."));

    ssize_t bytes_recieved;
    char incomming_data_buffer[buffLen];
    bytes_recieved = recv(socketfd, incomming_data_buffer, buffLen, 0);

    // If no data arrives, the program will just wait here until some data arrives.
    //if (bytes_recieved == 0) incomming_data_buffer = 'host shut down.';
    //if (bytes_recieved == -1) incomming_data_buffer = "recieve error!";

    //ui->plainTextEdit->insertPlainText(QString::number( bytes_recieved ) + QString( " bytes recieved :"));

    incomming_data_buffer[bytes_recieved] = '\0';
    //ui->plainTextEdit->insertPlainText(QString( incomming_data_buffer ));
    //ui->plainTextEdit->insertPlainText(QString( "Receiving complete. Closing socket..." ));
    return incomming_data_buffer;
}

void SocketStub::close_socket()
{
    char msg[5];
    strcpy(msg, "exit");
    int len = strlen(msg);
    ssize_t bytes_sent = send(socketfd, msg, len, 0);
    //freeaddrinfo(host_info_list);
    //close(socketfd);
    //socketfd.close();
}


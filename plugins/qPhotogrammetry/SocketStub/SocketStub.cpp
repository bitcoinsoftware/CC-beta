#include "SocketStub.h"
#include <string>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStringList>
#include <QPlainTextEdit>
#include <QFile>
#include <QDir>

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

QString SocketStub::send_command(QJsonObject jsonObject)
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
    char incomming_data_buffer[1000];
    bytes_recieved = recv(socketfd, incomming_data_buffer, 999, 0);

    // If no data arrives, the program will just wait here until some data arrives.
    //if (bytes_recieved == 0) incomming_data_buffer = 'host shut down.';
    //if (bytes_recieved == -1) incomming_data_buffer = "recieve error!";

    //ui->plainTextEdit->insertPlainText(QString::number( bytes_recieved ) + QString( " bytes recieved :"));

    incomming_data_buffer[bytes_recieved] = '\0';
    //ui->plainTextEdit->insertPlainText(QString( incomming_data_buffer ));
    //ui->plainTextEdit->insertPlainText(QString( "Receiving complete. Closing socket..." ));
    return QString(incomming_data_buffer);
}

char* SocketStub::send_f(QByteArray file, QString suffix) {
    char buffer[1001];
    buffer[1000]='\0';
    int size=file.length();
    //size=20;
    char start[10];
    start[0]='s';
    start[1]=size>>24;
    start[2]=size>>16;
    start[3]=size>>8;
    start[4]=size;
    start[5]=(suffix.length()>0?suffix.toLatin1().data()[0]:'\0');
    start[6]=(suffix.length()>1?suffix.toLatin1().data()[1]:'\0');
    start[7]=(suffix.length()>2?suffix.toLatin1().data()[2]:'\0');
    start[8]=(suffix.length()>3?suffix.toLatin1().data()[3]:'\0');
    start[9]=(suffix.length()>4?suffix.toLatin1().data()[4]:'\0');
    send(socketfd, start, 10, 0);
    recv(socketfd, buffer, 1000, 0);
    long long sent=0;
    while (sent<size) {
        sent+=send(socketfd, file.data()+sent, size-sent, 0);
    }
    
    return buffer;
}

/**
 * function sending files to a server
 * @files list of files
 * @buffLen length of buffer with returning message
 */
QString SocketStub::send_files(QString files) {
    QDir dir(files);
    QStringList list = dir.entryList(QDir::Files);
    ssize_t bytes_recieved;
    QString incomming_data_buffer("\nFile list from "+files+":\n");
    //send files
    for (QString s : list) {
        QString filename=dir.absolutePath()+"/"+s;
        //open file from path s
        QFile f(filename);
        int i=0;
        if (f.open(QIODevice::ReadOnly)) {//read and send file
            incomming_data_buffer+=filename+"\n";
            QByteArray file_b = f.readAll();
            f.close();
            char* answer=send_f(file_b,QFileInfo(f).suffix());
            incomming_data_buffer+=QString(answer[0])+QString::number(((answer[1]*256+answer[2])*256+answer[3])*256+answer[4])+QString(answer[5])+QString(answer[6])+QString(answer[7])+QString(answer[8])+QString(answer[9])+"\n";
        }
    }
    char end[10]="ending   ";
    send(socketfd, end, 10, 0);
    return incomming_data_buffer;
}

void SocketStub::close_socket()
{
    char msg[5];
    //freeaddrinfo(host_info_list);
    //close(socketfd);
    //socketfd.close();
}


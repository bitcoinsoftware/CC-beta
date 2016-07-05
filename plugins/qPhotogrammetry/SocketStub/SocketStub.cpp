#include "SocketStub.h"
#include <string>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStringList>
#include <QPlainTextEdit>
#include <QFile>
#include <QDir>
#include <QTextStream>

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

QString SocketStub::send_json(QJsonObject jsonObject, char option, char stages) {
    send(socketfd, &option, 1, 0);
    send(socketfd, &stages, 1, 0);
    QString ans("");
    if (option=='c') {
        QString s(QJsonDocument(jsonObject).toJson());
        send(socketfd, s.toLatin1().data(), s.length(), 0);
    }
    char answer[100];
    answer[recv(socketfd, answer, 99, 0)]='\0';
    return QString(answer);
}

void SocketStub::send_f(QByteArray file, QString suffix) {
    int size=file.length();
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
    long long sent=0;
    while (sent<size) {
        sent+=send(socketfd, file.data()+sent, size-sent, 0);
    }
}

/**
 * function sending files to a server
 * @files list of files
 * @buffLen length of buffer with returning message
 */
QString SocketStub::send_files(QString files) {
    QDir dir(files);
    QStringList list = dir.entryList(QDir::Files);
    QString incomming_data_buffer("File list from "+files+":\n");
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
            send_f(file_b,QFileInfo(f).suffix());
        }
    }
    char end[10]="ending   ";
    send(socketfd, end, 10, 0);
    return incomming_data_buffer;
}

char SocketStub::get_result(char stage, QString output_dir, QTextStream* log) {
    if (stage==-1) {
        char next;
        unsigned char info[5];
        recv(socketfd, &next, 1, 0);
        while (next=='f') {
            recv(socketfd, info, 5, 0);
            char filename[257];
            recv(socketfd, filename, info[4], 0);
            filename[info[4]]='\0';
            QFile f(output_dir+"/"+QString(filename));
            f.open(QIODevice::WriteOnly | QIODevice::Truncate);
            int size = ((info[0]*256+info[1])*256+info[2])*256+info[3];
            int dl = 0;
            char buffer[4096];
            while (dl<size) {
                int received = recv(socketfd, buffer, (size-dl>4096 ? 4096 : size-dl), 0);
                dl += received;
                f.write(buffer,received);
            }
            *log<<"file "<<filename<<" saved\n";
            recv(socketfd, &next, 1, 0);
        }
        return 0;
    } else {
        char st;
        recv(socketfd, &st, 1, 0);
        return st;
    }
}

void SocketStub::close_socket()
{
    //freeaddrinfo(host_info_list);
    //close(socketfd);
    //socketfd.close();
}


#include "bbbserver.h"
#include <QtCore>
BBBServer::BBBServer(QObject *parent) :
    QObject(parent)
{
    server=new QTcpServer(this);
    connect(server,SIGNAL(newConnection()),this,SLOT(acceptConnection()),Qt::DirectConnection);

    if(!server->listen(QHostAddress::Any,1234)){

        qDebug()<<"server cant start!";

    }
    else{
        qDebug()<<"server started! on thread "<<QThread::currentThreadId();

    }
}
void BBBServer::acceptConnection(){
    client=server->nextPendingConnection();
    connect(client, SIGNAL(readyRead()),
            this, SLOT(startRead()),Qt::DirectConnection);
    connect(client, SIGNAL(disconnected()),this, SLOT(clientDisconnected()),Qt::DirectConnection);
    qDebug()<<"client connected";
    client->write("hello client\r\n");
    client->flush();
}

void BBBServer::startRead(){
    char buffer[1024] = {0};
    client->read(buffer, client->bytesAvailable());
    //client->readLine(buffer, client->bytesAvailable());
    QString commands(buffer);
    qDebug()<<commands;

    commands=commands.simplified();
    if(commands.left(6)== this->start_a_run){//"start_"
        qDebug()<<"starting a run";
        client->write("starting a run\n");//echo back
        client->flush();
        emit signal_SPI_start(commands.right(3));
        qDebug()<<"not blocked";
    }
    else if(commands== this->stop_current_run){//"stop"
        qDebug()<<"Stopping the current run";
        client->write("Stopping the current run\n");//echo back
        client->flush();
        emit signal_SPI_stop();

    }
    else if(commands== this->test_device){
        qDebug()<<"waiting for dspic to response";

       // client->write("Stopping the current run\n");//echo back
       // client->flush();
        emit send_test_device();

    }
    else {

        client->write("Unknow command\n");//echo back
        client->flush();

    }



}
void BBBServer::clientDisconnected(){
    qDebug()<<"client disconnected";
}
BBBServer::~BBBServer(){
    server->close();
    client->close();

}

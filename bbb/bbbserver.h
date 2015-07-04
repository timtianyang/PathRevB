#ifndef BBBSERVER_H
#define BBBSERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>


class BBBServer : public QObject
{
    Q_OBJECT
public:
    explicit BBBServer(QObject *parent = 0);
    //BBBServer
    ~BBBServer();
signals:
    void signal_SPI_stop();
    void signal_SPI_start(QString);
    void send_test_device();
public slots:
    void acceptConnection(void);
    void startRead(void);
    void clientDisconnected(void);
private:
    QTcpServer *server;
    QTcpSocket *client;

    QString *start_a_run=new QString("start_");
    QString *stop_current_run=new QString("stop");
    QString *test_device=new QString("test");




};

#endif // BBBSERVER_H

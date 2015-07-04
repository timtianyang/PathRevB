#include <QCoreApplication>

#include <C920.h>
#include <SpiDriver.h>
#include <flashdriver_helper.h>
#include <bbbserver.h>
#include <sensor_interrupt.h>
#include <qdebug.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FlashDriver_Helper *flashDrive=new FlashDriver_Helper();
    flashDrive->mount_a_flashdrive();

    BBBServer server;
    QThread serverThread;
    server.moveToThread(&serverThread);
    serverThread.start();


    C920 cam;
    QThread camThread;
    cam.moveToThread(&camThread);

    SpiDriver spi;
    QThread spiThread;
    spi.moveToThread(&spiThread);

    sensor_Interrupt sensor_interrupt;
    QThread sensor_int_thread;
    sensor_interrupt.moveToThread(&sensor_int_thread);

    sensor_interrupt.connect(&server,SIGNAL(signal_SPI_start(QString)),SLOT(wait_for_sensor_edge()),Qt::QueuedConnection);
    sensor_interrupt.connect(&server,SIGNAL(signal_SPI_stop()),SLOT(stop_checking_sensor()),Qt::DirectConnection);
    spi.connect(&server,SIGNAL(signal_SPI_stop()),SLOT(stopCurrentRun()),Qt::DirectConnection);//block server thread until SPI stops
    spi.connect(&server,SIGNAL(signal_SPI_start(QString)),SLOT(startANewrun(QString)),Qt::QueuedConnection);;//won't blcok server
    spi.connect(&sensor_interrupt,SIGNAL(sensor_ready()),SLOT(sensor_int()),Qt::DirectConnection);

    spi.connect(&server,SIGNAL(send_test_device()),SLOT(test_dspic()));
    cam.connect(&spi,
                SIGNAL(timeToTakePic()),
                SLOT(takeOnePic()),Qt::QueuedConnection);//won't block SPI

    sensor_int_thread.start();
    camThread.start();
    spiThread.start();
    spiThread.setPriority(QThread::TimeCriticalPriority);

    while(1)
    {
        sleep(3);
    }
    qDebug()<<"done running";
    flashDrive->unmount_the_flashdrive();
    return a.exec();
}

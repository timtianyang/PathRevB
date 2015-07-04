#include "sensor_interrupt.h"
#include "GPIOManager.h"
#include "GPIOConst.h"
#include <QDebug>
sensor_Interrupt::sensor_Interrupt(QObject *parent) :
    QObject(parent)
{
    RTS = GPIO::GPIOManager::getInstance();
    pinRTS = GPIO::GPIOConst::getInstance()->getGpioByKey("P8_10");
    RTS->exportPin(pinRTS);
    RTS->setDirection(pinRTS, GPIO::INPUT);
    RTS->setEdge(pinRTS, GPIO::RISING);
    check_sensor=false;

}
void sensor_Interrupt::wait_for_sensor_edge(void){
    check_sensor=true;
    while(check_sensor){
        RTS->waitForEdge(pinRTS,GPIO::RISING);
       // interrupts++;
        emit sensor_ready();
    }
}
void sensor_Interrupt::stop_checking_sensor(void){
    check_sensor=false;
    //qDebug()<<"there are "+QString::number(interrupts)+"interrupts";

}

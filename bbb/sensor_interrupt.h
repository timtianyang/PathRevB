#ifndef SENSOR_INTERRUPT_H
#define SENSOR_INTERRUPT_H
#include "GPIOManager.h"
#include "GPIOConst.h"
#include <QObject>

class sensor_Interrupt : public QObject
{
    Q_OBJECT
public:
    explicit sensor_Interrupt(QObject *parent = 0);
    GPIO::GPIOManager* RTS;
    int pinRTS;
    bool check_sensor;
   // int interrupts=0;
signals:
    void sensor_ready();
public slots:
    void wait_for_sensor_edge();
    void stop_checking_sensor();
};

#endif // SENSOR_INTERRUPT_H

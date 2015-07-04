#ifndef FLASHDRIVER_HELPER_H
#define FLASHDRIVER_HELPER_H
#include <QtCore>
#include <stdint.h>
class FlashDriver_Helper
{
public:
    FlashDriver_Helper();
    void mount_a_flashdrive(void);
    void unmount_the_flashdrive(void);
    QString mounting_point;//the place where is flash drive was recognized

};

#endif // FLASHDRIVER_HELPER_H

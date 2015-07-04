#include "flashdriver_helper.h"
#include <sys/mount.h>
#include <errno.h>


FlashDriver_Helper::FlashDriver_Helper()
{
    this->mounting_point=QString("Just Init");//initialize
}
void FlashDriver_Helper::mount_a_flashdrive(void){
    qDebug("mounting a USB flash drive on sda...");
    if (mount("/dev/sda", "/media", "vfat", MS_NOATIME, NULL)){//mounting flash drive

        qDebug("re-trying sda1..");
        if (mount("/dev/sda1", "/media", "vfat", MS_NOATIME, NULL)){//mounting flash drive
            qDebug("sda1 failed...");
            this->mounting_point="NOWHERE";
            //return a.exec();
        }
        else{
            qDebug("sucessfully mounted a flash drive on sda1");
            this->mounting_point=QString("sda1");
        }
    }
    else{

        qDebug("sucessfully mounted a flash drive on sda");
         this->mounting_point=QString("sda");
    }
}
void FlashDriver_Helper::unmount_the_flashdrive(void){
    int status;
    status = umount2("/media",MNT_DETACH);
    if(status!=0){
        qDebug()<<"Fail to unmount"+ this->mounting_point;
        qDebug("USB unmount - status = %s\n", strerror(errno));
    }
    else{
        qDebug()<<"sucessfully unmounted a drive on "+ this->mounting_point;
    }
}

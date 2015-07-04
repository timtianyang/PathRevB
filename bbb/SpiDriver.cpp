#include "SpiDriver.h"
#include <QtCore>
#include <stdint.h>
#include <linux/spi/spidev.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <qdebug.h>
#include "GPIOManager.h"
#include "GPIOConst.h"
#include <QElapsedTimer>
#include <Sleeper.h>
#define MAX_PATH_LEN  40
#define SPI_DEVICE_PATH_BASE "/dev/spidev"

SpiDriver::SpiDriver()
{
    active = false;
    mode = 0;
    bpw = 8;
    speed = 0;
    fd = -1;
    lsb_first = false;
    SpiDriver::open(1,0);
    SpiDriver::setMode(0);
    SpiDriver::setSpeed(24000000);
    SpiDriver::setBitsPerWord(8);
    SpiDriver::setLSBFirst(false);

    spiReadFlag=false;
    sensor_has_new_data=false;
}

void SpiDriver::startANewrun(QString SPI_File_Name){
    spiReadFlag=true;//a new run has started

    //####################################preparing command and transfer buffer
    uint8_t wbuf[129];
    uint8_t rbuf[129];
    for(int i=1;i<129;i++){
        rbuf[i]=0x2F;
        wbuf[i]=0xAC;
    }
    wbuf[0]=0xBC;

    uint8_t w_set_samp_flag[]={0xAD,0xAD};//set sampling flag command
    uint8_t state_response[]={0xFF,0xFF};//response buff
    uint8_t w_clears_samp_flag[]={0xBD,0xBD};//clear sampling flag command

    txinfo.rx_buf = (__u64 ) state_response;
    txinfo.tx_buf = (__u64 ) w_set_samp_flag;
    txinfo.len = 2;
    txinfo.delay_usecs = 0;
    txinfo.speed_hz = speed;
    txinfo.bits_per_word = bpw;
    txinfo.cs_change = 1;
    SpiDriver::xfer1();//set sampling flag
    if(state_response[1]!=0xAD){
        qDebug()<<"fail to set dsPIC state";

    }//check echo back


    txinfo.rx_buf = (__u64 ) rbuf;//prepare transfer buffer
    txinfo.tx_buf = (__u64 ) wbuf;
    txinfo.len = 129;



    qDebug()<<"a new run has begun";


    QByteArray arrayWrite;


    int iteration=0;

    //CTS->setValue(pinCTS,GPIO::HIGH);

    // CTS->setValue(pinCTS,GPIO::LOW);//signal dsPIC to start a new run


    while(1){
        if(spiReadFlag==false){
            break;
        }
        while(sensor_has_new_data==0){
            if(!spiReadFlag){break;}
            //passive interrupt compensation
            // if(rbuf[127]==6&&rbuf[128]==0){//when dspic is 10 ahead
            //     sensor_has_new_data+=5;
            // }
            if(rbuf[127]==10&&rbuf[128]==0){//when dspic is 10 ahead
                sensor_has_new_data+=9;
            }
            else if(rbuf[127]==4&&rbuf[128]==0){//when dspic is 11 from BBB
                sensor_has_new_data+=2;

            }
            else if(rbuf[127]==5&&rbuf[128]==0){//when dspic is 10 from BBB
                sensor_has_new_data+=3;

            }
            else if(rbuf[127]==6&&rbuf[128]==0){//when dspic is 9 from BBB
                sensor_has_new_data+=4;

            }
            else if(rbuf[127]==15&&rbuf[128]==0){//when dspic is 9 from BBB
                sensor_has_new_data+=10;

            }
            else if(rbuf[127]==0&&rbuf[128]==5){//when dspic is 5 from BBB
                sensor_has_new_data+=9;}
            else if(rbuf[127]==0&&rbuf[128]==3){//when dspic is 3 from BBB
                sensor_has_new_data+=11;

            }

            else{
                Sleeper::usleep(5);//avoid CPU burning
            }

        }
        //if(!spiReadFlag){break;}
        sensor_has_new_data--;

        SpiDriver::xfer1();
        arrayWrite.append((const char*)(&rbuf[1]),128);

        iteration++;
        if(iteration%1000==0){
            emit timeToTakePic();
           // qDebug()<<"pic";
        }
    }

    txinfo.rx_buf = (__u64 ) state_response;//clear dsPIC samp flag
    txinfo.tx_buf = (__u64 ) w_clears_samp_flag;
    txinfo.len = 2;
    SpiDriver::xfer1();
    if(state_response[1]!=0xBD){
        qDebug()<<"fail to clear dsPIC state";

    }//check echo back


    qDebug()<<"there are "+QString::number(iteration)+" iterations";
    QString filename;
    filename.append("/media/");
    filename.append(SPI_File_Name);
    QFile afile(filename);

    afile.open(QIODevice::WriteOnly);

    afile.write(arrayWrite,arrayWrite.size());
    afile.close();

    qDebug()<<"SPI done";


    // RTS->~GPIOManager();

}
void SpiDriver::stopCurrentRun(void){
    if(!spiReadFlag){
        qDebug()<<"the run is already stoped";
    }
    else{
        qDebug()<<"spi thread triggered to stop";

        spiReadFlag=false;
    }

}
void SpiDriver::test_dspic(void){
    u_int8_t wbuf[50];
    u_int8_t rbuf[50];
    for(int i=1;i<30;i++){
        rbuf[i]=0x2F;
        wbuf[i]=0xAE;
    }
    wbuf[0]=0xBE;//reset the ptr
    txinfo.rx_buf = (__u64 ) rbuf;
    txinfo.tx_buf = (__u64 ) wbuf;
    txinfo.len = 30;
    txinfo.delay_usecs = 0;
    txinfo.speed_hz = speed;
    txinfo.bits_per_word = bpw;
    txinfo.cs_change = 1;

    SpiDriver::xfer1();
    QByteArray returninfo;

    returninfo.append(rbuf+1,50);
    int sizeofMsg=returninfo.indexOf('\0');
    returninfo.resize(sizeofMsg);
    qDebug()<<returninfo;
}
void SpiDriver::sensor_int(void){
    // qDebug()<<"sensor triggered";
    SpiMutex.lock();
    sensor_has_new_data++;
    SpiMutex.unlock();
}




int SpiDriver::open(int bus, int channel)
{
    if (active)
        close();

    if (bus < 0 || channel < 0)
        return -ENODEV;


    char path[MAX_PATH_LEN];
    if (snprintf(path, MAX_PATH_LEN, "%s%d.%d", SPI_DEVICE_PATH_BASE, bus,
                 channel) >= MAX_PATH_LEN)
        return -EINVAL;
    qDebug(path);
    if ((fd = ::open(path, O_RDWR, 0)) < 0)
    {
        qDebug("open(%s) failed\n", path);
        return fd;
    }

    uint8_t tmp;
    uint32_t tmp32;
    int r;
    if ((r = ioctl(fd, SPI_IOC_RD_MODE, &tmp)) < 0)
    {
        qDebug("ioctl(fd, SPI_IOC_RD_MODE, &tmp) failed\n");
        return r;
    }
    if ((r = ioctl(fd, SPI_IOC_WR_MODE, &tmp)) < 0)
    {
        qDebug("ioctl(fd, SPI_IOC_WR_MODE, &tmp) failed\n");
        return r;
    }
    mode = tmp;

    if ((r = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &tmp)) < 0)
    {
        qDebug("ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &tmp) failed\n");
        return r;
    }
    if ((r = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &tmp)) < 0)
    {
        qDebug("ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &tmp) failed\n");
        return r;
    }
    bpw = tmp;

    if ((r = ioctl(fd, SPI_IOC_RD_LSB_FIRST, &tmp)) < 0)
    {
        qDebug("ioctl(fd, SPI_IOC_RD_LSB_FIRST, &tmp) failed\n");
        return r;
    }
    if ((r = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &tmp)) < 0)
    {
        qDebug("ioctl(fd, SPI_IOC_WR_LSB_FIRST, &tmp) failed\n");
        return r;
    }
    this->lsb_first = lsb_first;

    if ((r = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &tmp32)) < 0)
    {
        qDebug("ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &tmp) failed\n");
        return r;
    }
    if ((r = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &tmp32)) < 0)
    {
        qDebug("ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &tmp) failed\n");
        return r;
    }
    speed = tmp32;

    active = true;
    return 1;
}

int SpiDriver::close()
{
    if (!active)
        return -ENODEV;
    qDebug("SPI::close()\n");
    mode = 0;
    bpw = 0;
    speed = 0;
    active = false;
    int tmpfd = fd;
    fd = -1;
    return ::close(tmpfd);
}

int SpiDriver::setMode(uint8_t mode)
{
    mode &= SPI_CPHA | SPI_CPOL;
    mode = (this->mode & ~(SPI_CPHA | SPI_CPOL)) | mode;

    int r = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (r < 0)
        return r;

    r = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (r < 0)
        return r;

    this->mode = mode;

    return 1;
}

int SpiDriver::setClockPolarity(uint8_t pol)
{
    pol &= SPI_CPOL;
    uint8_t mode = (this->mode & ~(SPI_CPOL)) | pol;
    return setMode(mode);
}

int SpiDriver::setClockPhase(uint8_t phase)
{
    phase &= SPI_CPHA;
    uint8_t mode = (this->mode & ~(SPI_CPHA)) | phase;
    return setMode(mode);
}

int SpiDriver::setLSBFirst(bool lsb_first)
{
    if (!active)
        return -ENODEV;
    int r;
    if ((r = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb_first)) < 0)
        return r;
    if ((r = ioctl(fd, SPI_IOC_RD_LSB_FIRST, &lsb_first)) < 0)
        return r;
    this->lsb_first = lsb_first;
    return 1;
}

int SpiDriver::setBitsPerWord(int bits)
{
    if (!active)
        return -ENODEV;
    int r;
    if ((r = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits)) < 0)
        return r;
    if ((r = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits)) < 0)
        return r;
    bpw = bits;
    return 1;
}

int SpiDriver::setSpeed(uint32_t speed)
{
    int r;
    if (!active)
        return -ENODEV;
    r = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (r < 0)
    {
        qDebug("ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed): %s", strerror(r));
        return r;
    }

    uint32_t tmp;
    r = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &tmp);
    if (r < 0)
    {
        qDebug("ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed): %s", strerror(r));
        return r;
    }
    this->speed = tmp;
    return 1;

}

int SpiDriver::write(uint8_t wbuf[], int len)
{
    return ::write(fd, wbuf, len);
}

int SpiDriver::read(uint8_t rbuf[], int len)
{
    memset(rbuf, 0, len);
    return ::read(fd, rbuf, len);
}

int SpiDriver::xfer1()
{
    /* struct spi_ioc_transfer txinfo;
    txinfo.tx_buf = (__u64 ) wbuf;
    txinfo.rx_buf = (__u64 ) rbuf;
    txinfo.len = len;
    txinfo.delay_usecs = 0;
    txinfo.speed_hz = speed;
    txinfo.bits_per_word = bpw;
    txinfo.cs_change = 1;
*/
    int len=129;
    int r = ioctl(fd, SPI_IOC_MESSAGE(1), &txinfo);
    if (r < 0)
    {
        qDebug("ioctl(fd, SPI_IOC_MESSAGE(1), &txinfo): %s (len=%d)\n",
               strerror(r), len);
        return r;
    }

    //deactivate CS line
    //uint8_t tmp;
    //::read(fd, &tmp, 0);
    return len;
}

SpiDriver::~SpiDriver()
{
    printf("SPI::~SPI()\n");
    close();
}




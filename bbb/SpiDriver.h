#ifndef SPI_THREAD_H
#define SPI_THREAD_H


#include <QtCore>
#include <stdint.h>
#include <linux/spi/spidev.h>
class SpiDriver : public QObject
{
    Q_OBJECT
public:

    /**
             * Default constructor for SPI class.
             */
    SpiDriver();

    /**
             * Function opens the SPI device.
             * @param bus
             * @param channel
             * @return
             */
    int open(int bus, int channel);

    /**
             * Function closes the SPI device.
             * @return
             */
    int close();

    /**
             * Functions sets the idle level and active edge of the clock signal.
             * @param mode
             * @return
             */
    int setMode(uint8_t mode);

    /**
             * Function sets idle level for the clock signal.
             * @param pol
             * @return
             */
    int setClockPolarity(uint8_t pol);

    /**
             *  Function sets active edge for the clock signal.
             * @param phase
             * @return
             */
    int setClockPhase(uint8_t phase);

    /**
             * Function sets order of bits in transmitted data.
             * @param lsb_first
             * @return
             */
    int setLSBFirst(bool lsb_first);

    /**
             * Function sets the number of bits per transferred word.
             * @param bits
             * @return
             */
    int setBitsPerWord(int bits);

    /**
             * Function set the speed of the SPI interface.
             * @param speed
             * @return
             */
    int setSpeed(uint32_t speed);


    /**
             * Method writes len bytes to the device.
             * @param wbuf
             * @param len
             * @return
             */
    int write(uint8_t wbuf[], int len);

    /**
             * Method reads len bytes from the device.
             * @param rbuf
             * @param len
             * @return
             */
    int read(uint8_t rbuf[], int len);

    /**
             * Function performs simultaneous read and write on the device.
             * @param wbuf
             * @param rbuf
             * @param len
             * @return
             */
    int xfer1();

    /**
             *
             */
    virtual ~SpiDriver();
private:
    uint8_t mode;
    uint8_t bpw;
    bool lsb_first;
    uint32_t speed;
    bool active;
    int fd;

    bool logging;
    bool spiReadFlag;
    QMutex SpiMutex;
    struct spi_ioc_transfer txinfo;
    volatile  int sensor_has_new_data;


signals:
    void timeToTakePic();
public slots:
    void startANewrun(QString);
    void stopCurrentRun(void);
    void sensor_int(void);//sensor data ready
    void test_dspic(void);
};


#endif // SPI_THREAD_H

#ifndef C920_H
#define C920_H


#include <QtCore>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>             /* getopt_long() */
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>

#include <linux/videodev2.h>
class C920: public QObject
{
    Q_OBJECT
public:
    // void run();
    C920();
    int xioctl(int fh, int request, void *arg);
    void process_image(const void *p, int size);
    int read_frame(void);
    void grab_frames(void);
    void stop_capturing(void);
    void start_capturing(void);
    void uninit_device(void);
    void init_read(unsigned int buffer_size) ;
    void init_mmap(void);
    void init_userp(unsigned int buffer_size);
    void init_device(void);
    void close_device(void);
    void open_device(void);
    void errno_exit(const char *s);
    void uinit_cam(void);
    // void changeName(char *name);

    unsigned int picCounts;
    //virtual ~SpiThread();
private:
    enum io_method {
        IO_METHOD_READ,
        IO_METHOD_MMAP,
        IO_METHOD_USERPTR,
    };
    struct buffer {
        void *start;
        size_t length;
    };
    char         *dev_name = "/dev/video0";
    const static enum         io_method io = IO_METHOD_MMAP;
    int          fd = -1;
    struct buffer       *buffers;
    unsigned int n_buffers;
    int          out_buf;
    const static int          frame_count = 2;
    int          set_format;
    unsigned int width ;
    unsigned int height ;
    unsigned int fps;
    const static unsigned int timeout = 1;
    const static unsigned int timeouts_max = 1;

    QString         *out_name;
    QString *base_name ;
    //const char *base_name = "/home/root/capture";
    unsigned int pixel_format;
    bool loggingMode;
    //void errno_exit(const char *s);
public slots:
    void takeOnePic(void);
};
#endif // C920_H

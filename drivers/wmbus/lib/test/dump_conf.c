#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>   /* File control definitions */
#include <errno.h>
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/ioctl.h> /* POSIX terminal control definitions */
#include <poll.h>

#define DUMP_MEMORY(buffer, size)     do {  unsigned int i = 0;                        \
                                            unsigned char *pbuffer = (unsigned char *) buffer;        \
                                            printf("Zone memoire %p sur %u octets\n", buffer, size);     \
                                            do {    printf("%02hhX ", (unsigned char) pbuffer[i++]);    \
                                                if((i % 16) == 0) { printf("\n"); }            \
                                            } while( i < size );                        \
                                            printf("\n");                            \
                                        } while(0)


int fdDongle;
char *device = "/dev/ttyUSB0";

int
main (int argc, char **argv)
{
    int ret;
    struct termios conf;
    char buffer[512];
    
    if (argc != 2) {
        printf("Usage : %s /dev/ttyUSBx\n", argv[0]);
        return 0;
    }

    // Open port
    fdDongle = open (argv[1], O_RDWR | O_NOCTTY);
    if (fdDongle < 0) {
        printf("Can't open \"%s\"", argv[1]);
        return -1;
    }

    tcgetattr(fdDongle, &conf);

    /* Set speed */
    ret = cfsetospeed (&conf, B9600);
    if (ret != 0) {
        printf("Error on cfsetospeed !\n");
        return -1;
    }

    ret = cfsetispeed (&conf, B9600);
    if (ret != 0) {
        printf("Error on cfsetispeed !\n");
        return -1;
    }

    conf.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);

    /* Set data length */
    conf.c_cflag &= ~CSIZE;
    conf.c_cflag |= CS8;

    /* Set parity */
    conf.c_cflag &= ~PARENB;

    /* Set bit stop length */
    conf.c_cflag &= ~CSTOPB;

    /* Config cts-dts */
    conf.c_cflag &= ~CRTSCTS;

    /* config Xon-Xoff */
    conf.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* Canonique read */
    conf.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    conf.c_oflag &= ~OPOST;
    conf.c_cc[VMIN] = 255;
    conf.c_cc[VTIME] = 1;

    /* Enable reception */
    conf.c_cflag |= (CREAD | CLOCAL);

    /* Clean uart buffer */
    ret = tcflush (fdDongle, TCIOFLUSH);
    if (ret != 0) {
        printf("Error on tcflush !\n");
        return -1;
    }

    /* Apply */
    ret = tcsetattr (fdDongle, TCSANOW, &conf);
    if (ret != 0) {
        printf("Can't apply uart configuration !\n");
        return -1;
    }
    
    // Disable Transparent mode for data reception
    char cmdDisableTransparentFormat[] = {0xff, 0x09, 0x03, 0x05, 0x01, 0x01, 0xf0};
    ret = write (fdDongle, cmdDisableTransparentFormat, sizeof(cmdDisableTransparentFormat));
    if (ret != sizeof(cmdDisableTransparentFormat)) {
        printf("Can't disable transparent mode for data reception !\n");
        return -1;
    }

    sleep(1);
    ret = read(fdDongle, buffer, 512);
    if (ret > 0) {
        DUMP_MEMORY(buffer, ret);
    }
    
    
    printf("INIT OK ----------------\n");
    char cmdDump[] = {0xff, 0x0A, 0x02, 0x00, 0x51, 0xA6};
    ret = write (fdDongle, cmdDump, sizeof(cmdDump));
    if (ret != sizeof(cmdDump)) {
        printf("Error on dump !\n");
        return -1;
    }    
    
    sleep(1);
    ret = read(fdDongle, buffer, 512);
    if (ret > 0) {
        int i;
        for (i=5; i<buffer[4]; i++) {
            printf("%x\t%hhx\n", i - 5, buffer[i]);   
        }
    }    
    
    
    
    
    
    

    return 0;
}


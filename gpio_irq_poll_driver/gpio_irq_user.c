#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <poll.h>

#define GPIO_MAGIC 'G'

#define GPIO_SET_PIN     _IOW(GPIO_MAGIC, 1, int)
#define GPIO_ENABLE_IRQ  _IO(GPIO_MAGIC, 3)
#define GPIO_GET_EVENT   _IOR(GPIO_MAGIC, 5, int)

int main()
{
    int fd;
    int pin = 17;
    int event = 0;

    struct pollfd pfd;

    fd = open("/dev/gpioirqdev", O_RDWR);
    if (fd < 0) {
        perror("open failed");
        return -1;
    }

    /* Set GPIO pin */
    ioctl(fd, GPIO_SET_PIN, &pin);

    /* Enable IRQ */
    ioctl(fd, GPIO_ENABLE_IRQ);

    pfd.fd = fd;
    pfd.events = POLLIN;

    printf("Waiting for GPIO interrupt...\n");

    while (1) {

        int ret = poll(&pfd, 1, -1); // blocking wait

        if (ret > 0) {

            if (pfd.revents & POLLIN) {

                ioctl(fd, GPIO_GET_EVENT, &event);

                printf("GPIO INTERRUPT OCCURRED! event=%d\n", event);
            }
        }
    }

    close(fd);
    return 0;
}

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/android/binder.h>
#include <linux/android/binderfs.h>

int main(int argc, char *argv[])
{
    int fd, ret, saved_errno;
    size_t len;
    struct binderfs_device device = { 0 };

    if (argc != 3) {
        printf("Usage: %s [binder-control path] [binder_name]\n"
            "\nExample)\n$ %s /dev/binderfs/binder-control binder\n"
            , argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }

    len = strlen(argv[2]);
    if (len > BINDERFS_MAX_NAME)
        exit(EXIT_FAILURE);

    memcpy(device.name, argv[2], len);

    fd = open(argv[1], O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        printf("%s - Failed to open binder-control device\n",
           strerror(errno));
        exit(EXIT_FAILURE);
    }

    ret = ioctl(fd, BINDER_CTL_ADD, &device);
    saved_errno = errno;
    close(fd);
    errno = saved_errno;
    if (ret < 0) {
        printf("%s - Failed to allocate new binder device\n",
           strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Allocated new binder device with major %d, minor %d, and "
       "name %s\n", device.major, device.minor,
       device.name);

    exit(EXIT_SUCCESS);
}
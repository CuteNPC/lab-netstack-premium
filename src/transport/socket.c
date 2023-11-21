#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "transport/mainthread.h"

int __real_socket(int domain, int type, int protocol);
int __real_bind(int socket, const struct sockaddr *address,
                socklen_t address_len);
int __real_listen(int socket, int backlog);
int __real_connect(int socket, const struct sockaddr *address,
                   socklen_t address_len);
int __real_accept(int socket, struct sockaddr *address,
                  socklen_t *address_len);
ssize_t __real_read(int fildes, void *buf, size_t nbyte);
ssize_t __real_write(int fildes, const void *buf, size_t nbyte);
int __real_close(int fildes);
int __real_getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints,
                       struct addrinfo **res);
int __real_setsockopt (int fd, int level, int optname,
		       const void *optval, socklen_t optlen);

#ifndef NOLABSTACK

int __wrap_socket(int domain, int type, int protocol)
{
    int res = socketLab(domain, type, protocol);
    if (res == -1)
        res = __real_socket(domain, type, protocol);
    return res;
}

int __wrap_bind(int socket, const struct sockaddr *address,
                socklen_t address_len)
{
    if (socket >= NETSTACK_FD_START)
        return bindLab(socket, address, address_len);
    else
        return __real_bind(socket, address, address_len);
}

int __wrap_listen(int socket, int backlog)
{
    if (socket >= NETSTACK_FD_START)
        return listenLab(socket, backlog);
    else
        return __real_listen(socket, backlog);
}

int __wrap_connect(int socket, const struct sockaddr *address,
                   socklen_t address_len)
{
    if (socket >= NETSTACK_FD_START)
        return connectLab(socket, address, address_len);
    else
        return __real_connect(socket, address, address_len);
}

int __wrap_accept(int socket, struct sockaddr *address,
                  socklen_t *address_len)
{
    if (socket >= NETSTACK_FD_START)
        return acceptLab(socket, address, address_len);
    else
        return __real_accept(socket, address, address_len);
}

ssize_t __wrap_read(int fd, void *buf, size_t nbyte)
{
    if (fd >= NETSTACK_FD_START)
        return readLab(fd, buf, nbyte);
    else
        return __real_read(fd, buf, nbyte);
}

ssize_t __wrap_write(int fd, const void *buf, size_t nbyte)
{
    if (fd >= NETSTACK_FD_START)
        return writeLab(fd, buf, nbyte);
    else
        return __real_write(fd, buf, nbyte);
}

int __wrap_close(int fd)
{
    if (fd >= NETSTACK_FD_START)
        return closeLab(fd);
    else
        return __real_close(fd);
}

int __wrap_getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints,
                       struct addrinfo **res)
{
    return __real_getaddrinfo(node, service, hints, res);
}

int __wrap_setsockopt (int fd, int level, int optname,
		       const void *optval, socklen_t optlen)
{
    if (fd >= NETSTACK_FD_START)
        return 0;
    else
        return __real_setsockopt(fd, level, optname, optval, optlen);
}
#else

int __wrap_socket(int domain, int type, int protocol)
{
    return -1;
}
int __wrap_bind(int socket, const struct sockaddr *address,
                wrap socklen_t address_len)
{
    return -1;
}
int __wrap_listen(int socket, int backlog)
{
    return -1;
}
int __wrap_connect(int socket, const struct sockaddr *address,
                   wrap socklen_t address_len)
{
    return -1;
}
int __wrap_accept(int socket, struct sockaddr *address,
                  wrap socklen_t *address_len)
{
    return -1;
}
ssize_t __wrap_read(int fildes, void *buf, size_t nbyte)
{
    return -1;
}
ssize_t __wrap_write(int fildes, const void *buf, size_t nbyte)
{
    return -1;
}
int __wrap_close(int fildes)
{
    return -1;
}
int __wrap_getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints,
                       struct addrinfo **res)
{
    return -1;
}
int __wrap_setsockopt (int fd, int level, int optname,
		       const void *optval, socklen_t optlen)
{
    return -1;
}


#endif
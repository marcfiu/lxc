/* SPDX-License-Identifier: LGPL-2.1+ */

#ifndef __LXC_AF_UNIX_H
#define __LXC_AF_UNIX_H

#include <stdio.h>
#include <sys/socket.h>
#include <stddef.h>
#include <sys/un.h>

#include "compiler.h"
#include "macro.h"
#include "memory_utils.h"

/*
 * Technically 253 is the kernel limit but we want to the struct to be a
 * multiple of 8.
 */
#define KERNEL_SCM_MAX_FD 252

struct unix_fds {
	__u32 fd_count_max;
	__u32 fd_count_ret;
	__s32 fd[KERNEL_SCM_MAX_FD];
} __attribute__((aligned(8)));

/* does not enforce \0-termination */
__hidden extern int lxc_abstract_unix_open(const char *path, int type, int flags);
__hidden extern void lxc_abstract_unix_close(int fd);
/* does not enforce \0-termination */
__hidden extern int lxc_abstract_unix_connect(const char *path);

__hidden extern int lxc_abstract_unix_send_fds(int fd, const int *sendfds,
					       int num_sendfds, void *data,
					       size_t size) __access_r(2, 3)
    __access_r(4, 5);

__hidden extern int lxc_abstract_unix_send_fds_iov(int fd, const int *sendfds,
						   int num_sendfds,
						   struct iovec *iov,
						   size_t iovlen)
    __access_r(2, 3);

__hidden extern ssize_t lxc_abstract_unix_recv_fds(int fd,
						   struct unix_fds *ret_fds,
						   void *ret_data,
						   size_t size_ret_data)
    __access_r(3, 4);

__hidden extern ssize_t lxc_abstract_unix_recv_one_fd(int fd, int *ret_fd,
						      void *ret_data,
						      size_t size_ret_data)
    __access_r(3, 4);

__hidden extern ssize_t lxc_abstract_unix_recv_two_fds(int fd, int *ret_fd);

__hidden extern int lxc_unix_send_fds(int fd, int *sendfds, int num_sendfds, void *data, size_t size);

__hidden extern int lxc_abstract_unix_send_credential(int fd, void *data, size_t size)
    __access_r(2, 3);

__hidden extern int lxc_abstract_unix_rcv_credential(int fd, void *data, size_t size)
    __access_w(2, 3);

__hidden extern int lxc_unix_sockaddr(struct sockaddr_un *ret, const char *path);
__hidden extern int lxc_unix_connect(struct sockaddr_un *addr);
__hidden extern int lxc_unix_connect_type(struct sockaddr_un *addr, int type);
__hidden extern int lxc_socket_set_timeout(int fd, int rcv_timeout, int snd_timeout);

static inline void put_unix_fds(struct unix_fds *fds)
{
	if (!IS_ERR_OR_NULL(fds)) {
		for (size_t idx = 0; idx < fds->fd_count_ret; idx++)
			close_prot_errno_disarm(fds->fd[idx]);
	}
}
define_cleanup_function(struct unix_fds *, put_unix_fds);

#endif /* __LXC_AF_UNIX_H */

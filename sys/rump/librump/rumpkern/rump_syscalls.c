/* $NetBSD: rump_syscalls.c,v 1.12 2008/07/16 15:43:30 pooka Exp $ */

/*
 * System call marshalling for rump.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.207 2008/07/16 15:42:54 pooka Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: rump_syscalls.c,v 1.12 2008/07/16 15:43:30 pooka Exp $");

#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/syscallargs.h>
#include "rump_syscalls.h"

#if	BYTE_ORDER == BIG_ENDIAN
#define SPARG(p,k)	((p)->k.be.datum)
#else /* LITTLE_ENDIAN, I hope dearly */
#define SPARG(p,k)	((p)->k.le.datum)
#endif

ssize_t
rump_sys_read(int fd, void * buf, size_t nbyte, int *error)
{
	register_t retval;
	struct sys_read_args arg;

	SPARG(&arg, fd) = fd;
	SPARG(&arg, buf) = buf;
	SPARG(&arg, nbyte) = nbyte;

	*error = sys_read(curlwp, &arg, &retval);
	return retval;
}

ssize_t
rump_sys_write(int fd, const void * buf, size_t nbyte, int *error)
{
	register_t retval;
	struct sys_write_args arg;

	SPARG(&arg, fd) = fd;
	SPARG(&arg, buf) = buf;
	SPARG(&arg, nbyte) = nbyte;

	*error = sys_write(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_open(const char * path, int flags, mode_t mode, int *error)
{
	register_t retval;
	struct sys_open_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, flags) = flags;
	SPARG(&arg, mode) = mode;

	*error = sys_open(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_close(int fd, int *error)
{
	register_t retval;
	struct sys_close_args arg;

	SPARG(&arg, fd) = fd;

	*error = sys_close(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_link(const char * path, const char * link, int *error)
{
	register_t retval;
	struct sys_link_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, link) = link;

	*error = sys_link(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_unlink(const char * path, int *error)
{
	register_t retval;
	struct sys_unlink_args arg;

	SPARG(&arg, path) = path;

	*error = sys_unlink(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_chdir(const char * path, int *error)
{
	register_t retval;
	struct sys_chdir_args arg;

	SPARG(&arg, path) = path;

	*error = sys_chdir(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_fchdir(int fd, int *error)
{
	register_t retval;
	struct sys_fchdir_args arg;

	SPARG(&arg, fd) = fd;

	*error = sys_fchdir(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_mknod(const char * path, mode_t mode, dev_t dev, int *error)
{
	register_t retval;
	struct sys_mknod_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, mode) = mode;
	SPARG(&arg, dev) = dev;

	*error = sys_mknod(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_chmod(const char * path, mode_t mode, int *error)
{
	register_t retval;
	struct sys_chmod_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, mode) = mode;

	*error = sys_chmod(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_chown(const char * path, uid_t uid, gid_t gid, int *error)
{
	register_t retval;
	struct sys_chown_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, uid) = uid;
	SPARG(&arg, gid) = gid;

	*error = sys_chown(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_unmount(const char * path, int flags, int *error)
{
	register_t retval;
	struct sys_unmount_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, flags) = flags;

	*error = sys_unmount(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_chflags(const char * path, u_long flags, int *error)
{
	register_t retval;
	struct sys_chflags_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, flags) = flags;

	*error = sys_chflags(curlwp, &arg, &retval);
	return retval;
}

void
rump_sys_sync(int *error)
{
	register_t retval;

	*error = sys_sync(curlwp, NULL, &retval);
}

int
rump_sys_symlink(const char * path, const char * link, int *error)
{
	register_t retval;
	struct sys_symlink_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, link) = link;

	*error = sys_symlink(curlwp, &arg, &retval);
	return retval;
}

ssize_t
rump_sys_readlink(const char * path, char * buf, size_t count, int *error)
{
	register_t retval;
	struct sys_readlink_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, buf) = buf;
	SPARG(&arg, count) = count;

	*error = sys_readlink(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_fsync(int fd, int *error)
{
	register_t retval;
	struct sys_fsync_args arg;

	SPARG(&arg, fd) = fd;

	*error = sys_fsync(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_rename(const char * from, const char * to, int *error)
{
	register_t retval;
	struct sys_rename_args arg;

	SPARG(&arg, from) = from;
	SPARG(&arg, to) = to;

	*error = sys_rename(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_mkfifo(const char * path, mode_t mode, int *error)
{
	register_t retval;
	struct sys_mkfifo_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, mode) = mode;

	*error = sys_mkfifo(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_mkdir(const char * path, mode_t mode, int *error)
{
	register_t retval;
	struct sys_mkdir_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, mode) = mode;

	*error = sys_mkdir(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_rmdir(const char * path, int *error)
{
	register_t retval;
	struct sys_rmdir_args arg;

	SPARG(&arg, path) = path;

	*error = sys_rmdir(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_utimes(const char * path, const struct timeval * tptr, int *error)
{
	register_t retval;
	struct sys_utimes_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, tptr) = tptr;

	*error = sys_utimes(curlwp, &arg, &retval);
	return retval;
}

ssize_t
rump_sys_pread(int fd, void * buf, size_t nbyte, int pad, off_t offset, int *error)
{
	register_t retval;
	struct sys_pread_args arg;

	SPARG(&arg, fd) = fd;
	SPARG(&arg, buf) = buf;
	SPARG(&arg, nbyte) = nbyte;
	SPARG(&arg, pad) = pad;
	SPARG(&arg, offset) = offset;

	*error = sys_pread(curlwp, &arg, &retval);
	return retval;
}

ssize_t
rump_sys_pwrite(int fd, const void * buf, size_t nbyte, int pad, off_t offset, int *error)
{
	register_t retval;
	struct sys_pwrite_args arg;

	SPARG(&arg, fd) = fd;
	SPARG(&arg, buf) = buf;
	SPARG(&arg, nbyte) = nbyte;
	SPARG(&arg, pad) = pad;
	SPARG(&arg, offset) = offset;

	*error = sys_pwrite(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_truncate(const char * path, int pad, off_t length, int *error)
{
	register_t retval;
	struct sys_truncate_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, pad) = pad;
	SPARG(&arg, length) = length;

	*error = sys_truncate(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_lchmod(const char * path, mode_t mode, int *error)
{
	register_t retval;
	struct sys_lchmod_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, mode) = mode;

	*error = sys_lchmod(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_lchown(const char * path, uid_t uid, gid_t gid, int *error)
{
	register_t retval;
	struct sys_lchown_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, uid) = uid;
	SPARG(&arg, gid) = gid;

	*error = sys_lchown(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_lutimes(const char * path, const struct timeval * tptr, int *error)
{
	register_t retval;
	struct sys_lutimes_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, tptr) = tptr;

	*error = sys_lutimes(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys_lchflags(const char * path, u_long flags, int *error)
{
	register_t retval;
	struct sys_lchflags_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, flags) = flags;

	*error = sys_lchflags(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys___stat30(const char * path, struct stat * ub, int *error)
{
	register_t retval;
	struct sys___stat30_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, ub) = ub;

	*error = sys___stat30(curlwp, &arg, &retval);
	return retval;
}

int
rump_sys___lstat30(const char * path, struct stat * ub, int *error)
{
	register_t retval;
	struct sys___lstat30_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, ub) = ub;

	*error = sys___lstat30(curlwp, &arg, &retval);
	return retval;
}


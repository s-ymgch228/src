/* $NetBSD: rump_syscalls.c,v 1.27 2009/01/23 19:30:16 pooka Exp $ */

/*
 * System call marshalling for rump.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.221 2009/01/13 22:27:44 pooka Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: rump_syscalls.c,v 1.27 2009/01/23 19:30:16 pooka Exp $");

#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/syscallargs.h>
#include <rump/rumpuser.h>
#include "rump_private.h"

#if	BYTE_ORDER == BIG_ENDIAN
#define SPARG(p,k)	((p)->k.be.datum)
#else /* LITTLE_ENDIAN, I hope dearly */
#define SPARG(p,k)	((p)->k.le.datum)
#endif

int rump_enosys(void);
int
rump_enosys()
{

	return ENOSYS;
}

ssize_t rump_sys_read(int, void *, size_t);
ssize_t
rump_sys_read(int fd, void * buf, size_t nbyte)
{
	register_t retval = 0;
	int error = 0;
	struct sys_read_args arg;

	SPARG(&arg, fd) = fd;
	SPARG(&arg, buf) = buf;
	SPARG(&arg, nbyte) = nbyte;

	error = sys_read(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_read,rump_enosys);

ssize_t rump_sys_write(int, const void *, size_t);
ssize_t
rump_sys_write(int fd, const void * buf, size_t nbyte)
{
	register_t retval = 0;
	int error = 0;
	struct sys_write_args arg;

	SPARG(&arg, fd) = fd;
	SPARG(&arg, buf) = buf;
	SPARG(&arg, nbyte) = nbyte;

	error = sys_write(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_write,rump_enosys);

int rump_sys_open(const char *, int, mode_t);
int
rump_sys_open(const char * path, int flags, mode_t mode)
{
	register_t retval = 0;
	int error = 0;
	struct sys_open_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, flags) = flags;
	SPARG(&arg, mode) = mode;

	error = sys_open(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_open,rump_enosys);

int rump_sys_close(int);
int
rump_sys_close(int fd)
{
	register_t retval = 0;
	int error = 0;
	struct sys_close_args arg;

	SPARG(&arg, fd) = fd;

	error = sys_close(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_close,rump_enosys);

int rump_sys_link(const char *, const char *);
int
rump_sys_link(const char * path, const char * link)
{
	register_t retval = 0;
	int error = 0;
	struct sys_link_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, link) = link;

	error = sys_link(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_link,rump_enosys);

int rump_sys_unlink(const char *);
int
rump_sys_unlink(const char * path)
{
	register_t retval = 0;
	int error = 0;
	struct sys_unlink_args arg;

	SPARG(&arg, path) = path;

	error = sys_unlink(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_unlink,rump_enosys);

int rump_sys_chdir(const char *);
int
rump_sys_chdir(const char * path)
{
	register_t retval = 0;
	int error = 0;
	struct sys_chdir_args arg;

	SPARG(&arg, path) = path;

	error = sys_chdir(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_chdir,rump_enosys);

int rump_sys_fchdir(int);
int
rump_sys_fchdir(int fd)
{
	register_t retval = 0;
	int error = 0;
	struct sys_fchdir_args arg;

	SPARG(&arg, fd) = fd;

	error = sys_fchdir(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_fchdir,rump_enosys);

int rump_sys_chmod(const char *, mode_t);
int
rump_sys_chmod(const char * path, mode_t mode)
{
	register_t retval = 0;
	int error = 0;
	struct sys_chmod_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, mode) = mode;

	error = sys_chmod(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_chmod,rump_enosys);

int rump_sys_chown(const char *, uid_t, gid_t);
int
rump_sys_chown(const char * path, uid_t uid, gid_t gid)
{
	register_t retval = 0;
	int error = 0;
	struct sys_chown_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, uid) = uid;
	SPARG(&arg, gid) = gid;

	error = sys_chown(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_chown,rump_enosys);

int rump_sys_unmount(const char *, int);
int
rump_sys_unmount(const char * path, int flags)
{
	register_t retval = 0;
	int error = 0;
	struct sys_unmount_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, flags) = flags;

	error = sys_unmount(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_unmount,rump_enosys);

int rump_sys_accept(int, struct sockaddr *, unsigned int *);
int
rump_sys_accept(int s, struct sockaddr * name, unsigned int * anamelen)
{
	register_t retval = 0;
	int error = 0;
	struct sys_accept_args arg;

	SPARG(&arg, s) = s;
	SPARG(&arg, name) = name;
	SPARG(&arg, anamelen) = anamelen;

	error = sys_accept(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_accept,rump_enosys);

int rump_sys_chflags(const char *, u_long);
int
rump_sys_chflags(const char * path, u_long flags)
{
	register_t retval = 0;
	int error = 0;
	struct sys_chflags_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, flags) = flags;

	error = sys_chflags(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_chflags,rump_enosys);

void rump_sys_sync(void);
void
rump_sys_sync(void )
{
	register_t retval = 0;
	int error = 0;

	error = sys_sync(curlwp, NULL, &retval);
	if (error) {
		retval = -1;
	}
}
__weak_alias(sys_sync,rump_enosys);

int rump_sys_ioctl(int, u_long, void *);
int
rump_sys_ioctl(int fd, u_long com, void * data)
{
	register_t retval = 0;
	int error = 0;
	struct sys_ioctl_args arg;

	SPARG(&arg, fd) = fd;
	SPARG(&arg, com) = com;
	SPARG(&arg, data) = data;

	error = sys_ioctl(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_ioctl,rump_enosys);

int rump_sys_symlink(const char *, const char *);
int
rump_sys_symlink(const char * path, const char * link)
{
	register_t retval = 0;
	int error = 0;
	struct sys_symlink_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, link) = link;

	error = sys_symlink(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_symlink,rump_enosys);

ssize_t rump_sys_readlink(const char *, char *, size_t);
ssize_t
rump_sys_readlink(const char * path, char * buf, size_t count)
{
	register_t retval = 0;
	int error = 0;
	struct sys_readlink_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, buf) = buf;
	SPARG(&arg, count) = count;

	error = sys_readlink(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_readlink,rump_enosys);

int rump_sys_fsync(int);
int
rump_sys_fsync(int fd)
{
	register_t retval = 0;
	int error = 0;
	struct sys_fsync_args arg;

	SPARG(&arg, fd) = fd;

	error = sys_fsync(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_fsync,rump_enosys);

int rump_sys_connect(int, const struct sockaddr *, unsigned int);
int
rump_sys_connect(int s, const struct sockaddr * name, unsigned int namelen)
{
	register_t retval = 0;
	int error = 0;
	struct sys_connect_args arg;

	SPARG(&arg, s) = s;
	SPARG(&arg, name) = name;
	SPARG(&arg, namelen) = namelen;

	error = sys_connect(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_connect,rump_enosys);

int rump_sys_bind(int, const struct sockaddr *, unsigned int);
int
rump_sys_bind(int s, const struct sockaddr * name, unsigned int namelen)
{
	register_t retval = 0;
	int error = 0;
	struct sys_bind_args arg;

	SPARG(&arg, s) = s;
	SPARG(&arg, name) = name;
	SPARG(&arg, namelen) = namelen;

	error = sys_bind(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_bind,rump_enosys);

int rump_sys_setsockopt(int, int, int, const void *, unsigned int);
int
rump_sys_setsockopt(int s, int level, int name, const void * val, unsigned int valsize)
{
	register_t retval = 0;
	int error = 0;
	struct sys_setsockopt_args arg;

	SPARG(&arg, s) = s;
	SPARG(&arg, level) = level;
	SPARG(&arg, name) = name;
	SPARG(&arg, val) = val;
	SPARG(&arg, valsize) = valsize;

	error = sys_setsockopt(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_setsockopt,rump_enosys);

int rump_sys_listen(int, int);
int
rump_sys_listen(int s, int backlog)
{
	register_t retval = 0;
	int error = 0;
	struct sys_listen_args arg;

	SPARG(&arg, s) = s;
	SPARG(&arg, backlog) = backlog;

	error = sys_listen(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_listen,rump_enosys);

int rump_sys_getsockopt(int, int, int, void *, unsigned int *);
int
rump_sys_getsockopt(int s, int level, int name, void * val, unsigned int * avalsize)
{
	register_t retval = 0;
	int error = 0;
	struct sys_getsockopt_args arg;

	SPARG(&arg, s) = s;
	SPARG(&arg, level) = level;
	SPARG(&arg, name) = name;
	SPARG(&arg, val) = val;
	SPARG(&arg, avalsize) = avalsize;

	error = sys_getsockopt(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_getsockopt,rump_enosys);

int rump_sys_rename(const char *, const char *);
int
rump_sys_rename(const char * from, const char * to)
{
	register_t retval = 0;
	int error = 0;
	struct sys_rename_args arg;

	SPARG(&arg, from) = from;
	SPARG(&arg, to) = to;

	error = sys_rename(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_rename,rump_enosys);

int rump_sys_mkfifo(const char *, mode_t);
int
rump_sys_mkfifo(const char * path, mode_t mode)
{
	register_t retval = 0;
	int error = 0;
	struct sys_mkfifo_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, mode) = mode;

	error = sys_mkfifo(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_mkfifo,rump_enosys);

int rump_sys_mkdir(const char *, mode_t);
int
rump_sys_mkdir(const char * path, mode_t mode)
{
	register_t retval = 0;
	int error = 0;
	struct sys_mkdir_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, mode) = mode;

	error = sys_mkdir(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_mkdir,rump_enosys);

int rump_sys_rmdir(const char *);
int
rump_sys_rmdir(const char * path)
{
	register_t retval = 0;
	int error = 0;
	struct sys_rmdir_args arg;

	SPARG(&arg, path) = path;

	error = sys_rmdir(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_rmdir,rump_enosys);

int rump_sys_nfssvc(int, void *);
int
rump_sys_nfssvc(int flag, void * argp)
{
	register_t retval = 0;
	int error = 0;
	struct sys_nfssvc_args arg;

	SPARG(&arg, flag) = flag;
	SPARG(&arg, argp) = argp;

	error = sys_nfssvc(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_nfssvc,rump_enosys);

ssize_t rump_sys_pread(int, void *, size_t, int, off_t);
ssize_t
rump_sys_pread(int fd, void * buf, size_t nbyte, int pad, off_t offset)
{
	register_t retval = 0;
	int error = 0;
	struct sys_pread_args arg;

	SPARG(&arg, fd) = fd;
	SPARG(&arg, buf) = buf;
	SPARG(&arg, nbyte) = nbyte;
	SPARG(&arg, pad) = pad;
	SPARG(&arg, offset) = offset;

	error = sys_pread(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_pread,rump_enosys);

ssize_t rump_sys_pwrite(int, const void *, size_t, int, off_t);
ssize_t
rump_sys_pwrite(int fd, const void * buf, size_t nbyte, int pad, off_t offset)
{
	register_t retval = 0;
	int error = 0;
	struct sys_pwrite_args arg;

	SPARG(&arg, fd) = fd;
	SPARG(&arg, buf) = buf;
	SPARG(&arg, nbyte) = nbyte;
	SPARG(&arg, pad) = pad;
	SPARG(&arg, offset) = offset;

	error = sys_pwrite(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_pwrite,rump_enosys);

int rump_sys_truncate(const char *, int, off_t);
int
rump_sys_truncate(const char * path, int pad, off_t length)
{
	register_t retval = 0;
	int error = 0;
	struct sys_truncate_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, pad) = pad;
	SPARG(&arg, length) = length;

	error = sys_truncate(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_truncate,rump_enosys);

int rump_sys___sysctl(const int *, u_int, void *, size_t *, const void *, size_t);
int
rump_sys___sysctl(const int * name, u_int namelen, void * old, size_t * oldlenp, const void * new, size_t newlen)
{
	register_t retval = 0;
	int error = 0;
	struct sys___sysctl_args arg;

	SPARG(&arg, name) = name;
	SPARG(&arg, namelen) = namelen;
	SPARG(&arg, old) = old;
	SPARG(&arg, oldlenp) = oldlenp;
	SPARG(&arg, new) = new;
	SPARG(&arg, newlen) = newlen;

	error = sys___sysctl(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys___sysctl,rump_enosys);

int rump_sys_lchmod(const char *, mode_t);
int
rump_sys_lchmod(const char * path, mode_t mode)
{
	register_t retval = 0;
	int error = 0;
	struct sys_lchmod_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, mode) = mode;

	error = sys_lchmod(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_lchmod,rump_enosys);

int rump_sys_lchown(const char *, uid_t, gid_t);
int
rump_sys_lchown(const char * path, uid_t uid, gid_t gid)
{
	register_t retval = 0;
	int error = 0;
	struct sys_lchown_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, uid) = uid;
	SPARG(&arg, gid) = gid;

	error = sys_lchown(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_lchown,rump_enosys);

int rump_sys_lchflags(const char *, u_long);
int
rump_sys_lchflags(const char * path, u_long flags)
{
	register_t retval = 0;
	int error = 0;
	struct sys_lchflags_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, flags) = flags;

	error = sys_lchflags(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_lchflags,rump_enosys);

int rump_sys_statvfs1(const char *, struct statvfs *, int);
int
rump_sys_statvfs1(const char * path, struct statvfs * buf, int flags)
{
	register_t retval = 0;
	int error = 0;
	struct sys_statvfs1_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, buf) = buf;
	SPARG(&arg, flags) = flags;

	error = sys_statvfs1(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys_statvfs1,rump_enosys);

int rump_sys___socket30(int, int, int);
int
rump_sys___socket30(int domain, int type, int protocol)
{
	register_t retval = 0;
	int error = 0;
	struct sys___socket30_args arg;

	SPARG(&arg, domain) = domain;
	SPARG(&arg, type) = type;
	SPARG(&arg, protocol) = protocol;

	error = sys___socket30(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys___socket30,rump_enosys);

int rump_sys___getfh30(const char *, void *, size_t *);
int
rump_sys___getfh30(const char * fname, void * fhp, size_t * fh_size)
{
	register_t retval = 0;
	int error = 0;
	struct sys___getfh30_args arg;

	SPARG(&arg, fname) = fname;
	SPARG(&arg, fhp) = fhp;
	SPARG(&arg, fh_size) = fh_size;

	error = sys___getfh30(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys___getfh30,rump_enosys);

int rump_sys___utimes50(const char *, const struct timeval *);
int
rump_sys___utimes50(const char * path, const struct timeval * tptr)
{
	register_t retval = 0;
	int error = 0;
	struct sys___utimes50_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, tptr) = tptr;

	error = sys___utimes50(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys___utimes50,rump_enosys);

int rump_sys___lutimes50(const char *, const struct timeval *);
int
rump_sys___lutimes50(const char * path, const struct timeval * tptr)
{
	register_t retval = 0;
	int error = 0;
	struct sys___lutimes50_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, tptr) = tptr;

	error = sys___lutimes50(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys___lutimes50,rump_enosys);

int rump_sys___stat50(const char *, struct stat *);
int
rump_sys___stat50(const char * path, struct stat * ub)
{
	register_t retval = 0;
	int error = 0;
	struct sys___stat50_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, ub) = ub;

	error = sys___stat50(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys___stat50,rump_enosys);

int rump_sys___lstat50(const char *, struct stat *);
int
rump_sys___lstat50(const char * path, struct stat * ub)
{
	register_t retval = 0;
	int error = 0;
	struct sys___lstat50_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, ub) = ub;

	error = sys___lstat50(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys___lstat50,rump_enosys);

int rump_sys___mknod50(const char *, mode_t, dev_t);
int
rump_sys___mknod50(const char * path, mode_t mode, dev_t dev)
{
	register_t retval = 0;
	int error = 0;
	struct sys___mknod50_args arg;

	SPARG(&arg, path) = path;
	SPARG(&arg, mode) = mode;
	SPARG(&arg, dev) = dev;

	error = sys___mknod50(curlwp, &arg, &retval);
	if (error) {
		retval = -1;
		rumpuser_seterrno(error);
	}
	return retval;
}
__weak_alias(sys___mknod50,rump_enosys);

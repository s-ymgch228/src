/*
 * $NetBSD: crypt.h,v 1.2 2005/01/11 22:40:00 christos Exp $
 */
char	*__md5crypt(const char *pw, const char *salt);	/* XXX */
char *__bcrypt(const char *, const char *);	/* XXX */
char *__crypt_sha1(const char *pw, const char *salt);
unsigned int __crypt_sha1_iterations (unsigned int hint);
void __hmac_sha1(unsigned char *, size_t, unsigned char *, size_t, unsigned char *);
void __crypt_to64(char *s, u_int32_t v, int n);

int __gensalt_blowfish(char *salt, size_t saltlen, size_t nrounds);
int __gensalt_old(char *salt, size_t saltsiz, size_t nrounds);
int __gensalt_new(char *salt, size_t saltsiz, size_t nrounds);
int __gensalt_md5(char *salt, size_t saltsiz, size_t nrounds);
int __gensalt_sha1(char *salt, size_t saltsiz, size_t nrounds);

#define SHA1_MAGIC "$sha1$"
#define SHA1_SIZE 20

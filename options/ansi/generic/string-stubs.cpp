
#include <string.h>
#include <errno.h>
#include <wchar.h>

#include <bits/ensure.h>

// memset() is defined in options/internals.
// memcpy() is defined in options/internals.
// memmove() is defined in options/internals.
// strlen() is defined in options/internals.

char *strcpy(char *__restrict dest, const char *src) {
	char *dest_bytes = (char *)dest;
	char *src_bytes = (char *)src;
	while(*src_bytes)
		*(dest_bytes++) = *(src_bytes++);
	*dest_bytes = 0;
	return dest;
}
char *strncpy(char *__restrict dest, const char *src, size_t max_size) {
	auto dest_bytes = static_cast<char *>(dest);
	auto src_bytes = static_cast<const char *>(src);
	size_t i = 0;
	while(*src_bytes && i < max_size) {
		*(dest_bytes++) = *(src_bytes++);
		i++;
	}
	while(i < max_size) {
		*(dest_bytes++) = 0;
		i++;
	}
	return dest;
}

char *strcat(char *__restrict dest, const char *__restrict src) {
	strcpy(dest + strlen(dest), src);
	return dest;
}
char *strncat(char *__restrict dest, const char *__restrict src, size_t max_size) {
	auto dest_bytes = static_cast<char *>(dest);
	auto src_bytes = static_cast<const char *>(src);
	dest_bytes += strlen(dest);
	size_t i = 0;
	while(*src_bytes && i < max_size) {
		*(dest_bytes++) = *(src_bytes++);
		i++;
	}
	*dest_bytes = 0;
	return dest;
}

int memcmp(const void *a, const void *b, size_t size) {
	for(size_t i = 0; i < size; i++) {
		auto a_byte = static_cast<const unsigned char *>(a)[i];
		auto b_byte = static_cast<const unsigned char *>(b)[i];
		if(a_byte < b_byte)
			return -1;
		if(a_byte > b_byte)
			return 1;
	}
	return 0;
}
int strcmp(const char *a, const char *b) {
	size_t i = 0;
	while(true) {
		unsigned char a_byte = a[i];
		unsigned char b_byte = b[i];
		if(!a_byte && !b_byte)
			return 0;
		// If only one char is null, one of the following cases applies.
		if(a_byte < b_byte)
			return -1;
		if(a_byte > b_byte)
			return 1;
		i++;
	}
}

int strcoll(const char *a, const char *b) {
	// TODO: strcoll should take "LC_COLLATE" into account.
	return strcmp(a, b);
}

int strncmp(const char *a, const char *b, size_t max_size) {
	size_t i = 0;
	while(true) {
		if(!(i < max_size))
			return 0;
		unsigned char a_byte = a[i];
		unsigned char b_byte = b[i];
		if(!a_byte && !b_byte)
			return 0;
		// If only one char is null, one of the following cases applies.
		if(a_byte < b_byte)
			return -1;
		if(a_byte > b_byte)
			return 1;
		i++;
	}
}
size_t strxfrm(char *__restrict dest, const char *__restrict src, size_t max_size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *memchr(const void *s, int c, size_t size) {
	auto s_bytes = static_cast<const unsigned char *>(s);
	for(size_t i = 0; i < size; i++)
		if(s_bytes[i] == static_cast<unsigned char>(c))
			return const_cast<unsigned char *>(s_bytes + i);
	return nullptr;
}
char *strchr(const char *s, int c) {
	size_t i = 0;
	while(s[i]) {
		if(s[i] == c)
			return const_cast<char *>(&s[i]);
		i++;
	}
	if(c == 0)
		return const_cast<char *>(&s[i]);
	return nullptr;
}
size_t strcspn(const char *s, const char *chrs) {
	size_t n = 0;
	while(true) {
		if(!s[n] || strchr(chrs, s[n]))
			return n;
		n++;
	}
}
char *strpbrk(const char *s, const char *chrs) {
	size_t n = 0;
	while(s[n]) {
		if(strchr(chrs, s[n]))
			return const_cast<char *>(s + n);
		n++;
	}
	return nullptr;
}
char *strrchr(const char *s, int c) {
	// The null-terminator is considered to be part of the string.
	size_t length = strlen(s);
	for(size_t i = 0; i <= length; i++) {
		if(s[length - i] == c)
			return const_cast<char *>(s + (length - i));
	}
	return nullptr;
}
size_t strspn(const char *s, const char *chrs) {
	size_t n = 0;
	while(true) {
		if(!s[n] || !strchr(chrs, s[n]))
			return n;
		n++;
	}
}
char *strstr(const char *s, const char *pattern) {
	for(size_t i = 0; s[i]; i++) {
		bool found = true;
		for(size_t j = 0; pattern[j]; j++) {
			if(!pattern[j] || s[i + j] == pattern[j])
				continue;

			found = false;
			break;
		}

		if(found)
			return const_cast<char *>(&s[i]);
	}

	return nullptr;
}
char *strtok_r(char *__restrict s, const char *__restrict del, char **__restrict m) {
	__ensure(m);

	// We use *m = null to memorize that the entire string was consumed.
	char *tok;
	if(s) {
		tok = s;
	}else if(*m) {
		tok = *m;
	}else {
		return nullptr;
	}

	// Skip initial delimiters.
	// After this loop: *tok is non-null iff we return a token.
	while(*tok && strchr(del, *tok))
		tok++;

	// Replace the following delimiter by a null-terminator.
	// After this loop: *p is null iff we reached the end of the string.
	auto p = tok;
	while(*p && !strchr(del, *p))
		p++;

	if(*p) {
		*p = 0;
		*m = p + 1;
	}else{
		*m = nullptr;
	}
	return tok;
}
char *strtok(char *__restrict s, const char *__restrict delimiter) {
        static char *saved;
        return strtok_r(s, delimiter, &saved);
}

// This is a GNU extension.
char *strchrnul(const char *s, int c) {
	size_t i = 0;
	while(s[i]) {
		if(s[i] == c)
			return const_cast<char *>(s + i);
		i++;
	}
	return const_cast<char *>(s + i);
}

double wcstod(const wchar_t *__restrict, wchar_t **__restrict) MLIBC_STUB_BODY
float wcstof(const wchar_t *__restrict, wchar_t **__restrict) MLIBC_STUB_BODY
long double wcstold(const wchar_t *__restrict, wchar_t **__restrict) MLIBC_STUB_BODY

long wcstol(const wchar_t *__restrict, wchar_t **__restrict, int) MLIBC_STUB_BODY
long long wcstoll(const wchar_t *__restrict, wchar_t **__restrict, int) MLIBC_STUB_BODY
unsigned long wcstoul(const wchar_t *__restrict, wchar_t **__restrict, int) MLIBC_STUB_BODY
unsigned long long wcstoull(const wchar_t *__restrict, wchar_t **__restrict, int) MLIBC_STUB_BODY

wchar_t *wcscpy(wchar_t *__restrict, const wchar_t *__restrict) MLIBC_STUB_BODY
wchar_t *wcsncpy(wchar_t *__restrict, const wchar_t *__restrict, size_t) MLIBC_STUB_BODY
wchar_t *wmemcpy(wchar_t *__restrict, const wchar_t *__restrict, size_t) MLIBC_STUB_BODY
wchar_t *wmemmove(wchar_t *, const wchar_t *, size_t) MLIBC_STUB_BODY

wchar_t *wcscat(wchar_t *__restrict, const wchar_t *__restrict) MLIBC_STUB_BODY
wchar_t *wcsncat(wchar_t *__restrict, const wchar_t *__restrict, size_t) MLIBC_STUB_BODY

int wcscmp(const wchar_t *, const wchar_t *) MLIBC_STUB_BODY
int wcscoll(const wchar_t *, const wchar_t *) MLIBC_STUB_BODY
int wcsncmp(const wchar_t *, const wchar_t *, size_t) MLIBC_STUB_BODY
int wcsxfrm(wchar_t *__restrict, const wchar_t *__restrict, size_t) MLIBC_STUB_BODY
int wmemcmp(const wchar_t *, const wchar_t *, size_t) MLIBC_STUB_BODY

wchar_t *wcschr(const wchar_t *, wchar_t) MLIBC_STUB_BODY
size_t wcscspn(const wchar_t *, const wchar_t *) MLIBC_STUB_BODY
wchar_t *wcspbrk(const wchar_t *, const wchar_t *) MLIBC_STUB_BODY
wchar_t *wcsrchr(const wchar_t *, wchar_t) MLIBC_STUB_BODY
size_t wcsspn(const wchar_t *, const wchar_t *) MLIBC_STUB_BODY
wchar_t *wcsstr(const wchar_t *, const wchar_t *) MLIBC_STUB_BODY
wchar_t *wcstok(wchar_t *__restrict, const wchar_t *__restrict, wchar_t **__restrict) MLIBC_STUB_BODY

wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t size) {
	auto s_bytes = s;
	for(size_t i = 0; i < size; i++)
		if(s_bytes[i] == c)
			return const_cast<wchar_t *>(s_bytes + i);
	return nullptr;
}

size_t wcslen(const wchar_t *) MLIBC_STUB_BODY
wchar_t *wmemset(wchar_t *, wchar_t, size_t) MLIBC_STUB_BODY

char *strerror(int e) {
	const char *s;
	switch(e) {
	case EAGAIN: s = "Operation would block (EAGAIN)"; break;
	case EACCES: s = "Access denied (EACCESS)"; break;
	case EBADF:  s = "Bad file descriptor (EBADF)"; break;
	case EEXIST: s = "File exists already (EEXIST)"; break;
	case EFAULT: s = "Access violation (EFAULT)"; break;
	case EINTR:  s = "Operation interrupted (EINTR)"; break;
	case EINVAL: s = "Invalid argument (EINVAL)"; break;
	case EIO:    s = "I/O error (EIO)"; break;
	case EISDIR: s = "Resource is directory (EISDIR)"; break;
	case ENOENT: s = "No such file or directory (ENOENT)"; break;
	case ENOMEM: s = "Out of memory (ENOMEM)"; break;
	case ENOTDIR: s = "Expected directory instead of file (ENOTDIR)"; break;
	case ENOSYS: s = "Operation not implemented (ENOSYS)"; break;
	case EPERM:  s = "Operation not permitted (EFAULT)"; break;
	case EPIPE:  s = "Broken pipe (EPIPE)"; break;
	case ESPIPE: s = "Seek not possible (ESPIPE)"; break;
	case ENXIO: s = "No such device or address (ENXIO)"; break;
	default:
		s = "Unknown error code (?)";
	}
	return const_cast<char *>(s);
}
// strlen() is defined in options/internals.

// POSIX extensions.

int strerror_r(int e, char *buffer, size_t bufsz) {
	auto s = strerror(e);
	strncpy(buffer, s, bufsz);
	// Note that strerror_r does not set errno on error!
	if(strlen(s) >= bufsz)
		return ERANGE;
	return 0;
}

void *mempcpy(void *dest, const void *src, size_t len) {
	return (char *)memcpy(dest, src, len) + len;
}

char *stpcpy(char *__restrict dest, const char *__restrict src) {
	auto n = strlen(src);
	memcpy(dest, src, n + 1);
	return dest + n;
}


/*\
 * sprtfstr.cxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "sprtfstr.hxx"

static const char *module = "sprtfstr";

#define CHKBUF(a,b) { \
    if (!a) {       \
        fprintf(stderr,"%s:ERROR: Memory FAILED on %d bytes!\n!", module, (int)b); \
        exit(1); \
    } \
}


// implementation
sprtfstr::sprtfstr(): _buf(0), _size(0), _strlen(0)
{
    _size = _increments;
    _buf = (char *)malloc(_size * sizeof(char));
    CHKBUF(_buf,_size);
    clear();
}

sprtfstr::~sprtfstr()
{
    if (_buf) {
	    free(_buf);
    }
}

void sprtfstr::clear()
{
    _strlen = 0;
    _buf[0] = (char)0;
}

const char *sprtfstr::printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    clear();
    const char *result = _appendf(fmt, ap);
    va_end(ap);

    return result;
}

const char *sprtfstr::appendf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    const char *result = _appendf(fmt, ap);
    va_end(ap);

    return result;
}

const char *sprtfstr::_appendf(const char *fmt, va_list ap)
{
#ifdef _MSC_VER
    // Window's verion of vsnprintf() returns -1 when the string won't
    // fit, so we need to keep trying with larger buffers until it
    // does.
    int newLen;
    while ((newLen = vsnprintf_s(_buf + _strlen, _size - _strlen,
				 _size - _strlen - 1, fmt, ap)) < 0) {
        _size += _increments;
	    _buf = (char *)realloc(_buf, _size);
        CHKBUF(_buf,_size);
    }
#else
    size_t newLen;
    va_list ap_copy;
    va_copy(ap_copy, ap);

    newLen = vsnprintf(_buf + _strlen, _size - _strlen, fmt, ap);
    if ((newLen + 1) > (_size - _strlen)) {
	    // This just finds the next multiple of _increment greater
	    // than the size we need.  Perhaps nicer would be to find the
	    // next power of 2?
    	_size = (_strlen + newLen + 1 + _increments) / _increments * _increments;
	    _buf = (char *)realloc(_buf, _size);
        CHKBUF(_buf,_size);
	    vsnprintf(_buf + _strlen, _size - _strlen, fmt, ap_copy);
    }
    va_end(ap_copy);
#endif
    _strlen += newLen;
    return _buf;
}


// eof = sprtfstr.cxx


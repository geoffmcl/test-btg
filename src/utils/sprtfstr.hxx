/*\
 * sprtfstr.hxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _SPRTFSTR_HXX_
#define _SPRTFSTR_HXX_

class sprtfstr {
public:
    sprtfstr();
    ~sprtfstr();

    const char *stg() { return _buf; }

    void clear();

    const char *printf(const char *fmt, ...);
    const char *appendf(const char *fmt, ...);

protected:

    char * _buf;
    size_t _size, _strlen;
    static const size_t _increments = 16;
    const char *_appendf(const char *fmt, va_list ap);

};


#endif // #ifndef _SPRTFSTR_HXX_
// eof - sprtfstr.hxx

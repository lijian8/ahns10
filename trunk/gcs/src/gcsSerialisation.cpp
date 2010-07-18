/**
 * \file
 * \author Tim Molloy
 *
 * Last Modified by: $Author$
 *
 * $LastChangedDate$
 * $Rev$
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Implementation of GCS Primitive Serialisation
 */

#include "primitive_serialisation.h"

int PackInt16(unsigned char *buf, int16_t i)
{
    unsigned char* p = (unsigned char*) &i;
    int ii = 0;

    for ( ii = 0; ii < 2; ++ii )
    {
        buf[ii] = p[1-ii];
    }
    return 2;
}

int UnpackInt16(const unsigned char *buf, int16_t *i)
{
    unsigned char* p = (unsigned char*) i;
    int ii = 0;

    for ( ii = 0; ii < 2; ++ii )
    {
        p[ii] = buf[ii];
    }
    return 2;
}

int PackUInt16(unsigned char *buf, uint16_t i)
{
    unsigned char* p = (unsigned char*) &i;
    int ii = 0;

    for ( ii = 0; ii < 2; ++ii )
    {
        buf[ii] = p[1-ii];
    }

    return 2;
}

int UnpackUInt16(const unsigned char *buf, uint16_t *i)
{
    unsigned char* p = (unsigned char*) i;
    int ii = 0;

    for ( ii = 0; ii < 2; ++ii )
    {
        p[ii] = buf[ii];
    }

    return 2;
}

int PackInt32(unsigned char *buf, int32_t i)
{
    unsigned char* p = (unsigned char*) &i;
    int ii = 0;

    for ( ii = 0; ii < 4; ++ii )
    {
        buf[ii] = p[3-ii];
    }

    return 4;
}

int UnpackInt32(const unsigned char *buf, int32_t *i)
{
    unsigned char* p = (unsigned char*) i;
    int ii = 0;

    for ( ii = 0; ii < 4; ++ii )
    {
        p[ii] = buf[3-ii];
    }
    return 4;
}

int PackUInt32(unsigned char *buf, uint32_t i)
{
    unsigned char* p = (unsigned char*) &i;
    int ii = 0;

    for ( ii = 0; ii < 4; ++ii )
    {
        buf[ii] = p[3-ii];
    }

    return 4;
}

int UnpackUInt32(const unsigned char *buf, uint32_t *i)
{
    unsigned char* p = (unsigned char*) i;
    int ii = 0;

    for ( ii = 0; ii < 4; ++ii )
    {
        p[ii] = buf[3-ii];
    }

    return 4;
}

int PackInt64(unsigned char *buf, int64_t i)
{
    unsigned char* p = (unsigned char*) &i;
    int ii = 0;

    for ( ii = 0; ii < 8; ++ii )
    {
        buf[ii] = p[7-ii];
    }

    return 8;
}

int UnpackInt64(const unsigned char *buf, int64_t *i)
{
    unsigned char* p = (unsigned char*) i;
    int ii = 0;

    for ( ii = 0; ii < 8; ++ii )
    {
        p[ii] = buf[ii];
    }

    return 8;
}

int PackUInt64(unsigned char *buf, uint64_t i)
{
    unsigned char* p = (unsigned char*) &i;
    int ii = 0;

    for ( ii = 0; ii < 8; ++ii )
    {
        buf[ii] = p[7-ii];
    }

    return 8;
}

int UnpackUInt64(const unsigned char *buf, uint64_t *i)
{
    unsigned char* p = (unsigned char*) i;
    int ii = 0;

    for ( ii = 0; ii < 8; ++ii )
    {
        p[ii] = buf[ii];
    }

    return 8;
}

int PackFloat32(unsigned char *buf, float f)
{
        return PackInt32(buf, *((int32_t*)&f));
}

int UnpackFloat32(const unsigned char *buf, float *f)
{
        return UnpackInt32(buf, (int32_t*)f);
}


int PackFloat64(unsigned char *buf, double d)
{
    unsigned char* p = (unsigned char*) &d;
    int i = 0;

    for (i = 0; i < 8; ++i)
    {
        buf[i] = p[7-i];
    }

    return sizeof(double);
}

int UnpackFloat64(const unsigned char *buf, double *d)
{
    unsigned char* p = (unsigned char*) d;
    int i = 0;

    for (i = 0; i < 8; ++i)
    {
        p[i] = buf[i];
    }

    return sizeof(double);
}

int PackUChar(unsigned char *buf, unsigned char c)
{
    // Record the char
    buf[0] = c;

    // Return the number of octets written to the buffer
    return 1;
}

int UnpackUChar(const unsigned char *buf, unsigned char *c)
{
    *c = buf[0];

    return 1;
}

int PackChar(unsigned char *buf, char c)
{
    return PackUChar(buf, c);
}

int UnpackChar(const unsigned char *buf, char *c)
{
    return UnpackUChar(buf, (unsigned char*)c);
}

int PackInt8(unsigned char *buf, int8_t i)
{
    return PackUChar(buf, i);
}

int UnpackInt8(const unsigned char *buf, int8_t *i)
{
    return UnpackUChar(buf, (unsigned char*)i);
}

int PackUInt8(unsigned char *buf, uint8_t i)
{
    return PackUChar(buf, i);
}

int UnpackUInt8(const unsigned char *buf, uint8_t *i)
{
    return UnpackUChar(buf, (unsigned char*)i);
}

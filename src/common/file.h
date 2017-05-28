/**
 * @file    src/common/file.h
 *
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _FILE_H_
#define _FILE_H_

#include <stdint.h>

namespace tmb_musicplayer
{

class File
{
public:

    virtual bool Open(const char* path) = 0;
    virtual char* GetString(char* buffer, int32_t bufferLength) = 0;
    virtual int32_t Tell() = 0;
    virtual int32_t Size() = 0;
    virtual bool Error() = 0;
    virtual bool IsEOF() = 0;
};
}

#endif /* _FILE_H_ */

/** @} */

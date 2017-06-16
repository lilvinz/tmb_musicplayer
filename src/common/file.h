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
#include <string>
#include <array>

namespace tmb_musicplayer
{

class File
{
public:

    virtual bool Open(const char* path) = 0;
    virtual uint32_t GetString(char* buffer, uint32_t bufferSize) = 0;
    virtual int32_t Tell() = 0;
    virtual bool Seek(int32_t pos) = 0;
    virtual int32_t Size() = 0;
    virtual bool Error() = 0;
    virtual bool IsEOF() = 0;
};
}

#endif /* _FILE_H_ */

/** @} */

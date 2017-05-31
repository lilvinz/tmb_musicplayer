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

namespace tmb_musicplayer
{

class File
{
public:

    virtual bool Open(const std::string& path) = 0;
    virtual bool GetString(std::string& buffer) = 0;
    virtual int32_t Tell() = 0;
    virtual void Seek(int32_t pos) = 0;
    virtual int32_t Size() = 0;
    virtual bool Error() = 0;
    virtual bool IsEOF() = 0;
};
}

#endif /* _FILE_H_ */

/** @} */

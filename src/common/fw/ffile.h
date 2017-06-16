/**
 * @file    src/common/ffile.h
 *
 * @brief CPP Wraper for fatfs file
 *
 * @addtogroup
 * @{
 */

#ifndef _FFILE_H_
#define _FFILE_H_

#include <stdint.h>
#include <string>
#include "ff.h"

#include "file.h"

namespace tmb_musicplayer
{

class FFile : public File
{
public:

    virtual bool Open(const char* path);
    virtual uint32_t GetString(char* buffer, uint32_t bufferSize);
    virtual int32_t Tell();
    virtual bool Seek(int32_t pos);
    virtual int32_t Size();
    virtual bool Error();
    virtual bool IsEOF();
private:
    FIL m_ff;
};
}

#endif /* _FFILE_H_ */

/** @} */

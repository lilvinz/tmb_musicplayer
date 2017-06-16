/**
 * @file    src/common/ffile.cpp
 *
 * @brief CPP Wraper for fatfs file
 *
 * @addtogroup
 * @{
 */

#include "ffile.h"
#include <string.h>

namespace tmb_musicplayer {

bool FFile::Open(const char* path) {
    FRESULT err = f_open(&m_ff, path, FA_READ);
    return err == FR_OK;
}

uint32_t FFile::GetString(char* buffer, uint32_t bufferSize) {
    if (f_gets(buffer, bufferSize, &m_ff) != NULL) {
        return strlen(buffer);
    }
    return 0;
}

int32_t FFile::Tell() {
    return f_tell(&m_ff);
}

bool FFile::Seek(int32_t pos) {
    return f_lseek(&m_ff, pos) == FR_OK;
}

int32_t FFile::Size() {
    return f_size(&m_ff);
}

bool FFile::Error() {
    return f_error(&m_ff);
}

bool FFile::IsEOF() {
    return f_eof(&m_ff);
}

}  // namespace tmb_musicplayer

/** @} */

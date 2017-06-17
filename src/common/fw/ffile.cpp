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

bool FFile::Create(const char* path) {
    FRESULT err = f_open(&m_ff, path, FA_OPEN_ALWAYS | FA_WRITE);
    return err == FR_OK;
}

bool FFile::Close() {
    return f_close(&m_ff) == FR_OK;
}

bool FFile::Sync() {
    return f_sync(&m_ff) == FR_OK;
}

uint32_t FFile::GetString(char* buffer, uint32_t bufferSize) {
    if (f_gets(buffer, bufferSize, &m_ff) != NULL) {
        return strlen(buffer);
    }
    return 0;
}

int32_t FFile::WriteString(const char* str) {
    return f_puts(str, &m_ff);
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

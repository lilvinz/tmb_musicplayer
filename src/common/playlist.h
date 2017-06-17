/**
 * @file    src/common/playlist.h
 *
 * @brief Simple m3u playlist
 *
 * @addtogroup
 * @{
 */

#ifndef _PLAYLIST_H_
#define _PLAYLIST_H_

#include "file.h"
#include <stdint.h>
#include <string>
#include <array>

namespace tmb_musicplayer
{

class Playlist
{
public:
    static const int32_t MaxTitleCount = 100;

    Playlist();
    ~Playlist();

    bool LoadFromFile(File* file);

    void Reset();
    uint32_t QueryNext(char* buffer, uint32_t bufferSize);
    uint32_t QueryPrev(char* buffer, uint32_t bufferSize);

    int32_t GetTitleCount() const {
        return m_titleCount;
    }
private:
    uint32_t QueryString(char* buffer, uint32_t bufferSize);

    File* m_file = NULL;

    std::array<char, 256> m_buffer;

    int32_t m_titleCount = 0;
    int32_t m_currentReadIndex = 0;
    int32_t m_readPositions[MaxTitleCount];
};
}

#endif /* _PLAYLIST_H_ */

/** @} */

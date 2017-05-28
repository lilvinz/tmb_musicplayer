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

namespace tmb_musicplayer
{

class Playlist
{
public:
    static const int32_t MaxTitleCount = 100;

    Playlist();
    ~Playlist();

    bool LoadFromFile(File* file);

    bool QueryNext(char* pszBuffer, int32_t bufferSize);
    bool QueryPrev(char* pszBuffer, int32_t bufferSize);
private:

    File* m_file;

    int32_t m_titleCount;
    int32_t m_readPositions[MaxTitleCount];
};
}

#endif /* _PLAYLIST_H_ */

/** @} */

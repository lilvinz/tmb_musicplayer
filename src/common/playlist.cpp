/**
 * @file    src/common/playlist.cpp
 * @brief
 *
 * @addtogroup
 * @{
 */
#include "playlist.h"

namespace tmb_musicplayer
{

Playlist::Playlist()
{

}

Playlist::~Playlist()
{

}

bool Playlist::LoadFromFile(File* file)
{
    return false;
}

bool Playlist::QueryNext(char* pszBuffer, int32_t bufferSize)
{
    return false;
}

bool Playlist::QueryPrev(char* pszBuffer, int32_t bufferSize)
{
    return false;
}

}

/** @} */

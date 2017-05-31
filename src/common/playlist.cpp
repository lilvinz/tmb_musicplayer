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
    m_file = file;
    m_titleCount = 0;
    m_currentReadIndex = -1;
    while (true)
    {
        auto readPos = file->Tell();
        std::string str(m_buffer.begin(), m_buffer.end());
        if (file->GetString(str))
        {
            // filter spaces in front of the text and comments
            auto iter = str.begin();
            for (; iter != str.end(); ++iter)
            {
                char c = *iter;
                if (c == '#') {
                    iter = str.end();
                    break;
                }
                else if (c != ' ') {
                    break;
                }
            }

            if (iter != str.end())
            {
                m_readPositions[m_titleCount++] =readPos;
                if (m_titleCount == MaxTitleCount) {
                    break;
                }
            }
        }
        else {
            break;
        }
    }
    return m_titleCount > 0;
}

bool Playlist::QueryNext(std::string& buffer)
{
    m_currentReadIndex++;
    if (m_currentReadIndex < m_titleCount) {
        if (QueryString(buffer)) {
            return true;
        }
    }
    m_currentReadIndex = m_titleCount;

    return false;
}

bool Playlist::QueryPrev(std::string& buffer)
{
    m_currentReadIndex--;
    if (m_currentReadIndex >= 0 && m_currentReadIndex < m_titleCount) {
        if (QueryString(buffer)) {
            return true;
        }
    }
    m_currentReadIndex = -1;

    return false;
}

bool Playlist::QueryString(std::string& buffer)
{
    m_file->Seek(m_readPositions[m_currentReadIndex]);
    std::string str(m_buffer.begin(), m_buffer.end());
    if (m_file->GetString(str))
    {
        buffer = str;
        return true;
    }
    return false;
}

}

/** @} */

/**
 * @file    src/common/playlist.cpp
 * @brief
 *
 * @addtogroup
 * @{
 */
#include "playlist.h"

#include <algorithm>

namespace tmb_musicplayer {

Playlist::Playlist() {
}

Playlist::~Playlist() {
}

bool Playlist::LoadFromFile(File* file) {
    m_file = file;
    m_titleCount = 0;
    m_currentReadIndex = -1;
    while (true) {
        auto readPos = file->Tell();
        auto pszBuffer = &m_buffer.front();
        if (file->GetString(pszBuffer, m_buffer.size())) {
            // filter spaces in front of the text and comments

            auto iter = m_buffer.begin();
            for (; iter != m_buffer.end(); ++iter) {
                char c = *iter;
                if (c == '#') {
                    iter = m_buffer.end();
                    break;
                } else if (c != ' ') {
                    break;
                }
            }

            if (iter != m_buffer.end()) {
                m_readPositions[m_titleCount++] = readPos;
                if (m_titleCount == MaxTitleCount) {
                    break;
                }
            }
        } else {
            break;
        }
    }
    return m_titleCount > 0;
}

void Playlist::Reset()
{
    m_currentReadIndex = -1;
}

uint32_t Playlist::QueryNext(char* buffer, uint32_t bufferSize) {
    m_currentReadIndex++;
    if (m_currentReadIndex < m_titleCount) {
        return QueryString(buffer, bufferSize);
    }
    m_currentReadIndex = m_titleCount;

    return 0;
}

uint32_t Playlist::QueryPrev(char* buffer, uint32_t bufferSize) {
    if (m_currentReadIndex > 0) {
        m_currentReadIndex--;
        if (m_currentReadIndex < m_titleCount) {
            return QueryString(buffer, bufferSize);
        }
    }
    return 0;
}

uint32_t Playlist::QueryString(char* buffer, uint32_t bufferSize) {
    if (m_file->Seek(m_readPositions[m_currentReadIndex])) {
        uint32_t chars = m_file->GetString(&(m_buffer.front()), m_buffer.size());
        if (chars > 0 && (chars < bufferSize)) {
            std::copy(m_buffer.begin(), m_buffer.begin() + chars, buffer);
            return chars;
        }
    }
    return 0;
}

}  // namespace tmb_musicplayer

/** @} */

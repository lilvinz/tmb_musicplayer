/*
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "gtest/gtest.h"

extern "C"
{
#include "ch.h"
#include "qhal.h"
}

#include "file.h"
#include <iostream>
#include <fstream>
#include <cstring>

#include "playlist.h"

class TestFile : public tmb_musicplayer::File
{
public:
    TestFile()
    {

    }

    virtual bool Open(const char* path)
    {
        m_file.open(path,  std::ios::binary);
        if (m_file.fail())
        {
            return false;
        }

        // read size
        m_file.seekg(0, std::ios::end);

        m_size = m_file.tellg();

        m_file.seekg(0, std::ios::beg);

        return true;
    }

    virtual char* GetString(char* buffer, int32_t bufferLength)
    {
        std::string line;
        if (std::getline(m_file, line))
        {
            std::strcpy(buffer, line.c_str());
            return buffer;
        }
        return NULL;
    };

    virtual int32_t Tell()
    {
        return m_file.tellg();
    };
    virtual int32_t Size()
    {
        return m_size;
    };
    virtual bool Error()
    {
        return m_file.fail();
    }
    virtual bool IsEOF()
    {
        return  m_file.eof();
    }
private:
    std::ifstream m_file;
    int32_t m_size;
};

class PlaylistTest: public ::testing::Test
{
protected:

    virtual void SetUp()
    {

    }

    virtual void TearDown()
    {

    }
};

TEST_F(PlaylistTest, load)
{
    char title[200];
    memset(title, 0, sizeof(title));

    TestFile plFile;
    plFile.Open("./playlist.m3u");

    tmb_musicplayer::Playlist pl;
    bool loaded = pl.LoadFromFile(&plFile);
    EXPECT_TRUE(loaded);

    bool getFirstTitle = pl.QueryNext(title, sizeof(title));
    EXPECT_TRUE(getFirstTitle);

    EXPECT_STREQ("/title1.mp3", title);
}

//===========================================================
//  Filename: remSnd.cpp
//      Sound class.
//      Copyright 2003
//      Created by Roman E. Marchenko, vortex@library.ntu-kpi.kiev.ua
//      $Id: remSnd.cpp,v 1.2 2003/09/26 21:04:57 vortex Exp $
//===========================================================

#include "SoundSource.h"
#include "al/al.h"
#include "al/alc.h"
#include "vorbis/codec.h"
#include <map>
#include <sstream>
#include <string>

#include "exception"

// Defines
#define NUM_OF_DYNBUF 2
#define DYNBUF_SIZE 10024

ALboolean CheckALError()
{
    ALenum ErrCode;
    std::string Err = "OpenAL error: ";
    auto code = alGetError();
    if ((ErrCode = alGetError()) != AL_NO_ERROR) {
        Err += (char*)alGetString(ErrCode);
        return AL_FALSE;
    }
    return AL_TRUE;
}

size_t ReadOgg(void* ptr, size_t size, size_t nmemb, void* datasource)
{
    std::istream* File = reinterpret_cast<std::istream*>(datasource);
    File->read((char*)ptr, size * nmemb);
    return static_cast<size_t>(File->gcount());
}

int SeekOgg(void* datasource, ogg_int64_t offset, int whence)
{
    std::istream* File = reinterpret_cast<std::istream*>(datasource);
    std::ios_base::seekdir Dir;
    File->clear();
    switch (whence) {
    case SEEK_SET:
        Dir = std::ios::beg;
        break;
    case SEEK_CUR:
        Dir = std::ios::cur;
        break;
    case SEEK_END:
        Dir = std::ios::end;
        break;
    default:
        return -1;
    }
    File->seekg((std::streamoff)offset, Dir);
    return (File->fail() ? -1 : 0);
}

long TellOgg(void* datasource)
{
    std::istream* File = reinterpret_cast<std::istream*>(datasource);
    return static_cast<long>(File->tellg());
}

int CloseOgg(void* datasource) { return 0; }

//-----------------------------------------------------------
//    Sound class
//-----------------------------------------------------------

SoundSource::SoundSource()
{
    for (int i = 0; i < 3; i++) {
        mPos[i] = mVel[i] = 0.0f;
    }
    mInfo = NULL;
    mVF = NULL;
    mComment = NULL;
}

SoundSource::~SoundSource()
{
    this->close();

    // Clear all buffers and sources
    for (auto i = _buffers.begin(); i != _buffers.end(); i++)
        alDeleteBuffers(1, &i->second.ID);
}

bool SoundSource::open(const std::string& filename, bool Streamed)
{
    // Create source
    alGenSources(1, &mSourceID);
    if (!CheckALError())
        return false;

    alSourcef(mSourceID, AL_PITCH, 1.0f);
    alSourcef(mSourceID, AL_GAIN, 1.0f);
    alSourcefv(mSourceID, AL_POSITION, mPos);
    alSourcefv(mSourceID, AL_VELOCITY, mVel);

    auto result = LoadOggFile(filename, Streamed);

    return result;
}

bool SoundSource::isStreamed() { return mStreamed; }

bool SoundSource::LoadOggFile(const std::string& Filename, bool Streamed)
{
    int i, DynBuffs = 1;
    ogg_int64_t BlockSize;
    // OAL specific
    SndInfo buffer;
    ALuint BufID = 0;
    // OggVorbis specific structures
    ov_callbacks cb;

    // Fill cb struct
    cb.close_func = CloseOgg;
    cb.read_func = ReadOgg;
    cb.seek_func = SeekOgg;
    cb.tell_func = TellOgg;

    // Create OggVorbis_File struct
    mVF = new OggVorbis_File;

    // Open Ogg file
    OggFile.open(Filename.c_str(), std::ios_base::in | std::ios_base::binary);

    // Generate local buffers
    if (ov_open_callbacks(&OggFile, mVF, NULL, -1, cb) < 0) {
        // This is not ogg bitstream. Return
        return false;
    }

    // Check for existance of sound
    if (!Streamed) {
        for (auto currentBuffer = _buffers.begin(); currentBuffer != _buffers.end();
             currentBuffer++) {
            if (currentBuffer->second.Filename == Filename)
                BufID = currentBuffer->first;
        }
        BlockSize = ov_pcm_total(mVF, -1) * 4;
    } else {
        BlockSize = DYNBUF_SIZE;
        DynBuffs = NUM_OF_DYNBUF;
        alSourcei(mSourceID, AL_LOOPING, AL_FALSE);
    }

    // Return vorbis_comment and vorbis_info structures
    mComment = ov_comment(mVF, -1);
    mInfo = ov_info(mVF, -1);

    // Fill buffer infos
    buffer.Rate = mInfo->rate;
    buffer.Filename = Filename;
    buffer.Format = (mInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    // Fill buffers with data each block by DYNBUF_SIZE bytes
    if (Streamed || !BufID) {
        for (i = 0; i < DynBuffs; i++) {
            // Create buffers
            alGenBuffers(1, &buffer.ID);
            if (!CheckALError())
                return false;
            _buffers.insert(
                std::pair<ALuint, SoundSource::SndInfo>(buffer.ID, buffer));
            // Read amount (DYNBUF_SIZE) data into each buffer
            ReadOggBlock(buffer.ID, static_cast<size_t>(BlockSize));
            if (!CheckALError())
                return false;

            if (Streamed) // Place buffer to queue
            {
                alSourceQueueBuffers(mSourceID, 1, &buffer.ID);
                if (!CheckALError())
                    return false;
            } else
                alSourcei(mSourceID, AL_BUFFER, buffer.ID);
        }
    } else {
        alSourcei(mSourceID, AL_BUFFER, _buffers[BufID].ID);
    }

    OggFile.close();

    return true;
}

bool SoundSource::ReadOggBlock(ALuint BufID, size_t Size)
{
    // vars
    char eof = 0;
    int current_section;
    long ret;
    char* PCM;

    size_t TotalRet = 0;

    if (Size < 1)
        return false;
    PCM = new char[Size];

    // Read loop
    while (TotalRet < Size) {
        ret = ov_read(mVF, PCM + TotalRet, Size - TotalRet, 0, 2, 1,
            &current_section);

        // if end of file or read limit exceeded
        if (ret == 0)
            break;
        else if (ret < 0) // Error in bitstream
        {
            //
        } else {
            TotalRet += ret;
        }
    }
    if (TotalRet > 0) {
        alBufferData(BufID, _buffers[BufID].Format, (void*)PCM, TotalRet,
            _buffers[BufID].Rate);
        CheckALError();
    }
    delete[] PCM;
    return (ret > 0);
}

void SoundSource::play(bool loop)
{

    mLooped = loop;

    alSourcei(mSourceID, AL_LOOPING, mLooped);
    alSourcePlay(mSourceID);
}

void SoundSource::pause()
{
    alSourcePause(mSourceID);
}

void SoundSource::close()
{
    alSourceStop(mSourceID);
    if (alIsSource(mSourceID))
        alDeleteSources(1, &mSourceID);
    if (!mVF) {
        ov_clear(mVF);
        delete mVF;
    }
}

void SoundSource::update()
{
    if (!mStreamed)
        return;

    int Processed = 0;
    ALuint BufID;

    alGetSourcei(mSourceID, AL_BUFFERS_PROCESSED, &Processed);

    // We still have processed buffers
    while (Processed--) {
        alSourceUnqueueBuffers(mSourceID, 1, &BufID);
        if (!CheckALError())
            return;
        if (ReadOggBlock(BufID, DYNBUF_SIZE) != 0) {
            alSourceQueueBuffers(mSourceID, 1, &BufID);
            if (!CheckALError())
                return;
        } else {
            ov_pcm_seek(mVF, 0);
            alSourceQueueBuffers(mSourceID, 1, &BufID);
            if (!CheckALError())
                return;

            if (!mLooped)
                stop();
        }
    }
}

void SoundSource::stop() { alSourceStop(mSourceID); }

void SoundSource::setVolume(float value)
{
    alSourcef(mSourceID, AL_GAIN, value);
}

void SoundSource::move(float X, float Y, float Z)
{
    ALfloat Pos[3] = { X, Y, Z };
    alSourcefv(mSourceID, AL_POSITION, Pos);
}

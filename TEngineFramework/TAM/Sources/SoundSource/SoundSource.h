//===========================================================
//  Filename: remSnd.h
//      Sound class.
//      Copyright 2003
//      Created by Roman E. Marchenko, vortex@library.ntu-kpi.kiev.ua
//      $Id: remSnd.h,v 1.2 2003/09/26 21:04:57 vortex Exp $
//===========================================================

#ifndef _REM_SND_H_
#define _REM_SND_H_

#include "map"

#include "AL/al.h"
#include "vorbis/vorbisfile.h"
#include <fstream>

#define NUM_OF_DYNBUF 2 // num buffers in queue

class SoundSource {
    struct SndInfo {
        unsigned int ID;
        std::string Filename;
        unsigned int Rate;
        unsigned int Format;
    };

    std::map<ALuint, SoundSource::SndInfo> _buffers;

    // OAL specific vars
    ALuint mSourceID;
    // vorbisfile specific vars
    OggVorbis_File* mVF;
    vorbis_comment* mComment;
    vorbis_info* mInfo;

    ALfloat mVel[3];
    ALfloat mPos[3];
    bool mLooped;

    std::ifstream OggFile;
    bool mStreamed;

    // Functions
    bool ReadOggBlock(ALuint BufID, size_t Size);
    bool LoadOggFile(const std::string& Filename, bool Streamed);

public:
    // Functions
    bool open(const std::string& filename, bool Streamed);
    bool isStreamed();
    void play(bool loop);
    void pause();
    void close();
    void update();
    void move(float X, float Y, float Z);
    void stop();
    void setVolume(float value);

    // Construction/destruction
    SoundSource();
    virtual ~SoundSource();
};

#endif

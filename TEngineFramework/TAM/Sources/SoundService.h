#pragma once

#include "map"
#include "memory"
#include "string"

#include "AL/alc.h"

#include "TUtils.h"

#include "ISoundService.h"

class SoundSource;

namespace TAMM {
    class SoundService : public ISoundService {

        ALCdevice* _device;
        ALCcontext* _context;

        std::map<std::wstring, std::shared_ptr<SoundSource>> _soundSources;

        void InitializeOpenAL();
        void DestroyOpenAL();

    public:
        SoundService();
        ~SoundService();

        bool addSound(const std::wstring& name, const std::wstring& pathToSoundFile);
        void removeSound(const std::wstring& name);
        void pause(const std::wstring& name);
        void play(const std::wstring& name, bool loop);
        void stop(const std::wstring& name);
        void setVolume(const std::wstring& name, float value);
        bool isExists(const std::wstring& name) const;
    };
}
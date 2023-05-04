#pragma once

#include "string"

#include "boost/smart_ptr.hpp"

#ifndef ISOUND_SERVICE
#define ISOUND_SERVICE
namespace TAMM {
    class ISoundService {
    public:
        virtual ~ISoundService() {}

        virtual bool addSound(const std::wstring& name, const std::wstring& pathToSoundFile) = 0;
        virtual void removeSound(const std::wstring& name) = 0;
        virtual void pause(const std::wstring& name) = 0;
        virtual void play(const std::wstring& name, bool loop) = 0;
        virtual void stop(const std::wstring& name) = 0;
        virtual void setVolume(const std::wstring& name, float value) = 0;
        virtual bool isExists(const std::wstring& name) const = 0;
    };
}
#endif
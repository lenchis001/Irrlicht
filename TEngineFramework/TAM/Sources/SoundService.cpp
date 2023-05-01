#include "SoundService.h"

#include "boost/filesystem.hpp"

#include "SoundSource/SoundSource.h"

TAMM::SoundService::SoundService() { InitializeOpenAL(); }

TAMM::SoundService::~SoundService() { DestroyOpenAL(); }

bool TAMM::SoundService::isExists(const std::wstring& name) const
{
    return _soundSources.find(name) != _soundSources.end();
}

bool TAMM::SoundService::addSound(const std::wstring& name, const std::wstring& pathToSoundFile)
{
    if (!boost::filesystem::exists(pathToSoundFile)) {
        return false;
    }

    std::shared_ptr<SoundSource> newSource(new SoundSource());

    newSource->open(TUtils::toString(pathToSoundFile), false);

    _soundSources.insert({ name, newSource });

    return true;
}

void TAMM::SoundService::removeSound(const std::wstring& name)
{
    _soundSources.erase(name);
}

void TAMM::SoundService::pause(const std::wstring& name)
{
    _soundSources.at(name)->pause();
}

void TAMM::SoundService::play(const std::wstring& name, bool loop)
{
    _soundSources.at(name)->play(loop);
}

void TAMM::SoundService::stop(const std::wstring& name)
{
    _soundSources.at(name)->stop();
}

void TAMM::SoundService::setVolume(const std::wstring& name, float value)
{
    _soundSources[name]->setVolume(value);
}

void TAMM::SoundService::InitializeOpenAL()
{
    // Position of the Listener.
    ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };

    // Velocity of the Listener.
    ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

    // Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
    // Also note that these should be units of '1'.
    ALfloat ListenerOri[] = { 0.0, 0.0, -1.0, 0.0, 1.0, 0.0 };

    // Open default sound device
    _device = alcOpenDevice(NULL);
    // Check for errors
    if (!_device) {
        throw std::runtime_error("Device can't be initialized.");
    }
    // Creating rendering context
    _context = alcCreateContext(_device, NULL);

    alcMakeContextCurrent(_context);

    // Set listener properties
    alListenerfv(AL_POSITION, ListenerPos);
    alListenerfv(AL_VELOCITY, ListenerVel);
    alListenerfv(AL_ORIENTATION, ListenerOri);
}

void TAMM::SoundService::DestroyOpenAL()
{
    // Shut down context
    alcMakeContextCurrent(NULL);
    // Destroy context
    alcDestroyContext(_context);
    // Close sound device
    alcCloseDevice(_device);
}

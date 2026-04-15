#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <mutex>

class Audio {
public:
    struct SoundSource {
        ALuint source;
        ALuint buffer;
        std::string name;
        bool looping;
        float volume;
        float pitch;
        
        SoundSource() : source(0), buffer(0), looping(false), volume(1.0f), pitch(1.0f) {}
    };

    Audio() : device(nullptr), context(nullptr), initialized(false) {}
    
    ~Audio() {
        shutdown();
    }

    bool init() {
        if (initialized) return true;

        device = alcOpenDevice(nullptr);
        if (!device) {
            std::cerr << "Failed to open audio device" << std::endl;
            return false;
        }

        context = alcCreateContext(device, nullptr);
        if (!context) {
            std::cerr << "Failed to create audio context" << std::endl;
            alcCloseDevice(device);
            device = nullptr;
            return false;
        }

        if (!alcMakeContextCurrent(context)) {
            std::cerr << "Failed to make context current" << std::endl;
            alcDestroyContext(context);
            alcCloseDevice(device);
            context = nullptr;
            device = nullptr;
            return false;
        }

        ALenum error = alGetError();
        if (error != AL_NO_ERROR) {
            std::cerr << "OpenAL error: " << error << std::endl;
            shutdown();
            return false;
        }

        initialized = true;
        std::cout << "Audio initialized" << std::endl;
        return true;
    }

    void shutdown() {
        if (!initialized) return;

        stopAll();

        for (auto& source : sources) {
            if (source.source != 0) {
                alDeleteSources(1, &source.source);
            }
        }
        sources.clear();

        for (auto& pair : soundBuffers) {
            if (pair.second != 0) {
                alDeleteBuffers(1, &pair.second);
            }
        }
        soundBuffers.clear();

        alcMakeContextCurrent(nullptr);
        if (context) {
            alcDestroyContext(context);
            context = nullptr;
        }
        if (device) {
            alcCloseDevice(device);
            device = nullptr;
        }

        initialized = false;
    }

    int loadSound(const std::string& filename, const std::string& soundName, bool loop = false) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = soundBuffers.find(soundName);
        if (it != soundBuffers.end()) {
            std::cerr << "Sound already loaded: " << soundName << std::endl;
            return -1;
        }

        ALuint buffer = 0;
        alGenBuffers(1, &buffer);
        
        if (alGetError() != AL_NO_ERROR) {
            std::cerr << "Failed to generate buffer for: " << filename << std::endl;
            return -1;
        }

        if (!loadWavFile(filename, buffer)) {
            std::cerr << "Failed to load sound: " << filename << std::endl;
            alDeleteBuffers(1, &buffer);
            return -1;
        }

        ALuint source = 0;
        alGenSources(1, &source);
        
        if (alGetError() != AL_NO_ERROR) {
            std::cerr << "Failed to generate source for: " << soundName << std::endl;
            alDeleteBuffers(1, &buffer);
            return -1;
        }

        alSourcei(source, AL_BUFFER, buffer);
        alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
        alSourcef(source, AL_GAIN, 1.0f);
        alSourcef(source, AL_PITCH, 1.0f);

        SoundSource soundSource;
        soundSource.source = source;
        soundSource.buffer = buffer;
        soundSource.name = soundName;
        soundSource.looping = loop;
        soundSource.volume = 1.0f;
        soundSource.pitch = 1.0f;

        sources.push_back(soundSource);
        soundBuffers[soundName] = buffer;
        
        std::cout << "Loaded: " << soundName << " (ID: " << sources.size() - 1 << ")" << std::endl;
        return static_cast<int>(sources.size() - 1);
    }

    void play(int index) {
        if (index < 0 || index >= static_cast<int>(sources.size())) {
            std::cerr << "Invalid sound index: " << index << std::endl;
            return;
        }
        alSourcePlay(sources[index].source);
    }

    void play(const std::string& soundName) {
        for (auto& source : sources) {
            if (source.name == soundName) {
                alSourcePlay(source.source);
                return;
            }
        }
        std::cerr << "Sound not found: " << soundName << std::endl;
    }

    void stop(int index) {
        if (index < 0 || index >= static_cast<int>(sources.size())) return;
        alSourceStop(sources[index].source);
    }

    void stop(const std::string& soundName) {
        for (auto& source : sources) {
            if (source.name == soundName) {
                alSourceStop(source.source);
                return;
            }
        }
    }

    void pause(int index) {
        if (index < 0 || index >= static_cast<int>(sources.size())) return;
        alSourcePause(sources[index].source);
    }

    void setVolume(int index, float volume) {
        if (index < 0 || index >= static_cast<int>(sources.size())) return;
        sources[index].volume = volume;
        alSourcef(sources[index].source, AL_GAIN, volume);
    }

    void setVolume(const std::string& soundName, float volume) {
        for (auto& source : sources) {
            if (source.name == soundName) {
                source.volume = volume;
                alSourcef(source.source, AL_GAIN, volume);
                return;
            }
        }
    }

    void setPitch(int index, float pitch) {
        if (index < 0 || index >= static_cast<int>(sources.size())) return;
        sources[index].pitch = pitch;
        alSourcef(sources[index].source, AL_PITCH, pitch);
    }

    void setLooping(int index, bool loop) {
        if (index < 0 || index >= static_cast<int>(sources.size())) return;
        sources[index].looping = loop;
        alSourcei(sources[index].source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    }

    bool isPlaying(int index) const {
        if (index < 0 || index >= static_cast<int>(sources.size())) return false;
        ALint state;
        alGetSourcei(sources[index].source, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }

    void stopAll() {
        for (auto& source : sources) {
            if (source.source != 0) {
                alSourceStop(source.source);
            }
        }
    }

    void update() {
        ALenum error = alGetError();
        if (error != AL_NO_ERROR) {
            std::cerr << "OpenAL error: " << error << std::endl;
        }
    }

    int getSourceIndex(const std::string& soundName) const {
        for (size_t i = 0; i < sources.size(); ++i) {
            if (sources[i].name == soundName) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    ALuint getSourceID(int index) const {
        if (index < 0 || index >= static_cast<int>(sources.size())) return 0;
        return sources[index].source;
    }

private:
    ALCdevice* device;
    ALCcontext* context;
    std::vector<SoundSource> sources;
    std::unordered_map<std::string, ALuint> soundBuffers;
    std::mutex mutex;
    bool initialized;

    bool loadWavFile(const std::string& filename, ALuint buffer) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open: " << filename << std::endl;
            return false;
        }

        char chunkId[5] = {0};
        uint32_t chunkSize;
        char format[5] = {0};

        file.read(chunkId, 4);
        file.read(reinterpret_cast<char*>(&chunkSize), 4);
        file.read(format, 4);

        if (std::string(chunkId) != "RIFF" || std::string(format) != "WAVE") {
            std::cerr << "Not a WAV file: " << filename << std::endl;
            return false;
        }

        char subchunkId[5] = {0};
        uint32_t subchunkSize;
        uint16_t audioFormat, numChannels, blockAlign, bitsPerSample;
        uint32_t sampleRate, byteRate;

        bool foundFmt = false;
        bool foundData = false;
        std::vector<char> audioData;

        while (!foundData && file.good()) {
            file.read(subchunkId, 4);
            file.read(reinterpret_cast<char*>(&subchunkSize), 4);

            if (std::string(subchunkId) == "fmt ") {
                foundFmt = true;
                file.read(reinterpret_cast<char*>(&audioFormat), 2);
                file.read(reinterpret_cast<char*>(&numChannels), 2);
                file.read(reinterpret_cast<char*>(&sampleRate), 4);
                file.read(reinterpret_cast<char*>(&byteRate), 4);
                file.read(reinterpret_cast<char*>(&blockAlign), 2);
                file.read(reinterpret_cast<char*>(&bitsPerSample), 2);
                
                if (subchunkSize > 16) {
                    file.seekg(subchunkSize - 16, std::ios::cur);
                }
            }
            else if (std::string(subchunkId) == "data") {
                foundData = true;
                audioData.resize(subchunkSize);
                file.read(audioData.data(), subchunkSize);
            }
            else {
                file.seekg(subchunkSize, std::ios::cur);
            }
        }

        if (!foundFmt || !foundData) {
            std::cerr << "Missing fmt or data chunk: " << filename << std::endl;
            return false;
        }

        if (audioFormat != 1) {
            std::cerr << "Not PCM WAV: " << filename << std::endl;
            return false;
        }

        ALenum formatAL;
        if (numChannels == 1) {
            formatAL = (bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
        } else if (numChannels == 2) {
            formatAL = (bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
        } else {
            std::cerr << "Unsupported channels: " << numChannels << std::endl;
            return false;
        }

        alBufferData(buffer, formatAL, audioData.data(), 
                     static_cast<ALsizei>(audioData.size()), sampleRate);

        if (alGetError() != AL_NO_ERROR) {
            std::cerr << "Failed to load buffer data" << std::endl;
            return false;
        }

        return true;
    }
};
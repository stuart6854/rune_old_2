#include "audio/audio.hpp"

#include "internal_common.hpp"
#include "al_common.hpp"

#include <AL/alc.h>
#include <AL/al.h>
#include <AudioFile/AudioFile.h>

#include <unordered_set>

namespace rune::audio
{
    constexpr auto STREAM_BUFFER_NUM = 4;
    constexpr auto STREAM_BUFFER_SIZE = 65536;  // 32kb
    constexpr auto MAX_CHANNELS = 512;

    struct SourceData
    {
        SourceId sourceId{};
        bool isPlaying{ false };

        u32 sampleRate{};
        ALenum format{};
        std::vector<u8> audioData{};

        bool isStreamed{ false };
        u64 streamCursor{ 0 };

        std::vector<ALuint> buffers{};
    };

    namespace
    {
        ALCdevice* g_device{ nullptr };                           // NOLINT
        ALCcontext* g_context{ nullptr };                         // NOLINT
        std::array<ALuint, STREAM_BUFFER_NUM> g_streamBuffers{};  // NOLINT
        std::unordered_map<SourceId, SourceData> g_sources{};     // NOLINT
        std::unordered_set<AudioId> g_audios{};                   // NOLINT

        void update_stream(
            ALuint source, ALenum format, u32 sampleRate, const std::vector<u8>& audioData, u64& streamCursor)
        {
            ALint buffersProcessed{ 0 };
            alCall(alGetSourcei, source, AL_BUFFERS_PROCESSED, &buffersProcessed);

            if (buffersProcessed <= 0)
            {
                return;
            }

            while (buffersProcessed--)
            {
                ALuint buffer{};
                alCall(alSourceUnqueueBuffers, source, 1, &buffer);

                ALsizei dataSize = STREAM_BUFFER_SIZE;

                std::vector<char> data(dataSize, 0);
                std::size_t dataSizeToCopy = STREAM_BUFFER_SIZE;
                if (streamCursor + STREAM_BUFFER_SIZE > audioData.size())
                {
                    dataSizeToCopy = audioData.size() - streamCursor;
                }

                std::memcpy(data.data(), &audioData[streamCursor], dataSizeToCopy);
                streamCursor += dataSizeToCopy;

                if (dataSizeToCopy < STREAM_BUFFER_SIZE)
                {
                    streamCursor = 0;
                    std::memcpy(&data[dataSizeToCopy], &audioData[streamCursor], STREAM_BUFFER_SIZE - dataSizeToCopy);
                    streamCursor = STREAM_BUFFER_SIZE - dataSizeToCopy;
                }

                alCall(alBufferData, buffer, format, data.data(), STREAM_BUFFER_SIZE, sampleRate);
                alCall(alSourceQueueBuffers, source, 1, &buffer);
            }
        }
    }

    void initialise()
    {
        auto availableDevices = get_available_devices(g_device).value_or(std::vector<std::string>());
        LOG_INFO("Available audio devices:");
        for (const auto& device : availableDevices)
        {
            LOG_INFO("  {}", device);
        }

        std::string_view defaultDevice = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
        LOG_INFO("Default audio device: {}", defaultDevice);

        g_device = alcCall(alcOpenDevice, nullptr, nullptr);
        if (!g_device)
        {
            LOG_ERROR("audio - Failed to create audio device!");
            return;
        }

        g_context = alcCall(alcCreateContext, g_device, g_device, nullptr);
        if (!g_context)
        {
            LOG_ERROR("audio - Failed to create audio context!");
            return;
        }
        if (!alcCall(alcMakeContextCurrent, g_device, g_context))
        {
            LOG_ERROR("audio - Failed to make context current!");
            return;
        }

        alCall(alGenBuffers, STREAM_BUFFER_NUM, g_streamBuffers.data());
    }

    void shutdown()
    {
        while (!g_sources.empty())
        {
            destroy_source(g_sources.begin()->first);
        }

        while (!g_audios.empty())
        {
            destroy_audio(*g_audios.begin());
        }

        alcMakeContextCurrent(g_context);
        alcCall(alcDestroyContext, g_device, g_context);
        g_context = nullptr;

        alcCall(alcCloseDevice, g_device, g_device);
        g_device = nullptr;
    }

    void update()
    {
        for (auto& [sourceId, sourceData] : g_sources)
        {
            if (sourceData.isPlaying)
            {
                update_stream(sourceId, sourceData.format, sourceData.sampleRate, sourceData.audioData, sourceData.streamCursor);
            }
        }
    }

    auto create_audio(std::string_view filename) -> AudioId
    {
        AudioFile<f32> audioFile{};
        if (!audioFile.load(filename.data()))
        {
            LOG_ERROR("audio : Failed to load audio file!");
            return 0;
        }
        const auto channels = audioFile.getNumChannels();
        const auto bitDepth = audioFile.getBitDepth();
        const auto sampleRate = audioFile.getSampleRate();
        std::vector<uint8_t> data{};
        audioFile.savePCMToBuffer(data);

        ALuint buffer{};
        alCall(alGenBuffers, 1, &buffer);

        ALenum format{};
        if (channels == 1 && bitDepth == 8)
        {
            format = AL_FORMAT_MONO8;
        }
        else if (channels == 1 && bitDepth == 16)
        {
            format = AL_FORMAT_MONO16;
        }
        else if (channels == 2 && bitDepth == 8)
        {
            format = AL_FORMAT_STEREO8;
        }
        else if (channels == 2 && bitDepth == 16)
        {
            format = AL_FORMAT_STEREO16;
        }
        else
        {
            LOG_ERROR("audio - Unrecognised wave format: channels={}, bitDepth={}", channels, bitDepth);
            return 0;
        }

        alCall(alBufferData, buffer, format, data.data(), ALsizei(data.size()), sampleRate);

        g_audios.insert(buffer);

        return buffer;
    }

    void destroy_audio(AudioId audio)
    {
        ALuint buffer = audio;
        alCall(alDeleteBuffers, 1, &buffer);

        g_audios.erase(audio);
    }

    auto create_source() -> SourceId
    {
        ALuint source{};
        alCall(alGenSources, 1, &source);
        set_source_pitch(source, 1.0f);
        set_source_volume(source, 1.0f);
        set_source_position(source, 0.0f, 0.0f, 0.0f);
        set_source_velocity(source, 0.0f, 0.0f, 0.0f);
        set_source_loop(source, false);

        g_sources[source] = { source };
        return source;
    }

    void destroy_source(SourceId source)
    {
        alCall(alDeleteSources, 1, &source);

        g_sources.erase(source);
    }

    void set_source_loop(SourceId source, bool loop)
    {
        alCall(alSourcei, source, AL_LOOPING, loop);
    }

    void set_source_volume(SourceId source, f32 volume)
    {
        alCall(alSourcef, source, AL_GAIN, volume);
    }

    void set_source_pitch(SourceId source, f32 pitch)
    {
        alCall(alSourcef, source, AL_PITCH, pitch);
    }

    void set_source_position(SourceId source, f32 x, f32 y, float z)
    {
        alCall(alSource3f, source, AL_POSITION, x, y, z);
    }

    void set_source_velocity(SourceId source, float x, float y, float z)
    {
        alCall(alSource3f, source, AL_VELOCITY, x, y, z);
    }

    auto get_source_state(SourceId source) -> SourceState
    {
        ALint state{};
        alCall(alGetSourcei, source, AL_SOURCE_STATE, &state);

        switch (state)
        {
            case AL_INITIAL: return SourceState::Initial;
            case AL_PLAYING: return SourceState::Playing;
            case AL_PAUSED: return SourceState::Paused;
            case AL_STOPPED: return SourceState::Stopped;
            default: break;
        }
        LOG_ERROR("audio - unknown source state!");
        return {};
    }

    void audio::source_play(SourceId source, std::string_view filename)
    {
        AudioFile<float> audioFile{};
        if (!audioFile.load(filename.data()))
        {
            LOG_ERROR("audio : Failed to load audio file!");
            return;
        }
        const auto channels = audioFile.getNumChannels();
        const auto bitDepth = audioFile.getBitDepth();
        const auto sampleRate = audioFile.getSampleRate();
        std::vector<uint8_t> audioData{};
        audioFile.savePCMToBuffer(audioData);

        ALuint buffer{};
        alCall(alGenBuffers, 1, &buffer);

        ALenum format{};
        if (channels == 1 && bitDepth == 8)
        {
            format = AL_FORMAT_MONO8;
        }
        else if (channels == 1 && bitDepth == 16)
        {
            format = AL_FORMAT_MONO16;
        }
        else if (channels == 2 && bitDepth == 8)
        {
            format = AL_FORMAT_STEREO8;
        }
        else if (channels == 2 && bitDepth == 16)
        {
            format = AL_FORMAT_STEREO16;
        }
        else
        {
            LOG_ERROR("audio - Unrecognised wave format: channels={}, bitDepth={}", channels, bitDepth);
            return;
        }

        auto& sourceData = g_sources.at(source);
        sourceData.sampleRate = sampleRate;
        sourceData.format = format;
        sourceData.audioData = audioData;

        sourceData.buffers.resize(STREAM_BUFFER_NUM);
        alCall(alGenBuffers, STREAM_BUFFER_NUM, sourceData.buffers.data());

        for (auto i = 0; i < STREAM_BUFFER_NUM; ++i)
        {
            alCall(alBufferData, sourceData.buffers[i], format, &audioData[i * STREAM_BUFFER_SIZE], STREAM_BUFFER_SIZE, sampleRate);
        }
        sourceData.streamCursor = STREAM_BUFFER_NUM * STREAM_BUFFER_SIZE;

        alCall(alSourceQueueBuffers, source, STREAM_BUFFER_NUM, sourceData.buffers.data());
        source_play(source);
    }

#if 0
    void source_play(SourceId source, AudioId audio)
    {
        auto& sourceData = g_sources.at(source);

        alCall(alSourceQueueBuffers, source, STREAM_BUFFER_NUM, sourceData.buffers.data());
        source_play(source);
    }
#endif

    void source_play(SourceId source)
    {
        auto& sourceData = g_sources.at(source);

        alCall(alSourcePlay, source);
        sourceData.isPlaying = true;
    }

    void source_pause(SourceId source)
    {
        alCall(alSourcePause, source);
    }

    void set_listener_position(float x, float y, float z)
    {
        alCall(alListener3f, AL_POSITION, x, y, z);
    }

}
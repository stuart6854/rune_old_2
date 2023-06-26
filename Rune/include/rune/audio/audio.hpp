// #TODO: Streaming audio (large audio files - eg. music, voice lines) -
// https://indiegamedev.net/2020/02/25/the-complete-guide-to-openal-with-c-part-2-streaming-audio/
// https://indiegamedev.net/2020/01/16/how-to-stream-ogg-files-with-openal-in-c/

#pragma once

#include <cstdint>
#include <string_view>

namespace rune::audio
{
    void initialise();
    void shutdown();
    void update();

    using AudioId = std::uint32_t;
    auto create_audio(std::string_view filename) -> AudioId;
    void destroy_audio(AudioId audio);

    using SourceId = std::uint32_t;
    auto create_source() -> SourceId;
    void destroy_source(SourceId source);

    void set_source_loop(SourceId source, bool loop);
    void set_source_pitch(SourceId source, float pitch);
    void set_source_volume(SourceId source, float volume);
    void set_source_position(SourceId source, float x, float y, float z);
    void set_source_velocity(SourceId source, float x, float y, float z);

    enum class SourceState : std::uint8_t
    {
        Initial,
        Playing,
        Paused,
        Stopped,
    };
    auto get_source_state(SourceId source) -> SourceState;

    void source_play(SourceId source, std::string_view filename);
    //    void source_play(SourceId source, AudioId audio);
    void source_play(SourceId source);
    void source_pause(SourceId source);

    void set_listener_position(float x, float y, float z);

}

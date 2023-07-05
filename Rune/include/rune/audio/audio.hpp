// #TODO: Streaming audio (large audio files - eg. music, voice lines) -
// https://indiegamedev.net/2020/02/25/the-complete-guide-to-openal-with-c-part-2-streaming-audio/
// https://indiegamedev.net/2020/01/16/how-to-stream-ogg-files-with-openal-in-c/
// #TODO: Positional Audio - https://indiegamedev.net/2020/04/12/the-complete-guide-to-openal-with-c-part-3-positioning-sounds/

#pragma once

#include "common_internal.hpp"

#include <cstdint>
#include <string_view>

namespace rune::audio
{
    void initialise();
    void shutdown();
    void update();

    using AudioId = u32;
    auto create_audio(std::string_view filename) -> AudioId;
    void destroy_audio(AudioId audio);

    using SourceId = u32;
    auto create_source() -> SourceId;
    void destroy_source(SourceId source);

    void set_source_loop(SourceId source, bool loop);
    void set_source_pitch(SourceId source, f32 pitch);
    void set_source_volume(SourceId source, f32 volume);
    void set_source_position(SourceId source, f32 x, f32 y, f32 z);
    void set_source_velocity(SourceId source, f32 x, f32 y, f32 z);

    enum class SourceState : u8
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

    void set_listener_position(f32 x, f32 y, float z);

}

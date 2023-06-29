#pragma once

#include <AL/alc.h>
#include <AL/al.h>

#include <vector>
#include <string>
#include <string_view>

#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)

namespace rune::audio
{
#pragma region Errors

    void check_al_errors(std::string_view filename, const u32 line)
    {
        ALCenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            LOG_ERROR("***ERROR*** ({} : {})", filename, line);
            switch (error)
            {
                case AL_INVALID_NAME: LOG_ERROR("AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function"); break;
                case AL_INVALID_ENUM: LOG_ERROR("AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function"); break;
                case AL_INVALID_VALUE: LOG_ERROR("AL_INVALID_VALUE: an invalid value was passed to an OpenAL function"); break;
                case AL_INVALID_OPERATION: LOG_ERROR("AL_INVALID_OPERATION: the requested operation is not valid"); break;
                case AL_OUT_OF_MEMORY:
                    LOG_ERROR("AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
                    break;
                default: LOG_ERROR("UNKNOWN AL ERROR: "); break;
            }
        }
    }

    void check_alc_errors(std::string_view filename, const u32 line, ALCdevice* device)
    {
        ALCenum error = alcGetError(device);
        if (error != ALC_NO_ERROR)
        {
            LOG_ERROR("***ERROR*** ({} : {})", filename, line);
            switch (error)
            {
                case ALC_INVALID_VALUE: LOG_ERROR("ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function"); break;
                case ALC_INVALID_DEVICE: LOG_ERROR("ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function"); break;
                case ALC_INVALID_CONTEXT: LOG_ERROR("ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function"); break;
                case ALC_INVALID_ENUM: LOG_ERROR("ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function"); break;
                case ALC_OUT_OF_MEMORY: LOG_ERROR("ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function"); break;
                default: LOG_ERROR("UNKNOWN ALC ERROR: {}", error);
            }
        }
    }

    template <typename alFunction, typename... Params>
    auto alCallImpl(std::string_view filename, const u32 line, alFunction function, Params... params) ->
        typename std::enable_if<std::is_same<void, decltype(function(params...))>::value, decltype(function(params...))>::type
    {
        function(std::forward<Params>(params)...);
        check_al_errors(filename, line);
    }

    template <typename alFunction, typename... Params>
    auto alCallImpl(std::string_view filename, const u32 line, alFunction function, Params... params) ->
        typename std::enable_if<!std::is_same<void, decltype(function(params...))>::value, decltype(function(params...))>::type
    {
        auto ret = function(std::forward<Params>(params)...);
        check_al_errors(filename, line);
        return ret;
    }

    template <typename alcFunction, typename... Params>
    auto alcCallImpl(std::string_view filename, const u32 line, alcFunction function, ALCdevice* device, Params... params) ->
        typename std::enable_if<std::is_same<void, decltype(function(params...))>::value, decltype(function(params...))>::type
    {
        function(std::forward<Params>(params)...);
        check_alc_errors(filename, line, device);
    }

    template <typename alcFunction, typename... Params>
    auto alcCallImpl(std::string_view filename, const u32 line, alcFunction function, ALCdevice* device, Params... params) ->
        typename std::enable_if<!std::is_same<void, decltype(function(params...))>::value, decltype(function(params...))>::type
    {
        auto ret = function(std::forward<Params>(params)...);
        check_alc_errors(filename, line, device);
        return ret;
    }

#pragma endregion

    auto get_available_devices(ALCdevice* device) -> std::optional<std::vector<std::string>>
    {
        RUNE_UNUSED(device);
#if 0
        const ALCchar* devices = alcCall(alcGetString, device, nullptr, ALC_DEVICE_SPECIFIER);
        if (!devices)
        {
            return std::nullopt;
        }

        const char* ptr = devices;

        std::vector<std::string> deviceVector;

        do
        {
            deviceVector.emplace_back(ptr);
            ptr += deviceVector.back().size() + 1;  // NOLINT
        } while (*(ptr + 1) != '\0');               // NOLINT

        return deviceVector;
#endif
        return {};
    }

}
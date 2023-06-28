// #TODO: Get monitors
// #TODO: Get supported resolutions/refresh rates
// #TODO: Gamepad connect/disconnect callback

#pragma once

#include "internal_common.hpp"

#include <glm/ext/vector_int2_sized.hpp>

#include <cstdint>
#include <string_view>

namespace rune::platform
{
    void initialise();
    void shutdown();

    void update();

#pragma region Time

    auto get_time() -> f64;

#pragma endregion

#pragma region Windows

    using WindowHandle = void*;

    auto create_window(i32 width, i32 height, std::string_view title) -> WindowHandle;
    void destroy_window(WindowHandle window);

    auto get_window_platform_handle(WindowHandle window) -> void*;
    bool has_window_requested_close(WindowHandle window);

    void show_window(WindowHandle window);
    void hide_window(WindowHandle window);

    auto get_window_size(WindowHandle window) -> glm::i32vec2;
    auto get_window_size_pixels(WindowHandle window) -> glm::i32vec2;
    auto get_window_position(WindowHandle window) -> glm::i32vec2;

    void set_window_size(WindowHandle window, i32 width, i32 height);
    void set_window_position(WindowHandle window, i32 x, i32 y);
    void set_window_title(WindowHandle window, std::string_view title);

    void set_window_windowed(WindowHandle window);
    void set_window_fullscreen(WindowHandle window);

#pragma endregion

#pragma region Input

    /* Mimics GLFW keyboard keys - https://www.glfw.org/docs/latest/group__keys.html */
    enum class Key : u16
    {
        Space = 32,
        Apostrophe = 39, /* ' */
        Comma = 44,      /* , */
        Minus = 45,      /* - */
        Period = 46,     /* . */
        Slash = 47,      /* / */
        Zero = 48,
        One = 49,
        Two = 50,
        Three = 51,
        Four = 52,
        Five = 53,
        Six = 54,
        Seven = 55,
        Eight = 56,
        Nine = 57,
        Semicolon = 59, /* ; */
        Equal = 61,     /* = */
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        LeftBracket = 91,  /* [ */
        Backslash = 92,    /* \ */
        RightBracket = 93, /* ] */
        GraveAccent = 96,  /* ` */
        World1 = 161,      /* non-US #1 */
        World2 = 162,      /* non-US #2 */

        /* Function keys */
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,
        Keypad0 = 320,
        Keypad1 = 321,
        Keypad2 = 322,
        Keypad3 = 323,
        Keypad4 = 324,
        Keypad5 = 325,
        Keypad6 = 326,
        Keypad7 = 327,
        Keypad8 = 328,
        Keypad9 = 329,
        KeypadDecimal = 330,
        KeypadDivide = 331,
        KeypadMultiply = 332,
        KeypadSubtract = 333,
        KeypadAdd = 334,
        KeypadEnter = 335,
        KeypadEqual = 336,
        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        LeftSuper = 343,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346,
        RightSuper = 347,
        Menu = 348,

        Last = Menu,
    };

    auto get_key_name(Key key) -> std::string_view;

    bool is_key_down(WindowHandle window, Key key);

    auto get_cursor_position(WindowHandle window) -> std::pair<f64, f64>;

    /* Mimics GLFW mouse buttons - https://www.glfw.org/docs/latest/group__buttons.html */
    enum class Button : u8
    {
        One = 0,
        Two = 1,
        Three = 2,
        Four = 3,
        Five = 4,
        Six = 5,
        Seven = 6,
        Eight = 7,
        Last = Eight,
        Left = One,
        Right = Two,
        Middle = Three,
    };
    bool is_mouse_button_down(WindowHandle window, Button button);

    enum class Gamepad : u8
    {
        One = 0,
        Two = 1,
        Three = 2,
        Four = 3,
        Five = 4,
        Six = 5,
        Seven = 6,
        Eight = 7,
        Nine = 8,
        Ten = 9,
        Eleven = 10,
        Twelve = 11,
        Thirteen = 12,
        Fourteen = 13,
        Fifteen = 14,
        Sixteen = 15,
        LAST = Sixteen,
    };
    bool is_gamepad_present(Gamepad gamepad);
    auto get_gamepad_name(Gamepad gamepad) -> std::string_view;

    enum class GamepadButton : u8
    {
        A = 0,
        B = 1,
        X = 2,
        Y = 3,
        LeftBumper = 4,
        RightBumper = 5,
        Back = 6,
        Start = 7,
        Guide = 8,
        LeftThumb = 9,
        RightThumb = 10,
        DPadUp = 11,
        DPadRight = 12,
        DPadDown = 13,
        DPadLeft = 14,
        Last = DPadLeft,

        Cross = A,
        Circle = B,
        Square = X,
        Triangle = Y,
    };
    bool is_gamepad_button_down(Gamepad gamepad, GamepadButton button);

    enum class GamepadAxis : u8
    {
        LeftX = 0,
        LeftY = 1,
        RightX = 2,
        RightY = 3,
        LeftTrigger = 4,
        RightTrigger = 5,
        Last = RightTrigger,
    };
    auto get_gamepad_axis_state(Gamepad gamepad, GamepadAxis axis) -> f32;

#pragma endregion

}
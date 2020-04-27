/*
Copyright (c) 2020 Andre Leiradella

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// LOVE
#include "wrap_Core.h"

// libc
#include "string.h"

namespace love
{
namespace libretro
{

static uint32_t djb2(const char *name)
{
    uint32_t hash = 5381;

    for (const uint8_t *str = (const uint8_t*) name; *str != 0; str++)
        hash = hash * 33 + *str;
    
    return hash;
}

static bool getDevice(const char *name, lrcpp::Device &device)
{
    switch (djb2(name))
    {
        case UINT32_C(0x7c9b47f5): device = lrcpp::Device::None; return strcmp(name, "none") == 0;
        case UINT32_C(0x073eba8c): device = lrcpp::Device::Joypad; return strcmp(name, "joypad") == 0;
        case UINT32_C(0x0ff24e0e): device = lrcpp::Device::Mouse; return strcmp(name, "mouse") == 0;
        case UINT32_C(0xbb5d5b76): device = lrcpp::Device::Keyboard; return strcmp(name, "keyboard") == 0;
        case UINT32_C(0xb1fab2e7): device = lrcpp::Device::Lightgun; return strcmp(name, "lightgun") == 0;
        case UINT32_C(0xf220fc17): device = lrcpp::Device::Analog; return strcmp(name, "analog") == 0;
        case UINT32_C(0xbbd12926): device = lrcpp::Device::Pointer; return strcmp(name, "pointer") == 0;
    }
}

static bool getInput(const char *name, Core::Input &input)
{
    switch (djb2(name))
    {
        case UINT32_C(0x7c9b47f5): input = Core::Input::NONE; return strcmp(name, "none") == 0;
        case UINT32_C(0xef160c6e): input = Core::Input::JOYPAD_B; return strcmp(name, "joypadb") == 0;
        case UINT32_C(0xef160c85): input = Core::Input::JOYPAD_Y; return strcmp(name, "joypady") == 0;
        case UINT32_C(0xfa5d528c): input = Core::Input::JOYPAD_SELECT; return strcmp(name, "joypadselect") == 0;
        case UINT32_C(0x6c777bba): input = Core::Input::JOYPAD_START; return strcmp(name, "joypadstart") == 0;
        case UINT32_C(0xd1d79d11): input = Core::Input::JOYPAD_UP; return strcmp(name, "joypadup") == 0;
        case UINT32_C(0xa629de64): input = Core::Input::JOYPAD_DOWN; return strcmp(name, "joypaddown") == 0;
        case UINT32_C(0xa62e14b7): input = Core::Input::JOYPAD_LEFT; return strcmp(name, "joypadleft") == 0;
        case UINT32_C(0x6c5f734a): input = Core::Input::JOYPAD_RIGHT; return strcmp(name, "joypadright") == 0;
        case UINT32_C(0xef160c6d): input = Core::Input::JOYPAD_A; return strcmp(name, "joypada") == 0;
        case UINT32_C(0xef160c84): input = Core::Input::JOYPAD_X; return strcmp(name, "joypadx") == 0;
        case UINT32_C(0xef160c78): input = Core::Input::JOYPAD_L; return strcmp(name, "joypadl") == 0;
        case UINT32_C(0xef160c7e): input = Core::Input::JOYPAD_R; return strcmp(name, "joypadr") == 0;
        case UINT32_C(0xd1d79baa): input = Core::Input::JOYPAD_L2; return strcmp(name, "joypadl2") == 0;
        case UINT32_C(0xd1d79c70): input = Core::Input::JOYPAD_R2; return strcmp(name, "joypadr2") == 0;
        case UINT32_C(0xd1d79bab): input = Core::Input::JOYPAD_L3; return strcmp(name, "joypadl3") == 0;
        case UINT32_C(0xd1d79c71): input = Core::Input::JOYPAD_R3; return strcmp(name, "joypadr3") == 0;
        case UINT32_C(0x57a09c7a): input = Core::Input::ANALOG_LEFT_X; return strcmp(name, "analogleftx") == 0;
        case UINT32_C(0x57a09c7b): input = Core::Input::ANALOG_LEFT_Y; return strcmp(name, "analoglefty") == 0;
        case UINT32_C(0x59fbdacd): input = Core::Input::ANALOG_RIGHT_X; return strcmp(name, "analogrightx") == 0;
        case UINT32_C(0x59fbdace): input = Core::Input::ANALOG_RIGHT_Y; return strcmp(name, "analogrighty") == 0;
        case UINT32_C(0x36407f59): input = Core::Input::ANALOG_B; return strcmp(name, "analogb") == 0;
        case UINT32_C(0x36407f70): input = Core::Input::ANALOG_Y; return strcmp(name, "analogy") == 0;
        case UINT32_C(0x5c0b5057): input = Core::Input::ANALOG_SELECT; return strcmp(name, "analogselect") == 0;
        case UINT32_C(0x58276c25): input = Core::Input::ANALOG_START; return strcmp(name, "analogstart") == 0;
        case UINT32_C(0xfe506d5c): input = Core::Input::ANALOG_UP; return strcmp(name, "analogup") == 0;
        case UINT32_C(0xd417ed6f): input = Core::Input::ANALOG_DOWN; return strcmp(name, "analogdown") == 0;
        case UINT32_C(0xd41c23c2): input = Core::Input::ANALOG_LEFT; return strcmp(name, "analogleft") == 0;
        case UINT32_C(0x580f63b5): input = Core::Input::ANALOG_RIGHT; return strcmp(name, "analogright") == 0;
        case UINT32_C(0x36407f58): input = Core::Input::ANALOG_A; return strcmp(name, "analoga") == 0;
        case UINT32_C(0x36407f6f): input = Core::Input::ANALOG_X; return strcmp(name, "analogx") == 0;
        case UINT32_C(0x36407f63): input = Core::Input::ANALOG_L; return strcmp(name, "analogl") == 0;
        case UINT32_C(0x36407f69): input = Core::Input::ANALOG_R; return strcmp(name, "analogr") == 0;
        case UINT32_C(0xfe506bf5): input = Core::Input::ANALOG_L2; return strcmp(name, "analogl2") == 0;
        case UINT32_C(0xfe506cbb): input = Core::Input::ANALOG_R2; return strcmp(name, "analogr2") == 0;
        case UINT32_C(0xfe506bf6): input = Core::Input::ANALOG_L3; return strcmp(name, "analogl3") == 0;
        case UINT32_C(0xfe506cbc): input = Core::Input::ANALOG_R3; return strcmp(name, "analogr3") == 0;
        case UINT32_C(0x0e3c1046): input = Core::Input::MOUSE_X; return strcmp(name, "mousex") == 0;
        case UINT32_C(0x0e3c1047): input = Core::Input::MOUSE_Y; return strcmp(name, "mousey") == 0;
        case UINT32_C(0x3da39939): input = Core::Input::MOUSE_LEFT; return strcmp(name, "mouseleft") == 0;
        case UINT32_C(0xf285880c): input = Core::Input::MOUSE_RIGHT; return strcmp(name, "mouseright") == 0;
        case UINT32_C(0x286fd988): input = Core::Input::MOUSE_WHEELUP; return strcmp(name, "mousewheelup") == 0;
        case UINT32_C(0x03c3149b): input = Core::Input::MOUSE_WHEELDOWN; return strcmp(name, "mousewheeldown") == 0;
        case UINT32_C(0x378b079d): input = Core::Input::MOUSE_MIDDLE; return strcmp(name, "mousemiddle") == 0;
        case UINT32_C(0x7a806e34): input = Core::Input::MOUSE_HORIZ_WHEELUP; return strcmp(name, "mousehorizwheelup") == 0;
        case UINT32_C(0x1c4b8447): input = Core::Input::MOUSE_HORIZ_WHEELDOWN; return strcmp(name, "mousehorizwheeldown") == 0;
        case UINT32_C(0xf753a2be): input = Core::Input::MOUSE_BUTTON_4; return strcmp(name, "mousebutton4") == 0;
        case UINT32_C(0xf753a2bf): input = Core::Input::MOUSE_BUTTON_5; return strcmp(name, "mousebutton5") == 0;
        case UINT32_C(0xbb5d5b76): input = Core::Input::KEYBOARD; return strcmp(name, "keyboard") == 0;
        case UINT32_C(0xedc591ff): input = Core::Input::LIGHTGUN_SCREEN_X; return strcmp(name, "lightgunscreenx") == 0;
        case UINT32_C(0xedc59200): input = Core::Input::LIGHTGUN_SCREEN_Y; return strcmp(name, "lightgunscreeny") == 0;
        case UINT32_C(0x8b6b3c7e): input = Core::Input::LIGHTGUN_IS_OFFSCREEN; return strcmp(name, "lightgunisoffscreen") == 0;
        case UINT32_C(0x5d1b73bb): input = Core::Input::LIGHTGUN_TRIGGER; return strcmp(name, "lightguntrigger") == 0;
        case UINT32_C(0x4ad1e4be): input = Core::Input::LIGHTGUN_RELOAD; return strcmp(name, "lightgunreload") == 0;
        case UINT32_C(0xc48f00b6): input = Core::Input::LIGHTGUN_AUX_A; return strcmp(name, "lightgunauxa") == 0;
        case UINT32_C(0xc48f00b7): input = Core::Input::LIGHTGUN_AUX_B; return strcmp(name, "lightgunauxb") == 0;
        case UINT32_C(0x57b3e4f5): input = Core::Input::LIGHTGUN_START; return strcmp(name, "lightgunstart") == 0;
        case UINT32_C(0x4d26e327): input = Core::Input::LIGHTGUN_SELECT; return strcmp(name, "lightgunselect") == 0;
        case UINT32_C(0xc48f00b8): input = Core::Input::LIGHTGUN_AUX_C; return strcmp(name, "lightgunauxc") == 0;
        case UINT32_C(0x2aea8525): input = Core::Input::LIGHTGUN_DPAD_UP; return strcmp(name, "lightgundpadup") == 0;
        case UINT32_C(0x8f971b78): input = Core::Input::LIGHTGUN_DPAD_DOWN; return strcmp(name, "lightgundpaddown") == 0;
        case UINT32_C(0x8f9b51cb): input = Core::Input::LIGHTGUN_DPAD_LEFT; return strcmp(name, "lightgundpadleft") == 0;
        case UINT32_C(0x837452de): input = Core::Input::LIGHTGUN_DPAD_RIGHT; return strcmp(name, "lightgundpadright") == 0;
        case UINT32_C(0x35f64e5e): input = Core::Input::POINTER_X; return strcmp(name, "pointerx") == 0;
        case UINT32_C(0x35f64e5f): input = Core::Input::POINTER_Y; return strcmp(name, "pointery") == 0;
        case UINT32_C(0x8259ff9c): input = Core::Input::POINTER_PRESSED; return strcmp(name, "pointerpressed") == 0;
    }

    return false;
}

static bool getKey(const char *name, lrcpp::Key &key)
{
    switch (djb2(name))
    {
        case UINT32_C(0xd56f0662): key = lrcpp::Key::Backspace; return strcmp(name, "backspace") == 0;
        case UINT32_C(0x0b88ad3c): key = lrcpp::Key::Tab; return strcmp(name, "tab") == 0;
        case UINT32_C(0x0f3b6d8c): key = lrcpp::Key::Clear; return strcmp(name, "clear") == 0;
        case UINT32_C(0x19306425): key = lrcpp::Key::Return; return strcmp(name, "return") == 0;
        case UINT32_C(0x1020ea43): key = lrcpp::Key::Pause; return strcmp(name, "pause") == 0;
        case UINT32_C(0xfbd10336): key = lrcpp::Key::Escape; return strcmp(name, "escape") == 0;
        case UINT32_C(0x105f1651): key = lrcpp::Key::Space; return strcmp(name, "space") == 0;
        case UINT32_C(0x81a0fe68): key = lrcpp::Key::Exclaim; return strcmp(name, "exclaim") == 0;
        case UINT32_C(0xfe78d685): key = lrcpp::Key::Quotedbl; return strcmp(name, "quotedbl") == 0;
        case UINT32_C(0x7c97c2c9): key = lrcpp::Key::Hash; return strcmp(name, "hash") == 0;
        case UINT32_C(0xf93894e3): key = lrcpp::Key::Dollar; return strcmp(name, "dollar") == 0;
        case UINT32_C(0x4eae4000): key = lrcpp::Key::Ampersand; return strcmp(name, "ampersand") == 0;
        case UINT32_C(0x103de0f3): key = lrcpp::Key::Quote; return strcmp(name, "quote") == 0;
        case UINT32_C(0x0f01ad46): key = lrcpp::Key::Leftparen; return strcmp(name, "leftparen") == 0;
        case UINT32_C(0x344a72d9): key = lrcpp::Key::Rightparen; return strcmp(name, "rightparen") == 0;
        case UINT32_C(0xab08a2eb): key = lrcpp::Key::Asterisk; return strcmp(name, "asterisk") == 0;
        case UINT32_C(0x7c9c54e9): key = lrcpp::Key::Plus; return strcmp(name, "plus") == 0;
        case UINT32_C(0x0f3d3632): key = lrcpp::Key::Comma; return strcmp(name, "comma") == 0;
        case UINT32_C(0x0feee651): key = lrcpp::Key::Minus; return strcmp(name, "minus") == 0;
        case UINT32_C(0x1484c6a8): key = lrcpp::Key::Period; return strcmp(name, "period") == 0;
        case UINT32_C(0x105ce6e0): key = lrcpp::Key::Slash; return strcmp(name, "slash") == 0;
        case UINT32_C(0x0002b5d5): key = lrcpp::Key::_0; return strcmp(name, "0") == 0;
        case UINT32_C(0x0002b5d6): key = lrcpp::Key::_1; return strcmp(name, "1") == 0;
        case UINT32_C(0x0002b5d7): key = lrcpp::Key::_2; return strcmp(name, "2") == 0;
        case UINT32_C(0x0002b5d8): key = lrcpp::Key::_3; return strcmp(name, "3") == 0;
        case UINT32_C(0x0002b5d9): key = lrcpp::Key::_4; return strcmp(name, "4") == 0;
        case UINT32_C(0x0002b5da): key = lrcpp::Key::_5; return strcmp(name, "5") == 0;
        case UINT32_C(0x0002b5db): key = lrcpp::Key::_6; return strcmp(name, "6") == 0;
        case UINT32_C(0x0002b5dc): key = lrcpp::Key::_7; return strcmp(name, "7") == 0;
        case UINT32_C(0x0002b5dd): key = lrcpp::Key::_8; return strcmp(name, "8") == 0;
        case UINT32_C(0x0002b5de): key = lrcpp::Key::_9; return strcmp(name, "9") == 0;
        case UINT32_C(0x0f3d3240): key = lrcpp::Key::Colon; return strcmp(name, "colon") == 0;
        case UINT32_C(0x3eb4052e): key = lrcpp::Key::Semicolon; return strcmp(name, "semicolon") == 0;
        case UINT32_C(0x7c9a055c): key = lrcpp::Key::Less; return strcmp(name, "less") == 0;
        case UINT32_C(0xfbb6b090): key = lrcpp::Key::Equals; return strcmp(name, "equals") == 0;
        case UINT32_C(0x0db4fd4f): key = lrcpp::Key::Greater; return strcmp(name, "greater") == 0;
        case UINT32_C(0xe71b785d): key = lrcpp::Key::Question; return strcmp(name, "question") == 0;
        case UINT32_C(0x0059773a): key = lrcpp::Key::At; return strcmp(name, "at") == 0;
        case UINT32_C(0xc6e5be4c): key = lrcpp::Key::Leftbracket; return strcmp(name, "leftbracket") == 0;
        case UINT32_C(0xd56cd6f1): key = lrcpp::Key::Backslash; return strcmp(name, "backslash") == 0;
        case UINT32_C(0x6176349f): key = lrcpp::Key::Rightbracket; return strcmp(name, "rightbracket") == 0;
        case UINT32_C(0x0f359d34): key = lrcpp::Key::Caret; return strcmp(name, "caret") == 0;
        case UINT32_C(0xd6b12c3f): key = lrcpp::Key::Underscore; return strcmp(name, "underscore") == 0;
        case UINT32_C(0xd54dd104): key = lrcpp::Key::Backquote; return strcmp(name, "backquote") == 0;
        case UINT32_C(0x0002b606): key = lrcpp::Key::A; return strcmp(name, "a") == 0;
        case UINT32_C(0x0002b607): key = lrcpp::Key::B; return strcmp(name, "b") == 0;
        case UINT32_C(0x0002b608): key = lrcpp::Key::C; return strcmp(name, "c") == 0;
        case UINT32_C(0x0002b609): key = lrcpp::Key::D; return strcmp(name, "d") == 0;
        case UINT32_C(0x0002b60a): key = lrcpp::Key::E; return strcmp(name, "e") == 0;
        case UINT32_C(0x0002b60b): key = lrcpp::Key::F; return strcmp(name, "f") == 0;
        case UINT32_C(0x0002b60c): key = lrcpp::Key::G; return strcmp(name, "g") == 0;
        case UINT32_C(0x0002b60d): key = lrcpp::Key::H; return strcmp(name, "h") == 0;
        case UINT32_C(0x0002b60e): key = lrcpp::Key::I; return strcmp(name, "i") == 0;
        case UINT32_C(0x0002b60f): key = lrcpp::Key::J; return strcmp(name, "j") == 0;
        case UINT32_C(0x0002b610): key = lrcpp::Key::K; return strcmp(name, "k") == 0;
        case UINT32_C(0x0002b611): key = lrcpp::Key::L; return strcmp(name, "l") == 0;
        case UINT32_C(0x0002b612): key = lrcpp::Key::M; return strcmp(name, "m") == 0;
        case UINT32_C(0x0002b613): key = lrcpp::Key::N; return strcmp(name, "n") == 0;
        case UINT32_C(0x0002b614): key = lrcpp::Key::O; return strcmp(name, "o") == 0;
        case UINT32_C(0x0002b615): key = lrcpp::Key::P; return strcmp(name, "p") == 0;
        case UINT32_C(0x0002b616): key = lrcpp::Key::Q; return strcmp(name, "q") == 0;
        case UINT32_C(0x0002b617): key = lrcpp::Key::R; return strcmp(name, "r") == 0;
        case UINT32_C(0x0002b618): key = lrcpp::Key::S; return strcmp(name, "s") == 0;
        case UINT32_C(0x0002b619): key = lrcpp::Key::T; return strcmp(name, "t") == 0;
        case UINT32_C(0x0002b61a): key = lrcpp::Key::U; return strcmp(name, "u") == 0;
        case UINT32_C(0x0002b61b): key = lrcpp::Key::V; return strcmp(name, "v") == 0;
        case UINT32_C(0x0002b61c): key = lrcpp::Key::W; return strcmp(name, "w") == 0;
        case UINT32_C(0x0002b61d): key = lrcpp::Key::X; return strcmp(name, "x") == 0;
        case UINT32_C(0x0002b61e): key = lrcpp::Key::Y; return strcmp(name, "y") == 0;
        case UINT32_C(0x0002b61f): key = lrcpp::Key::Z; return strcmp(name, "z") == 0;
        case UINT32_C(0x0e0d600d): key = lrcpp::Key::Leftbrace; return strcmp(name, "leftbrace") == 0;
        case UINT32_C(0x0b8860ba): key = lrcpp::Key::Bar; return strcmp(name, "bar") == 0;
        case UINT32_C(0x335625a0): key = lrcpp::Key::Rightbrace; return strcmp(name, "rightbrace") == 0;
        case UINT32_C(0x106d8717): key = lrcpp::Key::Tilde; return strcmp(name, "tilde") == 0;
        case UINT32_C(0xf8838478): key = lrcpp::Key::Delete; return strcmp(name, "delete") == 0;
        case UINT32_C(0x0b8888b0): key = lrcpp::Key::Kp0; return strcmp(name, "kp0") == 0;
        case UINT32_C(0x0b8888b1): key = lrcpp::Key::Kp1; return strcmp(name, "kp1") == 0;
        case UINT32_C(0x0b8888b2): key = lrcpp::Key::Kp2; return strcmp(name, "kp2") == 0;
        case UINT32_C(0x0b8888b3): key = lrcpp::Key::Kp3; return strcmp(name, "kp3") == 0;
        case UINT32_C(0x0b8888b4): key = lrcpp::Key::Kp4; return strcmp(name, "kp4") == 0;
        case UINT32_C(0x0b8888b5): key = lrcpp::Key::Kp5; return strcmp(name, "kp5") == 0;
        case UINT32_C(0x0b8888b6): key = lrcpp::Key::Kp6; return strcmp(name, "kp6") == 0;
        case UINT32_C(0x0b8888b7): key = lrcpp::Key::Kp7; return strcmp(name, "kp7") == 0;
        case UINT32_C(0x0b8888b8): key = lrcpp::Key::Kp8; return strcmp(name, "kp8") == 0;
        case UINT32_C(0x0b8888b9): key = lrcpp::Key::Kp9; return strcmp(name, "kp9") == 0;
        case UINT32_C(0xf5579a63): key = lrcpp::Key::Kpperiod; return strcmp(name, "kpperiod") == 0;
        case UINT32_C(0xd9a444f5): key = lrcpp::Key::Kpdivide; return strcmp(name, "kpdivide") == 0;
        case UINT32_C(0xa74b1d80): key = lrcpp::Key::Kpmultiply; return strcmp(name, "kpmultiply") == 0;
        case UINT32_C(0x3d88aeac): key = lrcpp::Key::Kpminus; return strcmp(name, "kpminus") == 0;
        case UINT32_C(0x09a0fde4): key = lrcpp::Key::Kpplus; return strcmp(name, "kpplus") == 0;
        case UINT32_C(0x3cfabffe): key = lrcpp::Key::Kpenter; return strcmp(name, "kpenter") == 0;
        case UINT32_C(0xdc89844b): key = lrcpp::Key::Kpequals; return strcmp(name, "kpequals") == 0;
        case UINT32_C(0x005979ca): key = lrcpp::Key::Up; return strcmp(name, "up") == 0;
        case UINT32_C(0x7c95cd5d): key = lrcpp::Key::Down; return strcmp(name, "down") == 0;
        case UINT32_C(0x10494163): key = lrcpp::Key::Right; return strcmp(name, "right") == 0;
        case UINT32_C(0x7c9a03b0): key = lrcpp::Key::Left; return strcmp(name, "left") == 0;
        case UINT32_C(0x04d4029a): key = lrcpp::Key::Insert; return strcmp(name, "insert") == 0;
        case UINT32_C(0x7c97fd8e): key = lrcpp::Key::Home; return strcmp(name, "home") == 0;
        case UINT32_C(0x0b886f1c): key = lrcpp::Key::End; return strcmp(name, "end") == 0;
        case UINT32_C(0x14364c47): key = lrcpp::Key::Pageup; return strcmp(name, "pageup") == 0;
        case UINT32_C(0xfaf1331a): key = lrcpp::Key::Pagedown; return strcmp(name, "pagedown") == 0;
        case UINT32_C(0x0059779c): key = lrcpp::Key::F1; return strcmp(name, "f1") == 0;
        case UINT32_C(0x0059779d): key = lrcpp::Key::F2; return strcmp(name, "f2") == 0;
        case UINT32_C(0x0059779e): key = lrcpp::Key::F3; return strcmp(name, "f3") == 0;
        case UINT32_C(0x0059779f): key = lrcpp::Key::F4; return strcmp(name, "f4") == 0;
        case UINT32_C(0x005977a0): key = lrcpp::Key::F5; return strcmp(name, "f5") == 0;
        case UINT32_C(0x005977a1): key = lrcpp::Key::F6; return strcmp(name, "f6") == 0;
        case UINT32_C(0x005977a2): key = lrcpp::Key::F7; return strcmp(name, "f7") == 0;
        case UINT32_C(0x005977a3): key = lrcpp::Key::F8; return strcmp(name, "f8") == 0;
        case UINT32_C(0x005977a4): key = lrcpp::Key::F9; return strcmp(name, "f9") == 0;
        case UINT32_C(0x0b886b4c): key = lrcpp::Key::F10; return strcmp(name, "f10") == 0;
        case UINT32_C(0x0b886b4d): key = lrcpp::Key::F11; return strcmp(name, "f11") == 0;
        case UINT32_C(0x0b886b4e): key = lrcpp::Key::F12; return strcmp(name, "f12") == 0;
        case UINT32_C(0x0b886b4f): key = lrcpp::Key::F13; return strcmp(name, "f13") == 0;
        case UINT32_C(0x0b886b50): key = lrcpp::Key::F14; return strcmp(name, "f14") == 0;
        case UINT32_C(0x0b886b51): key = lrcpp::Key::F15; return strcmp(name, "f15") == 0;
        case UINT32_C(0x3022d71e): key = lrcpp::Key::Numlock; return strcmp(name, "numlock") == 0;
        case UINT32_C(0x119988d5): key = lrcpp::Key::Capslock; return strcmp(name, "capslock") == 0;
        case UINT32_C(0xf5b3c191): key = lrcpp::Key::Scrollock; return strcmp(name, "scrollock") == 0;
        case UINT32_C(0x1a26f215): key = lrcpp::Key::Rshift; return strcmp(name, "rshift") == 0;
        case UINT32_C(0x0c27fe4f): key = lrcpp::Key::Lshift; return strcmp(name, "lshift") == 0;
        case UINT32_C(0x10462fac): key = lrcpp::Key::Rctrl; return strcmp(name, "rctrl") == 0;
        case UINT32_C(0x0fd99ca6): key = lrcpp::Key::Lctrl; return strcmp(name, "lctrl") == 0;
        case UINT32_C(0x7c9d3db8): key = lrcpp::Key::Ralt; return strcmp(name, "ralt") == 0;
        case UINT32_C(0x7c99f372): key = lrcpp::Key::Lalt; return strcmp(name, "lalt") == 0;
        case UINT32_C(0x104b6bde): key = lrcpp::Key::Rmeta; return strcmp(name, "rmeta") == 0;
        case UINT32_C(0x0fded8d8): key = lrcpp::Key::Lmeta; return strcmp(name, "lmeta") == 0;
        case UINT32_C(0x0c2f3ce0): key = lrcpp::Key::Lsuper; return strcmp(name, "lsuper") == 0;
        case UINT32_C(0x1a2e30a6): key = lrcpp::Key::Rsuper; return strcmp(name, "rsuper") == 0;
        case UINT32_C(0x7c9aba4a): key = lrcpp::Key::Mode; return strcmp(name, "mode") == 0;
        case UINT32_C(0xd3657a9b): key = lrcpp::Key::Compose; return strcmp(name, "compose") == 0;
        case UINT32_C(0x7c97d2ee): key = lrcpp::Key::Help; return strcmp(name, "help") == 0;
        case UINT32_C(0x102a0912): key = lrcpp::Key::Print; return strcmp(name, "print") == 0;
        case UINT32_C(0x1ceedc0c): key = lrcpp::Key::Sysreq; return strcmp(name, "sysreq") == 0;
        case UINT32_C(0x0f2c9f4a): key = lrcpp::Key::Break; return strcmp(name, "break") == 0;
        case UINT32_C(0x7c9a911a): key = lrcpp::Key::Menu; return strcmp(name, "menu") == 0;
        case UINT32_C(0x10289e52): key = lrcpp::Key::Power; return strcmp(name, "power") == 0;
        case UINT32_C(0x7c9672a0): key = lrcpp::Key::Euro; return strcmp(name, "euro") == 0;
        case UINT32_C(0x7c9f191b): key = lrcpp::Key::Undo; return strcmp(name, "undo") == 0;
    }

    return false;
}

static Core *luax_checkcore(lua_State *L, int idx)
{
    return luax_checktype<Core>(L, idx);
}

static int w_Core_step(lua_State *L)
{
    auto core = luax_checkcore(L, 1);
    core->step();
    return 0;
}

static int w_Core_getImage(lua_State *L)
{
    auto core = luax_checkcore(L, 1);
    luax_pushtype<love::graphics::Image>(L, core->getImage());
    return 1;
}

static int w_Core_setControllerPortDevice(lua_State *L)
{
    auto core = luax_checkcore(L, 1);
    int port = (int) luaL_checkinteger(L, 2) - 1;

    if (port < 0 || port >= Core::MaxPorts)
        return luaL_error(L, "Port outside of valid range [0,%d]", Core::MaxPorts - 1);
    
    if (lua_isnumber(L, 3))
    {
        int device = (int) luaL_checkinteger(L, 3);
        core->setControllerPortDevice(port, device);
    }
    else
    {
        lrcpp::Device device;
        const char *deviceStr = luaL_checkstring(L, 3);

        if (!getDevice(deviceStr, device))
            return luaL_error(L, "Unknown device '%s'", deviceStr);

        core->setControllerPortDevice(port, device);
    }

    return 0;
}

static int w_Core_setInput(lua_State *L)
{
    auto core = luax_checkcore(L, 1);
    int port = (int) luaL_checkinteger(L, 2) - 1;

    Core::Input input;
    const char *inputStr = luaL_checkstring(L, 3);

    if (!getInput(inputStr, input))
        return luaL_error(L, "Invalid core input '%s'", inputStr);
    
    if (lua_isnoneornil(L, 5))
    {
        int value = (int) luaL_checkinteger(L, 4);
            
        if (!core->setInput(port, input, value))
            return luaL_error(L, "Invalid core input '%s'", inputStr);
    }
    else
    {
        int index = (int) luaL_checkinteger(L, 4);
        int value = (int) luaL_checkinteger(L, 5);
            
        if (!core->setInput(port, input, index, value))
            return luaL_error(L, "Invalid core input '%s' with index %d", inputStr, index);
    }

    return 0;
}

static int w_Core_setKey(lua_State *L)
{
    auto core = luax_checkcore(L, 1);
    int port = (int) luaL_checkinteger(L, 2) - 1;

    Core::Input input;
    const char *inputStr = luaL_checkstring(L, 3);

    if (!getInput(inputStr, input))
        return luaL_error(L, "Invalid core input '%s'", inputStr);

    lrcpp::Key key;
    const char *keyStr = luaL_checkstring(L, 4);

    if (!getKey(keyStr, key))
        return luaL_error(L, "Invalid key '%s'", keyStr);
    
    int pressed = lua_toboolean(L, 5);

    if (!core->setKey(port, input, key, pressed != 0))
        return luaL_error(L, "Invalid key '%s'", keyStr);

    return 0;
}

static int w_Core_setVariable(lua_State *L)
{
    auto core = luax_checkcore(L, 1);
    const char *key = luaL_checkstring(L, 2);
    const char *value = luaL_checkstring(L, 3);

    lrcpp::Variable var;
    var.key = key;
    var.value = value;

    lua_pushboolean(L, core->setVariable(var) ? 1 : 0);
    return 1;
}

static int w_Core_getVariables(lua_State *L)
{
    auto core = luax_checkcore(L, 1);
    const auto &variables = core->getVariables();

    if (lua_istable(L, 2))
        lua_pushvalue(L, 2);
    else
        lua_createtable(L, 0, variables.size());

    for (const auto &var : variables)
    {
        lua_pushstring(L, var.value.c_str());
        lua_setfield(L, -2, var.key.c_str());
    }

    return 1;
}

static int w_Core_getInputDescriptors(lua_State *L)
{
    auto core = luax_checkcore(L, 1);
    const auto &descs = core->getInputDescriptors();

    if (lua_istable(L, 2))
        lua_pushvalue(L, 2);
    else
        lua_createtable(L, descs.size(), 0);

    int index = 1;

    for (const auto &desc : descs)
    {
        lua_createtable(L, 0, 5);

        lua_pushinteger(L, desc.port + 1);
        lua_setfield(L, -2, "port");
        lua_pushinteger(L, desc.device);
        lua_setfield(L, -2, "device");
        lua_pushinteger(L, desc.index);
        lua_setfield(L, -2, "index");
        lua_pushinteger(L, desc.id);
        lua_setfield(L, -2, "id");
        lua_pushstring(L, desc.description.c_str());
        lua_setfield(L, -2, "description");

        lua_rawseti(L, -2, index++);
    }

    return 1;
}

static int w_Core_getControllerInfo(lua_State *L)
{
    auto core = luax_checkcore(L, 1);
    const auto &infos = core->getControllerInfo();

    if (lua_istable(L, 2))
        lua_pushvalue(L, 2);
    else
        lua_createtable(L, infos.size(), 0);
    
    const size_t count = infos.size();

    for (size_t i = 0; i < count; i++)
    {
        const auto &info = infos[i];
        lua_rawgeti(L, -1, i + 1);

        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1);
            lua_createtable(L, info.types.size(), 0);
            lua_pushvalue(L, -1);
            lua_rawseti(L, -3, i + 1);
        }

        const size_t count2 = info.types.size();

        for (size_t j = 0; j < count2; j++)
        {
            const auto &desc = info.types[j];

            lua_pushinteger(L, desc.id);
            lua_setfield(L, -2, desc.desc.c_str());
        }

        lua_pop(L, 1);
    }

    lua_pushnil(L);
    lua_rawseti(L, -2, count + 1);
    return 1;
}

static int w_Core_getSystemInfo(lua_State *L)
{
    auto core = luax_checkcore(L, 1);
    const auto &info = core->getSystemInfo();

    if (lua_istable(L, 2))
        lua_pushvalue(L, 2);
    else
        lua_createtable(L, 0, 5);

    lua_pushstring(L, info.libraryName.c_str());
    lua_setfield(L, -2, "libraryName");
    lua_pushstring(L, info.libraryVersion.c_str());
    lua_setfield(L, -2, "libraryVersion");
    lua_pushstring(L, info.validExtensions.c_str());
    lua_setfield(L, -2, "validExtensions");
    lua_pushboolean(L, info.needFullPath);
    lua_setfield(L, -2, "needFullPath");
    lua_pushboolean(L, info.blockExtract);
    lua_setfield(L, -2, "blockExtract");

    return 1;
}

static int w_Core_getSystemAVInfo(lua_State *L)
{
    auto core = luax_checkcore(L, 1);
    const auto &info = core->getSystemAVInfo();

    if (lua_istable(L, 2))
        lua_pushvalue(L, 2);
    else
        lua_createtable(L, 0, 2);

    lua_getfield(L, -1, "geometry");

    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        lua_createtable(L, 0, 5);
        lua_pushvalue(L, -1);
        lua_setfield(L, -3, "geometry");
    }

    lua_pushinteger(L, info.geometry.baseWidth);
    lua_setfield(L, -2, "baseWidth");
    lua_pushinteger(L, info.geometry.baseHeight);
    lua_setfield(L, -2, "baseHeight");
    lua_pushinteger(L, info.geometry.maxWidth);
    lua_setfield(L, -2, "maxWidth");
    lua_pushinteger(L, info.geometry.maxHeight);
    lua_setfield(L, -2, "maxHeight");
    lua_pushnumber(L, info.geometry.aspectRatio);
    lua_setfield(L, -2, "aspectRatio");
    lua_pop(L, 1);

    lua_getfield(L, -1, "timing");

    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        lua_createtable(L, 0, 2);
        lua_pushvalue(L, -1);
        lua_setfield(L, -3, "timing");
    }

    lua_pushnumber(L, info.timing.fps);
    lua_setfield(L, -2, "fps");
    lua_pushnumber(L, info.timing.sampleRate);
    lua_setfield(L, -2, "sampleRate");
    lua_pop(L, 1);

    return 1;
}

static const luaL_Reg core_functions[] =
{
    {"step", w_Core_step},
    {"getImage", w_Core_getImage},
    {"setControllerPortDevice", w_Core_setControllerPortDevice},
    {"setInput", w_Core_setInput},
    {"setKey", w_Core_setKey},
    {"setVariable", w_Core_setVariable},
    {"getVariables", w_Core_getVariables},
    {"getInputDescriptors", w_Core_getInputDescriptors},
    {"getControllerInfo", w_Core_getControllerInfo},
    {"getSystemInfo", w_Core_getSystemInfo},
    {"getSystemAVInfo", w_Core_getSystemAVInfo},
    {0, 0}
};

int luaopen_core(lua_State *L)
{
    return luax_register_type(L, &Core::type, core_functions, nullptr);
}

} // libretro
} // love

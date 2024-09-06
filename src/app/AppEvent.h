/* Copyright 2024 teamprof.net@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#pragma once
#include <Arduino.h>

enum AppEvent : int16_t
{
    /////////////////////////////////////////////////////////////////////////////
    EventNull = 0,

    EventGpioISR = 10, // iParam=pin, uParam=value, lParam=millis()
    EventSystem,       // iParam=SystemTriggerSource

    /////////////////////////////////////////////////////////////////////////////
    EventUser = 500, // iParam=<UserTriggerSource>, uParam=<ButtonId>

    /////////////////////////////////////////////////////////////////////////////
};

enum SystemTriggerSource : int16_t
{
    SysInitDone = 0,
    SysSoftwareTimer,     // lParam=xTimer:uint32_t
    SysButtonClick,       // uParam=pin number
    SysButtonDoubleClick, // uParam=pin number
    SysButtonLongPress,   // uParam=pin number
};

typedef enum _UserTriggerSource : int16_t
{
    UserNull = 0,
    UserClick,
    UserDoubleClick,
    UserLongPress,
} UserTriggerSource;

typedef enum _ButtonId : int16_t
{
    ButtonIdNull = 0,
    ButtonIdGame,
    ButtonIdPlayer1,
    ButtonIdPlayer2,
} ButtonId;

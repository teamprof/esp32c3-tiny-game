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

///////////////////////////////////////////////////////////////////////////////
// ESP32C3 (ESP32-C3-WROOM-02-N4)
#if CONFIG_IDF_TARGET_ESP32C3
#define PIN_WS2812_DIN GPIO_NUM_2 //  (strapping, pull-up)
#define PIN_SW_PLAYER1 GPIO_NUM_6 // Player1 button (pull-up)
#define PIN_SW_PLAYER2 GPIO_NUM_7 // Player2 button (pull-up)
#define PIN_BOOT GPIO_NUM_9       // Boot button
#define PIN_SW_GAME GPIO_NUM_9    // Game button (pull-up)
#endif

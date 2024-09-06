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
#include "./src/app/ArduProfFreeRTOS.h"
#include "./src/app/AppContext.h"
#include "./src/app/AppLog.h"
#include "./src/app/thread/QueueMain.h"
#include "./src/app/thread/ThreadGame.h"

/////////////////////////////////////////////////////////////////////////////
static AppContext appContext = {0};

static void initGlobalVar(void)
{
}

static void createTasks(void)
{
    static freertos::QueueMain queueMain;
    static freertos::ThreadGame threadGame;

    appContext.queueMain = &queueMain;
    appContext.threadGame = &threadGame;

    static_cast<freertos::QueueMain *>(appContext.queueMain)->start(&appContext);
    static_cast<freertos::ThreadGame *>(appContext.threadGame)->start(&appContext);
}

void setup(void)
{
    Serial.begin(115200);
    while (!Serial)
    {
        delay(100);
    }
    Serial.println("initialized Serial");

    /////////////////////////////////////////////////////////////////////////////
    // Serial is used for DebugLog
    /////////////////////////////////////////////////////////////////////////////
    // set log output to serial port, and init log params such as log_level
    LOG_SET_LEVEL(DefaultLogLevel);
    LOG_SET_DELIMITER("");
    LOG_ATTACH_SERIAL(Serial); // debug log on TX0 (CH340)
    LOG_TRACE("initialized Serial/UART0 for debug log");

    initGlobalVar();
    createTasks();

    // LOG_DEBUG("setup done");

    // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop(void)
{
    // static_cast<freertos::QueueMain *>(appContext.queueMain)->messageLoop(1000);
    static_cast<freertos::QueueMain *>(appContext.queueMain)->messageLoopForever(); // never return
}

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
#include "./QueueMain.h"
#include "../util/EspUtil.h"
#include "../AppContext.h"
#include "../AppDef.h"

////////////////////////////////////////////////////////////////////////////////////////////
#define MIN_DEBOUNCE_TIME pdMS_TO_TICKS(1)

////////////////////////////////////////////////////////////////////////////////////////////
// Thread for core1
////////////////////////////////////////////////////////////////////////////////////////////
#define TASK_QUEUE_SIZE 2048 // message queue size for app task
#define TASK_PRIORITY 5

#define LOW_POWER_COUNT 5       // in unit of seconds
#define NO_OBJECT_COUNT (5 * 2) // (expiry seconds) x (timer frequency)

namespace freertos
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    QueueMain *QueueMain::_instance = nullptr;

    ////////////////////////////////////////////////////////////////////////////////////////////
    static uint8_t ucQueueStorageArea[TASK_QUEUE_SIZE * sizeof(Message)];
    static StaticQueue_t xStaticQueue;

    void QueueMain::printChipInfo(void)
    {
        PRINTLN("===============================================================================");
        PRINTLN("===============================================================================");
        PRINTLN("ESP.Reset reason=", getEspResetReasonString());
        PRINTLN("chipModel=", ESP.getChipModel(), ", chipRevision=", ESP.getChipRevision(),
                "\r\nNumber of cores=", ESP.getChipCores(), ", SDK version=", ESP.getSdkVersion());
        PRINTLN("===============================================================================");
        PRINTLN("===============================================================================");
    }

    /////////////////////////////////////////////////////////////////////////////
    QueueMain::QueueMain() : ardufreertos::MessageBus(TASK_QUEUE_SIZE, ucQueueStorageArea, &xStaticQueue),
                             _debounceTimer(queue(), EventSystem, SysSoftwareTimer),
                             _buttonBoot(queue()),
                             _buttonPlayer1(queue()),
                             _buttonPlayer2(queue()),
                             handlerMap()
    {
        _instance = this;

        handlerMap = {
            __EVENT_MAP(QueueMain, EventGpioISR),
            __EVENT_MAP(QueueMain, EventSystem),
            __EVENT_MAP(QueueMain, EventNull), // {EventNull, &QueueMain::handlerEventNull},
        };
    }

    void QueueMain::start(void *ctx)
    {
        LOG_TRACE("on core ", xPortGetCoreID(), ", xPortGetFreeHeapSize()=", xPortGetFreeHeapSize());
        MessageBus::start(ctx);

        printChipInfo();

        LOG_TRACE("CPU Frequency: ", getCpuFrequencyMhz(), " MHz");

        auto taskHandle = xTaskGetCurrentTaskHandle();
        vTaskPrioritySet(taskHandle, TASK_PRIORITY);
        LOG_TRACE("uxTaskPriorityGet()=", uxTaskPriorityGet(taskHandle));

        _buttonBoot.init(EventSystem, SysButtonClick, SysButtonDoubleClick, SysButtonLongPress);
        _buttonPlayer1.init(EventSystem, SysButtonClick, SysButtonDoubleClick, SysButtonLongPress);
        _buttonPlayer2.init(EventSystem, SysButtonClick, SysButtonDoubleClick, SysButtonLongPress);
        _debounceTimer.attachButton(&_buttonBoot);
    }

    void QueueMain::onMessage(const Message &msg)
    {
        auto func = handlerMap[msg.event];
        if (func)
        {
            (this->*func)(msg);
        }
        else
        {
            LOG_TRACE("Unsupported event=", msg.event, ", iParam=", msg.iParam, ", uParam=", msg.uParam, ", lParam=", msg.lParam);
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    __EVENT_FUNC_DEFINITION(QueueMain, EventGpioISR, msg) // void QueueMain::handlerEventGpioISR(const Message &msg)
    {
        uint8_t pin = msg.iParam;
        uint8_t value = msg.uParam;
        uint32_t time = msg.lParam;

        if (pin == _buttonBoot.getPin())
        {
            uint32_t ms = msg.lParam;
            _buttonBoot.onEventIsr(value, ms);
        }
        else if (pin == _buttonPlayer1.getPin())
        {
            if (value == _buttonPlayer1.getActiveState())
            {
                _buttonPlayer1.edgeFallingTime = time;
            }
            else
            {
                auto edgeFallingTime = _buttonPlayer1.edgeFallingTime;
                auto delta = (time > edgeFallingTime) ? (time - edgeFallingTime) : (edgeFallingTime - time);
                if (delta > MIN_DEBOUNCE_TIME)
                {
                    auto ctx = reinterpret_cast<AppContext *>(context());
                    postEvent(ctx->threadGame, EventUser, UserClick, ButtonId::ButtonIdPlayer1);
                }
            }
        }
        else if (pin == _buttonPlayer2.getPin())
        {
            if (value == _buttonPlayer2.getActiveState())
            {
                _buttonPlayer2.edgeFallingTime = time;
            }
            else
            {
                auto edgeFallingTime = _buttonPlayer2.edgeFallingTime;
                auto delta = (time > edgeFallingTime) ? (time - edgeFallingTime) : (edgeFallingTime - time);
                if (delta > MIN_DEBOUNCE_TIME)
                {
                    auto ctx = reinterpret_cast<AppContext *>(context());
                    postEvent(ctx->threadGame, EventUser, UserClick, ButtonId::ButtonIdPlayer2);
                }
            }
        }
        else
        {
            LOG_TRACE("unsupported button: GPIO", pin);
        }
    }

    __EVENT_FUNC_DEFINITION(QueueMain, EventSystem, msg) // void QueueMain::handlerEventSystem(const Message &msg)
    {
        enum SystemTriggerSource src = static_cast<SystemTriggerSource>(msg.iParam);
        switch (src)
        {
        case SysSoftwareTimer:
            handlerSoftwareTimer((TimerHandle_t)(msg.lParam));
            break;
        case SysButtonClick:
        {
            handlerButtonClick(msg);
            break;
        }
        case SysButtonDoubleClick:
        {
            handlerButtonDoubleClick(msg);
            break;
        }
        case SysButtonLongPress:
        {
            handlerButtonLongPress(msg);
            break;
        }
        default:
            LOG_TRACE("unsupported SystemTriggerSource=", src);
            break;
        }
    }

    // define EventNull handler
    __EVENT_FUNC_DEFINITION(QueueMain, EventNull, msg) // void QueueMain::handlerEventNull(const Message &msg)
    {
        LOG_TRACE("EventNull(", msg.event, "), iParam=", msg.iParam, ", uParam=", msg.uParam, ", lParam=", msg.lParam);
    }
    /////////////////////////////////////////////////////////////////////////////

    void QueueMain::handlerSoftwareTimer(TimerHandle_t xTimer)
    {
        if (xTimer == _debounceTimer.timer())
        {
            _debounceTimer.onEventTimer();
        }
        else
        {
            LOG_TRACE("unsupported timer handle=0x%04x", (uint32_t)(xTimer));
        }
    }

    void QueueMain::debounce(uint32_t start, uint32_t ms)
    {
        // simple debounce
        uint32_t currentMs = millis();
        uint32_t delta = (start < currentMs) ? currentMs - start : start - currentMs;
        if (delta < DEBOUNCE_TIME)
        {
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_TIME - delta));
        }
    }

    void QueueMain::handlerButtonClick(const Message &msg)
    {
        int16_t pin = msg.uParam;
        if (pin == _buttonBoot.getPin())
        {
            LOG_TRACE("ButtonClick: buttonBoot");

            auto ctx = reinterpret_cast<AppContext *>(context());
            postEvent(ctx->threadGame, EventUser, UserClick, ButtonId::ButtonIdGame);
        }
        else
        {
            LOG_TRACE("SysButtonClick: unsupported pin=", pin);
        }
    }
    void QueueMain::handlerButtonDoubleClick(const Message &msg)
    {
        int16_t pin = msg.uParam;
        if (pin == _buttonBoot.getPin())
        {
            LOG_TRACE("SysButtonDoubleClick: buttonBoot");
            auto ctx = reinterpret_cast<AppContext *>(context());
            postEvent(ctx->threadGame, EventUser, UserDoubleClick, ButtonIdGame);
        }
        else
        {
            LOG_TRACE("SysButtonDoubleClick: unsupported pin=", pin);
        }
    }
    void QueueMain::handlerButtonLongPress(const Message &msg)
    {
        int16_t pin = msg.uParam;
        if (pin == _buttonBoot.getPin())
        {
            LOG_TRACE("SysButtonLongPress: buttonBoot");
            auto ctx = reinterpret_cast<AppContext *>(context());
            postEvent(ctx->threadGame, EventUser, UserLongPress, ButtonIdGame);
        }
        else
        {
            LOG_TRACE("SysButtonLongPress: unsupported pin=", pin);
        }
    }

    /////////////////////////////////////////////////////////////////////////////

} // namespace freertos

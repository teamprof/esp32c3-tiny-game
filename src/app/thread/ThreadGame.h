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
#include <map>
#include "../ArduProfFreeRTOS.h"
#include "../AppEvent.h"
#include "../game/GameData.h"
#include "../game/PlayerData.h"
#include "../peripheral/RoundLed.h"

namespace freertos
{
    class ThreadGame : public ardufreertos::ThreadBase
    {
    public:
        ThreadGame();
        virtual void start(void *);

    protected:
        typedef void (ThreadGame::*handlerFunc)(const Message &);
        std::map<int16_t, handlerFunc> handlerMap;
        virtual void onMessage(const Message &msg);

        virtual void run(void);

    private:
        static ThreadGame *_instance;

        TaskHandle_t _taskInitHandle;

        ardufreertos::PeriodicTimer _timer1Hz;
        ardufreertos::PeriodicTimer _timerEngine;

        GameData _gameData;
        PlayerData _playersData[GamePlayer::NumPlayer];
        RoundLed _rLed;

        virtual void setup(void);
        virtual void delayInit(void);

        void handlerSoftwareTimer(TimerHandle_t xTimer);
        void handlerUserClick(ButtonId id);
        void handlerUserDoubleClick(ButtonId id);
        void handlerUserLongPress(ButtonId id);
        void updateState(void);
        void updateUi(void);

        void uiStateStart(GameData &gameData, PlayerData playersData[]);
        void uiStatePause(PlayerData playersData[]);
        void uiStateStop(GameData &gameData);
        void uiStateUnknown(void);

        void startGame(void);
        void stopGame(void);
        void pauseGame(void);
        void resumeGame(void);

        void resetPlayersData(void);
        void advancePlayerPosition(PlayerData &player);

        ///////////////////////////////////////////////////////////////////////
        // declare event handler
        ///////////////////////////////////////////////////////////////////////
        __EVENT_FUNC_DECLARATION(EventUser)
        __EVENT_FUNC_DECLARATION(EventSystem)
        __EVENT_FUNC_DECLARATION(EventNull) // void handlerEventNull(const Message &msg);
    };
} // namespace freertos
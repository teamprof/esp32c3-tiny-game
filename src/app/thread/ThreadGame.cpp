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
#include "./ThreadGame.h"
#include "../AppContext.h"
#include "../peripheral/RoundLed.h"

// ////////////////////////////////////////////////////////////////////////////////////////////
#define CLICKS_PER_STEP 1 // numer of clicks to advance 1 step

////////////////////////////////////////////////////////////////////////////////////////////
// Thread
////////////////////////////////////////////////////////////////////////////////////////////
// #define RUNNING_CORE 0 // dedicate core 0 for Thread
// #define RUNNING_CORE 1 // dedicate core 1 for Thread
#define RUNNING_CORE ARDUINO_RUNNING_CORE

#define TASK_NAME "ThreadGame"
#define TASK_STACK_SIZE 4096
#define TASK_PRIORITY 3
#define TASK_QUEUE_SIZE 128 // message queue size for app task

////////////////////////////////////////////////////////////////////////////////////////////
namespace freertos
{

    ////////////////////////////////////////////////////////////////////////////////////////////
    ThreadGame *ThreadGame::_instance = nullptr;

    static uint8_t ucQueueStorageArea[TASK_QUEUE_SIZE * sizeof(Message)];
    static StaticQueue_t xStaticQueue;

    static StackType_t xStack[TASK_STACK_SIZE];
    static StaticTask_t xTaskBuffer;
    ////////////////////////////////////////////////////////////////////////////////////////////

    ThreadGame::ThreadGame() : ThreadBase(TASK_QUEUE_SIZE, ucQueueStorageArea, &xStaticQueue),
                               _timer1Hz("Timer 1Hz",
                                         pdMS_TO_TICKS(1000),
                                         [](TimerHandle_t xTimer)
                                         {
                                             if (_instance)
                                             {
                                                 auto context = reinterpret_cast<AppContext *>(_instance->context());
                                                 if (context && context->threadGame)
                                                 {
                                                     static_cast<freertos::ThreadGame *>(context->threadGame)->postEvent(EventSystem, SysSoftwareTimer, 0, (uint32_t)xTimer);
                                                 }
                                             }
                                         }),
                               _timerEngine("Timer GameEngine",
                                            pdMS_TO_TICKS(125),
                                            [](TimerHandle_t xTimer)
                                            {
                                                if (_instance)
                                                {
                                                    auto context = reinterpret_cast<AppContext *>(_instance->context());
                                                    if (context && context->threadGame)
                                                    {
                                                        static_cast<freertos::ThreadGame *>(context->threadGame)->postEvent(EventSystem, SysSoftwareTimer, 0, (uint32_t)xTimer);
                                                    }
                                                }
                                            }),
                               _gameData(GameState::Stop, GamePlayer::PlayerNull, TimeSlotState::SlotGame),
                               _playersData{0},
                               _rLed(),
                               handlerMap()
    {
        _instance = this;

        handlerMap = {
            __EVENT_MAP(ThreadGame, EventUser),
            __EVENT_MAP(ThreadGame, EventSystem),
            __EVENT_MAP(ThreadGame, EventNull), // {EventNull, &ThreadGame::handlerEventNull},
        };
    }

    __EVENT_FUNC_DEFINITION(ThreadGame, EventUser, msg) // void ThreadGame::handlerEventUser(const Message &msg)
    {
        UserTriggerSource src = (UserTriggerSource)(msg.iParam);
        ButtonId id = (ButtonId)(msg.uParam);
        switch (src)
        {
        case UserClick:
            LOG_TRACE("UserClick: id=", id);
            handlerUserClick(id);
            break;
        case UserDoubleClick:
            handlerUserDoubleClick(id);
            break;
        case UserLongPress:
            handlerUserLongPress(id);
            break;
        default:
            LOG_TRACE("unsupported UserTriggerSource=", (uint16_t)(src));
            break;
        }
    }
    __EVENT_FUNC_DEFINITION(ThreadGame, EventSystem, msg) // void ThreadGame::handlerEventSystem(const Message &msg)
    {
        enum SystemTriggerSource src = static_cast<SystemTriggerSource>(msg.iParam);
        switch (src)
        {
        case SysSoftwareTimer:
            handlerSoftwareTimer((TimerHandle_t)(msg.lParam));
            break;
        default:
            LOG_TRACE("unsupported SystemTriggerSource=", src);
            break;
        }
    }
    __EVENT_FUNC_DEFINITION(ThreadGame, EventNull, msg) // void ThreadGame::handlerEventNull(const Message &msg)
    {
        LOG_DEBUG("EventNull(", msg.event, "), iParam = ", msg.iParam, ", uParam = ", msg.uParam, ", lParam = ", msg.lParam);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    void ThreadGame::onMessage(const Message &msg)
    {
        auto func = handlerMap[msg.event];
        if (func)
        {
            (this->*func)(msg);
        }
        else
        {
            LOG_DEBUG("Unsupported event = ", msg.event, ", iParam = ", msg.iParam, ", uParam = ", msg.uParam, ", lParam = ", msg.lParam);
        }
    }

    void ThreadGame::start(void *ctx)
    {
        LOG_TRACE("start() on core ", xPortGetCoreID(), ", xPortGetFreeHeapSize()=", xPortGetFreeHeapSize());
        ThreadBase::start(ctx);

        _taskHandle = xTaskCreateStaticPinnedToCore(
            [](void *instance)
            { static_cast<ThreadGame *>(instance)->run(); },
            TASK_NAME,
            TASK_STACK_SIZE, // This stack size can be checked & adjusted by reading the Stack Highwater
            this,
            TASK_PRIORITY, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
            xStack,
            &xTaskBuffer,
            RUNNING_CORE);
    }

    void ThreadGame::setup(void)
    {
        LOG_TRACE("on core ", xPortGetCoreID(), ", uxTaskPriorityGet()=", uxTaskPriorityGet(xTaskGetCurrentTaskHandle()), ", xPortGetFreeHeapSize()=", xPortGetFreeHeapSize());
        ThreadBase::setup();

        _rLed.init();
        _timerEngine.start();
    }

    void ThreadGame::run(void)
    {
        LOG_TRACE("run() on core ", xPortGetCoreID(), ", xPortGetFreeHeapSize()=", xPortGetFreeHeapSize());
        ThreadBase::run();
    }

    void ThreadGame::delayInit(void)
    {
        LOG_TRACE("delayInit() on core ", xPortGetCoreID(), ", xPortGetFreeHeapSize()=", xPortGetFreeHeapSize());

        //////////////////////////////////////////////////////////////
        // add time consuming init code here
        //////////////////////////////////////////////////////////////
    }

    void ThreadGame::handlerSoftwareTimer(TimerHandle_t xTimer)
    {
        if (xTimer == _timer1Hz.timer())
        {
            LOG_TRACE("_timer1Hz");
        }
        else if (xTimer == _timerEngine.timer())
        {
            updateState();
            updateUi();
        }
        else
        {
            LOG_WARN("unsupported timer handle=0x%04x", DebugLogBase::HEX, (uint32_t)(xTimer));
        }
    }

    void ThreadGame::handlerUserClick(ButtonId id)
    {
        GameData &gameData = _gameData;
        PlayerData *playersData = _playersData;

        switch (id)
        {
        case ButtonId::ButtonIdGame:
            if (gameData.state == GameState::Start)
            {
                pauseGame();
            }
            else if (gameData.state == GameState::Stop)
            {
                startGame();
            }
            else if (gameData.state == GameState::Pause)
            {
                resumeGame();
            }
            break;

        case ButtonId::ButtonIdPlayer1:
            if (gameData.state == GameState::Start)
            {
                advancePlayerPosition(playersData[GamePlayer::Player1]);
            }
            break;

        case ButtonId::ButtonIdPlayer2:
            if (gameData.state == GameState::Start)
            {
                advancePlayerPosition(playersData[GamePlayer::Player2]);
            }
            break;

        default:
            LOG_WARN("unsupported ButtonId=%d", id);
            break;
        }
    }
    void ThreadGame::handlerUserDoubleClick(ButtonId id)
    {
        LOG_TRACE("ButtonId=", id);
    }
    void ThreadGame::handlerUserLongPress(ButtonId id)
    {
        GameData &gameData = _gameData;

        LOG_TRACE("gameData.state=%d", gameData.state, ", ButtonId=", id);
        switch (id)
        {
        case ButtonId::ButtonIdGame:
            if (gameData.state == GameState::Start ||
                gameData.state == GameState::Pause)
            {
                stopGame();
            }
            break;

        default:
            LOG_WARN("unsupported ButtonId=%d", id);
            break;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    void ThreadGame::updateState(void)
    {
        GameData &gameData = _gameData;
        PlayerData *playersData = _playersData;
        switch (gameData.state)
        {
        case GameState::Start:
        {
            PlayerData &player1 = playersData[GamePlayer::Player1];
            PlayerData &player2 = playersData[GamePlayer::Player2];
            if (player1.position == player2.position)
            {
                if (player1.countLoop > player2.countLoop)
                {
                    gameData.winner = GamePlayer::Player1;
                    gameData.state = GameState::Stop;
                    LOG_TRACE("Player1 win");
                }
                else if (player1.countLoop < player2.countLoop)
                {
                    gameData.winner = GamePlayer::Player2;
                    gameData.state = GameState::Stop;
                    LOG_TRACE("Player2 win");
                }
            }
            break;
        }
        }
    }

    void ThreadGame::advancePlayerPosition(PlayerData &player)
    {
        if (++player.countClick >= CLICKS_PER_STEP)
        {
            uint16_t totalLeds = _rLed.getTotalLeds();
            player.countClick = 0;
            if (player.position < (totalLeds - 1))
            {
                player.position++;
            }
            else
            {
                player.position = 0;
                player.countLoop++;
            }
        }
    }

    // +------------------+------------+------------------------------+
    // | LED color        | state      | status                       |
    // +------------------+------------+------------------------------+
    // | red on (all)     | game stop  | no winner                    |
    // | red blink (all)  | game pause | no winner                    |
    // | green on (all)   | game stop  | player1 win                  |
    // | green on         | game pause | player1 position             |
    // | blue on (all)    | game stop  | player2 win                  |
    // | blue on          | game pause | player2 position             |
    // | green/blue blink | game start | player1 and player2 position |
    // +------------------+------------+------------------------------+
    void ThreadGame::updateUi(void)
    {
        static bool toggle = false;

        GameData &gameData = _gameData;
        PlayerData *playersData = _playersData;

        int slot = (int)gameData.timeSlotState;
        slot = slot < (int)(SlotMaxValue - 1) ? slot + 1 : 0;
        gameData.timeSlotState = (TimeSlotState)(slot);

        switch (gameData.state)
        {
        case GameState::Start:
            uiStateStart(gameData, playersData);
            break;
        case GameState::Stop:
            uiStateStop(gameData);
            break;
        case GameState::Pause:
            uiStatePause(playersData);
            break;
        default:
            LOG_WARN("unsupported gameData.state=", (int32_t)(gameData.state));
            uiStateUnknown();
            break;
        }
    }

    // blink leds according to player position
    void ThreadGame::uiStateStart(GameData &gameData, PlayerData playersData[])
    {
        uint16_t player1Position = playersData[GamePlayer::Player1].position;
        uint16_t player2Position = playersData[GamePlayer::Player2].position;

        _rLed.clearBuf();
        _rLed.setPlayer1LedBuf(player1Position, gameData.timeSlotState == TimeSlotState::SlotPlayer1);
        _rLed.setPlayer2LedBuf(player2Position, gameData.timeSlotState == TimeSlotState::SlotPlayer2);
        _rLed.uiShow();
    }

    // show leds according to player position
    void ThreadGame::uiStatePause(PlayerData playersData[])
    {
        uint16_t player1Position = playersData[GamePlayer::Player1].position;
        uint16_t player2Position = playersData[GamePlayer::Player2].position;

        _rLed.clearBuf();
        _rLed.setPlayer1LedBuf(player1Position, true);
        _rLed.setPlayer2LedBuf(player2Position, true);
        _rLed.uiShow();
    }

    void ThreadGame::uiStateStop(GameData &gameData)
    {
        switch (gameData.winner)
        {
        case GamePlayer::PlayerNull:
        {
            _rLed.setGameLed(true);
            break;
        }
        case GamePlayer::Player1:
        {
            _rLed.uiGamePlayer1Win();
            break;
        }
        case GamePlayer::Player2:
        {
            _rLed.uiGamePlayer2Win();
            break;
        }
        default:
            LOG_WARN("unsupported gameData.winner=", gameData.winner);
            uiStateUnknown();
            break;
        }
    }
    void ThreadGame::uiStateUnknown(void)
    {
        _rLed.uiClear();
    }

    void ThreadGame::startGame(void)
    {
        resetPlayersData();

        GameData &gameData = _gameData;
        gameData.state = GameState::Start;
        gameData.winner = GamePlayer::PlayerNull;
        gameData.timeSlotState = TimeSlotState::SlotGame;
    }
    void ThreadGame::stopGame(void)
    {
        GameData &gameData = _gameData;
        gameData.state = GameState::Stop;
    }
    void ThreadGame::pauseGame(void)
    {
        GameData &gameData = _gameData;
        gameData.state = GameState::Pause;
    }
    void ThreadGame::resumeGame(void)
    {
        GameData &gameData = _gameData;
        gameData.state = GameState::Start;
    }

    void ThreadGame::resetPlayersData(void)
    {
        PlayerData *playersData = _playersData;

        PlayerData &player1 = playersData[GamePlayer::Player1];
        player1.countClick = 0;
        player1.countLoop = 0;
        player1.position = 0;

        PlayerData &player2 = playersData[GamePlayer::Player2];
        player2.countClick = 0;
        player2.countLoop = 0;
        player2.position = 0;
    }

} // namespace freertos
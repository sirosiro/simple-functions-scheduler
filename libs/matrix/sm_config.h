#ifndef SM_CONFIG_H
#define SM_CONFIG_H

/*
 * state_machine - Configuration
 *
 * This file is for user-specific configuration of the state machine.
 * Define your modes, states, and events here.
 */

/* 1. モード(Mode)の定義 */
typedef enum {
    MODE_NORMAL,
    MODE_DIAGNOSTIC,
    /* ... add more modes as needed */
    MAX_MODES
} App_Modes;

/* 2. 状態(State)の定義 */
typedef enum {
    STATE_IDLE,
    STATE_ACTIVE,
    STATE_SUSPENDED,
    /* ... add more states as needed */
    MAX_STATES
} App_States;

/* 3. イベント(Event)の定義 */
typedef enum {
    EVENT_START,
    EVENT_STOP,
    EVENT_PAUSE,
    EVENT_RESUME,
    /* ... add more events as needed */
    MAX_EVENTS
} App_Events;

#endif

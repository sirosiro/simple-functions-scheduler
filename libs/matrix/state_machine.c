#include <stdint.h>
#include "state_machine.h"

/* NULL定義 (C89準拠のため) */
#ifndef NULL
#define NULL ((void *)0)
#endif

void SM_init(
    SM_Handle_t *handle,
    uint8_t initial_mode,
    uint8_t initial_state,
    uint8_t max_states,
    uint8_t max_events,
    const SM_Transition_t *transition_table,
    SM_Log_Func_t log_func,
    void *context)
{
    if (handle == NULL || transition_table == NULL) {
        return;
    }
    
    handle->current_mode = initial_mode;
    handle->current_state = initial_state;
    handle->max_states = max_states;
    handle->max_events = max_events;
    handle->transition_table = transition_table;
    handle->log_func = log_func;
    handle->context = context;
}

void SM_dispatch(SM_Handle_t *handle, uint8_t event, void *event_data)
{
    if (handle == NULL || event >= handle->max_events) {
        return; /* 無効なイベントまたはハンドル */
    }

    /* 現在のモードと状態を取得 */
    uint8_t mode = handle->current_mode;
    uint8_t state = handle->current_state;

    /* 3次元マトリクスから遷移情報を取得するためのインデックスを計算 */
    unsigned long index = (mode * handle->max_states * handle->max_events) + 
                          (state * handle->max_events) + 
                          event;

    const SM_Transition_t *transition = &handle->transition_table[index];

    /* 次の状態を保存 */
    uint8_t next_state = transition->next_state;

    /* ログ関数を呼び出す */
    if (handle->log_func != NULL) {
        handle->log_func(mode, state, event, next_state);
    }

    /* アクション関数を実行 */
    if (transition->action != NULL) {
        transition->action(handle->context, event_data);
    }

    /* 状態を更新 */
    handle->current_state = next_state;
}

void SM_setMode(SM_Handle_t *handle, uint8_t new_mode)
{
    if (handle == NULL) {
        return;
    }
    handle->current_mode = new_mode;
}

uint8_t SM_getCurrentState(const SM_Handle_t *handle)
{
    if (handle == NULL) {
        /* エラーを示す値を返す (例: 0xFF)
         * 本来は `sm_config.h` などで定義すべき */
        return 0xFF; 
    }
    return handle->current_state;
}

uint8_t SM_getCurrentMode(const SM_Handle_t *handle)
{
    if (handle == NULL) {
        return 0xFF;
    }
    return handle->current_mode;
}
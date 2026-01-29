#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdint.h> /* For uint8_t */

/*
 * Forward declaration of the handle struct to allow pointer usage in typedefs.
 */
struct SM_Handle_s;

/**
 * @brief アクション関数のポインタ型
 * @param context ユーザー定義のコンテキストデータへのポインタ
 * @param event_data イベントと共に渡されるデータへのポインタ
 */
typedef void (*SM_Action_t)(void *context, void *event_data);

/**
 * @brief 遷移テーブルの1要素を定義する構造体
 */
typedef struct {
    uint8_t next_state;
    SM_Action_t action;
} SM_Transition_t;

/**
 * @brief ログ記録用のコールバック関数型
 * @param mode 現在のモード
 * @param old_state 遷移前の状態
 * @param event 発生したイベント
 * @param new_state 遷移後の状態
 */
typedef void (*SM_Log_Func_t)(uint8_t mode, uint8_t old_state, uint8_t event, uint8_t new_state);

/**
 * @brief ステートマシン・ハンドル
 *        ステートマシンのインスタンスを管理します。
 */
typedef struct SM_Handle_s {
    uint8_t current_mode;
    uint8_t current_state;
    const SM_Transition_t *transition_table;
    SM_Log_Func_t log_func;
    void *context;
    
    /* マトリクスの次元情報 */
    uint8_t max_states;
    uint8_t max_events;

} SM_Handle_t;


/**
 * @brief ステートマシンを初期化する
 *
 * @param handle 初期化するステートマシンハンドルへのポインタ
 * @param initial_mode 初期モード
 * @param initial_state 初期状態
 * @param max_states 状態の最大数 (マトリクス次元)
 * @param max_events イベントの最大数 (マトリクス次元)
 * @param transition_table 3次元遷移テーブルへのポインタ
 * @param log_func ログ記録用のコールバック関数 (NULL許容)
 * @param context アクション関数に渡されるユーザーデータ (NULL許容)
 */
void SM_init(
    SM_Handle_t *handle,
    uint8_t initial_mode,
    uint8_t initial_state,
    uint8_t max_states,
    uint8_t max_events,
    const SM_Transition_t *transition_table,
    SM_Log_Func_t log_func,
    void *context
);

/**
 * @brief イベントを投入し、状態遷移を実行する
 *
 * @param handle ステートマシンハンドルへのポインタ
 * @param event 発生したイベント
 * @param event_data アクション関数に渡すイベントデータ (NULL許容)
 */
void SM_dispatch(SM_Handle_t *handle, uint8_t event, void *event_data);

/**
 * @brief ステートマシンのモードを外部から変更する
 *
 * @param handle ステートマシンハンドルへのポインタ
 * @param new_mode 新しいモード
 */
void SM_setMode(SM_Handle_t *handle, uint8_t new_mode);

/**
 * @brief 現在の状態を取得する
 *
 * @param handle ステートマシンハンドルへのポインタ
 * @return 現在の状態
 */
uint8_t SM_getCurrentState(const SM_Handle_t *handle);

/**
 * @brief 現在のモードを取得する
 *
 * @param handle ステートマシンハンドルへのポインタ
 * @return 現在のモード
 */
uint8_t SM_getCurrentMode(const SM_Handle_t *handle);

#endif /* STATE_MACHINE_H */

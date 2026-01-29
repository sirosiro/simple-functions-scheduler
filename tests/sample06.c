#include <stdio.h>
#include <string.h> /* For strcmp in test, though not used in final version */
#include <stdint.h> /* For uint8_t */

#include "../libs/matrix/state_machine.h"
#include "../libs/matrix/sm_config.h"

/* ============================================================================
 * テスト用アクション関数
 * ============================================================================ */
void action_entry_state_idle(void *context, void *event_data) {
    (void)context; /* 未使用引数 */
    printf("  [Action] Entering IDLE state.\n");
    if (event_data != NULL) {
        printf("    Event data: %s\n", (char*)event_data);
    }
}

void action_process_event_start(void *context, void *event_data) {
    (void)context;
    printf("  [Action] Processing START event.\n");
    if (event_data != NULL) {
        printf("    Event data: %s\n", (char*)event_data);
    }
}

void action_process_event_stop(void *context, void *event_data) {
    (void)context;
    printf("  [Action] Processing STOP event.\n");
    if (event_data != NULL) {
        printf("    Event data: %s\n", (char*)event_data);
    }
}

void action_mode_change_to_diagnostic(void *context, void *event_data) {
    (void)context;
    (void)event_data;
    printf("  [Action] Mode changed to DIAGNOSTIC. Performing diagnostic checks.\n");
}

void action_unhandled_event(void *context, void *event_data) {
    (void)context;
    (void)event_data;
    printf("  [Action] Unhandled event in current state/mode.\n");
}


/* ============================================================================
 * テスト用ログ関数
 * ============================================================================ */
void test_log_func(uint8_t mode, uint8_t old_state, uint8_t event, uint8_t new_state) {
    printf("[LOG] Mode %u, OldState %u, Event %u -> NewState %u\n", mode, old_state, event, new_state);
}

/* ============================================================================
 * 状態遷移テーブルの定義
 * ============================================================================ *//* SM_Transition_t transition_table[MAX_MODES][MAX_STATES][MAX_EVENTS]
 * 可読性のため、各モード・状態ごとにテーブルを分けて定義し、後で結合する
 * 実際には1次元配列として定義し、インデックス計算でアクセスする */

/* MODE_NORMAL の遷移定義 */
const SM_Transition_t normal_mode_transitions[MAX_STATES][MAX_EVENTS] = {
    /* STATE_IDLE */
    {
        /* EVENT_START */
        {STATE_ACTIVE, action_process_event_start},
        /* EVENT_STOP */
        {STATE_IDLE, action_unhandled_event},
        /* EVENT_PAUSE */
        {STATE_IDLE, action_unhandled_event},
        /* EVENT_RESUME */
        {STATE_IDLE, action_unhandled_event}
    },
    /* STATE_ACTIVE */
    {
        /* EVENT_START */
        {STATE_ACTIVE, action_unhandled_event},
        /* EVENT_STOP */
        {STATE_IDLE, action_process_event_stop},
        /* EVENT_PAUSE */
        {STATE_SUSPENDED, NULL}, /* アクションなし */
        /* EVENT_RESUME */
        {STATE_ACTIVE, action_unhandled_event}
    },
    /* STATE_SUSPENDED */
    {
        /* EVENT_START */
        {STATE_SUSPENDED, action_unhandled_event},
        /* EVENT_STOP */
        {STATE_IDLE, action_process_event_stop},
        /* EVENT_PAUSE */
        {STATE_SUSPENDED, action_unhandled_event},
        /* EVENT_RESUME */
        {STATE_ACTIVE, NULL} /* アクションなし */
    }
};

/* MODE_DIAGNOSTIC の遷移定義 */
const SM_Transition_t diagnostic_mode_transitions[MAX_STATES][MAX_EVENTS] = {
    /* STATE_IDLE */
    {
        /* EVENT_START (診断モードでは何もせずIDLEのまま) */
        {STATE_IDLE, action_unhandled_event},
        /* EVENT_STOP (診断モードを終了してNORMALモードへ) */
        {STATE_IDLE, action_mode_change_to_diagnostic}, /* 例: アクションでモードを戻す、または次のイベントで戻す */
        /* EVENT_PAUSE */
        {STATE_IDLE, action_unhandled_event},
        /* EVENT_RESUME */
        {STATE_IDLE, action_unhandled_event}
    },
    /* STATE_ACTIVE (診断モードではACTIVEにはならない想定) */
    {
        {STATE_IDLE, action_unhandled_event}, /* 全イベントでIDLEに戻る */
        {STATE_IDLE, action_unhandled_event},
        {STATE_IDLE, action_unhandled_event},
        {STATE_IDLE, action_unhandled_event}
    },
    /* STATE_SUSPENDED (診断モードではSUSPENDEDにはならない想定) */
    {
        {STATE_IDLE, action_unhandled_event}, /* 全イベントでIDLEに戻る */
        {STATE_IDLE, action_unhandled_event},
        {STATE_IDLE, action_unhandled_event},
        {STATE_IDLE, action_unhandled_event}
    }
};

/* 全体の遷移テーブル (1次元配列として定義) */
const SM_Transition_t full_transition_table[MAX_MODES * MAX_STATES * MAX_EVENTS];

/* 結合関数 (ビルド時に自動生成スクリプトなどで利用するイメージだが、ここでは手動でコピー)
 * C89で可変長配列の初期化はできないため、手動で要素を列挙するか、
 * マクロで巨大な静的配列を生成する、またはコンパイル時生成スクリプトを使うことを前提とする。
 * ここではテスト用に、初期化時にポインタを渡す形を採用し、テーブルは静的に定義
 * SM_initに渡すtransition_tableは、mode * MAX_STATES * MAX_EVENTS のオフセットを計算して渡す
 * または、このテーブルをユーザーが直接定義できるようにする (sm_config.hの隣にsm_transitions.cのようなファイルで) */

/* 上記のように直接定義するのではなく、SM_initに渡すポインタで制御する
 * このテストでは、MAX_MODES * MAX_STATES * MAX_EVENTS のサイズでテーブルを定義し、
 * 各モードのテーブルをそこに配置する。 */
const SM_Transition_t consolidated_transition_table[MAX_MODES * MAX_STATES * MAX_EVENTS] = {
    /* MODE_NORMAL の部分 */
    #define MODE_OFFSET_NORMAL (0 * MAX_STATES * MAX_EVENTS)
    #define NORMAL_TABLE_SIZE (MAX_STATES * MAX_EVENTS)
    #if MODE_OFFSET_NORMAL + NORMAL_TABLE_SIZE > (MAX_MODES * MAX_STATES * MAX_EVENTS)
        #error "Consolidated table size overflow for NORMAL mode"
    #endif
    /* MODE_NORMAL の遷移定義をコピー
     * C89では初期化子で配列をコピーできないため、ここは手動で要素を列挙するか、
     * 実行時にmemcpy等でコピーするが、今回は直接埋め込む形式とする
     * この方法は、テーブルが大きくなると非常に扱いにくいため、
     * 実際の運用では自動生成スクリプトの利用を強く推奨 */

    /* MODE_NORMAL (0) */
    /* STATE_IDLE (0) */
        /* EVENT_START (0) */
    {STATE_ACTIVE, action_process_event_start},
        /* EVENT_STOP (1) */
    {STATE_IDLE, action_unhandled_event},
        /* EVENT_PAUSE (2) */
    {STATE_IDLE, action_unhandled_event},
        /* EVENT_RESUME (3) */
    {STATE_IDLE, action_unhandled_event},

    /* STATE_ACTIVE (1) */
        /* EVENT_START (0) */
    {STATE_ACTIVE, action_unhandled_event},
        /* EVENT_STOP (1) */
    {STATE_IDLE, action_process_event_stop},
        /* EVENT_PAUSE (2) */
    {STATE_SUSPENDED, NULL},
        /* EVENT_RESUME (3) */
    {STATE_ACTIVE, action_unhandled_event},

    /* STATE_SUSPENDED (2) */
        /* EVENT_START (0) */
    {STATE_SUSPENDED, action_unhandled_event},
        /* EVENT_STOP (1) */
    {STATE_IDLE, action_process_event_stop},
        /* EVENT_PAUSE (2) */
    {STATE_SUSPENDED, action_unhandled_event},
        /* EVENT_RESUME (3) */
    {STATE_ACTIVE, NULL},
    
    /* MODE_DIAGNOSTIC (1) */
    /* STATE_IDLE (0) */
        /* EVENT_START (0) */
    {STATE_IDLE, action_unhandled_event},
        /* EVENT_STOP (1) */
    {STATE_IDLE, action_mode_change_to_diagnostic},
        /* EVENT_PAUSE (2) */
    {STATE_IDLE, action_unhandled_event},
        /* EVENT_RESUME (3) */
    {STATE_IDLE, action_unhandled_event},

    /* STATE_ACTIVE (1) */
        /* EVENT_START (0) */
    {STATE_IDLE, action_unhandled_event},
        /* EVENT_STOP (1) */
    {STATE_IDLE, action_unhandled_event},
        /* EVENT_PAUSE (2) */
    {STATE_IDLE, action_unhandled_event},
        /* EVENT_RESUME (3) */
    {STATE_IDLE, action_unhandled_event},

    /* STATE_SUSPENDED (2) */
        /* EVENT_START (0) */
    {STATE_IDLE, action_unhandled_event},
        /* EVENT_STOP (1) */
    {STATE_IDLE, action_unhandled_event},
        /* EVENT_PAUSE (2) */
    {STATE_IDLE, action_unhandled_event},
        /* EVENT_RESUME (3) */
    {STATE_IDLE, action_unhandled_event}
};


int main(void) {
    SM_Handle_t sm_handle;
    char my_context_data[] = "My SM Context";
    char event_payload_start[] = "Event_START_Payload";
    char event_payload_stop[] = "Event_STOP_Payload";

    printf("--- Running sample06.c: State Machine Library Test ---\n");

    /* ステートマシンの初期化 */
    printf("1. Initializing State Machine...\n");
    SM_init(
        &sm_handle,
        MODE_NORMAL,       /* 初期モード */
        STATE_IDLE,        /* 初期状態 */
        MAX_STATES,
        MAX_EVENTS,
        consolidated_transition_table, /* 統合された遷移テーブル */
        test_log_func,     /* ログ関数 */
        my_context_data    /* ユーザーコンテキスト */
    );

    printf("   Current Mode: %u, State: %u\n", SM_getCurrentMode(&sm_handle), SM_getCurrentState(&sm_handle));

/* ========================================================================
 * シナリオ1: 通常モードでの遷移
 * ======================================================================== */
    printf("\n2. Scenario 1: Transitions in NORMAL Mode\n");

    printf("   Dispatching EVENT_START from IDLE...\n");
    SM_dispatch(&sm_handle, EVENT_START, event_payload_start);
    printf("   Current Mode: %u, State: %u\n", SM_getCurrentMode(&sm_handle), SM_getCurrentState(&sm_handle));
    if (SM_getCurrentState(&sm_handle) != STATE_ACTIVE) {
        printf("   [ERROR] Expected state ACTIVE, got %u\n", SM_getCurrentState(&sm_handle));
        return 1;
    }

    printf("   Dispatching EVENT_PAUSE from ACTIVE...\n");
    SM_dispatch(&sm_handle, EVENT_PAUSE, NULL);
    printf("   Current Mode: %u, State: %u\n", SM_getCurrentMode(&sm_handle), SM_getCurrentState(&sm_handle));
    if (SM_getCurrentState(&sm_handle) != STATE_SUSPENDED) {
        printf("   [ERROR] Expected state SUSPENDED, got %u\n", SM_getCurrentState(&sm_handle));
        return 1;
    }

    printf("   Dispatching EVENT_RESUME from SUSPENDED...\n");
    SM_dispatch(&sm_handle, EVENT_RESUME, NULL);
    printf("   Current Mode: %u, State: %u\n", SM_getCurrentMode(&sm_handle), SM_getCurrentState(&sm_handle));
    if (SM_getCurrentState(&sm_handle) != STATE_ACTIVE) {
        printf("   [ERROR] Expected state ACTIVE, got %u\n", SM_getCurrentState(&sm_handle));
        return 1;
    }

    printf("   Dispatching EVENT_STOP from ACTIVE...\n");
    SM_dispatch(&sm_handle, EVENT_STOP, event_payload_stop);
    printf("   Current Mode: %u, State: %u\n", SM_getCurrentMode(&sm_handle), SM_getCurrentState(&sm_handle));
    if (SM_getCurrentState(&sm_handle) != STATE_IDLE) {
        printf("   [ERROR] Expected state IDLE, got %u\n", SM_getCurrentState(&sm_handle));
        return 1;
    }

/* ========================================================================
 * シナリオ2: モード変更と診断モードでの遷移
 * ======================================================================== */
    printf("\n3. Scenario 2: Mode Change and Transitions in DIAGNOSTIC Mode\n");

    printf("   Setting Mode to DIAGNOSTIC...\n");
    SM_setMode(&sm_handle, MODE_DIAGNOSTIC);
    printf("   Current Mode: %u, State: %u\n", SM_getCurrentMode(&sm_handle), SM_getCurrentState(&sm_handle));
    if (SM_getCurrentMode(&sm_handle) != MODE_DIAGNOSTIC) {
        printf("   [ERROR] Expected mode DIAGNOSTIC, got %u\n", SM_getCurrentMode(&sm_handle));
        return 1;
    }

    printf("   Dispatching EVENT_START from IDLE in DIAGNOSTIC Mode (should be unhandled)...\n");
    SM_dispatch(&sm_handle, EVENT_START, NULL);
    printf("   Current Mode: %u, State: %u\n", SM_getCurrentMode(&sm_handle), SM_getCurrentState(&sm_handle));
    if (SM_getCurrentState(&sm_handle) != STATE_IDLE) {
        printf("   [ERROR] Expected state IDLE, got %u\n", SM_getCurrentState(&sm_handle));
        return 1;
    }

    printf("   Dispatching EVENT_STOP from IDLE in DIAGNOSTIC Mode (should trigger mode change action)...\n");
    SM_dispatch(&sm_handle, EVENT_STOP, NULL);
    printf("   Current Mode: %u, State: %u\n", SM_getCurrentMode(&sm_handle), SM_getCurrentState(&sm_handle));
    /* NOTE: action_mode_change_to_diagnostic はモードを戻すアクションではない。
     * そのため、ここでは状態が変わらないことを確認する。 */
    if (SM_getCurrentState(&sm_handle) != STATE_IDLE) {
        printf("   [ERROR] Expected state IDLE, got %u\n", SM_getCurrentState(&sm_handle));
        return 1;
    }

    printf("   Setting Mode back to NORMAL...\n");
    SM_setMode(&sm_handle, MODE_NORMAL);
    printf("   Current Mode: %u, State: %u\n", SM_getCurrentMode(&sm_handle), SM_getCurrentState(&sm_handle));
    if (SM_getCurrentMode(&sm_handle) != MODE_NORMAL) {
        printf("   [ERROR] Expected mode NORMAL, got %u\n", SM_getCurrentMode(&sm_handle));
        return 1;
    }

    printf("\n--- sample06.c test finished successfully. ---\n");

    return 0;
}
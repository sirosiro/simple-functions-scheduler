# ARCHAEOLOGY_REPORT.md - Analysis of `simple-functions-scheduler`

This report details the findings from an archaeological analysis of the `simple-functions-scheduler` codebase. The goal is to reverse-engineer the implicit design philosophy and structure to facilitate future development and documentation.

---

## 0. ドメインの特定と外部規約の宣言

**目的:** このプロジェクトがソフトウェアの世界で「何者」であるかを定義し、それが準拠すべき外部のデファクトスタンダードを特定する。

**分析:**
ソースコード (`sfs.c`, `sfs.h`, `frcc.c`, `frcc.h`) および `Makefile` の解析から、このプロジェクトは以下のようなドメインに属すると推定されます。

- **ドメイン:** 軽量なタスクスケジューリングとリアルタイムタイマー機能を提供する、組み込みシステムまたはリソース制約のある環境向けのライブラリ。
- **特徴的なAPI:** `SFS_initialize`, `SFS_dispatch`, `SFS_fork`, `GetFreeRunCounter`, `FRCGapCheck`。
- **データ構造:** 固定長配列と単方向・双方向連結リストを基盤としたタスク管理 (`struct SFS_tg`) およびタイマー管理 (`struct FRC_tg`)。
- **メモリ管理:** 動的なメモリ割り当ては行われず、すべて静的またはスタック上で管理されています。
- **言語:** ANSI C (`-ansi` フラグが `Makefile` で指定されているため)。

**質問形式での提案:**
このコードベースは、固定されたリソースプールと協調的マルチタスク、およびハードウェア依存のフリーランカウンタを特徴としており、そのドメインは**『組み込みシステム向け軽量タスクスケジューラおよびリアルタイムタイマーライブラリ』**であると推定されます。

今後の開発と評価の基準として、このプロジェクトの基本原則として「**リソース制約の厳しい組み込み環境における信頼性と予測可能性を最優先する**」ことを採用しますか？もし採用されれば、多くの詳細な実装指示をこの原則で代替でき、全体の意図がより明確になります。

---

## 1. 推定される設計原則

コードベースから推定される主要な設計原則は以下の通りです。

*   **リソース効率性:** 静的メモリ割り当てのみを使用し、動的メモリ確保を避けることで、メモリフットプリントを最小限に抑え、予測可能なメモリ使用量を実現しています。これは、リソースが限られた組み込みシステムでは不可欠です。
*   **協調的マルチタスク:** `SFS_dispatch` ループ内でタスクが順次実行される形式であり、タスク自身が明示的に制御を解放するか、終了する必要があります。これは、カーネルのオーバーヘッドを削減し、特定のリアルタイム要件に適しています。
*   **シンプルさと直接性:** ポインタ操作によるリンクリスト管理や、カスタムの `strncpy` 実装など、低レベルの操作が直接的に行われています。抽象化レイヤーは最小限に抑えられています。
*   **時間測定の信頼性:** フリーランカウンタ (`frcc` モジュール) は、カウンタのロールオーバーと、割り込み不可区間を設けることで、原子的なアクセスと正確な時間差計算を保証しています。

**質問:** 「このコードベースは一貫して『リソース効率性』、『協調的マルチタスク』、『シンプルさと直接性』、『時間測定の信頼性』を主要な設計原則として利用しているように見受けられます。これらを本プロジェクトの主要な設計原則の一つと仮定しますか？」

---

## 2. 発見されたアーキテクチャ制約

*   **固定タスク数:** `sfs` モジュールは、`SFS` 配列のサイズ（`BODY` マクロ、現在 8）によって固定された数のタスクしか管理できません。これはシステム全体のタスク数に対する厳格な上限となります。
*   **シングルスレッド実行モデル:** `SFS_dispatch` が単一のスレッドで呼び出されることを前提としています。複数のOSスレッドから `SFS_dispatch` を呼び出すことは想定されていません。
*   **タスクデータのサイズ制限:** 各タスクに割り当てられる `work` バッファは `SFS_WORK_SIZE` (32バイト) に固定されており、タスクが保持できるローカルデータの量に制約があります。
*   **非リエントラントな文字列操作:** `sfs.c` 内のカスタム `strncpy`, `strcnt`, `strcmp` は、標準ライブラリ関数に依存せず、独自の動作をします。特に `strncpy` は標準と異なる挙動を持つ可能性があり、注意が必要です。`Makefile` で `-fno-builtin-strncpy` が指定されており、意図的にカスタム実装を使用していることが伺えます。

**質問:** 「`sfs` モジュールの固定タスク数（最大8）、シングルスレッド実行モデル、タスクごとの固定データサイズ制限、そしてカスタム文字列操作関数の使用は、プロジェクトのアーキテクチャ制約として認識し、維持すべきものと定義してよろしいですか？」

---

## 3. 発見された例外と矛盾

*   **カスタム `strncpy` の存在理由:** `sfs.c` にカスタムの `strncpy` が実装されており、`Makefile` で `-fno-builtin-strncpy` が指定されています。これは標準ライブラリの `strncpy` の挙動に何らかの問題があったか、あるいは特定の組み込み環境での最適化、あるいはより厳密な制御（例：NULL終端の保証など）を意図している可能性があります。現在のコードでは、カスタム `strncpy` は `s1 < s2` の場合の処理が複雑であり、標準ライブラリ版よりも安全性が高いとは断言できません。

**質問:** 「`sfs.c` に実装されているカスタム `strncpy` の必要性について、この設計が意図的なものであるか（特定のプラットフォームや要件によるものなど）、それとも見直すべき技術的負債であるかを明確にする必要があります。現在の実装を維持する意図はありますか？」

---

## 4. 利用されていないコード（Dead Code）の可能性

*   `frcc.c` の `gFreeRunCounterMini` と `GetFreeRunGapMini` は `frcc.h` で `extern` 宣言されていますが、現在のソースコードベース (`sfs.c`, `sample*.c`) では直接利用されている箇所が見当たりませんでした。これらは古い機能、特定のテスト用、あるいは将来の拡張のために残されている可能性があります。

**質問:** 「`gFreeRunCounterMini` および `GetFreeRunGapMini` (frcc.c) は、現在のプロジェクト内のどこからも呼び出されていません。これらは廃止された機能として削除を検討しますか？あるいは、外部や特定のビルド構成でのみ利用されるため、維持すべきですか？」

---

## 5. コメントと実装の乖離

現状の解析では、ソースコードのコメントと実際の実装内容に大きな乖離は見られませんでした。しかし、全ファイルの詳細なレビューはまだ行っていないため、このセクションは今後の深掘りによって更新される可能性があります。

---

## 6. 提案されるインテント・コメント (SFS モジュール)

`sfs` モジュールの解析結果に基づき、主要な関数に対するインテント・コメントの草案を提案します。

### `SFS_initialize`
```c
/**
 * @brief スケジューラを初期化し、利用可能なタスク制御ブロックを準備する。
 * @responsibility システム起動時に一度だけ呼び出され、タスク管理のための内部状態を初期化する。
 *                  固定数のタスク制御ブロックをフリーリストに登録し、実行待ちタスクリストを空にする。
 * @precondition なし。システム起動時に一度だけ呼び出されるべきである。
 * @postcondition スケジューラがタスク受け入れ可能な状態になる。
 */
short SFS_initialize(void);
```

### `SFS_dispatch`
```c
/**
 * @brief 現在実行可能なすべてのタスクを優先度順にディスパッチ（実行）する。
 * @responsibility スケジューラのメインループから定期的に呼び出され、協調的マルチタスクを実現する。
 *                  実行中のタスクは、自身の処理が完了するまでCPUを占有し、明示的に制御を返却する。
 * @precondition SFS_initializeが呼び出され、スケジューラが初期化されていること。
 * @postcondition 実行待ちタスクリスト内の全タスクが一度ずつ実行される。
 * @return 実行されたタスクの数。
 */
short SFS_dispatch(void);
```

### `SFS_fork`
```c
/**
 * @brief 新しいタスクを生成し、スケジューラに登録する。
 * @responsibility 指定された名前、優先度、エントリポイントを持つタスクをタスク制御ブロックに割り当て、
 *                  実行待ちタスクリストに優先度順で挿入する。
 * @param name タスクを識別するための一意な名前。最大SFS_NAME_SIZE-1文字。
 * @param order タスクの実行順序を決定する優先度（数値が小さいほど高優先度）。
 * @param func タスクのエントリポイントとなる関数ポインタ。
 * @precondition SFS_initializeが呼び出され、スケジューラが初期化されていること。
 *                 'name'はNULL終端されていること。
 * @postcondition 新しいタスクが実行待ちタスクリストに追加される。利用可能なタスク制御ブロックが減少する。
 * @return 0: 成功、-1: 利用可能なタスク制御ブロックがない、またはエラー。
 */
short SFS_fork(char *name,short order,void (*func)());
```

### `SFS_kill`
```c
/**
 * @brief 現在実行中のタスクを終了するようマークする。
 * @responsibility 現在実行中のタスクの関数ポインタを内部的な解放処理 (`SFS_giveup`) に置き換える。
 *                  実際のタスク制御ブロックの解放は、次のSFS_dispatchサイクルで処理される。
 * @precondition SFS_dispatchによってタスクが現在実行中であること。
 * @postcondition 現在実行中のタスクは、次回SFS_dispatchが呼び出された際に実行待ちリストから削除され、
 *                 タスク制御ブロックがフリーリストに返却される。
 * @return 常に0を返す。
 */
short SFS_kill(void);
```

---

## 7. コンポーネント設計仕様の具体化

解析したソースコードから、`ARCHITECTURE_MANIFEST.md` の「4. コンポーネント設計仕様」を、実装の詳細がイメージできるレベルまで具体的に記述し、提案します。

### 7.1. SFS (Simple Function Scheduler) モジュール

-   **責務 (Responsibility):**
    *   固定数のタスク制御ブロック (TCB) を管理し、タスクの生成、実行、終了、変更を協調的マルチタスクモデルで提供する。
    *   タスクを優先度 `order` に基づいて実行待ちリストに登録し、その順序でディスパッチする。
    *   タスクが共有可能なワークバッファを提供し、タスク間でデータを交換できるようにする。

-   **提供するAPI (Public API):**
    *   `short SFS_initialize(void)`:
        *   責務: スケジューラの内部状態とタスク制御ブロックのプールを初期化する。
        *   戻り値: `0` (成功)。
    *   `short SFS_dispatch(void)`:
        *   責務: 現在のアクティブタスクリストを順番に実行する。各タスクは自身が制御を返却するまで実行される。
        *   戻り値: `実行されたタスクの数`。
    *   `short SFS_fork(char *name, short order, void (*entry_point)(void))`:
        *   責務: 新しいタスクを生成し、フリーリストからTCBを取得して初期化し、`order` に基づいて実行待ちリストに挿入する。
        *   `name`: タスク名。関数内でコピーして使用するため、呼び出し元は自身のポインタ管理責任を持つ。
        *   `order`: タスクの優先度。数値が小さいほど高優先度。
        *   `entry_point`: タスクのメイン処理を行う関数へのポインタ。
        *   戻り値: `0` (成功), `-1` (タスク制御ブロックの割り当て失敗)。
    *   `void *SFS_work(void)`:
        *   責務: 現在実行中のタスクに割り当てられた汎用ワークバッファへのポインタを返す。
        *   戻り値: `void*` (現在のタスクの `work` バッファへのポインタ)。
    *   `void *SFS_otherWork(char *name)`:
        *   責務: 指定された名前のタスクに割り当てられた汎用ワークバッファへのポインタを返す。
        *   `name`: ワークバッファを取得したいタスクの名前。
        *   戻り値: `void*` (指定タスクの `work` バッファへのポインタ), `NULL` (タスクが見つからない場合)。
    *   `short SFS_kill(void)`:
        *   責務: 現在実行中のタスクを終了処理に移行させる。実際の削除は `SFS_dispatch` の次のサイクルで行われる。
        *   戻り値: `0` (成功)。
    *   `short SFS_change(char *name, short order, void (*entry_point)(void))`:
        *   責務: 現在実行中のタスクの名前、優先度、エントリポイントを変更する。
        *   `name`: 新しいタスク名。
        *   `order`: 新しい優先度。
        *   `entry_point`: 新しいタスクのエントリポイント。
        *   戻り値: `0` (成功)。

-   **主要なデータ構造 (Key Data Structures):**
    *   `struct SFS_tg`:
        ```c
        struct SFS_tg {
          char name[SFS_NAME_SIZE];      // タスク名 (固定長)
          unsigned short order;          // 実行優先度 (小さいほど高優先度)
          struct SFS_tg *pFront;       // 実行待ちリストの前のタスクへのポインタ (双方向リスト用)
          struct SFS_tg *pBack;        // 実行待ちリストの次のタスクへのポインタ (双方向リスト用)
          void (*pFunction)(void);       // タスクのエントリポイント関数ポインタ
          char work[SFS_WORK_SIZE];      // タスク固有の汎用ワークバッファ
        };
        ```
    *   `static struct SFS_tg SFS[BODY]`: 全タスク制御ブロックを保持する固定長配列。
    *   `static struct SFS_tg *pTask`: 実行待ちのアクティブなタスクリストのヘッドポインタ。`order` に基づいてソートされた双方向連結リスト。
    *   `static struct SFS_tg *pPool`: 利用可能なタスク制御ブロックのフリーリストのヘッドポインタ。単方向連結リスト。

-   **状態とライフサイクル (State and Lifecycle):**
    *   **TCBの状態:**
        *   `Pooled`: `pPool` リストに存在し、利用可能な状態。
        *   `Active`: `pTask` リストに存在し、実行待ちまたは実行中の状態。
        *   `Killed`: `SFS_kill` により `pFunction` が `SFS_giveup` に置き換えられた状態。`SFS_dispatch` で実行された後 `Pooled` に戻る。
    *   **スケジューラのライフサイクル:** `SFS_initialize` で初期化され、`SFS_dispatch` をループで呼び出すことでタスクが実行される。タスクは `SFS_fork` で追加され、`SFS_kill` で論理的に削除、`SFS_giveup` で物理的に削除される。

-   **重要なアルゴリズム (Key Algorithms):**
    *   **タスク登録 (`SFS_regist`):** タスクの `order` に基づくソート済み挿入。ヘッド、テール、中間への挿入を双方向リンクリストで処理。
    *   **タスク解放 (`SFS_giveup`):** 双方向リンクリストからの要素削除。ヘッド、テール、中間からの削除を処理し、フリーリスト (`pPool`) に戻す。

### 7.2. FRCC (Free Run Clock Counter) モジュール

-   **責務 (Responsibility):**
    *   ハードウェア（外部）によって周期的にインクリメントされるフリーランカウンタ（8ビット版と32ビット版）を提供する。
    *   カウンタのロールオーバーを考慮した正確な時間差計算機能を提供する。
    *   フリーランカウンタへの割り込み安全なアクセスを保証する。
    *   ワンショットまたは繰り返し可能な時間経過チェック機能を提供する。

-   **提供するAPI (Public API):**
    *   `unsigned char GetFreeRunGapMini(unsigned char vPastCount)`:
        *   責務: 8ビットフリーランカウンタ `gFreeRunCounterMini` と過去の値 `vPastCount` との間の時間差を計算する。カウンタのロールオーバーを考慮する。
        *   `vPastCount`: 過去のカウンタ値。
        *   戻り値: `unsigned char` (経過時間)。
    *   `void FRCInterrupt(void (*di)(void), void (*ei)(void))`:
        *   責務: フリーランカウンタへの原子的なアクセスを保証するために使用される、割り込み禁止/許可関数を登録する。
        *   `di`: 割り込み禁止関数へのポインタ。
        *   `ei`: 割り込み許可関数へのポインタ。
    *   `unsigned long GetFreeRunCounter(void)`:
        *   責務: 現在の32ビットフリーランカウンタ `gFreeRunCounter` の値を割り込み安全に取得する。
        *   戻り値: `unsigned long` (現在のカウンタ値)。
    *   `unsigned long GetFreeRunGap(unsigned long vFarstCount, unsigned long vSecondCount)`:
        *   責務: 2つの32ビットカウンタ値間の時間差を計算する。カウンタのロールオーバーを考慮する。
        *   `vFarstCount`: 開始カウンタ値。
        *   `vSecondCount`: 終了カウンタ値。
        *   戻り値: `unsigned long` (経過時間)。
    *   `void FRCGapCheckStart(FRC *vGapChk, unsigned long vStopGap)`:
        *   責務: 時間経過チェック用の `FRC` 構造体を初期化する。現在のカウンタ値を `StartPoint` として記録し、目標経過時間 `vStopGap` を設定する。
        *   `vGapChk`: 初期化する `FRC` 構造体へのポインタ。
        *   `vStopGap`: チェックする時間間隔。
    *   `unsigned long FRCGapCheck(FRC *vGapChk)`:
        *   責務: `FRCGapCheckStart` で設定された時間間隔が経過したかをチェックする。
        *   `vGapChk`: チェックする `FRC` 構造体へのポインタ。
        *   戻り値: `0` (時間経過済み), `それ以外の値` (残り時間)。
    *   `void FRCGapCheckStop(FRC *vGapChk)`:
        *   責務: `FRC` 構造体の時間経過チェックを停止し、無効化する。
        *   `vGapChk`: 停止する `FRC` 構造体へのポインタ。

-   **主要なデータ構造 (Key Data Structures):**
    *   `unsigned char gFreeRunCounterMini`: 8ビットフリーランカウンタ（グローバル変数）。
    *   `unsigned long gFreeRunCounter`: 32ビットフリーランカウンタ（グローバル変数）。
    *   `struct FRCGapChk_tg`:
        ```c
        typedef struct FRCGapChk_tg {
          char OneShot;          // 0: 経過済み, 1: 監視中, -1: 未使用/停止
          unsigned long StartPoint;   // 測定開始時のFreeRunCounter値
          unsigned long StopGap;      // 監視する時間間隔
        }FRC;
        ```
    *   `static void (*_di)(void)`: 割り込み禁止関数への内部ポインタ。
    *   `static void (*_ei)(void)`: 割り込み許可関数への内部ポインタ。

-   **状態とライフサイクル (State and Lifecycle):**
    *   `FRC` 構造体の `OneShot` メンバにより、タイマーの状態が管理される。
    *   `OneShot = 1`: 監視中。
    *   `OneShot = 0`: 時間経過済み（ワンショット）。
    *   `OneShot = -1`: 未使用または停止状態。

-   **重要なアルゴリズム (Key Algorithms):**
    *   **カウンタロールオーバー対応の時間差計算:**
        *   `if (iNowCount >= vPastCount) { return iNowCount - vPastCount; } else { return ((型)-1)-vPastCount+iNowCount; }`
        *   これにより、カウンタが最大値から0にロールオーバーした場合でも正確な時間差を計算できる。
    *   **原子的なカウンタ読み出し:** 登録された割り込み禁止/許可関数 (`_di`, `_ei`) を利用して、グローバルカウンタの読み出し中に割り込みが発生しないことを保証する。

---

I will now write this content to the file `ARCHAEOLOGY_REPORT.md`.
---
name: rnoh-stability-review
description: >-
  用于对 OpenHarmony React Native / RNOH / React Native Harmony 框架代码做稳定性专项检视。
  当用户提供代码片段、文件路径、PR diff，或想评估某段代码是否存在以下风险时使用：
  销毁后回调（UAF）、空指针解引用、noexcept 滥用导致 abort、锁顺序死锁、持锁回调、
  错误线程访问、监听器未注销、线程泄漏、HTTP 回调未清理、NAPI reference 未释放、
  ArkUINode 或 Surface 初始化时序错误、混淆崩溃、API12 兼容缺失、JS undefined 访问、
  弱引用缺失、定时器或 VSync 回调与对象生命周期不同步、资源释放未形成闭环、
  sort 比较器违反严格弱序导致崩溃、容器越界访问、状态机顺序错乱。
  也适用于：检查某改动是否会引入新稳定性问题、判断历史修复模式是否已正确应用、
  对照稳定性编码规范和华为官方通用稳定性规范做全面自查（逐条覆盖 RNOH 规范项、
  Node-API、libuv、ArkTS/NDK 引用规范、IPC/Ashmem、ArkUI/媒体/图形易错 API、
  NativeWindow/XComponent 生命周期、底层 C++ 内存与整数安全等检查项）。
allowed-tools:
  - Read
  - Grep
  - Glob
  - Bash
---

# RNOH 稳定性代码检视

这个 skill 用于在代码层面对稳定性风险做系统性预判，在问题发生之前发现潜在隐患，或在已有日志基础上验证根因是否落在特定代码路径。

优先目标有两个：

1. 不停留在"可能有问题"，而是给出具体的风险位置、触发条件和修复方向。
2. 对每个发现，都要说清楚是哪种稳定性风险家族，以及历史上是否有类似修复可以参考。
3. 除 RNOH 历史修复模式外，补充对照华为官方稳定性编码规范中的通用约束，避免遗漏 Node-API、libuv、IPC、ArkUI 和图形接口层面的稳定性问题。

## 外部规范基线

在检视 OpenHarmony / HarmonyOS 相关代码时，除了本仓历史修复与本地规范外，还应补充参考以下华为官方规范：

- 稳定性编码规范总览：https://developer.huawei.com/consumer/cn/doc/best-practices/bpta-stability-coding-standard
- NDK 开发 ArkTS 侧编码规范：https://developer.huawei.com/consumer/cn/doc/best-practices/bpta-stability-coding-standard-ndk-arkts
- Node-API 开发规范：https://developer.huawei.com/consumer/cn/doc/best-practices/bpta-stability-coding-standard-node
- C++ 编码规范：https://developer.huawei.com/consumer/cn/doc/best-practices/bpta-stability-coding-standard-cpp
- libuv 使用规范及案例：https://developer.huawei.com/consumer/cn/doc/best-practices/bpta-stability-coding-standard-libuv
- 易错 API 的使用规范：https://developer.huawei.com/consumer/cn/doc/best-practices/bpta-stability-coding-standard-api

如果用户要求“按官方规范补齐”、“对照华为规范检查”或“补充通用稳定性规则”，默认把这些规则纳入检视范围，而不是只检查 RNOH 特有风险。

## 检视核心原则

### 目标与边界

**检视的目标是：**

- 在崩溃、卡死、泄漏发生前识别真实风险，而不是抓住一切潜在写法一律标高。
- 给出具体位置和可执行的修复方向，不停留在"可能有问题"。
- 肯定代码中已经正确实现的防护模式，不只列缺陷。
- 基于已有材料继续分析，不因为缺少某项信息就停住。

**不是目标：**

- 对不会引发崩溃 / 卡死 / 泄漏的代码风格或命名问题发表意见。
- 为了看起来全面而虚造低风险问题。

### 反馈风格

反馈应做到建设性、有依据、专注代码本身而非个人。优先解释*为什么*某处有风险，而不是直接下裁决。

对于有疑问但不确定的地方，使用提问式语言：

```
✅ "如果 destroy() 在回调执行时被调用，这里会访问已释放对象吗？"
❌ "这里会 UAF。"
```

### 严重性标签

对每个发现用统一标签标注，便于作者快速定位优先级：

| 标签            | 含义                                                          | 行动           |
| --------------- | ------------------------------------------------------------- | -------------- |
| 🔴 `[阻塞]`     | 有明确触发路径，历史已有同类崩溃/泄漏修复，或显然缺少必要保护 | 必须修复后合入 |
| 🟡 `[重要]`     | 竞态或时序风险需特定条件才能触发，或历史相似但路径待确认      | 应修复，可讨论 |
| 🟢 `[建议]`     | 代码不符合框架防护惯例，不一定立刻崩溃，但存在隐患            | 非阻塞，建议   |
| 💡 `[优化]`     | 可选的更优实现方式，不涉及稳定性风险                          | 考虑即可       |
| 🎉 `[正确实现]` | 防护模式已正确应用，值得明确肯定                              | 在报告中列出   |

## 何时使用

以下场景优先使用这个 skill：

- 用户提交了代码片段或文件内容，希望检查是否存在稳定性风险。
- 用户想在提交 PR 前做稳定性自查，或在代码评审时做系统检查。
- 用户已通过 triage 定位到可疑模块或代码路径，想进一步在代码层面确认。
- 用户想知道某段改动是否遗漏了历史已修复的防护模式。
- 用户想确认 TurboModule、组件、Surface、动画、图片等模块的生命周期保护是否完整。

如果用户只是询问稳定性概念或想分析崩溃日志，优先使用文档或 rnoh-stability-triage，不要强行进入代码检视流程。

## 最小必要输入

检视时不要因为缺少某项信息就停住，基于已有材料继续分析：

- **必要**：代码片段、文件内容或可访问的工作区文件路径。
- **有帮助**：模块名称、所在子系统（C++ 框架层 / ArkTS 适配层 / JS 业务层）。
- **有帮助**：对应的触发场景，例如页面销毁、重载、页面切换、动画、图片加载。
- **可选**：已知的崩溃日志或报错信息，用于验证性检视。
- **可选**：版本信息，用于判断是否已有相应历史修复。

如果用户只给了文件路径，先读取相关代码，再基于代码内容做检视，不要直接要求用户把内容贴出来。

## 检视工作流

### 第 0 步：确定检视目标并获取代码

先判断用户提供的是哪种输入，直接对应处理方式：

| 输入类型               | 处理方式                                                              |
| ---------------------- | --------------------------------------------------------------------- |
| 代码片段已直接贴入     | 直接进入第 1 步                                                       |
| 提供了文件路径         | 用 `Read` 工具先读取对应文件，再进入第 1 步                           |
| 提供了 GitCode PR 链接 | 按下方「GitCode PR 自动读取流程」获取 diff，再进入第 1 步             |
| 要求检视当前本地改动   | 运行 `git diff`（未暂存）和 `git diff --staged`（已暂存）获取所有改动 |
| 没有任何代码           | 直接告知用户需要提供代码片段、文件路径或 PR 链接                      |

如果用户提供的是 PR，在分析前先读取 PR 描述和历史评论，了解既有讨论和已知背景。

#### GitCode PR 自动读取流程

**第一步：从 PR 页面获取 source repo 和 source branch**

抓取 PR 页面（无需 Token，公开仓库）：

```
GET https://gitcode.com/{owner}/{repo}/pull/{N}
```

从页面文本中找到形如「从 `{fork_user}/{fork_repo}：{branch}` 合入到 ...」的字段，提取：

- `source_repo_url`：`https://gitcode.com/{fork_user}/{fork_repo}.git`
- `source_branch`：分支名

也可以用 API（需要 Token）一次性拿到结构化数据：

```
GET https://gitcode.com/api/v5/repos/{owner}/{repo}/pulls/{N}?access_token=<TOKEN>
```

从 `head.repo` 和 `head.ref` 字段提取同样信息。

**第二步：fetch source 分支到本地临时分支**

```powershell
git fetch {source_repo_url} {source_branch}:pr-{N}
```

例：

```powershell
git fetch https://gitcode.com/zheng-jun-feng/WQL0408_02.git 0.77-stable:pr-2538
```

**第三步：读取 diff**

```powershell
# 查看变更文件列表
git show {commit_sha} --stat
# 导出完整 diff（单文件）
git show {commit_sha} -- {file_path} > d:\git0407\pr{N}_diff.txt
```

如果不知道 commit sha，也可以用分支范围：

```powershell
git diff {target_branch}...pr-{N} -- {file_path}
```

**第四步：检视完成后清理**

```powershell
git branch -d pr-{N}
```

> **注意**：source fork 仓库必须是公开的，否则 fetch 步骤需要凭据。如果遇到权限问题，改用 API（`/pulls/{N}/files`）+ Token 获取 `patch` 字段。

### 第 1 步：明确检视范围

先确认代码属于哪一层和哪个模块：

- **C++ 框架层**：packages/react-native-harmony/、packages/react-native/、与 C++ 侧逻辑直接相关的 .cpp / .h 文件。
- **ArkTS 适配层**：.ets 文件，封装 HarmonyOS 系统 API 与 RN 框架之间的适配逻辑。
- **JS/TS 业务层**：JS/TS 文件，框架主流程或 TurboModule 接口实现。

再确认改动涉及哪些生命周期阶段：

- 创建 / 初始化 / 启动阶段
- 运行 / 回调 / 驱动帧 / 事件处理阶段
- 销毁 / 停止 / 清理 / 析构阶段
- 多线程交汇 / 跨线程调度

把这两个维度组合，作为后续检视的主要入手点。

### 第 2 步：按稳定性类别做系统检视

按下面几类逐一扫描，不要只检查"看起来可疑"的部分。对于每类，先判断代码是否属于高风险区域，再按对应清单逐项检查。

---

#### 检视分类 A：应用异常退出风险（CppCrash / JS Crash）

**C++ 层销毁后访问**

- 是否存在异步回调（VSync、Display、Timer、HTTP、NAPI）在对象销毁后仍可能执行？
  - 典型危险：AnimatedNodesManager::runUpdates 被 VSync 回调驱动，在 AnimatedTurboModule 销毁后仍执行。
  - 检查点：析构函数或 `destroy()` 方法中是否明确停止回调调度？
- 异步任务是否持有 `this` 或对象成员的强引用？
  - 建议模式：使用 `weak_ptr<T> self = weak_from_this()`，回调中 `auto locked = self.lock(); if (!locked) return;`。
- 对象销毁路径是否解绑了所有异步源？包括：
  - VSync/Display 回调注销
  - Timer 取消（cancelTimer / stopTimer）
  - EventBeat 解绑
  - HTTP 请求中止
  - observer / listener 反注册

**销毁顺序问题**

- 依赖关系链中，是否先销毁了被依赖对象，导致依赖者继续访问？
  - 典型危险：surface stop 时 LayoutAnimationKeyFrameManager 已销毁，UIManager 仍尝试进入。
  - 检查点：stopSurface → unregisterSurface 的顺序是否在主线程侧统一管控？
- 析构函数是否可能触发对其他已释放对象的回调？

**初始化时序问题**

- ArkUINode 是否在确认 NodeApi 已初始化后才调用 `createNode`？
  - 典型危险：NodeApi 未就绪时调用返回 nullptr，后续断言 abort。
  - 检查点：`createNode` 返回值是否被检查？nullptr 时是否有兜底？
- WindowDecor / Modal 等接口调用是否在窗口状态就绪后才执行？
  - 典型危险：`getWindowDecorVisible` 在 Modal 初始化阶段调用时窗口未就绪。
- Surface start / stop 顺序是否与 Scheduler、UIManager 状态保持一致？

**noexcept 边界**

- 标记为 `noexcept` 的函数内部是否调用了可能抛异常的代码？
  - 典型危险：析构函数、系统回调接口、虚函数覆盖中误加 `noexcept`，内部抛出时触发 terminate → SIGABRT。
  - 检查模式：搜索 `noexcept`，逐一确认内部是否有非 noexcept 的调用链。
- 析构函数是否有未捕获的异常路径？

**空值保护**

- C++ 侧：裸指针、`std::optional`、工厂方法返回值是否在使用前做了 nullptr 检查？
- ArkTS/JS 侧：异步回调、可选参数、跨层返回值是否做了 undefined/null 保护？
  - 典型危险：WebSocket close 路径中访问已被置为 undefined 的 socket。
  - 检查点：`?.` 可选链或 `if (socket == undefined) return;` 是否存在？
- `shared_ptr::get()` 使用前是否检查有效性？`weak_ptr::lock()` 后是否判空？

**返回路径完整性**

- 非 void 函数是否所有控制流分支都有明确返回值？
  - 典型危险：声明返回对象自身或状态值，但某分支遗漏 return，调用方继续执行 → SIGTRAP。
- 异常处理边界是否完整：是否所有可能抛异常的路径都被捕获并处理？

**排序与容器操作安全**

- 传给 `std::sort` 的比较器是否满足严格弱序（strict weak ordering）？
  - 典型危险：写成 `left.x <= right.x` 而非 `left.x < right.x`，当存在相等元素时行为未定义，排序过程中可能发生错误访问或崩溃。
  - 检查点：搜索 `std::sort`，逐一确认比较器不是 `<=`、`>=` 或其他会对相等元素返回 true 的形式。
  - 历史案例：sort 崩溃（0.72 提交 c822670b0）
- 如果业务上需要保持相等元素原始相对顺序，是否改用了 `std::stable_sort`？
- 容器（vector/array/map/string）的访问是否在高频路径（动画帧/滚动）中存在并发修改或越界风险？

**Release / 混淆崩溃**

- 是否使用了双下划线前缀（`__xxx`）的私有 ArkTS 字段或方法？
  - 典型危险：Release 构建时混淆器会重命名双下划线前缀标识符，导致运行时找不到方法而崩溃。
  - 修复方向：改为单下划线前缀（`_xxx`）或去掉前缀。
- 是否在 `.ets` 文件中手工声明了系统 API（如 `px2vp`）而不是从 `@ohos.arkui` 正确导入？
  - 典型危险：混淆后方法名被改写，手工声明的版本无法匹配运行时符号，崩溃。
- API12 兼容：是否调用了只在 API12+ 可用的接口？是否有降级判断？
  - 典型危险：`getSDKApiVersion` 仅在 API12+ 有效，低版本调用 → SIGABRT。
  - 检查点：多线程调用路径上是否注意了此类 API 的线程限制？

---

#### 检视分类 B：应用冻屏风险（死锁 / 线程阻塞）

**锁顺序**

- 代码中是否存在对多个 mutex 的嵌套加锁？
  - 所有需要同时持有多个锁的地方，加锁顺序是否全局一致？
  - 典型危险：A 先锁 mutex1 再锁 mutex2，B 先锁 mutex2 再锁 mutex1 → 死锁。
  - 检查点：用注释或文档明确多锁顺序，或使用 `std::lock()` 同时获取多锁。
- 历史高风险模块：`FontRegistry`、`inflightAnimations_`（LayoutAnimation）、`TextMeasureRegistry`。

**锁重入**

- 是否在已持有锁的情况下调用了可能再次请求同一锁的函数？
  - 非可重入锁重入 → 死锁。
  - 检查点：持有锁时发射的事件和触发的回调，是否可能在同一线程上再次进入同一临界区？

**持锁回调 / 持锁等待**

- 是否在持有锁的状态下发起跨线程调度（runOnQueue、postTask）并 **等待** 对方返回？
  - 典型危险：主线程持锁等 JS 线程，JS 线程也在等主线程释放锁。
  - 检查点：`waitForSyncTask` 类型调用的调用链上是否保持锁持有状态？
- 是否在持有锁的状态下直接调用用户回调或触发事件？
  - 用户回调内可能尝试获取同一锁或触发新的调度，形成隐式死锁。

**线程访问合法性**

- 是否在非 UI 线程访问了只能在主线程 / UI 线程操作的对象？
  - ArkUI 节点操作（ArkUINode）必须在 UI 线程。
  - NAPI 调用（napi_call_function、napi_create_xxx）必须在 JS 线程。
  - 典型危险：Worker TurboModule 在 Worker 线程直接操作主线程对象。
  - 检查点：代码中是否有明确的线程切换（`runOnQueue(mainThreadQueue, ...)`）？
- schedulerDidSetIsJSResponder / reportMount 类型接口：调用路径是否确保在规定线程上？

**生命周期切换阻塞**

- 生命周期方法（`onBackground`、`onForeground`、`onWindowStageDestroy`）是否在执行路径上等待异步结果或持有锁？
- 页面首次创建时是否在同步路径上触发了大量组件构建或布局测量？

---

#### 检视分类 C：内存异常风险（OOM / 地址越界）

**所有权与生命周期**

- 是否存在 `shared_ptr` 循环引用？
  - 检查点：A 持有 B 的 `shared_ptr`，B 持有 A 的 `shared_ptr` → 改其中一个为 `weak_ptr`。
- 是否在对象销毁后，某个地方仍保持了对该对象的裸指针引用？
  - 所有权不清晰的地方是否改为 `shared_ptr` / `weak_ptr`？
- 容器或缓存在多线程访问时是否有锁保护？
  - 典型危险：`inflightAnimations_` 在多个 surface 并发写入，无锁 → 数据竞争 → 崩溃。

**UAF 典型路径**

- 图片加载回调：ImageComponentInstance 销毁后，HTTP 回调回来继续访问已释放的 URI 缓存对象。
  - 检查点：回调中是否先检查 ComponentInstance 是否仍有效（`destroyed` 标志 / 弱引用）？
- JSVM / HostObjectProxy：JS 运行时销毁后，持有的 JSVM 引用继续被使用。
  - 检查点：`JSVMPointerValue` 解引用前是否确认 JSVM 仍存活？
- 动画驱动：animationTick 回调中访问已销毁的 AnimationDriver。

**高频路径的分配压力**

- 渲染帧回调（onUITick / VSync）内是否有不必要的对象分配或内存拷贝？
- 滚动、动画等高频触发路径上是否有临时对象频繁创建但未复用？

---

#### 检视分类 D：资源泄漏风险

**监听器 / 回调注册配对检查**

- 是否每一个 `addListener` / `subscribe` / `addEventListener` 都有对应的 `removeListener` / `unsubscribe` / `removeEventListener`，且在实例销毁时执行？
- 是否每一个 observer 注册都有对应的反注册，且不依赖 GC 触发释放？
- 典型泄漏模块：WindowManager、EventEmitter、DeviceEventEmitter、TurboModule 事件监听。

**线程和任务生命周期**

- 创建的线程 / 线程池是否在实例销毁时停止并等待回收（join）？
- 提交到 queue 的异步任务，在队列销毁或实例销毁时是否有取消机制？
- 典型危险：TurboModule 析构时，任务队列中仍有对该模块的引用，线程持续运行。

**HTTP / 网络请求**

- HTTP 请求发起后，回调对象（闭包、lambda）是否持有了长生命周期对象的强引用？
- 请求完成、取消或超时时，是否释放了回调持有的所有上下文引用？
- HttpSession / HttpClient 实例是否在使用完成后明确销毁？
- 典型危险：`m_requestDeleter` 不释放 → HTTP 回调泄漏。

**NAPI Reference 与 JS 对象持有**

- `napi_create_reference` 是否配对了 `napi_delete_reference`？
- JSVM code cache、ScriptContext、JSContext 是否在生命周期结束时释放？
- JS 端持有的 Native 对象（通过 `NativeEventEmitter`、`EventTarget`、TurboModule）是否在页面卸载时清理？

**Surface 与 RNInstance 资源**

- Surface 停止、reload、切换 bundle 时，所有注册到 surface 的资源是否完整回收？
- RNInstance 销毁时，TurboModule 持有的系统资源（调度器、句柄、定时器）是否全部释放？

---

#### 检视分类 E：华为官方通用规范补充（Node-API / libuv / ArkUI / IPC / 图形 / 底层 C++）

这类检查项不一定是 RNOH 独有问题，但在 OpenHarmony 混合栈代码里非常常见。只要改动涉及 NDK、Node-API、libuv、ArkTS 调用 native、IPC、媒体、图形或底层 C++，都要补扫这一类。

**E1. NDK / ArkTS so 引用与导出规范**

- ArkTS 侧 import 的 so 名称是否与 `oh-package.json5`、`CMakeLists.txt` 产物名、`index.d.ts` 导出名一致？
  - 典型风险：依赖名、导入名、导出名不一致，运行时调用失败或模块加载异常。
- 跨模块暴露 native 能力时，是否通过统一出口导出，而不是在业务侧直接依赖内部 so 名称？
- 如果是导出方模块，`index.ets` 是否统一导出 napi 对象，避免多处散落引用？

**E2. Node-API / Ark Runtime 专项检查**

- `napi_get_cb_info` 使用时，`argc` 是否正确初始化，`argv` 长度是否大于等于声明的 `argc`？
  - 典型风险：未初始化 `argc` 或 `argv` 长度不足，导致越界写。
- 是否正确使用 `napi_open_handle_scope` / `napi_close_handle_scope` 管理回调中创建的 `napi_value`，尤其是在循环和 `uv_queue_work` 回调中？
- 是否存在跨线程使用 `napi_env`、`napi_value`、`napi_ref`、`napi_add_env_cleanup_hook` / `napi_remove_env_cleanup_hook` 的情况？
  - 典型风险：非 JS 线程或错误 env 线程上调用 NAPI，直接引发崩溃。
- 是否错误保存并复用已失效的 `napi_env`、已删除的 `napi_ref`，或超出 scope 生命周期继续使用 `napi_value`？
- `napi_create_async_work` / `napi_async_work` 是否把 `napi_delete_async_work` 放在 complete 回调中，而不是提前释放？
- `napi_wrap` 最后一个参数如果接收了 `napi_ref`，后续是否调用 `napi_remove_wrap` 或等效释放逻辑？
- 是否错误手动释放 `napi_get_arraybuffer_info` / `napi_get_buffer_info` 拿到的 buffer 指针？
  - 典型风险：ArrayBuffer 内存由引擎管理，手动释放会 double free。
- 是否把 `napi_create_external` / `napi_create_external_arraybuffer` 创建的对象跨线程传递？
- 模块注册是否满足：`nm_register_func` 用 `static`，constructor 入口函数名唯一，`.nm_modname` 与模块名完全匹配？

**E3. libuv / 事件循环 / fd 生命周期**

- `uv_queue_work` 对应的 `uv_work_t` 是否只在 `after_work_cb` 中释放？
  - 典型风险：在 work 完成前提前 delete，导致 `uv__queue_done`、`after_work_cb` 或线程池执行阶段 UAF。
- 如果自定义上下文对象和 `uv_work_t` 生命周期无法完全同步，是否将两者拆开创建，通过 `work->data` 关联？
- `uv_close` 后是否仍然立即释放 Handle 所在对象，或把 Handle 作为普通成员随宿主析构一起释放？
  - 典型风险：`uv_close` 是异步关闭，必须等 close callback 后再释放 Handle 本体。
- 是否在错误线程上调用 `uv_close`，或在同一个 Handle 上做多次初始化 / 绑定到多个 loop？
  - 典型风险：事件循环死循环、功能失效或 crash。
- 是否在主线程 / UI 线程调用 libuv 的阻塞式同步接口（如同步文件操作）？
  - 典型风险：Freeze、掉帧、主线程卡死。
- 是否把 `uv_cancel` 当作“不会再执行 after_work_cb”？
  - 正确理解：`uv_cancel` 不保证阻止后续 complete 回调，清理逻辑仍应放在 `after_work_cb` 中。
- fd 资源是否遵守“谁申请谁释放”，close 后是否立即置为 `-1`，是否存在透传 fd 后双重 close？

**E4. ArkUI / 媒体 / IPC / 图形易错 API**

- 是否在业务代码中使用 Inspector 相关接口（如 `getFilteredInspectorTree`、`getInspectorInfo`）？
  - 典型风险：性能低、异常时可能闪退；仅建议用于调试。
- `OH_NativeXComponent_RegisterCallback` 注册的回调对象，是否保证在 `OnSurfaceDestroyed` 之前始终有效？`nativewindow` 是否在 `OnSurfaceDestroyed` 后停止使用？
- 是否在 `aboutToAppear` / `aboutToDisappear` 中调用 `animateTo`、`animateToImmediately`、`keyframeAnimateTo`？
- 是否在动画闭包里切换 `if/else` 数据保护分支，导致默认转场延长组件生命周期并访问无效数据？
- `Scroller.currentOffset`、`getRectangleById`、`getInspectorByKey` 这类接口结果是否做了空值 / 默认值保护？
- `@ohos.measure`、`@ohos.font` 是否在缺少明确 UIContext 的生命周期中调用，而不是使用 `this.getUIContext()` 对应接口？
- `UIExtensionComponent.send/sendSync` 是否仅在 receiver 注册完成后调用，并在 `onError` / `onRelease` / `onTerminated` 后停止使用？
- IPC / RPC 使用时，是否满足：
  - `writeString` 长度不超过 40960 字节；
  - `MessageSequence` 总传输量不超过 200KB，超限改用 RawData / Ashmem；
  - `data` / `reply` 在 `.finally()` 或回调取数完成后再 `reclaim()`；
  - `Ashmem` 释放顺序为先 `unmapAshmem` 再 `closeAshmem`；
  - `writeArrayBuffer/readArrayBuffer` 第一参数必须是 `ArrayBuffer` 且读写 `TypeCode` 一致；
  - `onRemoteRequest` / `RemoteMessageRequest` 返回值正确反映处理成功与否。
- 媒体 / 相机 / 音频常见易错点：
  - `on/off` 是否使用同一个回调实例，避免匿名函数导致 off 失效；
  - 是否在相机回调中直接再次 `on/off` 导致死锁；
  - `CameraInput.open()` 之后再 `addInput`，`addInput` 之后再 `addOutput`，配置流程是否包裹在 `beginConfig` / `commitConfig` 之间；
  - `close()` / `release()` 等 Promise 接口是否 `await`，保证时序正确；
  - OHAudio `Release` 后是否继续使用旧裸指针；音频回调结构体是否全部初始化；写音频数据时是否明确 buffer 已写满或返回 INVALID。
- 图形 / NativeWindow 使用时，是否重复调用 `OH_NativeWindow_DestroyNativeWindow`，或在 XComponent/nativewindow 析构后继续访问原句柄？

**E5. 底层 C++ 安全约束**

- 指针、资源描述符、BOOL、类成员变量是否在声明或构造阶段完成初始化？释放后是否立即置为新值 / 空值 / `-1`？
- 构造函数内是否执行了可能失败的操作、启动线程，或析构职责不完整？是否存在 `delete this`？
- 数组作为函数参数时是否同步传入长度；字符串 / 指针参数是否检查空值；循环是否有明确退出条件？
- 字符串 / 数组 / 内存操作是否满足：
  - 空间充足；
  - 字符串有 `\0` 结束；
  - 外部输入作为索引或长度前已校验；
  - `printf` / `snprintf` 等格式串不受外部输入控制，参数类型和个数匹配。
- 整数运算是否检查溢出、符号翻转、除零；是否错误进行有符号位运算、指针和整数直接互转、外部控制循环次数未校验？
- 内存申请前是否校验大小合法性，申请后是否判空，是否访问未初始化内存？

---

### 第 3 步：对照历史修复模式做对比

检视时，结合已知历史修复模式，判断代码是否已经包含了应有的保护：

#### C++ 层常见防护模式

| 问题类型         | 历史修复典型模式                                                                            |
| ---------------- | ------------------------------------------------------------------------------------------- |
| 销毁后回调       | 析构中 `stopCallbackDispatch()`；回调入口检查 `destroyed_` 标志；回调持弱引用 `weak_ptr<T>` |
| 初始化时序       | `createNode` 前检查 `NodeApi::isInitialized()`；assert 改为条件返回                         |
| 锁并发           | `std::scoped_lock` 同时获取多锁；全局统一加锁顺序约定                                       |
| noexcept 滥用    | 移除误加 noexcept；在边界函数内用 `try-catch` 包裹                                          |
| 生命周期销毁顺序 | `stop → unregisterSurface` 统一在主线程队列串行执行                                         |
| 排序比较器违规   | 使用满足严格弱序的比较器（`<`，不能用 `<=`）；需稳定顺序时用 `std::stable_sort`             |

#### ArkTS/JS 层常见防护模式

| 问题类型            | 历史修复典型模式                                              |
| ------------------- | ------------------------------------------------------------- |
| undefined/null 访问 | `socket?.close()` 或 `if (obj == undefined) return;`          |
| 混淆崩溃            | 去掉 `__` 前缀；`px2vp` 等内置方法不手工声明直接调用          |
| API12 兼容          | `versionCode >= 12` 判断后调用；多线程场景避免非线程安全 API  |
| 监听器泄漏          | 在组件 `aboutToDisappear` 中调用所有 `off` / `removeListener` |

如果发现代码缺少这些防护，应在检视意见中明确指出并给出对应的修复示例。

历史修复参考文档路径：

- docs/zh-cn/稳定性/稳定性历史修复/0.72稳定性修复汇总/
- docs/zh-cn/稳定性/稳定性历史修复/0.77稳定性修复汇总/
- docs/zh-cn/稳定性/稳定性历史修复/0.82稳定性修复汇总/

如需对照完整稳定性编码规范（1.1-5.4 所有规范项）做系统性自查，读取 [references/coding-standards-checklist.md](references/coding-standards-checklist.md)。检视范围较大或用户明确要求覆盖所有规范时优先读取。

如果用户明确要求“按华为官方网页补充规范”或代码同时涉及 Node-API / libuv / ArkUI / IPC / 图形接口，除本地 checklist 外，还必须执行检视分类 E，不得只给出 RNOH 历史修复视角的结论。

### 第 4 步：对问题分级

用「检视核心原则」中定义的严重性标签对每个发现标注（🔴 → 🟡 → 🟢 → 💡），并在报告中同步列出 🎉 正确实现的防护。

分级要点：

- 🔴 **阻塞（会引发 crash / 卡死 / 严重泄漏）**：有明确触发路径且历史已有同类修复，或显然缺少必要保护（析构后回调无任何保护、noexcept 内明确有 throw）。**凡是分析后确认会导致 crash 的问题，必须标为 🔴 并在标题行中注明「⚠️ 会引发 crash」**。
- 🟡 重要：竞态或时序风险，但可能需要特定并发时序才能触发；或历史上有相似问题，当前路径是否完全重合需进一步确认。若该风险在极端条件下确定会 crash，也应升级为 🔴。
- 🟢 建议：不符合框架防护惯例，潜在隐患不一定立刻崩溃（如缺少 `if (!obj) return;` 但调用方实际保证非空）。

不要把所有发现都标为 🔴；只有证据充分的才标阻塞。

### 第 5 步：给出修复建议

对每个风险，按下面结构给出修复建议：

1. **具体位置**：哪个文件、哪个函数或哪段代码。
2. **问题描述**：是什么风险，触发条件是什么，为什么危险。
3. **建议修复**：给出具体的修复方向，优先给代码级别的建议（示例片段或模式名），而不是泛泛说"加保护"。
4. **参考历史修复**（如有）：列出相关历史修复条目的版本、问题名和提交链接。

如果存在多个发现，按风险等级从高到低排列，优先处理高等级。

如果只能给一个最重要的建议，优先给"最可能立刻引发问题"的建议，而不是罗列所有可能动作。

### 第 6 步：判断是否还需要补充信息

如果因为信息不足无法得出确定结论：

- 明确说明是哪个地方、哪段逻辑需要补充，不要泛泛说"信息不足"。
- 建议对方提供：所在线程的调用来源、销毁路径的完整代码、相关对象的生命周期管理方式。
- 如果有可复现的问题环境，建议在可疑的回调入口、销毁路径、锁获取前后增加日志，确认实际执行顺序，再继续分析。

### 第 7 步：检视后清理（可选）

如果本次检视演练用到了 git 操作（如 checkout PR 分支），检视完成后询问用户是否需要切回当前工作分支。如果用户确认，运行 `git checkout <branch>` 完成切换。

## 输出格式

除非用户明确要求别的格式，否则按下面结构输出：

---

# 稳定性代码检视报告

## 检视范围

- 检视文件 / 模块：
- 涉及生命周期阶段：
- 代码层次（C++ / ArkTS / JS）：

## 总体结论

用 2 到 3 句话总结整体风险等级和最主要的风险类型。

## 已正确实现的防护

🎉 `[正确实现]` — 列出代码中已经正确实现的稳定性防护模式，给作者正向反馈。如果没有值得提及的内容，可以省略本节。

## 发现的稳定性风险

对每个风险按如下格式输出（按 🔴 → 🟡 → 🟢 顺序排列）：

---

🔴 **[阻塞] R1 [风险类型]：[简短标题]** ⚠️ 会引发 crash

- **位置**：文件名 + 函数名 / 行号（如已知）
- **是否会 crash**：**是** / 否 / 特定条件下是（说明条件）
- **问题描述**：具体是什么风险，在什么条件下触发
- **触发条件**：哪个生命周期阶段 / 哪种并发时序下会出问题
- **建议修复**：给出具体修复方向（如有代码示例，优先给）
- **参考历史修复**（如有）：
  - 版本：
  - 问题：
  - 提交 / MR：[提交号](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/提交号) / [!MR号](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/MR号)

---

（🟡 和 🟢 级别使用相同结构；💡 优化项只需简短描述，不需要展开）

## 仍需补充的信息

- 如有不确定的地方，列出最多 3 项最值得补充的信息。
- 如当前无法判断风险是否存在，说明还需要看哪部分代码或执行路径。

## 检视结论

- **[ ] ✅ 通过** — 未发现稳定性风险，可合入
- **[ ] 💬 建议** — 有 🟢 / 💡 级建议，不阻塞合入，可酌情跟进
- **[ ] 🔄 需修改** — 存在 🔴 / 🟡 级风险，建议修复后合入

---

## 质量要求

- 用中文输出。
- 每个风险描述要具体：不要只写"可能有 UAF"，要写"在 XXX 场景下，YYY 对象先于 ZZZ 回调销毁，回调执行时访问已释放对象"。
- 不要把代码质量问题（命名、格式）混入稳定性风险报告；这个 skill 只关注崩溃、卡死、泄漏风险。
- 历史修复链接只在有充分对应关系时才引用，不要为凑链接而引用不相关的修复。
- 如果代码没有发现稳定性风险，直接说明，并简要列出检视覆盖的关键点，让用户知道检视工作是真正做了而不是走过场。
- 在没有发现明确风险时，不要为了"看起来全面"而虚造低风险问题。
- 如果最终判断置信度较低，不能只写"无法判断"；必须给出需要补充的关键信息，或者建议在问题环境的关键位置增加日志再复查。

# RNOH 稳定性编码规范快速自查清单

本文件是 [稳定性编码规范](../../../../../稳定性问题介绍/稳定性编码规范.md) 的精简对照版，
供代码检视时按规范条目逐项自查使用。每条规范后标注了所属编号，方便对应原文查阅完整示例和历史案例。

加载时机：当检视范围较大、或用户明确要求覆盖所有规范时读取本文件。单模块快速检视可直接使用 SKILL.md 中的检视分类 A-E。

---

## 1. 内存与对象安全

- [ ] **1.1 异步回调弱引用**：VSync、Display、Timer、HTTP、NAPI 等异步回调不捕获 `this` 或对象强引用；统一使用 `weak_ptr`，回调入口先 `lock()` 判空再执行；只读数据按值拷贝到闭包。

- [ ] **1.2 多线程锁保护**：所有被多线程并发访问的容器、缓存、状态字段都有同步保护，读写路径都加锁，不只保护写不保护读。高风险对象：`inflightAnimations_`、`TextMeasureRegistry`、`FontRegistry`、listeners/callbacks 集合。

- [ ] **1.3 工厂方法返回值检查**：`createNode()`、`lookup()`、`find()`、`getDescriptor()`、`optional` 风格返回值在使用前做有效性检查；空值时不继续向下执行，在入口统一建立失败分支。

- [ ] **1.4 指针和成员状态使用前判空**：裸指针、`weak_ptr::lock()` 结果、`m_eventEmitter`、`m_props`、`m_state`、`m_surface`、`m_scheduler` 等成员在使用前显式校验；生命周期切换、异步回调晚到时尤其注意。

- [ ] **1.5 shared_ptr 循环引用**：双向引用链（A 持有 B 的 `shared_ptr`，B 持有 A 的 `shared_ptr`）中，将其中一侧改为 `weak_ptr`；listener、callback owner、上下文对象和 manager 之间的双向引用重点检查。

---

## 2. 生命周期与销毁清理

- [ ] **2.1 析构/destroy() 停止所有异步来源**：VSync 或 Display 回调注销、Timer 取消（cancelTimer/stopTimer）、EventBeat/动画帧驱动解绑、HTTP 请求中止、observer/listener/event emitter 反注册；遵循"谁注册谁释放"原则。

- [ ] **2.2 销毁顺序与依赖链一致**：先停止依赖上游对象的使用者，再销毁被依赖资源，最后清理管理者和全局状态；surface stop 时确保 Scheduler、UIManager、LayoutAnimation 等访问路径已先停止。

- [ ] **2.3 初始化路径确认依赖已 ready**：ArkUINode 创建前确认 ArkUINodeContext 和 NodeApi 已初始化；窗口属性读取前确认窗口状态稳定；bundle 加载前确认平台能力可用；ready 检查放在入口。

- [ ] **2.4 状态机迁移顺序一致**：状态切换路径唯一；回调触发不重入改变当前状态；外部事件通知在内部状态完成更新之后发出；高频场景（滚动、拖拽、动画结束、页面退出）尤其注意。

- [ ] **2.5 注册与注销形成闭环**：`addListener/removeListener`、`subscribe/unsubscribe`、`addEventListener/removeEventListener`、`napi_create_reference/napi_delete_reference` 必须成对；NAPI reference 不得跨线程释放。

---

## 3. 并发与线程

- [ ] **3.1 多锁加锁顺序全局一致**：有嵌套加锁的地方全局使用一致的顺序；使用 `std::scoped_lock` 或 `std::lock` 原子获取多把锁；在注释或文档中明确锁顺序约束。

- [ ] **3.2 持锁不发起跨线程同步等待**：持有锁时不调用 `waitForSyncTask` 类同步等待接口、不向其他线程投递任务后阻塞等待；正确模式：先复制数据、释放锁、再调度；持锁时也不直接调用用户回调。

- [ ] **3.3 每类操作在规定线程上执行**：ArkUI 节点操作在 UI 线程；`markDirty` 等脏标记在主线程；NAPI 调用在 JS 线程；窗口属性读取在允许的线程上；跨线程访问必须显式切换到目标队列。

- [ ] **3.4 getter 也要考虑线程安全**：getter 内部读取可变共享状态时必须加锁；不返回已失效的引用/指针/上下文；不把栈上临时状态暴露到异步回调或其他线程。

- [ ] **3.5 事务处理数据源一致顺序稳定**：performTransaction 前后阶段使用同一份 transaction 数据；并行切片不打乱 Create/Insert/Delete 等 mutation 顺序；依赖顺序的操作在拆分时保留顺序约束。

---

## 4. 异常与边界处理

- [ ] **4.1 noexcept 只用于真正不抛异常的函数**：析构函数、虚函数覆盖、系统回调接口中不随意加 `noexcept`；内部有可能抛异常时要么去掉 noexcept，要么在边界处 try-catch 转化为可恢复错误。

- [ ] **4.2 非 void 函数所有分支都有返回值**：链式接口、状态/布尔/指针返回函数、switch 状态转换函数中，所有控制流分支都有明确 `return`；无返回值的遗漏分支可能触发 SIGTRAP 或后续未定义行为。

- [ ] **4.3 JS/ArkTS 层全面做空值保护**：异步回调中访问对象属性、关闭/销毁/重置路径中访问 socket/window/context、可选参数和跨层返回值读取前做 `undefined`/`null` 保护；使用可选链 `?.` 或显式判断。

- [ ] **4.4 边界输入在入口做合法性检查**：props、模块参数、路径、配置、事件数据在入口校验类型和合法性；非法 keyboardType/枚举值、目录/文件/句柄类型混用、缺少关键字段时尽早失败并给出日志。

- [ ] **4.5 容器和字符串访问检查边界**：`vector`/`array`/`map`/`string` 的索引、位置和值使用前确认有效；并发场景或状态切换时上一行读到的 size 下一行未必仍然有效；删除全部 item 后不按旧索引访问。

- [ ] **4.6 std::sort 比较器满足严格弱序**：比较器不能写成 `<=`/`>=`，必须是严格的 `<`；不满足严格弱序的比较器行为是未定义行为，不能靠"小数据量看起来正常"来判断正确；需要保持相等元素原始相对顺序时改用 `std::stable_sort`。

---

## 5. 平台与模块兼容

- [ ] **5.1 ArkTS 禁止手工 declare 系统函数**：不通过 `declare function` 手工声明 `px2vp` 等系统函数；使用平台正式提供的导入方式（如 `@kit.ArkUI`），否则 release 混淆后无法按原始符号名解析。

- [ ] **5.2 ArkTS 私有字段禁用双下划线前缀**：不使用 `__xxx` 作为私有字段和方法命名；release 构建混淆时 `__` 前缀标识符会被重命名，运行时找不到属性或方法；改用 `_xxx` 或语义化命名。

- [ ] **5.3 平台 API 做版本兼容判断**：调用只在特定 API Level 下可用的接口前先判断平台版本；API12 相关能力、页面注册能力等不满足版本要求时提供降级实现或跳过路径。

- [ ] **5.4 自定义组件命名不与框架前缀冲突**：自定义组件和模块不使用框架保留前缀（如 `RCT`），否则运行时与框架内置对象冲突，导致注册失败或组件创建崩溃。

---

## 6. 华为官方通用稳定性规范补充

### 6.1 NDK / ArkTS so 引用一致性

- [ ] **6.1.1 so 名称一致**：`oh-package.json5` 依赖名、`CMakeLists.txt` 产物名、ArkTS import 名称、`.d.ts` 导出名保持一致，避免运行时加载失败或调用错位。

- [ ] **6.1.2 跨模块统一导出**：har/hsp 对外暴露 native 能力时，通过统一出口导出 napi 对象，不在业务侧散落引用内部 so。

### 6.2 Node-API / Ark Runtime

- [ ] **6.2.1 napi_get_cb_info 参数边界**：`argc` 已正确初始化；当 `argv != nullptr` 时，`argv` 长度大于等于声明的 `argc`，避免越界写。

- [ ] **6.2.2 handle scope 生命周期管理**：循环、异步回调、`uv_queue_work` 的 JS 线程回调中，创建 `napi_value` 时正确使用 `napi_open_handle_scope` / `napi_close_handle_scope`。

- [ ] **6.2.3 NAPI 线程归属正确**：不跨线程使用 `napi_env`、`napi_value`、`napi_ref`，`napi_add_env_cleanup_hook` / `napi_remove_env_cleanup_hook` 只在对应 JS 线程调用。

- [ ] **6.2.4 async work 生命周期正确**：`napi_delete_async_work` 放在 complete 回调中执行，不提前释放 `napi_async_work`。

- [ ] **6.2.5 wrap / ref 释放闭环**：`napi_wrap` 如果接收了 `napi_ref`，后续调用 `napi_remove_wrap` 或等效逻辑释放；强引用 `napi_ref` 使用后及时 `napi_delete_reference`。

- [ ] **6.2.6 ArrayBuffer 不手动释放**：通过 `napi_get_arraybuffer_info` / `napi_get_buffer_info` 获取的指针不手动 `delete/free`，内存由引擎管理。

- [ ] **6.2.7 external 对象不跨线程传递**：`napi_create_external` 系列创建的 JS 对象不通过 worker 或其他跨线程方式传递。

- [ ] **6.2.8 模块注册唯一且匹配**：`nm_register_func` 用 `static`，constructor 注册函数命名唯一，`.nm_modname` 与模块名完全匹配。

### 6.3 libuv / 事件循环 / fd

- [ ] **6.3.1 uv_work_t 只在 after_work_cb 中释放**：`uv_queue_work` 的 `uv_work_t` 及其绑定上下文不会在 work 完成前提前释放。

- [ ] **6.3.2 uv_work_t 与业务对象按需分离**：当业务对象和 libuv request 生命周期不完全同步时，通过 `work->data` 解耦，不让宿主析构顺带释放 `uv_work_t`。

- [ ] **6.3.3 uv_close 异步关闭规则**：调用 `uv_close` 后，不立即释放 Handle 本体；真正释放放在 close callback 中，且在 loop 所在线程调用。

- [ ] **6.3.4 单个 Handle 不跨 loop 重复初始化**：同一个 `uv_async_t` / `uv_timer_t` 等 Handle 不在多个事件循环上重复初始化。

- [ ] **6.3.5 主线程不调用阻塞式 libuv 接口**：文件同步读写、阻塞型 libuv 接口不在 UI 线程执行，避免 freeze。

- [ ] **6.3.6 uv_cancel 不等于跳过完成回调**：清理逻辑不依赖 `uv_cancel` 阻止 `after_work_cb`，释放动作仍放在完成回调中。

- [ ] **6.3.7 fd 所有权唯一**：遵守“谁申请谁释放”，close 后立即置 `-1`，透传 fd 时明确约定禁止重复 close。

### 6.4 ArkUI / 媒体 / IPC / 图形易错 API

- [ ] **6.4.1 Inspector 不进入业务路径**：不在正式业务代码中使用 Inspector 查询接口，避免性能问题和异常闪退。

- [ ] **6.4.2 XComponent / NativeWindow 生命周期正确**：注册到 `OH_NativeXComponent_RegisterCallback` 的回调在 `OnSurfaceDestroyed` 前持续有效；`nativewindow` 不在销毁后继续访问。

- [ ] **6.4.3 动画时机合法**：不在 `aboutToAppear` / `aboutToDisappear` 中调用 `animateTo`、`animateToImmediately`、`keyframeAnimateTo`。

- [ ] **6.4.4 动画闭包不破坏 if/else 数据保护**：动画中切换 `if/else` 分支时，要额外做空值保护或显式设置 `TransitionEffect.IDENTITY`，避免默认转场导致访问失效数据。

- [ ] **6.4.5 ArkUI 返回值判空 / 判默认值**：`currentOffset`、`getRectangleById`、`getInspectorByKey` 等接口结果使用前做空值、空串或全 0 默认值判断。

- [ ] **6.4.6 UIContext 相关 API 在正确上下文调用**：`@ohos.measure`、`@ohos.font` 等依赖 UI 上下文的接口通过 `this.getUIContext()` 对应方法调用，不在上下文不明确的生命周期里直接使用。

- [ ] **6.4.7 UIExtension 可用期判断**：`send` / `sendSync` 仅在 receiver 注册完成后调用；`onError` / `onRelease` / `onTerminated` 后立即视为失效。

- [ ] **6.4.8 监听注销使用同一回调实例**：媒体、会话、ArkUI 等 `on/off` 配对时，不用匿名函数注销已注册监听。

- [ ] **6.4.9 相机配置和释放顺序正确**：`CameraInput.open()` 后再 `addInput`，先 `addInput` 再 `addOutput`，配置包裹在 `beginConfig/commitConfig` 之间；`close/release` 等 Promise 接口使用 `await`。

- [ ] **6.4.10 音频回调和裸指针安全**：OHAudio `Release` 后不复用旧裸指针；回调结构体全部初始化；数据不足时避免向 buffer 写入不完整帧或错误使用无返回值回调。

- [ ] **6.4.11 IPC 传输边界与回收**：`writeString` 不超过 40960 字节；`MessageSequence` 总量不超过 200KB；`data/reply` 在取数结束后 `reclaim()`；Ashmem 释放顺序为 `unmap` 再 `close`；`ArrayBuffer` 读写类型码一致。

- [ ] **6.4.12 NativeWindow 不重复 destroy**：不在 `OH_NativeImage_Destroy` 已接管释放后再次调用 `OH_NativeWindow_DestroyNativeWindow`；不在 nativewindow 析构后继续调用接口。

### 6.5 底层 C++ 通用安全约束

- [ ] **6.5.1 指针 / 句柄 / BOOL 初始化**：指针变量、资源描述符、BOOL、类成员在声明或构造函数中赋初值；释放后立即置空、置新值或置 `-1`。

- [ ] **6.5.2 构造函数不做高风险操作**：构造函数中不做可能失败的操作、不创建线程；如分配了手动资源，类必须有对应析构函数；禁止 `delete this`。

- [ ] **6.5.3 函数参数与循环安全**：数组参数同时传长度；只读指针参数加 `const`；字符串 / 指针参数检查 NULL；循环具有明确退出条件。

- [ ] **6.5.4 字符串 / 数组 / 格式化安全**：外部输入作为索引或长度前先校验；格式串不受外部控制；格式参数的类型和数量与实参一致；字符串确保 `\0` 结束。

- [ ] **6.5.5 整数与指针安全**：检查溢出、除零、符号反转；不对有符号整数做位运算；不直接进行整数与指针互转，需使用 `intptr_t` 等安全类型。

- [ ] **6.5.6 内存申请与初始化**：申请前校验大小，申请后判空；不访问未初始化内存；释放后立即清理引用。

---

## 快速使用方式

1. **PR 全面自查**：逐条过一遍 1.1-6.5，对照改动代码标记 ✓（满足）/ ✗（不满足）/ N/A（不涉及）。
2. **模块专项检视**：根据模块性质选取相关分类，如 C++ 生命周期类变更重点看 1.x 和 2.x；多线程相关变更重点看 3.x。
3. **Bug 根因对照**：根据历史案例定位根因所属规范编号，确认修复模式是否符合对应规范。

原始规范全文路径（含历史案例和代码示例）：

- `docs/zh-cn/稳定性/稳定性问题介绍/稳定性编码规范.md`

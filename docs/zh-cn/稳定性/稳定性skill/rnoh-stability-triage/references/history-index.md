# 历史稳定性修复索引

这个索引用于快速匹配问题家族，不代替原始稳定性修复汇总。

使用方式：

1. 先根据日志关键词和模块名定位到问题类别。
2. 再根据版本范围查看相近分支是否已有修复。
3. 如果出现高匹配，再打开工作区中的原始汇总文档确认具体提交、MR、版本号和详细描述。

原始文档位置：

- docs/zh-cn/稳定性/稳定性历史修复/0.72稳定性修复汇总/
- docs/zh-cn/稳定性/稳定性历史修复/0.77稳定性修复汇总/
- docs/zh-cn/稳定性/稳定性历史修复/0.82稳定性修复汇总/

## 分类说明

稳定性修复汇总当前按 4 类组织：

1. 应用异常退出
2. 应用冻屏
3. 内存异常
4. 资源泄漏

## 分支概览

### 0.72 分支

- 条目总数约 95 条，覆盖面最广。
- 适合查找历史老问题、长期存在的问题家族、典型 crash 模式。
- 常见主题：UAF、空指针、Image 崩溃、Animated 崩溃、ScrollView 崩溃、生命周期清理、死锁、异常处理缺失、API 兼容。

### 0.77 分支

- 条目总数约 42 条。
- 聚焦中后期稳定性问题，很多条目与 0.82 存在同族关系。
- 常见主题：JSVM 销毁 UAF、图片回调悬空指针、EventBeat 生命周期问题、ArkUINode 初始化时序、FontRegistry 死锁、HostObjectProxy/JSVM 异常处理、混淆与 API12 兼容。

### 0.82 分支

- 条目总数约 19 条。
- 更适合匹配最近版本中已经确认的稳定性问题。
- 常见主题：JSVM 销毁、图片回调、TM/HTTP 资源泄漏、ArkUINode 初始化、Inspector WebSocket 生命周期、Create mutations 顺序风险、HostObjectProxy 和 JSVMPointerValue 崩溃、API12 与混淆兼容。

## 类别索引

### 1. 应用异常退出

常见关键词：

- SIGSEGV
- SIGABRT
- abort
- undefined error
- bundle load
- symbol conflict
- callback after destroy
- reload crash
- sort crash
- strict weak ordering
- stable_sort
- comparator
- container bounds
- out of bounds

高频模块：

- ArkUINode
- ArkTSWebsocket
- ArkUISurface
- NativeAnimatedTurboModule
- RNInstance
- RegisterPageNameTurboModule
- ResourceJSBundleProvider
- NativeNodeApi

代表性历史问题：

- 0.82.5：ArkUINode 在 NodeApi 未初始化完成时创建，createNode 返回空后断言 abort。提交 754a2da27，MR !2049。
- 0.82.5：Inspector WebSocket 关闭阶段继续访问已置空 socket。提交 fae39e8e0，MR !2040。
- 0.82.5：Modal 显示过早调用 getWindowDecorVisible，窗口状态未就绪时异常。提交 38e3b1f6f，MR !2019。
- 0.82.3：surface 停止时继续访问已销毁 animationDriver。提交 3477a6516，MR !1989。
- 0.82.3：AnimatedTurboModule 销毁后帧回调继续执行。提交 966d5fb27，MR !1988。
- 0.82.17：ArkTS 模块手工声明 px2vp，混淆后方法名无法解析并崩溃。提交 267143411，MR !2339。
- 0.82.17：API12 环境下 RegisterPageName 和 StatusBar delegate 适配不完整。提交 a23c64db2，MR !2227。
- 0.77.22：HSP 下 bundle 加载崩溃。提交 ee1fe3485，MR !1630。
- 0.72：std::sort 比较器不满足严格弱序（使用 `<=`）导致排序崩溃。提交 c822670b0。

优先匹配信号：

- 进程直接退出，并能看到 native crash、abort 或 JS 异常。
- 问题发生在启动、reload、关闭连接、实例销毁、surface stop 等阶段。
- 日志里出现 undefined error、abort、bundle 加载失败、符号冲突或销毁后回调继续执行。

### 2. 应用冻屏

常见关键词：

- deadlock
- re-entrant lock
- thread block
- freeze
- APP_INPUT_BLOCK
- LIFECYCLE_TIMEOUT
- reportMount
- FontRegistry

高频模块：

- FontRegistry
- ShadowTreeRegistry
- SchedulerDelegate
- TurboModuleProvider
- RNInstance

代表性历史问题：

- 0.82.17：reportMount 持锁回调导致 ShadowTreeRegistry 锁重入。提交 ab0d11db7，MR !2310。
- 0.82.3：FontRegistry 两把锁顺序不一致导致死锁。提交 c2815a9a7，MR !1987。
- 0.77.44：FontRegistry 锁顺序不一致导致死锁。提交 278db7406，MR !1984。
- 0.77.59：reportMount 期间 ShadowTreeRegistry 锁重入。提交 d4db9af69，MR !2283。
- 0.77.18：getTurboModule 持锁创建 TM 导致死锁并引发卡死。提交 7420d467f。
- 0.72.112：SafeAreaInsetsProvider 在 Worker 上下文同步取窗口属性导致卡死或崩溃。提交 280abaf70，MR !1918。

优先匹配信号：

- 进程仍然存活，但界面无响应、点击无反应或被系统判定为超时。
- 日志里出现死锁、锁重入、线程等待、THREAD_BLOCK、APP_INPUT_BLOCK、LIFECYCLE_TIMEOUT。
- 问题集中在主线程阻塞、锁竞争、同步等待或跨线程相互等待。

### 3. 内存异常

常见关键词：

- UAF
- dangling pointer
- dangling reference
- 裸指针
- 资源未释放
- memory leak
- c_str
- callback after destroy

高频模块：

- JSVMRuntime
- JSVMPointerValue
- ImageComponentInstance
- EventBeat
- ShadowView
- NetworkingTurboModule
- AppearanceTurboModule

代表性历史问题：

- 0.82.18：通过临时字符串的 c_str 向 ShadowView 传递组件名，存在悬空指针风险。提交 ab62e9d2f，MR !2384。
- 0.82.18：JSVM 销毁阶段继续清理失效引用，OH_JSVM_DestroyEnv 期间触发 UAF。提交 15c664801，MR !2387。
- 0.82.3：图片加载回调晚于实例销毁，读取 URI 裸指针导致崩溃。提交 409e3d355，MR !1985。
- 0.82.3：HTTP 请求回调和 TurboModule 析构阶段资源未释放导致内存泄漏。提交 c56478622，MR !1990。
- 0.77.59：JSVM 销毁阶段 UAF。提交 8707f1395，MR !2388。
- 0.77.44：图片回调晚于实例销毁，悬空 URI 崩溃。提交 ac5add422，MR !1949。
- 0.72.128：JS Runtime UAF 导致崩溃。提交 fdec3268a，MR !2380。
- 0.72.112：TurboModule 多处资源泄漏。提交 58179914c，MR !1964。

优先匹配信号：

- 崩溃线程是 JS 线程，且栈里出现 JSVM、Image、EventBeat、ShadowView。
- 销毁后回调仍执行。
- 日志出现释放后访问、悬空 URI、DestroyEnv、UAF、memory leak。

### 4. 资源泄漏

常见关键词：

- memory leak
- handle leak
- thread leak
- listener not removed
- callback not cleared
- cache not released
- request not cleaned up

高频模块：

- NetworkingTurboModule
- AppearanceTurboModule
- JSVM
- TurboModule
- ComponentInstance

代表性历史问题：

- 0.82.3：HTTP 请求回调和 TurboModule 析构阶段资源未释放导致内存泄漏。提交 c56478622，MR !1990。
- 0.77.40：创建并保存 JSVM code cache 后未释放句柄导致泄漏。提交 e6a993196，MR !1902。
- 0.72.112：TurboModule 多处资源泄漏。提交 58179914c，MR !1964。
- 0.72.59：ComponentInstance 内存泄漏。提交 4ae6d4561，MR !618。
- 0.72.128：runOnQueueSync 在 JS 线程上内存泄漏。提交 781706f14，MR !2263。

优先匹配信号：

- 应用短期不一定退出，但长时间运行后内存、线程数、句柄数持续增长。
- 日志和快照显示对象、线程、句柄、监听器或请求资源未按预期释放。
- 问题常与回调未解绑、cache 未释放、线程未回收、实例销毁后仍被持有有关。

## 建议的匹配策略

### 高匹配

同时满足以下三项中的两项以上：

- 模块或类名一致。
- 触发阶段一致。
- 错误类型一致。

### 中匹配

- 问题家族一致，但模块或路径不完全相同。
- 可以引用为“相似历史修复”。

### 低匹配

- 只有关键词重合。
- 只能作为排查方向，不能直接给升级或回捞结论。

## 归因提醒

- 有历史修复高匹配且用户版本较旧时，优先考虑框架已修复问题。
- 没有高匹配时，不要直接否定框架问题，还需要结合当前代码保护逻辑判断。
- 没有用户代码时，不要强行把问题归为用户实现错误。
- 但如果日志显示明显非法调用或接入不满足契约，也要如实指出更像用户代码问题。

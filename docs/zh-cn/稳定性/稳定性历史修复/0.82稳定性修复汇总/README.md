# 0.82 稳定性历史修复汇总

本文档汇总 0.82 分支中的稳定性历史修复，并按应用异常退出、应用冻屏、内存异常、资源泄漏四类重新编排，便于按故障现象回溯问题。各分类内条目均按修改日期由远到近排列。

## 分类汇总表

### 应用异常退出

| # | 修改日期 | 版本 | 问题描述 | 影响模块 | 问题类型 | 提交 / PR | 详细内容 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | 2026-01-07 | 0.82.3 | AnimatedTurboModule 销毁后回调仍执行导致崩溃 | NativeAnimatedTurboModule / AnimatedNodesManager | CppCrash | [966d5fb27](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/966d5fb27a221769f3b71d86b31608479106e8a6) / [!1988](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/1988) | [查看详情](01-应用异常退出.md#966d5fb27) |
| 2 | 2026-01-08 | 0.82.3 | surface 停止时访问已销毁 animationDriver 崩溃 | ArkUISurface / LayoutAnimation | CppCrash | [3477a6516](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/3477a651643df920d7651699b7a213ad2da45e71) / [!1989](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/1989) | [查看详情](01-应用异常退出.md#3477a6516) |
| 3 | 2026-01-13 | 0.82.5 | Modal 窗口状态未就绪时调用 getWindowDecorVisible | ModalHostView / DisplayMetrics / ArkTSBridge | JS Crash | [38e3b1f6f](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/38e3b1f6f93ba94d36dc957cbe53463b1ccc3ae3) / [!2019](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/2019) | [查看详情](01-应用异常退出.md#38e3b1f6f) |
| 4 | 2026-01-15 | 0.82.5 | Inspector WebSocket 关闭阶段访问已置空 socket | ArkTSWebsocket / Inspector | JS Crash | [fae39e8e0](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/fae39e8e03d603c15c47accbd611f78cf14b5b39) / [!2040](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/2040) | [查看详情](01-应用异常退出.md#fae39e8e0) |
| 5 | 2026-01-15 | 0.82.5 | NodeApi 未就绪时创建 ArkUINode 导致 abort | ArkUINode / NodeApi | CppCrash | [754a2da27](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/754a2da27b5f8c0c8d8f532fb32a5b47587be997) / [!2049](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/2049) | [查看详情](01-应用异常退出.md#754a2da27) |
| 6 | 2026-02-11 | 0.82.17 | API12 环境兼容性缺失导致崩溃 | RegisterPageNameTurboModule / react-native.patch | CppCrash | [a23c64db2](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/a23c64db2962bdba7c2b10dc88bc3441a0bda53a) / [!2227](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/2227) | [查看详情](01-应用异常退出.md#a23c64db2) |
| 7 | 2026-03-07 | 0.82.17 | 混淆后 px2vp 无法解析导致 undefined error(82) | SafeAreaInsetsProvider / KeyboardObserverTurboModule / StatusBarTurboModule | JS Crash | [267143411](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/2671434116bd5b60adf5719a3304012341a789a4) / [!2339](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/2339) | [查看详情](01-应用异常退出.md#267143411) |
| 8 | 2026-03-10 | 0.82.18 | Fabric Create mutations 重排风险 | SchedulerDelegate / Fabric Mounting | CppCrash | [fdfa1c7e3](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/fdfa1c7e35a9abb16f6079734275c1d8c93645f5) / [!2371](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/2371) | [查看详情](01-应用异常退出.md#fdfa1c7e3) |
| 9 | 2026-03-10 | 0.82.17 | ScrollView 滑动结束路径状态切换崩溃 | ScrollViewComponentInstance | CppCrash | [13806f11a](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/13806f11ab3b57b4fcfafb6b3eb541b67792f230) / [!2362](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/2362) | [查看详情](01-应用异常退出.md#13806f11a) |
| 10 | 2026-03-19 | 0.82分支 | Getter 跨线程共享状态导致间歇性崩溃 | HostObjectProxy / JSVMRuntime | CppCrash | [917398990](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/917398990a15d1515543b6f5faebb7179b823c27) / [!2396](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/2396) | [查看详情](01-应用异常退出.md#917398990) |

### 应用冻屏

| # | 修改日期 | 版本 | 问题描述 | 影响模块 | 问题类型 | 提交 / PR | 详细内容 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | 2026-01-07 | 0.82.3 | FontRegistry 锁顺序不一致导致死锁 | FontRegistry | 线程阻塞 | [c2815a9a7](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/c2815a9a7e5f85a836afe19567b197d0783742aa) / [!1987](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/1987) | [查看详情](02-应用冻屏.md#c2815a9a7) |
| 2 | 2026-03-04 | 0.82.17 | reportMount 触发 ShadowTreeRegistry 锁重入 | SchedulerDelegate / ShadowTreeRegistry | 线程阻塞 | [ab0d11db7](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/ab0d11db7249f65647d89775feb448a240afa64e) / [!2310](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/2310) | [查看详情](02-应用冻屏.md#ab0d11db7) |

### 内存异常

| # | 修改日期 | 版本 | 问题描述 | 影响模块 | 问题类型 | 提交 / PR | 详细内容 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | 2026-01-08 | 0.82.3 | 图片回调晚于实例销毁导致悬空 URI 崩溃 | ImageComponentInstance | 地址越界 | [409e3d355](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/409e3d355406c54c30d2bf19d706b15f2dc8e2df) / [!1985](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/1985) | [查看详情](03-内存异常.md#409e3d355) |
| 2 | 2026-03-07 | 0.82.17 | JSVMPointerValue 克隆与弱引用恢复路径崩溃 | JSVMRuntime / JSVMPointerValue | 地址越界 | [7029f4747](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/7029f47479d2a476a29c28e90a8dc18cb29ae1f0) / [!2337](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/2337) | [查看详情](03-内存异常.md#7029f4747) |
| 3 | 2026-03-09 | 0.82.17 | Enumerator 解包 Proxy 路径空指针崩溃 | HostObjectProxy | 地址越界 | [fd6f29ca0](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/fd6f29ca064698bdd98d7e6ca868a9d5c535f834) / [!2330](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/2330) | [查看详情](03-内存异常.md#fd6f29ca0) |
| 4 | 2026-03-10 | 0.82.17 | JSVMPointerValue 构造阶段触发 Mapping error82 | JSVMRuntime / JSVMPointerValue | 地址越界 | [431297d2e](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/431297d2e8a4d56cdd4e33b7a482c80e9300a2cc) / [!2361](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/2361) | [查看详情](03-内存异常.md#431297d2e) |
| 5 | 2026-03-12 | 0.82.18 | JSVM 销毁阶段 UAF 崩溃 | JSVMRuntime / JSVMPointerValue | 地址越界 | [15c664801](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/15c6648016a3b997855c65527a83f2940a9f94aa) / [!2387](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/2387) | [查看详情](03-内存异常.md#15c664801) |
| 6 | 2026-03-12 | 0.82.18 | ShadowView 组件名悬空指针风险 | MountingManagerCAPI / ShadowView | 地址越界 | [ab62e9d2f](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/ab62e9d2fcb6f4843c9ca3ffd671d2d07b617d75) / [!2384](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/2384) | [查看详情](03-内存异常.md#ab62e9d2f) |

### 资源泄漏

| # | 修改日期 | 版本 | 问题描述 | 影响模块 | 问题类型 | 提交 / PR | 详细内容 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | 2026-01-08 | 0.82.3 | TurboModule 与网络请求回调内存泄漏 | NetworkingTurboModule / AppearanceTurboModule / HTTP callbacks | 长期未释放资源 | [c56478622](https://gitcode.com/OpenHarmony-RN/ohos_react_native/commit/c56478622aee23466d20fcc72fee4ea876cf2d64) / [!1990](https://gitcode.com/OpenHarmony-RN/ohos_react_native/merge_requests/1990) | [查看详情](04-资源泄漏.md#c56478622) |

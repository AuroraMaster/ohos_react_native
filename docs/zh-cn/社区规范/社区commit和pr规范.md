# 社区 Commit 和 PR 规范

本规范与前文《社区 Changelog 规范》配套使用，基于 Conventional Commits 1.0.0 标准制定，确保 Commit 信息可直接用于自动化生成 Changelog，同时规范 PR 协作流程，降低社区协作成本。

## 一、Commit 提交规范

### 1. 核心格式（强制）

Commit 信息必须严格遵循以下结构，分为标题行、主体内容、页脚三部分，各部分之间空一行：

```plaintext
<type>(<scope>): <subject>

<body>

<footer>
```

#### （1）标题行（强制，单行，≤100 字符）

**type（类型，强制）**：与 Changelog 变更类型一一对应，仅可使用以下值：

| type 值 | 对应 Changelog 类型 | 说明 |
| :--- | :--- | :--- |
| feat | Added | 新增功能、特性、模块、API |
| fix | Fixed | 修复 Bug、异常、兼容性问题 |
| docs | Documentation | 文档变更（新增教程、补全 API 文档、勘误等） |
| style | - | 代码格式调整（不影响功能逻辑，如缩进、分号、空格等） |
| refactor | Changed | 代码重构（既不新增功能也不修复 Bug，但改变现有逻辑） |
| perf | Performance | 性能优化（不改变功能逻辑） |
| test | - | 测试相关变更（新增 / 修改测试用例） |
| chore | - | 构建流程、工具链、依赖管理相关变更（不影响用户使用） |
| deprecated | Deprecated | 标记计划在未来版本中移除的功能、API、配置项 |
| removed | Removed | 正式移除已标记为弃用的功能、API、配置项、依赖 |
| security | Security | 安全漏洞、隐私合规相关的修复与优化 |
| revert | - | 回滚之前的 commit |

**scope（作用域，必选）**：说明变更影响的组件/模块，使用 PascalCase 命名规范

**subject（主题，强制）**：简洁描述变更内容，要求：
- 使用动词原形、现在时（如 add 而非 added/adds）
- 首字母小写，结尾不加句号
- 聚焦 "做了什么"，而非 "为什么做"

#### （2）主体内容（可选，可多行）

- 说明变更的动机（为什么做这个变更）
- 对比变更前后的行为差异
- 技术实现细节（可选）
- 影响范围说明

#### （3）页脚（可选，多行）

- 关联 Issue/PR：格式为 Closes #123、Fixes #456、Related to #789
- 破坏性变更（强制标注）：若存在破坏性变更，必须以 BREAKING CHANGE: 开头，详细说明变更内容、影响范围、适配方案
- 签名：Signed-off-by: Your Name <email@example.com>

### 2. RNOH Scope 规范

Scope 必须使用以下预定义值（PascalCase 命名）：

#### 基础组件
- View, Text, Image, TextInput, ScrollView, FlatList, ListView, WebView, Modal, RefreshControl, ActivityIndicator

#### 布局组件
- Flex, Absolute, SafeArea, KeyboardAvoidingView

#### API 模块
- Networking, Storage, AsyncStorage, Clipboard, Linking, Alert, Toast, Permissions, DeviceInfo

#### 动画系统
- Animated, Reanimated, LayoutAnimation, GestureHandler

#### 导航
- Navigation, ReactNavigation, StackNavigator

#### 样式系统
- StyleSheet, StyleSheetOptimizer, Theme

#### 桥接层
- Bridge, TurboModules, Fabric, LegacyComponents

#### JS 引擎
- Hermes, JSC, JSI, JSVM

#### 构建系统
- BuildSystem, CMake, HAP, HAR, Symbols

#### 开发工具
- DevTools, Debugger, Profiler, Flipper

#### 测试
- Tests, E2E, UnitTests

#### 其他
- Docs, Examples, Config, CLI, Scripts

### 3. RNOH 特殊场景 Commit 要求

#### Symbols 变更
必须说明符号处理情况：
- 符号分离方式
- 剥离策略
- 符号文件路径

#### Native 层变更
必须说明：
- 符号处理情况
- .so 文件影响
- 兼容性说明

#### 构建系统变更
必须说明对以下模式的影响：
- Debug 模式
- Release 模式
- Release2 模式

#### 组件变更
必须说明：
- 兼容性影响
- API 变化
- 破坏性变更适配方案

### 4. Commit 模板（可直接复制使用）

将以下内容保存为项目根目录的 `.gitmessage` 文件，执行 `git config commit.template .gitmessage` 即可启用模板：

```plaintext
# <type>(<scope>): <subject> (≤100字符)
#
# type 类型（必选）:
#   feat        - 新增功能
#   fix         - 修复bug
#   docs        - 文档更新
#   style       - 代码格式
#   refactor    - 代码重构
#   perf        - 性能优化
#   test        - 测试相关
#   chore       - 构建/工具链
#   deprecated  - 标记弃用功能
#   removed     - 移除已弃用功能
#   security    - 安全修复
#   revert      - 回滚
#
# scope 作用域（必选，使用PascalCase）:
#   基础组件: View, Text, Image, TextInput, ScrollView, FlatList, ListView, WebView, Modal, RefreshControl, ActivityIndicator
#   布局组件: Flex, Absolute, SafeArea, KeyboardAvoidingView
#   API模块: Networking, Storage, AsyncStorage, Clipboard, Linking, Alert, Toast, Permissions, DeviceInfo
#   动画系统: Animated, Reanimated, LayoutAnimation, GestureHandler
#   导航: Navigation, ReactNavigation, StackNavigator
#   样式系统: StyleSheet, StyleSheetOptimizer, Theme
#   桥接层: Bridge, TurboModules, Fabric, LegacyComponents
#   JS引擎: Hermes, JSC, JSI, JSVM
#   构建系统: BuildSystem, CMake, HAP, HAR, Symbols
#   开发工具: DevTools, Debugger, Profiler, Flipper
#   测试: Tests, E2E, UnitTests
#   其他: Docs, Examples, Config, CLI, Scripts
#
# subject 主题（必选）:
#   - 使用动词原形、现在时（如 add 而非 added/adds）
#   - 首字母小写，结尾不加句号
#   - 聚焦 "做了什么"，而非 "为什么做"
#
# 示例:
#   feat(ScrollView): 添加下拉刷新支持
#   fix(Hermes): 修复内存泄漏问题
#   chore(Symbols): 优化Release模式符号处理

# 主体内容（空一行后开始）
# - 说明变更动机（为什么做这个变更）
# - 对比变更前后的行为差异
# - 技术实现细节（可选）
# - 影响范围说明
#
# RNOH特别提示:
# - Symbols变更: 说明符号处理情况（符号分离、剥离、路径等）
# - Native层变更: 说明符号处理和.so文件影响
# - 构建系统变更: 说明对Debug/Release模式的影响
# - 组件变更: 说明兼容性和API变化

# 页脚（空一行后开始）
# - 关联 Issue: Closes #123, Fixes #456, Related to #789
# - 破坏性变更: BREAKING CHANGE: 详细说明变更内容、影响范围、适配方案
# - 签名: Signed-off-by: Your Name <email@example.com>
```

### 5. Commit 示例

#### （1）普通功能新增

```plaintext
feat(ScrollView): 添加下拉刷新支持

新增 ScrollView 下拉刷新功能，支持自定义刷新指示器和回调处理，提升用户体验。

- 实现onRefresh回调机制
- 添加RefreshControl集成
- 支持自定义刷新指示器样式

Closes #234
Signed-off-by: Your Name <email@example.com>
```

#### （2）Bug 修复

```plaintext
fix(Hermes): 修复内存泄漏问题

问题描述：JS对象未正确释放导致内存持续增长
解决方案：在JSI层添加引用计数管理，确保对象生命周期正确
影响范围：所有使用Hermes引擎的场景

Fixes #189
Signed-off-by: Your Name <email@example.com>
```

#### （3）构建系统变更

```plaintext
chore(Symbols): 优化Release模式符号处理

使用llvm-objcopy分离符号到.sym文件，减小HAR包体积约30%。

符号处理流程：
1. 编译生成包含完整符号的.so文件
2. 使用llvm-objcopy提取符号到.sym文件
3. 使用llvm-objcopy剥离.so中的符号
4. 使用llvm-objcopy添加调试链接
5. 只打包剥离后的.so到har包
6. .sym文件单独保存到symbols/目录

影响范围：
- Release模式：.so文件体积减小30%，符号单独保存
- Release2模式：同Release模式
- Debug模式：不受影响，符号保留在.so中

Closes #456
Signed-off-by: Your Name <email@example.com>
```

#### （4）破坏性变更

```plaintext
refactor(Bridge): 升级TurboModules接口签名算法

重构TurboModules接口签名算法，提升接口安全性，旧版签名方式不再兼容。

BREAKING CHANGE:
- 变更内容：TurboModules接口签名算法升级
- 影响范围：所有使用TurboModules的Native模块
- 适配方案：参考文档《TurboModules接口升级指南》更新模块代码

Closes #789
Signed-off-by: Your Name <email@example.com>
```

## 二、PR 提交规范

### 1. PR 核心要求

- **"一个 PR 只做一件事"**：避免将多个无关变更合并到一个 PR 中，确保 Review 与回滚可控
- **标题遵循 Commit 规范**：PR 标题直接对应最终合并的 Commit 标题（Squash Merge 场景下）
- **描述完整清晰**：通过模板说明变更内容的内容、动机、测试情况，降低 Review 成本

### 2. PR 描述模板（可直接复制使用）

在 GitCode 仓库设置中添加 PR 模板（`.gitcode/PULL_REQUEST_TEMPLATE.md`）：

```markdown
## 变更类型

请在对应类型前打 `√`，仅可选择一项：

- [ ] feat（新增功能，对应 Changelog Added）
- [ ] fix（修复 Bug，对应 Changelog Fixed）
- [ ] docs（文档变更，对应 Changelog Documentation）
- [ ] refactor（代码重构，对应 Changelog Changed）
- [ ] perf（性能优化，对应 Changelog Performance）
- [ ] test（测试相关）
- [ ] chore（构建/工具链/依赖）
- [ ] deprecated（标记弃用，对应 Changelog Deprecated）
- [ ] removed（移除功能，对应 Changelog Removed）
- [ ] security（安全修复，对应 Changelog Security）
- [ ] revert（回滚）

## 变更 Scope

请选择变更影响的组件/模块（可多选）：

- [ ] 基础组件（View, Text, Image, ScrollView等）
- [ ] 布局组件（Flex, Absolute, SafeArea等）
- [ ] API模块（Networking, Storage, Permissions等）
- [ ] 动画系统（Animated, Reanimated, GestureHandler等）
- [ ] 导航（Navigation, StackNavigator等）
- [ ] 样式系统（StyleSheet, Theme等）
- [ ] 桥接层（Bridge, TurboModules, Fabric等）
- [ ] JS引擎（Hermes, JSC, JSI, JSVM等）
- [ ] 构建系统（BuildSystem, CMake, HAP, HAR, Symbols等）
- [ ] 开发工具（DevTools, Debugger, Profiler等）
- [ ] 测试（Tests, E2E, UnitTests）
- [ ] 其他（Docs, Examples, Config, CLI, Scripts等）

## 变更内容

- 简要描述本次变更的具体内容
- 若为破坏性变更，请详细说明影响范围与适配方案

## RNOH 特殊说明

- [ ] Symbols变更（说明符号处理情况）
- [ ] Native层变更（说明符号处理和.so文件影响）
- [ ] 构建系统变更（说明对Debug/Release模式的影响）
- [ ] 组件变更（说明兼容性和API变化）

## 关联 Issue

- Closes #123（自动关闭 Issue）
- Related to #456（仅关联不关闭）

## 测试情况

- [ ] 已通过本地完整测试（单元测试/集成测试）
- [ ] 已通过 CI 自动构建与测试
- [ ] 已补充/更新对应的测试用例
- [ ] 已在目标环境（HarmonyOS设备/模拟器）验证功能

## 检查项

- [ ] 代码符合项目编码规范
- [ ] 已更新相关文档（若涉及功能变更）
- [ ] 无敏感信息泄露
- [ ] 已同步更新 Changelog（可选，建议随 PR 同步更新 `[Unreleased]` 区块）
- [ ] Commit 信息符合规范（包含正确的 type 和 scope）
```

### 3. PR 协作流程

1. **Draft 状态启动**：PR 创建时先设为 Draft 状态，完成开发、自测、文档更新后再转为 Ready for Review
2. **Review 要求**：至少 1 名核心维护者 Review 通过后方可合并，大型变更需 2 名及以上维护者 Review
3. **合并方式**：
   - 小型变更（单 Commit）：使用 Squash Merge，合并后 Commit 标题与 PR 标题一致
   - 大型变更（多 Commit 且逻辑清晰）：使用 Rebase Merge，保留 Commit 历史
4. **合并后同步**：合并后由维护者或自动化工具同步更新 Changelog 的 [Unreleased] 区块，版本发布时统一归档

## 三、与 Changelog 的联动规则

1. **自动化生成基础**：Commit 的 type 直接映射 Changelog 的变更类型，subject + body 可作为 Changelog 条目的原始素材
2. **破坏性变更同步**：Commit/PR 中标记的 BREAKING CHANGE 必须在 Changelog 中单独高亮，并补充适配指南
3. **版本发布流程**：
   - 发布前将 Changelog 的 [Unreleased] 区块改为对应版本号 + 日期
   - 基于 Commit 历史补充遗漏的变更条目
   - 执行版本发布，同步更新 Release 说明与 Changelog

## 四、Commitlint 校验

项目已配置 commitlint 自动校验，确保 Commit 信息符合规范：

### 校验规则

1. **type 校验**：必须是预定义的类型之一
2. **scope 校验**：必须是预定义的 scope 之一，使用 PascalCase 命名
3. **subject 校验**：不能为空，首字母小写，结尾不加句号
4. **header 长度**：不超过 100 字符
5. **签名校验**：必须包含 Signed-off-by

### 安装和使用

```bash
# 安装依赖
npm install --save-dev @commitlint/cli @commitlint/config-conventional husky

# 配置 Git hooks
npx husky install
npx husky add .husky/commit-msg 'npx --no -- commitlint --edit $1'

# 启用 Commit 模板
git config commit.template .gitmessage
```

### 使用方式

```bash
# 使用模板辅助
git commit

# 普通提交（会自动校验）
git commit -m "feat(ScrollView): 添加下拉刷新支持"
```

### 常见错误

1. **scope 不存在**
   ```
   ✖   scope must be one of [View, Text, Image, ...]
   ✖   found scroll-view in commit message header
   ```

2. **scope 大小写错误**
    ```
    ✖   scope must be PascalCase
    ✖   found SCROLLVIEW in commit message header
    ```

3. **缺少签名**
   ```
   ✖   commit message must contain a signed-off-by line
   ```

4. **header 过长**
   ```
   ✖   header may not be longer than 100 characters
   ```

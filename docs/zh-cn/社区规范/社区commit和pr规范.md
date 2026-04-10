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

#### （1）标题行（强制，单行，≤72 字符）

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

**scope（作用域，可选）**：说明变更影响的模块 / 组件，如 auth、user-list、api-v2，无明确作用域可省略

**subject（主题，强制）**：简洁描述变更内容，要求：
- 使用动词原形、现在时（如 add 而非 added/adds）
- 首字母小写，结尾不加句号
- 聚焦 "做了什么"，而非 "为什么做"

#### （2）主体内容（可选，多行）

- 说明变更的动机（为什么做这个变更）
- 对比变更前后的行为差异
- 避免技术实现细节，保持对用户 / 贡献者友好

#### （3）页脚（可选，多行）

- 关联 Issue/PR：格式为 Closes #123、Fixes #456、Related to #789
- 破坏性变更（强制标注）：若存在破坏性变更，必须以 BREAKING CHANGE: 开头，详细说明变更内容、影响范围、适配方案

### 2. Commit 模板（可直接复制使用）

将以下内容保存为项目根目录的 `.gitmessage` 文件，执行 `git config commit.template .gitmessage` 即可启用模板：

```plaintext
# <type>(<scope>): <subject> (≤72字符)
# 可选 type: feat|fix|docs|style|refactor|perf|test|chore
# 示例: feat(auth): add sms two-factor verification

# 主体内容（空一行后开始）
# - 说明变更动机
# - 对比变更前后的行为差异

# 页脚（空一行后开始）
# - 关联 Issue: Closes #123
# - 破坏性变更: BREAKING CHANGE: 详细说明变更与适配方案
```

### 3. Commit 示例

#### （1）普通功能新增

```plaintext
feat(user): add batch role permission management

新增用户角色权限批量分配功能，支持通过 CSV 导入或多选用户进行批量操作，降低管理员配置成本。

Closes #234
```

#### （2）Bug 修复

```plaintext
fix(upload): resolve image upload failure on iOS 18

修复 iOS 18 系统下因 WebKit 表单数据解析规则变更导致的图片上传失败问题，兼容 iOS 16+ 全版本。

Fixes #189
```

#### （3）破坏性变更

```plaintext
refactor(auth): upgrade signature algorithm from MD5 to SHA256

重构用户认证接口签名算法，提升接口安全性，旧版 MD5 签名方式不再兼容。

BREAKING CHANGE:
:
- 变更内容：认证接口签名算法由 MD5 升级为 SHA256
- 影响范围：所有调用 `/v1/auth/*` 接口的下游应用
- 适配方案：参考文档《认证接口签名算法升级指南》替换签名逻辑，测试通过后再升级

Closes #456
```

## 二、PR 提交规范

### 1. PR 核心要求

- **"一个 PR 只做一件事"**：避免将多个无关变更合并到一个 PR 中，确保 Review 与回滚可控
- **标题遵循 Commit 规范**：PR 标题直接对应最终合并的 Commit 标题（Squash Merge 场景下）
- **描述完整清晰**：通过模板说明变更内容的内容、动机、测试情况，降低 Review 成本

### 2. PR 描述模板（可直接复制使用）

在 GitHub/GitLab 仓库设置中添加 PR 模板（`.github/PULL_REQUEST_TEMPLATE.md`）：

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

## 变更内容

- 简要描述本次变更的具体内容
- 若为破坏性变更，请详细说明影响范围与适配方案

## 关联 Issue

- Closes #123（自动关闭 Issue）
- Related to #456（仅关联不关闭）

## 测试情况

- [ ] 已通过本地完整测试（单元测试/集成测试）
- [ ] 已通过 CI 自动CI 构建与测试
- [ ] 已补充/更新对应的测试用例
- [ ] 已在目标环境（如 iOS/Android/浏览器）验证功能

## 检查项

- [ ] 代码符合项目编码规范
- [ ] 已更新相关文档（若涉及功能变更）
- [ ] 无敏感信息泄露
- [ ] 已同步更新 Changelog（可选，建议随 PR 同步更新 `[Unreleased]` 区块）
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

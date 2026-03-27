# React Native for OpenHarmony (RNOH)

## 📢 代码仓迁移公告

各位开发者、合作伙伴：

大家好！

为更好地聚焦 React Native 于 OpenHarmony 生态的建设与发展，RN Sig（React Native 特别兴趣组） 已正式成立。作为 RN Sig 运作的第一步，我们将对 RNOH（React Native OpenHarmony）源码仓库进行组织架构迁移。

### 一、迁移内容
- **原仓库地址**：https://gitcode.com/openharmony-sig/ohos_react_native
- **新仓库地址**：https://gitcode.com/openharmony-RN/ohos_react_native

### 二、迁移原因
- **明确责任归属**：RN Sig 的成立标志着 RNOH 进入独立、专项化运作阶段，新组织空间更符合 SIG 实体定位，便于后续版本规划与治理。
- **优化协作路径**：独立组织空间可更清晰地区分 RN 生态相关代码与 OpenHarmony SIG 下的其他孵化项目，降低协作混淆风险。
- **长期演进需要**：为 RNOH 后续多仓管理、CI/CD 策略差异化、文档与示例统一归口奠定基础。

### 三、影响说明
- **访问地址变更**：原有基于 openharmony-sig 的仓库地址将不再更新，请各相关方及时更新本地仓库远程地址。
- **权限与订阅**：新组织下的权限体系将按 RN Sig 成员重新梳理，代码提交、Issue 与 PR 入口将统一迁移至新地址。
- **兼容性**：本次仅为仓库物理位置迁移，代码内容、版本历史、分支信息均完整保留，不影响现有功能与 API 兼容性。

### 四、后续安排
迁移执行期间，仓库将短暂处于只读状态，完成后原仓库将标记为"已归档"。

请各依赖方在 【具体时间点】 前完成本地仓库的远程地址切换，避免提交失败。

感谢大家一直以来对 RNOH 的关注与贡献，RN Sig 将持续推动 React Native 在 OpenHarmony 上的能力完善与生态繁荣。如有疑问，欢迎联系 RN Sig 邮箱或在本公告下留言。

                            **RN Sig 工作小组**  
                            2026年03月26日

---

欢迎阅读React Native for OpenHarmony文档，参与React Native for OpenHarmony开发者文档开源项目，与我们一起完善开发者文档。

## RNOH版本演进规划和分支策略
您可以在[RNOH版本演进规划和分支策略](https://gitcode.com/openharmony-sig/ohos_react_native/blob/master/docs/zh-cn/roadmap.md)中了解更多关于我们对React Native的OpenHarmony适配版本的说明。

## 启动

### 前提条件

#### 安装了HarmonyOS NEXT的模拟器或真机

该项目需要HarmonyOS NEXT上进行测试。

#### 华为开发者账号

运行应用程序时，需要用到华为开发者账号进行签名。

#### DevEco Studio

从[官网](https://developer.huawei.com/consumer/cn/deveco-studio/)下载并安装最新版本 DevEco Studio。

### 启动项目

> 如果是编译源码，在拉取工程后需要在工程的根目录执行`git submodule update --init --recursive`命令，拉取三方库依赖。

1. 打开终端（命令行工具）并导航到react-native-harmony-cli目录，然后执行 `npm i && npm pack`。
1. 在终端中导航到react-native-harmony目录，然后执行 `npm i && npm pack`。
1. 在终端中导航到react-native-harmony-sample-package目录，然后执行 `npm i && npm pack`。
1. 导航到tester目录，然后执行 `npm run i` （不是 npm i）。
1. 在tester目录中运行 `npm start` 以启动Metro服务器。
1. 设置`RNOH_C_API_ARCH`环境变量的值为`1`，具体参考[配置CAPI版本环境变量](docs/zh-cn/环境搭建.md#set_capi_path)。
1. 在DevEco Studio中打开 `tester/harmony` 项目并等待后台同步完成。
1. 连接设备。
1. 点击 File > Project Structure > Signing Configs，登录并完成签名。
1. 选择 `entry`（右上角）。
1. 点击【Run 'entry'】按钮（右上角）运行项目。

**说明：** 该tester项目工程主要用于开发自测试各种用例场景，如果在运行tester中的用例时遇到问题，通过提issues来咨询。

## 使用说明

- React Native的使用问题可查阅[React Native 官网](https://reactnative.dev/)或[React Native 中文网](https://reactnative.cn/)。
- RNOH的使用问题可查阅[中文文档](./docs/zh-cn/README.md)或[英文文档](./docs/en/README.md)。

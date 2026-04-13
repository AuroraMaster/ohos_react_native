# RNOH 构建与符号管理工程级指南

## 1. 概述

本文档描述了 RNOH (React Native OpenHarmony) 项目的构建流程和符号管理策略，确保在 Debug 和 Release 模式下都能正确处理符号信息，便于开发和调试。

## 2. 构建模式对比

| 配置项 | Debug 模式 | Release 模式 |
|--------|-----------|--------------|
| CMAKE_BUILD_TYPE | Debug | Release |
| nativeLib.debugSymbol.strip | false | true |
| 符号处理 | 保留在 .so 中 | 分离为独立 .sym 文件 |
| 优化级别 | 无 | 最高 |
| 适合场景 | 开发调试 | 生产发布 |

## 3. 符号管理策略

### 3.1 业界最佳实践

**不推荐**在 Release 版本的 har 包中包含符号文件，原因如下：

1. **包体积**：符号文件可能很大（几MB到几十MB）
2. **安全性**：符号信息暴露代码结构
3. **分发效率**：增加网络传输和部署时间
4. **不符合规范**：Release版本应该优化体积

### 3.2 符号处理流程

#### Debug 模式
```
1. 编译生成 .so 文件（包含完整符号）
2. 不生成 .sym 文件
3. 直接打包 .so 到 har 包
```

#### Release 模式
```
1. 编译生成 .so 文件（包含完整符号）
2. 使用 llvm-objcopy 提取符号到 .sym 文件
3. 使用 llvm-objcopy 剥离 .so 中的符号
4. 使用 llvm-objcopy 添加调试链接
5. 只打包剥离后的 .so 到 har 包
6. .sym 文件单独保存到 symbols/ 目录
```

## 4. 本地编译配置

### 4.1 本地编译入口

本地编译使用 `D:\82\packages\tester\harmony\entry\build-profile.json5`，CMake 路径为 `./src/main/cpp/CMakeLists.txt`,需要根据自身工程设计对应调整。

### 4.2 build-profile.json5配置

参考配置文件：[build-profile_module.json5](../../../packages/react-native-harmony/scripts/resources/build-profile_module.json5)

```json5
{
  "apiType": "stageMode",
  "targets": [
    {
      "name": "default",
      "runtimeOS": "HarmonyOS"
    }
  ],
  "buildOptionSet": [
    {
      "name": "release",
      "externalNativeOptions": {
        "path": "./src/main/cpp/CMakeLists.txt",
        "arguments": [
          "-DRNOH_APP_DIR=${RNOH_CPP_DIR}",
          "-DLOG_VERBOSITY_LEVEL=1",
          "-DWITH_HITRACE_SYSTRACE=ON",
          "-DWITH_HITRACE_REACT_MARKER=ON",
          "-DADD_JSVM_SO=ON",
          "-DCMAKE_BUILD_TYPE=RelWithDebInfo"
        ],
        "cppFlags": "-fstack-protector-strong -Wl,-z,noexecstack"
      },
      "nativeLib": {
        "debugSymbol": {
          "strip": true,
          "exclude": [
            "librnoh_core.so",
            "librnoh_core_package.so",
            "libreactnative.so",
            "libjsvmtooling.so"
          ]
        },
        "headerPath": [
          "./src/main/include"
        ],
        "librariesInfo": [
          {
            "name": "librnoh_core.so",
            "linkLibraries": [
              "libace_napi.z.so",
              "libace_ndk.z.so",
              "libbundle_ndk.z.so",
              "libdeviceinfo_ndk.z.so",
              "libhilog_ndk.z.so",
              "librawfile.z.so",
              "libnative_vsync.so",
              "libnative_drawing.so",
              "libnative_display_soloist.so",
              "libqos.so",
              "react-native-openharmony::reactnative",
              "react-native-openharmony::jsi"
            ]
          }
        ]
      }
    }
  ]
}
```

### 4.3 CMakeLists.txt配置

在 [CMakeLists.txt](../../../packages/tester/harmony/react_native_openharmony/src/main/cpp/CMakeLists.txt) 中，符号分离通过CMakeLists.txt 中添加额外命令。
```MakeFile
# 使用 add_custom_target 实现跨目录符号分离
function(separate_debug_symbols)
    # 只在 Release 或 RelWithDebInfo 模式执行
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        return()
    endif()

    # 检查 CMAKE_OBJCOPY 是否存在
    if(NOT CMAKE_OBJCOPY)
        message(WARNING "CMAKE_OBJCOPY not found, skip debug symbol separation")
        return()
    endif()

    # 创建一个全局的符号分离目标
    set(symbol_target "separate_all_debug_symbols")
    if(NOT TARGET ${symbol_target})
        add_custom_target(${symbol_target} ALL
            COMMENT "Start separating all debug symbols..."
        )
    endif()

    # 遍历所有传入的目标
    foreach(target ${ARGN})
        if(NOT TARGET ${target})
            message(WARNING "${target} not found, skip")
            continue()
        endif()

        # 获取目标文件路径
        set(target_file "$<TARGET_FILE:${target}>")
        set(sym_file "${target_file}.sym")

        # 为每个目标添加自定义命令，依赖于原目标
        add_custom_command(TARGET ${symbol_target} POST_BUILD
            # 1. 导出符号
            COMMAND ${CMAKE_OBJCOPY} --only-keep-debug
                "${target_file}"
                "${sym_file}"
                || true
            COMMENT "Export debug symbols to: ${target}.sym"

            # 2. 剥离符号
            COMMAND ${CMAKE_OBJCOPY} --strip-debug --strip-unneeded
                "${target_file}"
                || true
            COMMENT "Strip debug symbols from file: ${target}.so"

            # 3. 关联符号
            COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink="${sym_file}"
                "${target_file}"
                || true
            COMMENT "Add debug symbol link to: ${target}.so, debug symbol file is: ${target}.so.sym"
        )

        # 确保符号分离在原目标编译完成后执行
        add_dependencies(${symbol_target} ${target})
    endforeach()
endfunction()

# 调用符号分离（确认目标名正确）
separate_debug_symbols(
    rnoh_core
    rnoh_core_package
    reactnative
    jsvmtooling
)
```

## 5. 流水线编译配置

### 5.1 流水线编译入口

流水线编译使用 `D:\82\packages\react-native-harmony\scripts\build-har.js`

### 5.2 [build-profile_module.json5](../../../packages/react-native-harmony/scripts/resources/build-profile_module.json5)配置

```json5
{
  "apiType": "stageMode",
  "targets": [
    {
      "name": "default",
      "runtimeOS": "HarmonyOS"
    }
  ],
  "buildOptionSet": [
    {
      "name": "release",
      "externalNativeOptions": {
        "path": "./src/main/cpp/CMakeLists.txt",
        "arguments": [
          "-DRNOH_APP_DIR=${RNOH_CPP_DIR}",
          "-DLOG_VERBOSITY_LEVEL=1",
          "-DWITH_HITRACE_SYSTRACE=ON",
          "-DWITH_HITRACE_REACT_MARKER=ON",
          "-DADD_JSVM_SO=ON",
          "-DCMAKE_BUILD_TYPE=RelWithDebInfo"
        ],
        "cppFlags": "-fstack-protector-strong -Wl,-z,noexecstack"
      },
      "nativeLib": {
        "debugSymbol": {
          "strip": true,
          "exclude": [
            "librnoh_core.so",
            "librnoh_core_package.so",
            "libreactnative.so",
            "libjsvmtooling.so"
          ]
        },
        "headerPath": [
          "./src/main/include"
        ],
        "librariesInfo": [
          {
            "name": "librnoh_core.so",
            "linkLibraries": [
              "libace_napi.z.so",
              "libace_ndk.z.so",
              "libbundle_ndk.z.so",
              "libdeviceinfo_ndk.z.so",
              "libhilog_ndk.z.so",
              "librawfile.z.so",
              "libnative_vsync.so",
              "libnative_drawing.so",
              "libnative_display_soloist.so",
              "libqos.so",
              "react-native-openharmony::reactnative",
              "react-native-openharmony::jsi"
            ]
          }
        ]
      }
    }
  ]
}
```

### 5.3 build-har.js符号文件处理

```javascript
function copySymFiles(symDirPath, outputDir) {
  try {
    console.log(`Copying symbol files to: ${outputDir}`);

    if (!fs.existsSync(symDirPath)) {
      console.log(`Symbol directory not found: ${symDirPath}`);
      return;
    }

    const symFiles = fs.readdirSync(symDirPath).filter(file => file.endsWith('.sym'));

    if (symFiles.length === 0) {
      console.log('No .sym files found in symbol directory');
      return;
    }

    if (!fs.existsSync(outputDir)) {
      fs.mkdirSync(outputDir, { recursive: true });
    }

    console.log(`Found ${symFiles.length} .sym files`);

    symFiles.forEach(symFile => {
      const srcPath = pathUtils.join(symDirPath, symFile);
      const destPath = pathUtils.join(outputDir, symFile);
      fs.copyFileSync(srcPath, destPath);
      console.log(`Copied ${symFile}`);
    });
  } catch (err) {
    console.error(`Failed to copy symbol files: ${err}`);
  }
}
```

### 5.4 符号文件路径

符号文件生成在以下路径：
- Release: `build/default/intermediates/cmake/default/default/obj/arm64-v8a`
- Release2: `build/release2/intermediates/cmake/default/default/obj/arm64-v8a`

符号文件最终保存到：
- Release: `./symbols/release`
- Release2: `./symbols/release2`

## 6. package.json配置

```json
{
  "files": [
    "./Libraries/**/*",
    "./delegates/**/*",
    "./src/**/*",
    "./types/**/*",
    "./*.js",
    "./*.json",
    "./LICENSE",
    "./LICENSE-Meta",
    "./NOTICE.md",
    "./metro.config.d.ts",
    "./react_native_openharmony.har",
    "./react_native_openharmony_release.har",
    "./react_native_openharmony_release2.har",
    "./symbols/**/*"
  ]
}
```

## 7. 构建产物

### 7.1 har包内容

- `react_native_openharmony.har` - Debug版本，包含完整符号的.so文件
- `react_native_openharmony_release.har` - Release版本，包含剥离符号的.so文件
- `react_native_openharmony_release2.har` - Release2版本，包含剥离符号的.so文件

### 7.2 符号文件

- `symbols/release/` - Release版本的符号文件
- `symbols/release2/` - Release2版本的符号文件

## 8. 常见问题

### 8.1 llvm-objcopy 未找到

**原因**：工具链配置错误

**解决方案**：确保使用 DevEco Studio 自带的 HarmonyOS NDK，build-profile.json5 中的 `nativeCompiler: 'BiSheng'` 配置会自动配置正确的工具链

### 8.2 符号文件未生成

**原因**：separate_debug_symbols 命令执行失败

**解决方案**：
1. 检查 Cmake 中的 separate_debug_symbols 配置是否正确
2. 确认 llvm-objcopy 命令在系统路径中可用
3. 查看构建日志中的错误信息

### 8.3 符号文件过大

**原因**：Release模式未正确剥离符号

**解决方案**：
1. 确认 `nativeLib.debugSymbol.strip: true`  配置正确
2. 检查 separate_debug_symbols 命令是否执行成功
3. 验证 .so 文件确实被剥离了符号

## 9. 调试符号使用

### 9.1 本地调试

使用 Debug 版本的 har 包，.so 文件包含完整符号，可以直接调试。

### 9.2 生产环境调试

1. 使用 Release 版本的 har 包
2. 保存对应的 .sym 文件
3. 在崩溃分析平台（如 Bugly、Sentry 等）上传 .sym 文件
4. 使用 .sym 文件解析崩溃堆栈

#### 9.2.1 使用 .sym 文件解析崩溃堆栈

**方法一：使用 llvm-addr2line 工具**

```bash
# 基本语法
llvm-addr2line -e <符号文件> -f -C <地址>

# 示例
llvm-addr2line -e librnoh_core.so.sym -f -C 0x12345678
```

**方法二：使用 llvm-objdump 工具**

```bash
# 查看符号表
llvm-objdump -t librnoh_core.so.sym | grep <函数名>

# 查看反汇编代码
llvm-objdump -d librnoh_core.so.sym | grep -A 10 <地址>
```

#### 9.2.2 崩溃堆栈解析示例

**原始崩溃堆栈：**
```
*** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
Build fingerprint: 'Huawei/HarmonyOS/HarmonyOS:5.0.0/12:...'
pid: 12345, tid: 67890, name: rnoh_thread
signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0x0
    #00 pc 00012345  /data/app/el2/100/base/com.example.app/lib/entry/librnoh_core.so (RNOHCore::processEvent+124)
    #01 pc 00023456  /data/app/el2/100/base/com.example.app/lib/entry/librnoh_core.so (RNOHCore::handleMessage+56)
    #02 pc 00034567  /data/app/el2/100/base/com.example.app/lib/entry/librnoh_app.so (main+88)
    #03 pc 00045678  /system/lib64/libc.so (pthread_start+78)
```

**解析步骤：**

1. **提取地址：**
   - `#00 pc 00012345` → 地址：0x00012345
   - `#01 pc 00023456` → 地址：0x00023456
   - `#02 pc 00034567` → 地址：0x00034567

2. **计算实际地址（需要减去so基地址）：**
   ```bash
   # 假设so基地址为0x7f8a1b2c0000
   # 实际地址 = 基地址 + 偏移地址
   #00: 0x7f8a1b2c0000 + 0x00012345 = 0x7f8a1b2d2345
   #01: 0x7f8a1b2c0000 + 0x00023456 = 0x7f8a1b2e3456
   #02: 0x7f8a1b2c0000 + 0x00034567 = 0x7f8a1b2f4567
   ```

3. **使用llvm-addr2line解析：**
   ```bash
   # 解析 #00
   llvm-addr2line -e librnoh_core.so.sym -f -C 0x7f8a1b2d2345
   # 输出：RNOHCore::processEvent()
   #       /path/to/rnoh/core.cpp:245

   # 解析 #01
   addr2line -e librnoh_core.so.sym -f -C 0x7f8a1b2e3456
   # 输出：RNOHCore::handleMessage()
   #       /path/to/rnoh/core.cpp:312

   # 解析 #02
   llvm-addr2line -e librnoh_app.so.sym -f -C 0x7f8a1b2f4567
   # 输出：main()
   #       /path/to/rnoh/app.cpp:56
   ```

4. **解析后的堆栈：**
   ```
   #00 RNOHCore::processEvent() at /path/to/rnoh/core.cpp:245
   #01 RNOHCore::handleMessage() at /path/to/rnoh/core.cpp:312
   #02 main() at /path/to/rnoh/app.cpp:56
   #03 pthread_start() at /system/lib64/libc.so:78
   ```

#### 9.2.3 自动化解析脚本

**创建解析脚本：**
```bash
#!/bin/bash
# crash_stack_resolver.sh

SYMBOL_FILE=$1
BASE_ADDR=$2
STACK_FILE=$3

if [ $# -ne 3 ]; then
    echo "Usage: $0 <symbol_file> <base_addr> <stack_file>"
    exit 1
fi

echo "解析崩溃堆栈..."
echo "符号文件: $SYMBOL_FILE"
echo "基地址: $BASE_ADDR"
echo "堆栈文件: $STACK_FILE"
echo "----------------------------------------"

while read -r line; do
    if [[ $line =~ pc\ ([0-9a-f]+) ]]; then
        offset_addr="0x${BASH_REMATCH[1]}"
        real_addr=$((BASE_ADDR + offset_addr))
        
        echo "原始地址: $offset_addr"
        echo "实际地址: 0x$(printf '%x' $real_addr)"
        
        result=$(llvm-addr2line -e "$SYMBOL_FILE" -f -C "0x$(printf '%x' $real_addr)")
        echo "解析结果: $result"
        echo "----------------------------------------"
    fi
done < "$STACK_FILE"
```

**使用脚本：**
```bash
# 假设崩溃堆栈保存在 crash.txt
chmod +x crash_stack_resolver.sh
./crash_stack_resolver.sh librnoh_core.so.sym 0x7f8a1b2c0000 crash.txt
```

#### 9.2.4 符号文件匹配注意事项

1. **版本匹配**：确保 .sym 文件与崩溃的 .so 文件版本完全一致
2. **架构匹配**：确保符号文件与崩溃设备的架构一致（arm64-v8a等）
3. **构建类型匹配**：Release版本的崩溃必须使用Release版本的符号文件
4. **符号完整性**：验证符号文件包含所有必要的调试信息

**验证符号文件：**
```bash
# 检查符号文件格式
file librnoh_core.so.sym
# 输出应该包含：ELF 64-bit LSB shared object, ARM aarch64

# 检查符号表内容
llvm-nm librnoh_core.so.sym | head -20

# 检查调试信息
llvm-readelf -S librnoh_core.so.sym | grep debug
```

## 10. 快速参考

### 10.1 关键文件路径

| 文件 | 路径 |
|------|------|
| 本地编译配置 | `{PRJ_DIR}\packages\tester\harmony\entry\build-profile.json5` |
| 本地CMake配置 | `{PRJ_DIR}\packages\tester\harmony\entry\src\main\cpp\CMakeLists.txt` |
| 流水线编译脚本 | `{PRJ_DIR}\packages\react-native-harmony\scripts\build-har.js` |
| 流水线模块配置 | `{PRJ_DIR}\packages\react-native-harmony\scripts\resources\build-profile_module.json5` |
| RNOH CMake配置 | `{PRJ_DIR}\packages\tester\harmony\react_native_openharmony\src\main\cpp\CMakeLists.txt` |

### 10.2 关键命令

```bash
# 构建Debug版本
hvigorw --mode module -p product=default -p module=react_native_openharmony@default -p buildMode=debug assembleHar

# 构建Release版本
hvigorw --mode module -p product=default -p module=react_native_openharmony@default -p buildMode=release -p debuggable=false assembleHar

# 构建Release2版本
hvigorw --mode module -p product=release2 -p module=react_native_openharmony@default -p buildMode=release -p debuggable=false assembleHar

# 符号分离命令
llvm-objcopy --only-keep-debug input.so output.sym
llvm-objcopy --strip-debug input.so
llvm-objcopy --add-gnu-debuglink=output.sym input.so
```

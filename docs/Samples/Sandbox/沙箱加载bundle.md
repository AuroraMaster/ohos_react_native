## rawfile推入沙箱
有两种方式可以向沙箱推入bundle
### 1. bundle在rawfile中推入沙箱


```
 // 将bundlejs下载并保存到沙箱
      let uint8Array = await this.rnAbility.context.resourceManager.getRawFileContent('rawfile/bundle/bp/sandbox.harmony.bundle')
      let rawBuffer = uint8Array.buffer
      let num = rawBuffer.byteLength
      // 获取沙箱路径
      const sandboxDir = this.rnAbility.context.filesDir
      const bundlePath = sandboxDir + '/' + this.bundlePath
      let stream = fs.createStreamSync(bundlePath, 'w')
      stream.writeSync(rawBuffer)
      stream.closeSync()
      this.hasBundle = true
```

### 2. hdc工具向应用沙箱推送文件
参考[向应用沙箱推送文件](https://developer.huawei.com/consumer/cn/doc/harmonyos-guides-V1/send-file-to-app-sandbox-0000001630305921-V1)
## Pushing Raw Files to the Sandbox
You can push bundles to the sandbox in either of the following ways:
### 1. Pushing Bundles to the Sandbox in the rawfile Directory


```
 // Download bundle.js and save it to the sandbox.
      let uint8Array = await this.rnAbility.context.resourceManager.getRawFileContent('rawfile/bundle/bp/sandbox.harmony.bundle')
      let rawBuffer = uint8Array.buffer
      let num = rawBuffer.byteLength
      // Obtain the sandbox path.
      const sandboxDir = this.rnAbility.context.filesDir
      const bundlePath = sandboxDir + '/' + this.bundlePath
      let stream = fs.createStreamSync(bundlePath, 'w')
      stream.writeSync(rawBuffer)
      stream.closeSync()
      this.hasBundle = true
```

### 2. Pushing Files to the Application Sandbox Using hdc
For details, see [Pushing Files to an Application Sandbox Directory](https://developer.huawei.com/consumer/en/doc/harmonyos-guides-V1/send-file-to-app-sandbox-0000001630305921-V1).

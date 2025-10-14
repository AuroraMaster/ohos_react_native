## 1 打包场景
### 1.1 打包路径
假设为打包路径为根路径：/  
rn代码路径：/aaa/bbb/c.tsx  
图片1所在路径：/aaa/bbb/d.png rn使用写法：require('./d.png')  
图片2所在路径：/eee/f.png rn使用写法：require('../../eee/f.png')  
图片3所在路径：/aaa/ggg/h.png rn使用写法：require('../ggg/h.png')  

### 1.2 图片路径前缀
图片1生成的路径前缀：/aaa/bbb/  
图片2生成的路径前缀：/eee/  
图片3生成的路径前缀：/aaa/ggg/  

### 1.3 资源打包结果
assets是在package.json里面指定的目录，开发者可以任意设置目录    
assets  
|--aaa/bbb/d.png  
|--aaa/ggg/h.png  
|--eee/f.png  

## 2 沙箱场景图片路径
在沙箱场景下假设bundle所在路径 /data/storage/base/files/dir1/bundle.harmony.js

图片1沙箱场景实际路径： /data/storage/base/files/dir1/aaa/bbb/d.png

图片2沙箱场景实际路径： /data/storage/base/files/dir1/eee/f.png

图片3沙箱场景实际路径： /data/storage/base/files/dir1/aaa/ggg/h.png

重点：沙箱场景下图片的路径是根据bundle路径作为前缀，图片与工程打包路径的相对路径为后缀，合成完整的路径。这里没有assets目录

## 3 RAWFILE场景图片路径
RAWFILE场景下假设bundle所在路径 resource://RAWFILE/dir1/bundle.harmony.js

图片1rawfile场景实际路径： resource://RAWFILE/assets/aaa/bbb/d.png

图片2rawfile场景实际路径： resource://RAWFILE/assets/eee/f.png

图片3rawfile场景实际路径： resource://RAWFILE/assets/aaa/ggg/h.png

重点：RAWFILE场景下图片路径的前缀指定为resource://RAWFILE/assets/，图片与工程打包路径的相对路径为后缀，合成完整的路径。这里有assets目录
## 1 Packing
### 1.1 Packing Path
Assume that the packing path is the root path: /  
RN code path: /aaa/bbb/c.tsx  
Path of image 1: /aaa/bbb/d.png; RN format: require('./d.png')  
Path of image 2: /eee/f.png; RN format: require('../../eee/f.png')  
Path of image 3: /aaa/ggg/h.png; RN format: require('../ggg/h.png')  

### 1.2 Image Path Prefix
Path prefix of image 1: /aaa/bbb/  
Path prefix of image 2: /eee/  
Path prefix of image 3: /aaa/ggg/  

### 1.3 Resource Packing Result
You can set any directory under **assets**, which is the directory specified in **package.json**.  
assets  
|--aaa/bbb/d.png  
|--aaa/ggg/h.png  
|--eee/f.png  

## 2 Sandbox Image Path
In the sandbox scenario, assume that the bundle path is /data/storage/base/files/dir1/bundle.harmony.js.

Actual path of image 1: /data/storage/base/files/dir1/aaa/bbb/d.png

Actual path of image 2: /data/storage/base/files/dir1/eee/f.png

Actual path of image 3: /data/storage/base/files/dir1/aaa/ggg/h.png

Note: In the sandbox scenario, the bundle path is used as the prefix of the image path, and the relative path of the image and project packing path is used as the suffix to form a complete path. The **assets** directory does not exist.

## 3 Image Path in RAWFILE
In the RAWFILE scenario, assume that the bundle path is resource://RAWFILE/dir1/bundle.harmony.js.

Actual path of image 1: resource://RAWFILE/assets/aaa/bbb/d.png

Actual path of image 2: resource://RAWFILE/assets/eee/f.png

Actual path of image 3: resource://RAWFILE/assets/aaa/ggg/h.png

Note: In the RAWFILE scenario, the prefix of the image path is specified as **resource://RAWFILE/assets/**, and the relative path of the image and project packing path is the suffix to form a complete path. The **assets** directory exists.

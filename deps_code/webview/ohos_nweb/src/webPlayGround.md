# webPlayGround功能<br>
该功能开启，可以使应用开发者将[OpenHarmony-TPC](https://gitcode.com/openharmony-tpc)提供的web组件
作为应用实际使用的web组件，起到替换系统默认web组件的效果。<br>

# 如何使用webPlayGround功能<br>
## 功能开启有三个条件
1、设备开启开发者模式<br>
2、应用签名证书为debug类型，要求应用包信息中appProvisionType:debug<br>
3、应用配置文件中添加配置，添加对应的应用环境变量<br>
//Appcope/app.json5<br>
"appEnvironments": [
    {
        "name":"enableArkWebPlayGround",
        "value":"true"
    }
]

## 替换资源

这一步需要编译web组件，并将编译结果按要求放置到应用工程对应目录参与应用编译打包。<br>
web组件编译命令可以参考:[chromium_src](https://gitcode.com/openharmony-tpc/chromium_src)<br>
编译对应设备类型的web组件，参考<br>
***
64位设备<br>
./build.sh  -t w -A rk3568_64 -j 10<br>
***
32位设备<br>
./build.sh  -t w -A rk3568 -j 10<br>
***
编译结果为ohos_nweb.hap，修改hap文件后缀名为zip，解压编译结果<br>

├── ohos_nweb.hap<br>
├── ohos_nweb.zip<br>
├zip解压结果<br>
├── ets<br>
│   └── modules.abc<br>
├── libs<br>
│   └── armeabi-v7a<br>
│       ├── libarkweb_crashpad_handler.so<br>
│       ├── libarkweb_engine.so<br>
│       ├── libarkweb_render.so<br>
│       └── libffmpeg.so<br>
├── module.json<br>
├── pack.info<br>
├── resources<br>
│   ├── base<br>
│   │   ├── media<br>
│   │   │   └── app_icon.png<br>
│   │   └── profile<br>
│   │       └── main_pages.json<br>
│   └── rawfile<br>
│       ├── icudtl.dat<br>
│       ├── locales<br>
│       │   ├── bo-CN.pak<br>
│       │   ├── en-US.pak<br>
│       │   ├── ug.pak<br>
│       │   ├── zh-CN.pak<br>
│       │   ├── zh-HK.pak<br>
│       │   └── zh-TW.pak<br>
│       ├── resources.pak<br>
│       └── snapshot_blob.bin<br>
└── resources.index<br>

### 打包动态库文件
将上述解压结果中libs目录放到应用目录下$app/entry下，修改设备架构<br>
例如:<br>
将armeabi-v7a改为arm64-v8a<br>

### 打包其余文件
将ohos_nweb.hap 文件重命名为nweb.hap，放到$app/entry/src/main/resources/resfile路径，须新建resfile目录<br>

## 重新编译应用，安装即可

# 日志
## 功能使能成功日志：webPlayGround opened<br>

## 功能使能失败日志：webPlayGround not opened for isDebugApp ...<br>

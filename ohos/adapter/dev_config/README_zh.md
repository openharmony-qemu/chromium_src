# Chromium 项目: 浏览器/Electron/CEF 通过 dev_config.json 定制功能

## dev_config.json 文件介绍

dev_config.json 文件是一个配置文件，用于弥补鸿蒙平台不像其他PC平台能够有启动参数的缺陷，启动参数可以看链接
[https://peter.sh/experiments/chromium-command-line-switches/](https://peter.sh/experiments/chromium-command-line-switches/)

比如在 windows 平台或者 mac / Linux 中想要在启动的时候抓trace 15秒钟，就可以使用如下的命令

```bash
chrome --trace-startup=benchmark,blink,v8,cc,gpu,navigation,toplevel,viz,ui,views --trace-startup-duration=15
```

使用 `trace-startup` 和 `trace-startup-duration` 两个配置项，读取命令行配置文件是使用
`base::CommandLine` 类定义的，该类的文件在 `base/command_line.h` 中，
是Chromium读取配置项的核心。

上述的两个配置项在 `services/tracing/public/cpp/trace_startup_config.cc` 文件的
`TraceStartupConfig::TraceStartupConfig` 构造函数中进行获取 `base::CommandLine`
并读取配置项来设置 `tracing::TraceStartupConfig` 实例的成员属性。

由于鸿蒙平台不支持命令行参数，因而定制相关的操作需要额外的配置文件，在应用启动时会把配置文件转化成启动参数。

当前实现的机制就是 `dev_config.json` 文件，具体逻辑在 `ohos/adapter/dev_config/dev_config.h` 中。

一个符合要求的 `dev_config.json` 文件如下所示

```json
{
  "trace-startup" : true,
  "trace-categories":
    "benchmark,blink,v8,cc,gpu,navigation,toplevel,viz,ui,views",
  "trace-startup-file":
    "/data/storage/el2/base/cache/trace.json",
  "trace-startup-duration": 15,
  "trace-startup-record-mode": "record-until-full",
  "long-tracing": true,
  "remote-debugging": true,
  "remote-debugging-port": 9222,
  "remote-allow-origins": "http://localhost:9222",
  "disable-occlusion-feature": false,
  "disable-partial-swap": false,
  "disable-page-policy": false
}
```

所有的配置项都是可选的，不提供会使用下面介绍的各个选项的默认值

上面的启动命令使用 `dev_config.json` 配置文件就是如下所示:

原始的命令:

```bash
chrome --trace-startup=benchmark,blink,v8,cc,gpu,navigation,toplevel,viz,ui,views --trace-startup-duration=15
```

对应的 json 文件:

```json
{
  "trace-startup" : true,
  "trace-categories":
    "benchmark,blink,v8,cc,gpu,navigation,toplevel,viz,ui,views",
  "trace-startup-duration": 15,
}
```

### trace 相关

配置 chrome trace 相关的选项

#### trace-startup

默认值: `false`

同命令行参数 `--trace-startup` 如果提供则会开启 启动录制trace 的功能

#### trace-categories

默认值:
`benchmark,blink,blink_gc,v8,cc,gpu,navigation,toplevel,viz,ui,views,`
`disk_cache,latency,renderer.scheduler,sequence_manager,timeline.frame,`
`disabled-by-default-v8.gc,disabled-by-default-blink_gc`

设置启动 trace 和后面 `long-trace` 模式开启的 trace 种类

具体的所有的 trace 种类见文件 `base/trace_event/builtin_categories.h` 中

要抓所有的非 `disabled-by-default-` 的种类可以使用 `*`

#### trace-startup-file

默认值: chrometrace.log

chromium 的 perfetto trace 文件保存的名称

会在应用的临时文件夹 (也就是 `/data/app/el2/${用户id通常100}/base/${包名}/temp`) 中保存该文件名的文件

该文件可以通过该网站来访问: [https://ui.perfetto.dev/](https://ui.perfetto.dev/)

#### trace-startup-duration

默认值: 5

设置启动 trace 记录的时间，同 `--trace-startup-duration`，启动后多少秒以后停止 trace 记录并生成 trace 文件，

不提供则默认是 5 秒

#### trace-startup-record-mode

默认值: record-until-full

启动 trace 的录制模式, 有 3 种

- `"record-until-full"` 录制满了提前结束
- `"record-continuously"` 录制满了则丢弃前面的内容
- `"record-as-much-as-possible"` 用一个非常大的缓存录制，满了提前结束

#### long-tracing

默认值: false

如果是 false 则在开启 chrome tracing 才会在 `hitrace` 中录制对应的泳道，

如果是 true 则开启 `hitrace` 录制会有 Chromium 鸿蒙化适配到 hitrace 的同步和异步 trace，
也就是说会持久开启一个只对接和写入 `hitrace` 的旁路

用于适配性能工厂以及其他自动化性能测试

### remote debug port 相关配置

这些配置能够让应用导出一个 debug port 来接收远程 debug port

remote debug 详见 Google 的链接
[devtools/remote-debugging/local-server](https://developer.chrome.com/docs/devtools/remote-debugging/local-server)

#### remote-debugging

默认值: false

是否开启 remote debug port,  如果为 false 则忽略下面的配置: 

#### remote-allow-origins

默认值: 空

如果 `remote-debugging` 为 `true` 则必须制定一个，
否则会拒绝所有的 remote debug port 链接，见下面:

可以使用 `*` 来允许所有的来源的链接，**但是通常有安全风险**

#### remote-debugging-port

默认值: 9222

当前开放的端口，默认是 9222, 需要和 `remote-allow-origins` 配合

如果是 9222 则 `remote-allow-origins` 需要是 `"http://localhost:9222"`

##### 如何让开发机来远程链接

在一个能够通过usb线并能够使用 `hdc` 链接鸿蒙PC的开发机上通过如下命令

```bash
hdc fport tcp:9222 tcp:9222
```

可以让当地开发机的 9222 端口路由到鸿蒙PC的 9222 端口。

此时打开开发机的Chrome进入如下的网址

```txt
chrome://inspect/#devices
```

稍等一会就能查询到远程机的 tab 页在 `Remote Target` 的列表中了

### 功能的开关

是否开关对应的功能, 配置项设置为 true 则关闭一些功能

#### disable-occlusion-feature

默认值: false

设置为 true 则屏蔽鸿蒙OS XComponent的遮挡停止 vsync 能力，避免 cef / electron 的白屏问题

#### disable-partial-swap

默认值: false

设置为 true 则关闭 gl 的局部刷新能力，进行屏幕全量刷新

#### disable-page-policy

默认值: false

暂时无效没启用的开关

## 通过打包把 dev_config.json 加入

可以在打包应用的时候，把配置文件加入如下路径中，
也就是和 libadapter.so 同路径下

- chromium:
  `chromium/libs/arm64-v8a/dev_config.json`
- electron:
  `electron/libs/arm64-v8a/dev_config.json`
- cef:
  `entry/libs/arm64-v8a/dev_config.json`

就能打包后启动 dev_config.json 中的配置

**注意，这个修改的 dev_config.json 会被下面的用户目录下的 dev_config.json 配置覆盖**

## 通过公共事件写入用户选项文件夹的 dev_config.json 文件

提供给能够通过开发机用 hdc 连接鸿蒙条件下开发和测试人员定制

用户选项文件夹的 dev_config.json 会覆盖打包的 dev_config.json，
注意此处的覆盖是一旦存在用户数据的 `dev_config.json` 就不再读取应用打包目录的 `dev_config.json`，
不存在精细的按配置项覆盖的能力

### 用户数据文件的路径

用户数据文件的路径在

沙箱路径下:

```
/data/storage/el2/base/preferences/dev_config.json
```

绝对的 hdc shell 物理路径下:

```
/data/app/el2/${用户id通常100}/base/${包名}/preferences
```

### 通过 hdc file send 推送 dev_config.json

在能够链接 hdc 的开发机上输入如下的命令；

```bash
hdc file send ${开发机物理路径}/dev_config.json /data/app/el2/${用户id通常100}/base/${包名}/preferences/
```

就能把 dev_config.json 文件推送到用户数据文件夹中

新的鸿蒙OS版本没有往应用沙盒推送的权限，则此时需要下面的方法

### 通过发送公共事件触发弹框选择导入 dev_config.json

#### 公共事件更新 dev_config.json 的前置要求

必须编译的时候在 `gn` 的配置文件中添加

```gn
enable_cem_update_devconfig = true
```

才能在编译最终生成的 浏览器 / cef / electron 中添加通过公共事件添加用户目录下的 dev_config.json 的能力
(el1目录下以及)

需要用户能够通过 `hdc shell` 链接 pc,
同时保持需要推入 `dev_config.json` 文件的浏览器应用正在运行中

通过发送如下的命令来开启 dev_config.json 导入

```bash
hdc shell cem publish -e chromium_update_config -d <应用包名>:update_config
```

会弹出一个文件选择对话框，此时需要在对话框中选择一个用户目录下的 dev_config.json
(可以通过 hdc file send 送到用户目录下或者用本地的记事本创建一个并修改)
然后该文件会拷贝到应用沙箱里面的
`/data/app/el2/${用户id通常100}/base/${包名}/preferences/dev_config.json` 位置处

#### 查看推入的 dev_config.json 文件

链接 hdc 的并有权限的用户可以通过

```bash
hdc shell cat /data/app/el2/${用户id通常100}/base/${包名}/preferences/dev_config.json
```

来直接查看

这个目录非 root 权限的用户可以通过文件浏览器来查看，具体步骤是

- 进入文件管理
- 在界面的左侧的 存储位置 边栏中选择 电脑 🖥️ 图标
- 在 `系统` / `个人` 的两个盘符图标中选择 `系统`
- 双击文件夹 `应用数据` > 双击文件夹 `el2` > 双击文件夹 `base`
- 双击进入应用的中文/英文包名对应的文件夹
- 双击进入 `preferences` 文件夹
- 可以看到 `dev_config.json` 文件

这个文件在文件管理中是启动文本编辑器编辑的，编辑完成以后可以保存，也可以在文件管理中删除

## 删除推入用户目录的 dev_config.json

有两种方式来删除

1. 通过文件管理删除
   见上面的通过文件管理查看的步骤，在进入 `preferences` 后可以通过文件管理删除 dev_config.json
2. 通过公共事件删除
   见下面

### 通过公共事件删除用户目录的 dev_config.json

在保持需要删除 dev_config.json 文件的浏览器应用打开的场景下,
输入如下的命令即发送公共事件来删除对应包中的 `dev_config.json`

```bash
hdc shell cem publish -e chromium_update_config -d ${包名}:clear
```

## dev_config.json 的覆盖机制

覆盖机制是有用户目录 `/data/app/el2/${用户id通常100}/base/${包名}/preferences/dev_config.json`
的 `dev_config.json` 文件则优先读取该文件, 如果该文件不存在,
则使用包中的 `libs/arm64-v8a/dev_config.json` 中的文件,
如果都不存在则使用 `dev_config.json` 中每个配置项的默认值。

### 如果安装包中有 dev_config.json 中想要去除影响恢复成默认值

因为用户数据的 `/data/app/el2/${用户id通常100}/base/${包名}/preferences/dev_config.json`
的内容会优先覆盖安装包的内容，因而只用推送一个

```json
{}
```

空的 `dev_config.json` 文件即可, 空的文件会使用各个配置项的默认值,
和所有地方没有 `dev_config.json` 作用相同

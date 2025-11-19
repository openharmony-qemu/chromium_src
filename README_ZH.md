# Chromium
## 简介
### 软件架构
![image.png](https://raw.gitcode.com/user-images/assets/4371737/71f65bf3-20bf-4c4b-a255-e385c5f75ba7/image.png 'image.png')

架构图中CEF、ArkWeb仓和当前仓Chromium联合编译出Web内核，编译产物为NWeb.hap，通过二进制集成在OpenHarmony系统中。

Chromium是 Google 主导的开源 Web 浏览器项目，旨在构建更安全、更快、更稳定的 Web 平台的项目。多进程架构是Chromium 最核心的设计, 它将浏览器功能划分到多个独立的进程中，以实现安全性，稳定性。本仓是从上游社区的一个分支。

Chromium总体采用分层架构，主要包含一下几个模块：

- Chrome模块： Chrome浏览器特有的组件，包括标签页Tabs，设置Settings，工具栏Toolbar，导航栏Omnibox
- Component模块： 可重用的功能集合，专注于特定的功能逻辑除了chrome浏览器外其他浏览器也可自行集成，包括自动填充Autofill，书签Bookmark，密码管理PasswordManager , 历史记录管理History。
- Content模块：WebContents是 Content 层对一个“标签页”内容的核心抽象，最上层的 Chrome 通过持有和操作WebContents对象来命令Content层导航、停止加载、执行 JavaScript等。RenderProcessHost是浏览器进程中控制一个渲染器进程的对象，负责管理该渲染器进程的生命周期（启动、关闭）和所有与它的 IPC 通信。Process Launchers (进程启动器)这是负责启动和沙箱化所有其他辅助进程（GPU、Utility、Plugin、Zygote）的代码。IPC Handlers接收和响应来自所有子进程（渲染器、GPU 等）消息的端点，当渲染器进程需要下载图片时，它会发送一个 IPC 消息，由浏览器进程中的 Resource Dispatcher 接收。Resource Dispatcher (资源转发器)负责接收渲染器进程发来的所有特权请求（如网络请求、文件访问），并安全地将它们分派给底层的 Platform 模块（如 net 库）去执行。
- Blink模块：v8主要用来来解析和执行JavaScript 。cc负责将页面的各个图层进行合成。

- Platform模块：net网络库负责所有网络协议（HTTP、QUIC、DNS）、Cookie 管理等。mojo为Chromium 现代的 IPC 进程间通信)框架。device提供访问底层硬件的 API（如 USB、蓝牙、传感器）。schedule任务调度器负责管理所有进程和线程上的任务队列。

- Hook模块：Hook并不是一个独立的模块，Hook指的是对原生Chromium各个模块像钩子一样的嵌入式修改的集合。通过对Chromium原生文件嵌入像钩子一样的小段修改，让其指向ArkWeb仓的chromium_ext模块的独立文件。通过这种方式，能有效避免对原生Chromium嵌入大段的修改，也避免了再本仓中新增不属于Chromium的ArkWeb的文件，实现了Chromium和Arkweb的解耦。 Hook的方式通常有以下几种：1. 让原生类继承chromium_ext模块中的基类，扩展的功能都放在chromium_ext中的基类里。 2. 在chromium_ext创建派生类继承Chromium中的原生基类，扩展功能放到派生类中，创建实例时改为创建派生类。3. 让原生类持有chromium_ext中的工具类，扩展的功能都放在chromium_ext中的工具类。4. 将扩展功能逻辑提取为独立文件放到chromium_ext中，这类文件名通常以for_include结尾， 让原生类直接include该类型的文件进行扩展。

## 使用说明
1. 下载代码：以132_trunk为例，要下载其他分支代码，请替换-b之后的分支名，参数列表详见8。
    ```
    repo init -u https://gitcode.com/openharmony-tpc/manifest.git -b 132_trunk -m developer.xml --no-repo-verify
    repo sync -c
    repo forall -c 'git lfs pull'
    ```

2. 执行预编译下载，安装编译工具链及Sdk。
    ```shell
    ./prebuilts_download.sh
    ```

3. 编译
   
    编译同时构建未签名Hap包：
	
	形态：rk3568
	```
	./build.sh  -t w -A rk3568
    ```
    仅编译so库：
	```
	./build.sh -A rk3568
    ```
	
	形态：rk3568_64
	```
	./build.sh  -t w -A rk3568_64
    ```
    仅编译so库：
	```
	./build.sh -A rk3568_64
    ```

4. 签名

    形态：rk3568
    ```
    ./sign.sh rk3568
    ```
    形态：rk3568_64
    ```
    ./sign.sh rk3568_64
    ```

5. 调试方法

    方法一：替换so库

    编译完成后，在out目录下找到对应so库产物，将它们推送到设备中
    ```
    hdc shell "mount -o remount,rw /"
    hdc file send libnweb_render.so /data/app/el1/bundle/public/com.ohos.nweb/libs/arm
    hdc file send libweb_engine.so /data/app/el1/bundle/public/com.ohos.nweb/libs/arm
    pause
    hdc shell reboot
    pause
    ```

    方法二：替换hap包
    
    编译完成后，在out目录下找到NWeb-rk3568.hap或者NWeb-rk3568_64.hap, 将它推送到设备中。

    ```
    hdc shell "mount -o remount,rw /"
    hdc file send NWeb-rk3568.hap /system/app/com.ohos.nweb/NWeb.hap
    hdc shell "rm /data/* -rf"
    hdc shell reboot
    ```
6. 所有Chromium仓对应目录映射关系

    https://gitcode.com/openharmony-tpc/manifest/blob/chromium/chromium.xml

7. 上库指导

    7.1 将chromium_src 仓 fork到自己的私仓

    7.2 下载全量代码

    7.3 修改调试代码

    7.4 将文件添加到暂存区

    使用git add将修改后的文件添加到暂存区

    7.5 显示工作区和暂存区的状态

    使用git status查看自己的修改是否放到暂存区，查看项目历史信息使用git log。

    7.6 将工作区内容或暂存区内容提交到版本库

    使用git commit -sm”提交信息描述” 将修改后的文件进行提交，***注意-s一定不能漏，这个是签名，否则提的PR会报DCO错误***。

    DCO签署链接：***https://dco.openharmony.cn/sign-dco***

    7.7 将代码提交到对应fork出来的私仓地址上

    如：git push ***https://gitcode.com/[gitcodeUserName]/chromium_src***

    7.8 新建PR

    7.9 如果涉及联合构建，建立ISSUE，并在需要联合构建的PR中都绑定该ISSUE

    7.10 在PR下面评论start build开始构建

    7.11 联系committer加分

8. chromium各版manifest分支名

    99分支：chromium

    114分支：114_trunk

    配套OpenHarmony 3.2Release分支：3.2_Release

    配套OpenHarmony 4.0Release分支：4.0_Release

    配套OpenHarmony 4.1 Beta1 分支：master114_20231218

## 其他相关代码仓地址：

[架构图中的Webview ArkTS仓
](https://gitcode.com/openharmony/interface_sdk-js/tree/master/api)

[架构图中的Webview NDK仓
](https://gitcode.com/openharmony/interface_sdk_c/tree/master/web)

[架构图中的Web Component仓
](https://gitcode.com/openharmony/arkui_ace_engine/tree/master/frameworks/core/components/web)

[架构图中的Web_Webview仓
](https://gitcode.com/openharmony/web_webview)

[架构图中的ArkWeb仓](https://gitcode.com/openharmony-sig/chromium_arkweb)

[架构图中的CEF仓](https://gitcode.com/openharmony-tpc/chromium_cef)

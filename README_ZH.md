# Chromium
## 简介
### 软件架构
![image.png](https://raw.gitcode.com/user-images/assets/4371737/c396f50d-f9bd-41e9-bcbf-e395821e3b6f/image.png 'image.png')
- Chromium:   Google 主导的开源 Web 浏览器项目，旨在构建更安全、更快、更稳定的 Web 平台的项目。多进程架构是Chromium 最核心的设计, 它将浏览器功能划分到多个独立的进程中,以实现安全性,稳定性和分层架构。
- CEF：全称Chromium Embedded Framework，是一个基于Google Chromium 的开源项目。
- Arkweb:  旨在将 Chromium Web 引擎集成到 OpenHarmony (OHOS) 操作系统中。它作为系统 `Web` 组件的基础，为 OHOS 应用程序提供强大的 Web 渲染能力。基于Chromium和CEF二次扩展，在原有功能的基础上，基于Openharmoy平台扩展了很多新性，如广告拦截，任务下载，输入框填充等。

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

[架构图中的Webview ts仓
](https://gitcode.com/openharmony/interface_sdk-js/tree/master/api)

[架构图中的Webview NDK仓
](https://gitcode.com/openharmony/interface_sdk_c/tree/master/web)

[架构图中的Web Component仓
](https://gitcode.com/openharmony/arkui_ace_engine/tree/master/frameworks/core/components/web)

[架构图中的web_webview仓
](https://gitcode.com/openharmony/web_webview)

[架构图中的Arkweb仓](https://gitcode.com/openharmony-sig/chromium_arkweb)

[架构图中的CEF仓](https://gitcode.com/openharmony-tpc/chromium_cef)

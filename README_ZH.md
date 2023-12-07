# Chromium
## 简介
### 软件架构
软件架构说明
![](figures/Web-architecture_ZH.png "web软件架构图")
* webview组件：OpenHarmony的UI组件。
* nweb：基于CEF构建的OpenHarmony Web组件的Native引擎，主要构建Web组件浏览器内核的部分能力。
* CEF：CEF全称Chromium Embedded Framework，是一个基于Google Chromium 的开源项目。
## 使用说明
1. 下载代码
    ```
    repo init -u https://gitee.com/openharmony-sig/manifest -b master -m chromium.xml --no-repo-verify
    repo sync -c
    repo forall -c 'git lfs pull'
    ```
2. 编译
   
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
    ***如若找不到sdk压缩包，可以下载大文件***
    ```
    cd src
    git lfs pull
    ```
3. 签名
    ```
    执行./sign.sh
    ```
4. 调试方法

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
5. 所有Chromium仓对应目录映射关系

    https://gitee.com/openharmony-sig/manifest/blob/master/chromium.xml

6. 上库指导

    6.1 将chromium_src 仓 fork到自己的私仓

    6.2 下载全量代码

    6.3 修改调试代码

    6.4 将文件添加到暂存区

    使用git add将修改后的文件添加到暂存区

    6.5 显示工作区和暂存区的状态

    使用git status查看自己的修改是否放到暂存区，查看项目历史信息使用git log。

    6.6 将工作区内容或暂存区内容提交到版本库

    使用git commit -sm”提交信息描述” 将修改后的文件进行提交，***注意-s一定不能漏，这个是签名，否则提的PR会报DCO错误***。

    DCO签署链接：***https://dco.openharmony.cn/sign-dco***

    6.7 将代码提交到对应fork出来的私仓地址上

    如：git push ***https://gitee.com/[giteeUserName]/chromium_src***

    6.8 新建PR

    6.9 如果涉及联合构建，建立ISSUE，并在需要联合构建的PR中都绑定该ISSUE

    6.10 在PR下面评论start build开始构建

    6.11 联系committer加分

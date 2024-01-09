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

    https://gitee.com/openharmony-sig/manifest/blob/master/chromium.xml

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

    如：git push ***https://gitee.com/[giteeUserName]/chromium_src***

    7.8 新建PR

    7.9 如果涉及联合构建，建立ISSUE，并在需要联合构建的PR中都绑定该ISSUE

    7.10 在PR下面评论start build开始构建

    7.11 联系committer加分

## 二进制来源说明
1. 本项目的[chromium_third_party_ohos_prebuilts](https://gitee.com/openharmony-sig/chromium_third_party_ohos_prebuilts)仓中存在两个自研二进制文件，分别为[libnweb_ohos_adapter.z.so](https://gitee.com/openharmony-sig/chromium_third_party_ohos_prebuilts/tree/master/libs)、[sysroot-20231205.tar.gz](https://gitee.com/openharmony-sig/chromium_third_party_ohos_prebuilts/blob/master/sysroot-20231205.tar.gz)。

2. 上述二进制文件均为OpenHarmony源码基于OpenHarmony OpenHarmony-v4.1-Beta1 Tag点编译生成。源码获取可参考[OpenHarmony-v4.1-Beta1代码获取指导](https://gitee.com/openharmony/docs/blob/master/zh-cn/release-notes/OpenHarmony-v4.1-beta1.md#%E6%BA%90%E7%A0%81%E8%8E%B7%E5%8F%96),[编译构建指导](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/subsystems/subsys-build-all.md)
    ```
    代码下载命令：
    repo init -u https://gitee.com/openharmony/manifest -b OpenHarmony-4.1-Beta1 --no-repo-verify
    repo sync -c
    repo forall -c 'git lfs pull'
    ```

3. 上述二进制在本项目中基于 ***RK3568*** 产品编译生成，编译命令为
    ```
    ./build.sh --product-name rk3568  --ccache
    ```
4. 二进制在OpenHarmony编译产物中的存放路径

    libnweb_ohos_adapter.z.so : 存放路径为out/rk3568/web/webview

    sysroot-20231205.tar.gz ： 存放路径为out/rk3568/obj/third_party/musl/usr，压缩文件为usr下include、lib文件夹压缩而成

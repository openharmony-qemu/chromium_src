
# Chromium

## 简介

Chromium 是一款由 Google 主导开发的开源网络浏览器，Google Chrome 的源代码也源自 Chromium。Chromium 采用 BSD 许可证和其他宽松的开源许可证发布，旨在打造更安全、更快速、更稳定的互联网体验。

OpenHarmony chromium 基于Chromium对OH平台进行了适配

## 使用说明
1. repo配置

下载码云repo工具(可以参考码云帮助中心：https://gitee.com/help/articles/4316)：

    ```
    mkdir -p ~/bin
    curl https://gitee.com/oschina/repo/raw/fork_flow/repo-py3 > ~/bin/repo
    chmod a+x ~/bin/repo
    export PATH=~/bin/:$PATH
    pip install -i https://pypi.tuna.tsinghua.edu.cn/simple requests
    ```
   
2. 代码下载

    ```
    repo init -u  https://gitcode.com/openharmony-tpc/manifest.git -b pc_chromium_132 -m chromium.xml --no-repo-verify
    repo sync -c
    repo forall -c 'git lfs pull'
    ```

3. 内核代码构建

首次构建需要安装chromium依赖：

    ```
    ./src/build/install-build-deps.sh --no-chromeos-fonts
    ```
	
所有依赖完成安装后

    ```
    ./build.sh -t chrome_main_web
    ```

4. hap工程验证

   - 将./src/ohos/app/ohos_hap目录拷贝到本地
   - 拷贝动态库：将./src/out/musl_64目录下libadapter.so、libchrome_main_web.so，./src/ohos_sdk/openharmony/native/llvm/lib/aarch64-linux-ohos目录下libc++_shared.so动态库拷贝到本地hap工程./chromium/libs/arm64-v8a目录下
   - 拷贝资源文件：将./src/out/musl_64目录下resources.pak、chrome_100_percent.pak、chrome_200_percent.pak、icudtl.dat、snapshot_blob.bin、v8_context_snapshot.bin、locales拷贝到本地hap工程./web_engine/src/main/resources/resfile目录下
   - 签名：可以选择自动签名或手动签名方式，具体可参考https://developer.huawei.com/consumer/cn/doc/harmonyos-guides/ide-signing
   - 依次点击 DevEco Studio菜单【Build】-【Build Hap(s)/App(s)】-【Build Hap(s)】完成Hap包构建，输出路径在./chromium/build/default/outputs/default目录下
   - 签名包为chromium-default-signed.hap，未签名包为chromium-default-unsigned.hap

6. 运行验证hap包

    - 安装签名包chromium-default-signed.hap进行验证测试
    - 具体可参考如下步骤

    ```
    hdc shell aa force-stop com.huawei.ohos_chromium
    hdc uninstall com.huawei.ohos_chromium
    hdc install chromium-default-signed.hap
    hdc shell aa start -a EntryAbility -b com.huawei.ohos_chromium
    ```


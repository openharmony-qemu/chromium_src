
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

5. 运行验证hap包

    - 安装签名包chromium-default-signed.hap进行验证测试
    - 具体可参考如下步骤

    ```
    hdc shell aa force-stop com.huawei.ohos_chromium
    hdc uninstall com.huawei.ohos_chromium
    hdc install chromium-default-signed.hap
    hdc shell aa start -a EntryAbility -b com.huawei.ohos_chromium
    ```

6. 命令行参数

    在使用命令行参数时，特别是那些涉及安全性的参数，需要谨慎操作以确保系统的安全性和稳定性，风险参数包括不限于命令行如下：

    - `--remote-debugging-port`
      - 用途：启用远程调试功能，指定调试端口号。
      - 注意事项：确保调试端口仅在受信任的网络环境中开放，避免暴露在公共网络中，以防被恶意攻击。

    - `--disable-web-security`
      - 用途：禁用同源策略，允许跨域请求。
      - 注意事项：仅在开发或测试环境中使用，切勿在生产环境中启用，以防止潜在的安全漏洞。

    - `--no-sandbox`
      - 用途：禁用沙箱机制，降低进程隔离保护。
      - 注意事项：使用时需确保环境安全，避免恶意软件利用此配置进行攻击。

    - `--ignore-certificate-errors`
      - 用途：忽略证书错误，允许自签名证书。
      - 注意事项：仅在受信任的环境中使用，避免在生产环境中启用，以防中间人攻击。

    - `--gpu-launcher`
      - 用途：指定GPU进程的启动器命令。
      - 注意事项：主要用于高级调试或特定GPU配置，需了解其具体用法和潜在影响。

    - `--inspect` 和 `--inspect-brk`
      - 用途：启动调试服务器，支持后台调试和启动时暂停。
      - 注意事项：避免在生产环境中使用，确保调试过程的安全性。

    - `--host-rules`
      - 用途：配置网络请求的路由或重定向。
      - 注意事项：正确配置语法，确保网络请求的安全性和合规性。

    总结：相关参数在开发和调试中非常有用，但需谨慎使用，确保环境安全，避免在生产环境中启用可能削弱安全性的参数。
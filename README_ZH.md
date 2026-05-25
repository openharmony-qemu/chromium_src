# ArkWeb 项目

## 概述

ArkWeb 项目旨在将 Chromium Web 引擎集成到 OpenHarmony操作系统中，它对 Chromium 源代码进行了扩展和增强，以使其适应OpenHarmony架构和 API。ArkWeb在Chromium原有功能的基础上，基于OpenHarmony平台扩展了很多新特性，如广告拦截，网络加速，输入框填充等。它作为系统 Web 组件的基础，为OpenHarmony应用程序提供强大的 Web 渲染能力。

## 架构

项目遵循分层架构，图中黄色Chromium模块即本仓内容：

![image.png](https://raw.gitcode.com/user-images/assets/4371737/906916fd-de01-4567-b2c9-b7b17488ce6d/image.png 'image.png')

架构图中[CEF](https://gitcode.com/openharmony-tpc/chromium_cef)，[chromium_arkweb](https://gitcode.com/openharmony-tpc/chromium_arkweb)和当前Chromium仓联合编译出Web内核，编译产物为NWeb.hap，通过二进制集成在OpenHarmony系统中。

## 使用说明
1. 下载代码：以132_trunk分支为例，要下载其他分支代码请替换-b 后带的manifest分支参数，参数列表详见7。
    ```
    repo init -u https://gitcode.com/openharmony-tpc/manifest -b 132_trunk -m developer.xml --no-repo-verify
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

3. 签名

    形态：rk3568
    ```
    ./sign.sh rk3568
    ```
    形态：rk3568_64
    ```
    ./sign.sh rk3568_64
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

    编译完成后，在out目录下找到NWeb-rk3568.hap或者NWeb-rk3568_64.hap， 将它推送到设备中。

    ```
    hdc shell "mount -o remount,rw /"
    hdc file send NWeb-rk3568.hap /system/app/com.ohos.nweb/NWeb.hap
    hdc shell "rm /data/* -rf"
    hdc shell reboot
    ```
5. 所有Chromium仓对应目录映射关系

    https://gitcode.com/openharmony-tpc/manifest/blob/chromium/chromium.xml


## 其他相关代码仓地址：

[架构图中的Webview ArkTS仓
](https://gitcode.com/openharmony/interface_sdk-js/tree/master/api)

[架构图中的Webview NDK仓
](https://gitcode.com/openharmony/interface_sdk_c/tree/master/web)

[架构图中的Web Component仓
](https://gitcode.com/openharmony/arkui_ace_engine/tree/master/frameworks/core/components/web)

[架构图中的web_webview仓
](https://gitcode.com/openharmony/web_webview)

[架构图中的chromium_arkweb仓](https://gitcode.com/openharmony-tpc/chromium_arkweb)

[架构图中的CEF仓](https://gitcode.com/openharmony-tpc/chromium_cef)

---

# Chromium docs

> 以下为来自 [docs/README.md](docs/README.md) 的 Chromium 原始文档，供开发者参考。内容涵盖源码获取、构建、测试、设计文档及开发指南等。

本目录包含 [Chromium 项目](https://www.chromium.org/Home/) 的文档，使用 [Gitiles-flavored Markdown] 格式编写，由 Gitiles 自动[渲染][rendered by Gitiles]。

[Gitiles-flavored Markdown]: https://gerrit.googlesource.com/gitiles/+/master/Documentation/markdown.md
[rendered by Gitiles]: https://chromium.googlesource.com/chromium/src/+/main/docs/

**如果您添加了新文档，也请在下方的文档索引中添加相应的链接。**

[TOC]

## 创建文档

### 指南

*   请参阅 [Chromium 文档指南](docs/documentation_guidelines.md) 和 [Chromium 文档最佳实践](docs/documentation_best_practices.md)。
*   Markdown 文档必须遵循 [Markdown 风格指南](https://chromium.googlesource.com/chromium/src/+/HEAD/styleguide/markdown/markdown.md)。

### 预览更改

#### 使用 [md_browser](tools/md_browser/) 进行本地预览

```bash
# 在 Chromium 检出目录中
./tools/md_browser/md_browser.py
```

这只是一个近似预览。实际的 **gitiles** 视图可能有所不同。

#### 通过 Gerrit 的 Gitiles 链接在线预览

1.  上传补丁到 Gerrit，或收到代码审查请求。
    例如 https://chromium-review.googlesource.com/c/3362532
2.  查看特定的 .md 文件。
    例如 https://chromium-review.googlesource.com/c/3362532/2/docs/README.md
3.  您会看到类似以下内容 <br>
    Base
    [预览](https://chromium.googlesource.com/chromium/src/+/ad44f6081ccc6b92479b12f1eb7e9482f474859d/docs/README.md)
    -> Patchset 3
    [预览](https://chromium.googlesource.com/chromium/src/+/refs/changes/32/3362532/3/docs/README.md)
    | DOWNLOAD <br>
    显示在页面左上角。点击第二个
    "[预览](https://chromium.googlesource.com/chromium/src/+/refs/changes/32/3362532/3/docs/README.md)"
    链接即可打开当前补丁集的预览。

此 **gitiles** 视图是权威视图，与提交后的效果完全一致。

## 文档索引

**注意**：此处并非所有文档的完整列表。

### 代码检出与构建
*   [Linux 构建说明](docs/linux/build_instructions.md) - Linux
*   [Mac 构建说明](docs/mac_build_instructions.md) - MacOS
*   [Windows 构建说明](docs/windows_build_instructions.md) - Windows
*   [Android 构建说明](docs/android_build_instructions.md) - Android 目标平台
    （在 Linux 主机上）
*   [Cast 构建说明](docs/linux/cast_build_instructions.md) - Cast 目标平台
    （在 Linux 主机上）
*   [Cast for Android 构建说明](docs/android_cast_build_instructions.md) -
    Cast for Android（在 Linux 主机上）
*   [Fuchsia 构建说明](docs/fuchsia/build_instructions.md) -
    Fuchsia 目标平台（在 Linux 主机上）
*   [iOS 构建说明](docs/ios/build_instructions.md) - iOS 目标平台（在 MacOS 主机上）
*   [Gerrit 指南](docs/gerrit_guide.md) - 设置 Gerrit 访问权限
*   [Gerrit 重新认证](docs/gerrit_reauth.md) - Gerrit 重新认证指南
*   [Chrome OS 构建说明](docs/chromeos_build_instructions.md) - Chrome OS
*   [Linux Chromium ARM 方案](docs/linux/chromium_arm.md) - 在 Linux 上为 ARM 构建 Chromium 的方案。
*   [Chrome 组件构建](docs/component_build.md) - 使用更多库实现更快的构建
*   [`cr` 用户手册](docs/cr_user_manual.md) - `cr` 工具的使用手册，该工具试图将 Chromium 开发中使用的各种工具隐藏在一个抽象层之后

### 设计文档
*   参见 [design/README.md](docs/design/README.md)

### 集成开发环境 (IDE) 配置指南
*   [Android Studio](docs/android_studio.md) - 用于 Android 构建的 Android Studio
*   [Atom](docs/atom.md) - Atom 跨平台代码编辑器
*   [CLion](docs/clion.md) - CLion IDE，支持 GUI 调试。
*   [Eclipse for Android](docs/eclipse.md) - 用于 Android 的 Eclipse
*   [Eclipse for Linux](docs/linux/eclipse_dev.md) - 用于其他平台的 Eclipse
    （本指南为 Linux 编写，但也可能适用于 Windows/MacOS）
*   [EMACS 说明](docs/emacs.md) - EMACS 命令/风格/工具集成
*   [Qt Creator](docs/qtcreator.md) - 使用 Qt Creator 作为 IDE 或 GUI 调试器
*   [Visual Studio Code](docs/vscode.md) - Visual Studio Code

### Git
*   [Git 实用手册](docs/git_cookbook.md) - 常见任务的 Git 方案集合
*   [Git 技巧](docs/git_tips.md) - 更多 Git 技巧
*   [Git 子模块](docs/git_submodules.md) - Git 子模块（技巧、常见问题）

### Clang
*   [Clang 编译器](docs/clang.md) - Clang 编译器的通用信息，Mac 和 Linux 上默认使用
*   [Clang 工具重构](docs/clang_tool_refactoring.md) - 利用 Clang 工具执行基于 AST 感知的重构
*   [Clang 静态分析器](docs/clang_static_analyzer.md) - 如何在构建时启用静态分析
*   [Clang 代码覆盖率封装器](docs/clang_code_coverage_wrapper.md) - 为源文件的子集启用 Clang 代码覆盖率插桩。
*   [编写 Clang 插件](docs/writing_clang_plugins.md) - 不建议编写 Clang 插件，但如果您一定要写，请阅读此文
*   [更新 Clang](docs/updating_clang.md) - 更新用于构建的 Clang 版本
*   [在 Chromium C++ 代码上使用 clang-format](docs/clang_format.md) - 在 C++ 代码上调用 clang-format 的多种方式
*   [Clang Tidy](docs/clang_tidy.md) - Chromium 中对 `clang-tidy` 工具的支持
*   [更新 Clang Format 二进制文件](docs/updating_clang_format_binaries.md) - 如何更新随 Chromium 检出附带的 clang-format 二进制文件

### 通用开发
*   [为 Chromium 做贡献](docs/contributing.md) - 为 Chromium 代码库做贡献的参考工作流程。
*   [提交检查清单](docs/commit_checklist.md) - 在 Gerrit 上传 CL 之前的精简检查清单。
*   [代码审查](docs/code_reviews.md) - 代码审查要求和指南
*   [依赖管理](docs/dependencies.md) - 管理依赖项（DEPS、Git 子模块）
*   [尊重式代码审查](docs/cr_respect.md) - 面向代码审查员的指南
*   [尊重式变更](docs/cl_respect.md) - 面向代码作者的指南
*   [强制代码审查推广](docs/code_review_owners.md) - 与代码审查和 OWNERS 相关的即将推出的策略变更
*   [LUCI 迁移常见问题](docs/luci_migration_faq.md) - Chromium 从 Buildbot 迁移到 LUCI 构建器的常见问题
*   [持续集成 UI 导览](docs/tour_of_luci_ui.md) - 我们持续集成系统 LUCI 的用户界面导览
*   [解析测试结果](docs/parsing_test_results.md) - 如何理解 polygerrit 和 CI 构建发出的结果的入门介绍。
*   [Closure 编译](docs/closure_compilation.md) - _Closure_ JavaScript 编译器
*   [Chrome 中的线程与任务](docs/threading_and_tasks.md) - 如何在 Chrome 中运行任务和处理线程安全。
*   [Callback<> 与 Bind()](docs/callback.md) - 关于 Callback、Closure 和 Bind() 的全部内容。
*   [Chromium Views UI](docs/ui/index.md) - 使用桌面 UI 框架。
*   [Views 平台样式](docs/ui/views/platform_style.md) - 视图如何适配不同原生平台的样式
*   [Tab Helpers](docs/tab_helpers.md) - 使用 WebContents/WebContentsObserver 为浏览器标签页添加功能。
*   [添加 third_party 库](docs/adding_to_third_party.md) - 如何将代码放入 third_party/ 目录
*   [Chromium Views 图形化调试辅助工具](docs/graphical_debugging_aid_chromium_views.md) - 在调试过程中可视化视图树
*   [位图管线](docs/bitmap_pipeline.md) - 位图如何从渲染器传递到屏幕。
*   [标志位保护指南](docs/flag_guarding_guidelines.md) - 何时使用服务器控制的开关和 A/B 实验来安全地推出变更。
*   [使用 Origin Trials 框架](docs/origin_trials_integration.md) - 一个用于有条件地启用实验性 API 以进行测试的框架。
*   [Chrome 同步](https://source.chromium.org/chromium/chromium/src/+/main:docs/website/site/developers/design-documents/sync) -
    允许跨设备同步数据的子系统的文档。
*   [Ozone 概述](docs/ozone_overview.md) - Ozone 是窗口系统与底层输入和图形之间的抽象层。
*   [项目规划中考虑分支日期的指南](docs/release_branch_guidance.md) - 在安排项目工作时，分支日期前后应该做什么（和不做什么）。
*   [关注列表](docs/infra/watchlists.md) - 使用关注列表获取您感兴趣的 CL 的通知。
*   [关闭流程](docs/shutdown.md) - 解释 Chrome 关闭的步骤，以便更轻松地确定在哪里添加新的关闭操作。
*   [API 密钥](docs/api_keys.md) - 当您需要为自定义构建、分支、标准 Chromium 集成访问 Google API，或者正在构建 ChromiumOS（用于登录）时。
*   [用户教育](components/user_education/README.md) - 创建产品内帮助 (IPH) 和教程来引导用户了解 Chromium 功能。
*   [User-Agent](docs/user_agent/README.md) - User-Agent 和 User-Agent Client Hints。

### 测试
*   [运行和调试 Web 测试](docs/testing/web_tests.md)
*   [关于禁用测试](docs/testing/on_disabling_tests.md)
*   [编写 Web 测试](docs/testing/writing_web_tests.md) - 使用 `content_shell` 的 Web 测试
*   [Web 测试期望结果与基线](docs/testing/web_test_expectations.md) - 设置 Web 测试的预期结果。
*   [Web 测试技巧](docs/testing/web_tests_tips.md) - Web 测试的最佳实践
*   [带手动回退的 Web 测试](docs/testing/web_tests_with_manual_fallback.md) - 编写模拟手动干预的测试
*   [扩展 Web 测试框架](docs/how_to_extend_web_test_framework.md)
*   [修复 Web 测试不稳定性](docs/testing/identifying_tests_that_depend_on_order.md) - 诊断和修复由于顺序依赖导致的 Web 测试不稳定问题。
*   [使用 `content_shell` 运行 Web 测试](docs/testing/web_tests_in_content_shell.md) - 手动运行 Web 测试。
*   [Web 平台测试](docs/testing/web_platform_tests.md) - 浏览器厂商共享的测试
*   [在 `content_shell` 中使用 Crashpad](docs/testing/using_crashpad_with_content_shell.md) - 在布局测试崩溃时无需附加调试器即可捕获堆栈跟踪
*   [测试说明](docs/testing/test_descriptions.md) - 可以构建的单元测试目标，附有相关描述。
*   [Fuzz 测试](testing/libfuzzer/README.md) - Chromium 中的 Fuzz 测试。
*   [使用 AddressSanitizer (asan) 和 LeakSanitizer (lsan) 运行 Chrome 测试](docs/testing/linux_running_asan_tests.md) -
    使用 ASAN 和 LSAN 构建运行 Chrome 测试，以检测地址访问问题和内存泄漏。
*   [代码覆盖率](docs/testing/code_coverage.md) - Chromium 的代码覆盖率。
*   [Gerrit 中的代码覆盖率](docs/testing/code_coverage_in_gerrit.md) - Gerrit 中每个 CL 的代码覆盖率，辅助代码审查。

### 配置文档

*   [配置：偏好设置、Settings、功能、开关与标志](docs/configuration.md) - 解释控制新功能的不同方式。
*   [在 chrome://flags 中添加新的功能标志](docs/how_to_add_your_feature_flag.md) - 添加新功能标志以实验您的功能的快速指南。
*   [运行时启用功能](https://chromium.googlesource.com/chromium/src/+/main/third_party/blink/renderer/platform/RuntimeEnabledFeatures.md)
*   [在 content 层初始化 Blink 运行时功能](docs/initialize_blink_features.md)
*   [将功能与 Origin Trials 框架集成](docs/origin_trials_integration.md)

### GPU 相关文档
*   [GPU Pixel Wrangling](docs/gpu/pixel_wrangling.md) - GPU Pixel Wrangling（GPU 值班轮换）的说明。
*   [调试 GPU 相关代码](docs/gpu/debugging_gpu_related_code.md) - 调试 GPU 和图形相关代码的提示。
*   [GPU 测试](docs/gpu/gpu_testing.md) - Chromium GPU 测试基础设施的说明。
*   [GPU 机器人详情](docs/gpu/gpu_testing_bot_details.md) - 关于机器人维护方式的深入说明。

### 其他 Linux 专属文档
*   [Linux 代理配置](docs/linux/proxy_config.md) - Linux 上的网络代理来源
*   [在 Linux 上调试 SSL](docs/linux/debugging_ssl.md) - 在 Linux 中调试 SSL 代码的技巧
*   [Linux 证书管理](docs/linux/cert_management.md) - 在 Linux 中管理 X.509 证书
*   [Linux 调试技巧](docs/linux/debugging.md)
*   [Linux GTK 主题集成](docs/linux/gtk_theme_integration.md) - 让 Chrome 匹配 GTK+ 主题。
*   [Linux 上的浏览器插件](docs/linux/plugins.md) - 关于浏览器插件在 Linux 上工作方式的链接集合
*   [Linux 崩溃转储](docs/linux/crash_dumping.md) - Breakpad 如何将崩溃报告上传到 Google 崩溃服务器。
*   [Linux Minidump 转 Core](docs/linux/minidump_to_core.md) - 如何将 Breakpad 生成的 minidump 文件转换为大多数调试器可读的 core 文件
*   [Linux 沙箱 IPC](docs/linux/sandbox_ipc.md) - 用于将请求从调用栈底部路由到浏览器中的底层 UPC 系统。
*   [Linux 开发版作为默认浏览器](docs/linux/dev_build_as_default_browser.md) - 如何在 Linux 中将 Chrome 开发版配置为默认浏览器。
*   [Linux Chromium 软件包](docs/linux/chromium_packages.md) - 一些 Linux 发行版提供的 Chromium 浏览器（非 Chrome）软件包。
*   [`seccomp` 沙箱崩溃转储](docs/seccomp_sandbox_crash_dumping.md) - 关于在 seccomp 沙箱中运行的进程的崩溃转储说明。
*   [Linux 密码存储](docs/linux/password_storage.md) - Chromium 与 Linux 之间的钥匙串集成。
*   [Linux Sublime 开发](docs/sublime_ide.md) - 在 Linux 上使用 Sublime 作为 Chromium 开发的 IDE。
*   [构建和调试 GTK](docs/linux/building_debug_gtk.md) - 使用较低的优化级别和/或更多调试符号来针对 GTK 构建 Chromium。
*   [调试 GTK](docs/linux/debugging_gtk.md) - 使用 GTK 调试包和相关工具。
*   [Chroot 说明](docs/linux/using_a_chroot.md) - 设置 chroot 以解决某些 Linux 版本中 libfreetype 的差异。
*   [Linux 沙箱机制](sandbox/linux/README.md) - Linux 多进程模型，用于隔离具有不同权限的浏览器组件。
*   [Zygote 进程](docs/linux/zygote.md) - Linux Zygote 进程（用于派生新进程）的工作原理。
*   [在 Linux 上运行 Web 测试](docs/testing/web_tests_linux.md) - 运行 Web 测试的 Linux 专属说明。
*   [Linux Sysroot 镜像](docs/linux/sysroot.md) - 构建如何在 Linux 上使用库
*   [Linux 硬件视频解码](docs/linux/hw_video_decode.md) - 在 Linux 上启用硬件视频解码路径

### 其他 MacOS 专属文档
*   [Mac 调试技巧](docs/mac/debugging.md) - Mac 调试入门介绍，以及一系列有用的技巧。
*   [在 Mac 上使用 CCache](docs/ccache_mac.md) - 在 Mac 上使用 ccache 配合 clang/ninja 加速构建
*   [Cocoa 技巧与窍门](docs/cocoa_tips_and_tricks.md) - 编写 Cocoa 视图和控制器时使用的惯用法集合

### 其他 Windows 专属文档
*   [处理 cygwin rebaseall 故障](docs/cygwin_dll_remapping_failure.md)
*   [在 Chromium 中开发 ANGLE](docs/angle_in_chromium.md) - 基于 DirectX 构建的 OpenGL ES 2.0
*   [Windows 拆分 DLL](docs/windows_split_dll.md) - 将 `chrome.dll` 拆分为多个 DLL，以解决 Windows 上的工具链限制。
*   [Windows 原生窗口遮挡追踪](docs/windows_native_window_occlusion_tracking.md)
*   [Windows PWA 集成](docs/windows_pwa_integration.md) - 与 Windows 上渐进式 Web 应用的集成
*   [Windows 快捷方式和任务栏处理](docs/windows_shortcut_and_taskbar_handling.md)
*   [Windows 虚拟桌面集成](docs/windows_virtual_desktop_handling.md)

### 其他 Android 专属文档
*   [Chrome for Android 中的 Google Play 服务](docs/google_play_services.md)
*   [在 Java 中访问 C++ 枚举](docs/android_accessing_cpp_enums_in_java.md) - 如何在 Java 代码中使用 C++ 定义的枚举
*   [在 Android 上分析 Content Shell](docs/profiling_content_shell_on_android.md) - 在 Android 上为 `content_shell` 设置性能分析
*   [远程操作 Android](docs/working_remotely_with_android.md) - 在远程机器上为连接到本地机器的 Android 设备进行构建
*   [Android 测试说明](docs/testing/android_test_instructions.md) - 在 Android 设备或模拟器上运行构建。
*   [Android 调试](docs/android_debugging_instructions.md) - 调试 Android 上运行的 Java 和/或 C/C++ 代码的工具和技巧。
*   [Android 日志](docs/android_logging.md) - Chrome 的日志 API 如何在 Android 上与 `android.util.Log` 配合使用，以及使用指南。
*   [Android Java 静态分析](build/android/docs/lint.md) - 使用 'lint' 工具在编译时捕获 Java 相关问题。
*   [Java 代码覆盖率](build/android/docs/coverage.md) - 使用 EMMA 工具收集代码覆盖率数据。
*   [动态功能模块 (DFM)](docs/android_dynamic_feature_modules.md) - 它们是什么以及如何创建新模块。
*   [其他构建相关的 Android 文档](build/android/docs/README.md)
*   [Chrome for Android UI](docs/ui/android/overview.md) - 开发 UI 的资源和最佳实践

### 其他 iOS 专属文档
*   [Chromium for iOS 的持续构建和测试基础设施](docs/ios/infra.md)
*   [在 Chrome for iOS 中打开链接](docs/ios/opening_links.md) - 如何让您的 iOS 应用在 Chrome 中打开链接。
*   [Chrome for iOS 的 User Agent](docs/ios/user_agent.md) - 关于 Chrome for iOS 中 User Agent 字符串的说明。
*   [本地运行 iOS 测试套件](docs/ios/testing.md)
*   [在 iOS 中管理项目文件](docs/ios/working_with_files.md) - 如何在 iOS Chromium 项目中添加、删除和重命名文件。

### 其他 Chrome OS 专属文档
*   [设置强制门户和其他受限网络](docs/login/restrictive_networks.md)
*   [企业注册](docs/enterprise/enrollment.md)
    *   [Kiosk 模式和公共会话](docs/enterprise/kiosk_public_session.md)
*   [调试 OOBE/登录/锁屏中的 UI](docs/login/ui_debugging.md)
*   [Chrome OS 上的 Chrome 日志](docs/chrome_os_logging.md)
*   [调试技巧](docs/testing/chromeos_debugging_tips.md)

### 其他 WebUI 专属文档
*   [WebUI 说明](docs/webui/webui_explainer.md) - 关于使用 Web 技术（即 chrome:// URL）实现的 Chrome UI 的 C++ 和 TypeScript 基础设施代码的说明。
*   [优化 Chrome Web UI](docs/webui/optimizing_web_uis.md) - 提高 WebUI 性能的注意事项
*   [WebUI 上的 Trusted Types](docs/webui/trusted_types_on_webui.md) - 在 WebUI 中编码时注意 Trusted Types 的技巧。
*   [chrome-untrusted:// 常见问题](docs/webui/chrome_untrusted.md) - 关于使用 `chrome-untrusted://` 方案托管处理不可信内容的 WebUI 的说明。

### 媒体
*   [音频焦点处理](docs/media/audio_focus.md) - 多个 MediaSession 音频流如何交互
*   [HTMLMediaElements 的自动播放](docs/media/autoplay.md) - HTMLMediaElements 如何实现自动播放。
*   [延迟追踪](docs/media/latency_tracing.md) - 如何使用 `"audio.latency"` 追踪类别来测量音频延迟。
*   [Piranha Plant](docs/piranha_plant.md) - MediaStreams 的未来架构
*   [媒体捕获](docs/media/capture/README.md) - 使浏览器能够从自身或底层操作系统捕获像素和音频的功能和 API。
*   [视频编码加速器测试](docs/media/gpu/video_encoder_test_usage.md) - 如何使用加速视频编码器测试程序。
*   [视频解码器测试](docs/media/gpu/video_decoder_test_usage.md) - 运行视频解码器测试。
*   [视频解码器性能测试](docs/media/gpu/video_decoder_perf_test_usage.md) - 运行视频解码器性能测试。

### 无障碍
*   [无障碍概述](docs/accessibility/overview.md) - Chromium 中无障碍相关问题和方法的概述。
*   [无障碍测试](docs/accessibility/browser/tests.md) - 在代码库中查找无障碍相关测试的位置。
*   [Chrome OS 上的 ChromeVox](docs/accessibility/os/chromevox.md) - 在 Chrome OS 上启用语音反馈 (ChromeVox)。
*   [桌面 Linux 上的 ChromeVox](docs/accessibility/os/chromevox_on_desktop_linux.md) - 在桌面 Linux 上启用语音反馈 (ChromeVox)。
*   [离屏、不可见和尺寸](docs/accessibility/browser/offscreen.md) - Chrome 如何在无障碍树中定义离屏、不可见和尺寸。
*   [文本转语音](docs/accessibility/browser/tts.md) - Chrome 和 Chrome OS 中文本转语音的概述。
*   [Chrome OS 中的 BRLTTY](docs/accessibility/os/brltty.md) - Chrome OS 与 BRLTTY 集成以支持可刷新盲文显示器
*   [Chrome OS 上的 PATTS](docs/accessibility/os/patts.md) - 关于 Chrome OS 上使用的 PATTS 语音合成引擎的说明
*   [VoiceOver](docs/ios/voiceover.md) - 在 iOS 上的 Chromium 中使用 Apple 的 VoiceOver 功能。

### 内存
*   [内存概述](docs/memory/README.md)
*   [使用外部工具进行堆分析](docs/memory/heap_profiling_external.md)

### 内存基础设施时间线分析 (MemoryInfra)
*   [概述](docs/memory-infra/README.md)
*   [GPU 分析](docs/memory-infra/probe-gpu.md)
*   [为组件添加追踪](docs/memory-infra/adding_memory_infra_tracing.md)
*   [启用启动追踪](docs/memory-infra/memory_infra_startup_tracing.md)
*   [CC 中的内存使用](docs/memory-infra/probe-cc.md)
*   [内存基准测试](docs/memory-infra/memory_benchmarks.md)
*   [堆分析](docs/memory-infra/heap_profiler.md)

### 指标
*   [直方图](tools/metrics/histograms/README.md)
*   [用户操作](tools/metrics/actions/README.md)
*   [代码审查指南](tools/metrics/histograms/review_guidelines.md)

### 其他
*   [常用 URL](docs/useful_urls.md) - 各种工具和仪表板的链接集合
*   [ERC IRC](docs/erc_irc.md) - 在 EMACS 中使用 ERC 访问 IRC
*   [Kiosk 模式](docs/kiosk_mode.md) - 模拟 Kiosk 模式。
*   [用户标识映射](docs/user_handle_mapping.md) - 开发者在 Chromium/IRC/Google 中的名称
*   [文档最佳实践](docs/documentation_best_practices.md)
*   [文档指南](docs/documentation_guidelines.md)
*   [Chromium 浏览器与 Google Chrome 的区别](docs/chromium_browser_vs_google_chrome.md) -
    _Chromium 浏览器_ 和 _Google Chrome_ 有什么区别？
*   [Google Chrome 品牌构建](docs/google_chrome_branded_builds.md)
*   [使用 WPAD 的代理自动配置](docs/proxy_auto_config.md) - WPAD 服务器如何用于自动设置代理。
*   [在 VMWare 上安装 Chromium OS](docs/installation_at_vmware.md) - 如何在 VMWare 上安装 Chromium OS。
*   [用户数据目录](docs/user_data_dir.md) - 各平台上用户数据和缓存目录的确定方式。
*   [用户数据存储](docs/user_data_storage.md) - 用户数据中文件的策略文档。

### Mojo 与服务
*   [Mojo 与服务入门](docs/mojo_and_services.md) - Chromium 中 Mojo 和服务的快速入门介绍，含示例
*   [Mojo API 参考](mojo/README.md) - 关于 Mojo 各方面的详细参考文档
*   [服务开发指南](services/README.md) - Chromium 代码树中的服务开发指南
*   [Chromium 功能服务化](docs/servicification.md) - 将新的和现有的子系统集成为 Chromium 服务的通用建议
*   [将旧版 IPC 转换为 Mojo](docs/mojo_ipc_conversion.md) - 实际 IPC 转换工作的技巧和常见模式
*   [Mojo "风格" 指南](docs/security/mojo.md) - 来自 Mojo 和 IPC 审查员的最佳实践建议
*   [D-Bus Mojo 连接服务](docs/dbus_mojo_connection_service.md) - Chrome 中用于引导 CrOS 服务 Mojo 连接的服务。

### 安全
*   [二选一规则](docs/security/rule-of-2.md) - 处理不可信内容（如从网络下载的任何内容）时的重要安全规则。

### 速度
*   [Chrome 速度](docs/speed/README.md) - Chrome 中性能测量和性能回归的文档。
*   [Chrome 速度指标](docs/speed_metrics/README.md) - 关于 Web 上的用户体验指标及其 JavaScript API 的文档。

### UI
*   [Chromium UI 平台](docs/ui/index.md) - 用户界面相关的一切

### "What's Up With That" 文字稿

这些是 [What's Up With
That](https://www.youtube.com/playlist?list=PL9ioqAuyl6ULIdZQys3fwRxi3G3ns39Hq)
的文字稿，该系列视频是对 Chromium 软件工程师的访谈。

*   [What's Up With Pointers - 第 1 集](docs/transcripts/wuwt-e01-pointers.md)
*   [What's Up With DCHECKs - 第 2 集](docs/transcripts/wuwt-e02-dchecks.md)
*   [What's Up With //content - 第 3 集](docs/transcripts/wuwt-e03-content.md)
*   [What's Up With Tests - 第 4 集](docs/transcripts/wuwt-e04-tests.md)
*   [What's Up With BUILD.gn - 第 5 集](docs/transcripts/wuwt-e05-build-gn.md)
*   [What's Up With Open Source - 第 6 集](docs/transcripts/wuwt-e06-open-source.md)
*   [What's Up With Mojo - 第 7 集](docs/transcripts/wuwt-e07-mojo.md)
*   [What's Up With Processes - 第 8 集](docs/transcripts/wuwt-e08-processes.md)
*   [What's Up With Site Isolation - 第 9 集](docs/transcripts/wuwt-e09-site-isolation.md)
*   [What's Up With Web Platform - 第 10 集](docs/transcripts/wuwt-e10-web-platform.md)
*   [What's Up With Web Standards - 第 11 集](docs/transcripts/wuwt-e11-web-standards.md)
*   [What's Up With Base - 第 12 集](docs/transcripts/wuwt-e12-base.md)

### 可能已过时
*   [TPM 快速参考](docs/tpm_quick_ref.md) - 可信平台模块 (Trusted Platform Module) 说明。
*   [系统加固功能](docs/system_hardening_features.md) - 当前和计划中的 Chrome OS 安全功能列表。
*   [WebView 策略](docs/webview_policies.md)
*   [Linux 性能分析](docs/linux/profiling.md) - 如何在 Linux 上对 Chromium 进行性能分析
*   [Linux 图形管线](docs/linux/graphics_pipeline.md)
*   [Linux `SUID` 沙箱](docs/linux/suid_sandbox.md) - 在 Linux 上使用 SUID 二进制文件对渲染器进行沙箱化
*   [Linux `SUID` 沙箱开发](docs/linux/suid_sandbox_development.md) -
    上述系统的开发说明。
*   [Linux PID 命名空间支持](docs/linux/pid_namespace_support.md)
*   [Vanilla msysgit 工作流](docs/vanilla_msysgit_workflow.md) - 在 Windows 上使用基本原生 Git 的工作流。
*   [旧版选项](docs/chrome_settings.md) - Material Design 之前的 chrome://settings 说明。

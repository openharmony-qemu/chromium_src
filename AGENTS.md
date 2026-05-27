# Chromium Src（ArkWeb 内核层）指引

## 项目定位

本仓库对应 Chromium 源码的 `src/` 目录，是 ArkWeb 引擎的内核层。与 `chromium_cef`（CEF 封装层）、`chromium_arkweb`（ArkWeb 定制层，联合编译时挂载为 `arkweb/` 目录）、`chromium_third_party`（第三方库）联合编译为 NWeb.hap。

本仓库在上游 Chromium 基础上新增 `ohos/`、`ohos_resources/` 等目录，承载 OpenHarmony 平台的系统适配层。对 Chromium 的定制修改则通过 `arkweb/chromium_ext/`（在 chromium_arkweb 仓库中）以 `_for_include` 机制注入原文件，原文件仅添加 `#include` 和条件编译宏。

## 代码地图

### Chromium 上游核心（遵循 Chromium 目录约定）

```
content/           Content Shell 层（浏览器/渲染/Utility 进程核心）
  browser/         浏览器进程（400+ 子目录，WebContents、导航、渲染宿主等）
  renderer/        渲染进程（Blink 绑定、DOM 操作、Frame 观察）
  public/          Content 公共接口
base/              Chromium 基础库（线程、文件、回调、JSON 等）
net/               网络栈（HTTP/HTTPS/QUIC/DNS/Cookie）
cc/                合成器（Compositor，渲染管线）
gpu/               GPU 进程（命令缓冲、Skia、GL/Vulkan）
mojo/              Mojo IPC（进程间通信框架）
ipc/               旧版 IPC（部分模块仍在使用）
ui/                UI 层（事件、控件、键盘、鼠标、触摸）
v8/                (third_party 内) V8 JavaScript 引擎
blink/             (third_party 内) Blink 渲染引擎（DOM/CSS/HTML）
chrome/            Chrome 浏览器（部分模块被 OH 复用）
components/        Chromium 组件（密码管理、安全浏览、下载等）
services/          Services 层（网络服务、存储服务等）
sandbox/           沙箱（进程隔离安全边界）
gin/               Gin（V8 ↔ C++ 绑定）
```

### OpenHarmony 扩展（本仓库维护）

```
ohos/
  adapter/           OH 系统适配层（40+ 适配器）
                       适配器将 Chromium 需要的系统服务抽象为统一接口
                       包含：accessibility / battery / bluetooth / browser /
                       cert_manager / device / drag_drop / file_manager /
                       font / geolocation / ime / media / net / notification /
                       printer / sensor / window 等
  patch/             OH 补丁目录
ohos_resources/     OH 资源文件
  components/       组件字符串资源（ohos_components_strings.grd）
  content_change_detection/  内容变化检测
  data_detector/    数据检测
  dom_tree_extractor/  DOM 树提取
  highlight_specified_content/  内容高亮
ohos_arkweb_playground/  开发测试用 Hap
ohos_sdk/           OH SDK 工具链（编译工具、签名证书等）
```

### ArkWeb 定制层（chromium_arkweb 仓库，联合编译时挂载为 `arkweb/`）

这是对 Chromium 定制修改的核心机制，目录结构镜像 Chromium 源码树。chromium_src 中的原文件仅添加一行 `#include` 或条件编译宏，完整实现放在 `arkweb/chromium_ext/` 对应目录。

```
arkweb/
  chromium_ext/              ArkWeb 定制代码（镜像 Chromium 目录结构）
    base/                    ← 镜像 base/
    content/                 ← 镜像 content/
    media/                   ← 镜像 media/
    third_party/blink/       ← 镜像 third_party/blink/
    net/                     ← 镜像 net/
    cc/                      ← 镜像 cc/
    ui/                      ← 镜像 ui/
    components/              ← 镜像 components/
    chrome/                  ← 镜像 chrome/
    services/                ← 镜像 services/
    gpu/                     ← 镜像 gpu/
    sandbox/                 ← 镜像 sandbox/
    ...                      共 26 个子目录
    chromium_ext.gni         源文件注册清单
  ohos_nweb/                 NWeb 核心（WebView API 层）
  ohos_adapter_ndk/          NDK 适配层
  build/features/
    features.gni             Feature Flag 定义
    BUILD.gn                 生成 C++ features.h 头文件
```

侵入式修改的三种形态：

1. **`_for_include` 代码片段注入**（主要方式）：代码片段通过 `#include` 嵌入 Chromium 原文件
   - `xxx_for_include.cc` — 注入方法实现（在原文件 namespace 关闭后）
   - `xxx_for_include_public.h` — 注入类 public 成员（在 class 的 public: 区域）
   - `xxx_for_include_private.h` — 注入类 private 成员（在 class 的 private: 区域）
   - `xxx_before_for_include.cc` — 在原文件头部注入（额外 #include 等）
   - `xxx_for_include_file.cc` — 注入额外头文件依赖
2. **`_ext.*` 独立扩展类**：如 `render_thread_impl_ext.cc`、`web_contents_impl_ext.h`
3. **`_utils.*` 工具辅助类**：如 `sys_info_utils_ext.h`

原文件中的条件编译宏（按数量排列）：
- `BUILDFLAG(ARKWEB_*)` 具体功能 flag — **8087 处**，涉及 250+ 个功能开关
- `BUILDFLAG(IS_ARKWEB)` 平台级 flag — **693 处**
- `#include "arkweb/chromium_ext/*_for_include*"` 注入点 — **209 处**

读取原文件时，遇到 `BUILDFLAG(ARKWEB_*)` 或 `BUILDFLAG(IS_ARKWEB)` 即说明有定制修改，必须查看 `arkweb/chromium_ext/` 下对应的 `_for_include` / `_ext` / `_utils` 文件，才能获得完整实现。

### GN Label 重定向

根目录下同时存在同名的 `ohos_*` 文件和 `ohos_*` 目录。文件是 GN label 重定向，使 GN target `//ohos_nweb` 等指向 `arkweb/` 下的实际目录（由 chromium_arkweb 仓库联合编译时挂载）：

```
ohos_nweb       → arkweb/ohos_nweb
ohos_nweb_ex    → arkweb/ohos_nweb_ex
ohos_nweb_turbo → arkweb/ohos_nweb_ex/ohos_nweb_turbo
ohos_glue       → arkweb/glue
ohos_browser_shell → arkweb/ohos_nweb_ex/ohos_browser_shell
ohos_build      → arkweb/build
```

注意：这些重定向指向的 `arkweb/` 目录不在本仓库中，由 chromium_arkweb 仓库在联合编译时提供。

### 多仓库编译关系

```
chromium_src (本仓库)         ← Chromium 内核 + ohos/adapter 系统适配 + ohos_resources
  + chromium_arkweb           ← arkweb/chromium_ext/ 定制层 + ohos_nweb/ + ohos_adapter_ndk/
  + chromium_cef              ← CEF 封装 + OH 扩展 (ohos_cef_ext)
  + chromium_third_party      ← 第三方库 (blink/v8/skia/icu 等)
  ↓ 联合编译（chromium_arkweb 挂载为 src/arkweb/）
NWeb.hap (ArkWebCore)
```

## 知识路由

| 遇到什么问题 | 读什么 | 关键概念 |
|-------------|--------|---------|
| 浏览器进程如何管理页面 | `content/browser/` | WebContents 是核心对象，持有 RenderFrameHost 树 |
| 渲染进程如何工作 | `content/renderer/` | RenderFrameImpl 对应一个 Frame，通过 Mojo 与浏览器进程通信 |
| 进程间通信 | `mojo/public/` + `ipc/` | Mojo 是新 IPC（推荐），ipc/ 是旧版；浏览器↔渲染必须走 IPC |
| 网络请求处理 | `net/` | URLRequest → HttpStream → Socket；OH 通过 adapter/net 适配网络栈 |
| 沙箱机制 | `sandbox/` | 浏览器进程无沙箱，渲染/Utility/GPU 进程有沙箱 |
| OH 系统服务对接 | `ohos/adapter/` | 每个 adapter 子目录对应一个 OH 系统服务，如 battery / geolocation |
| GN 编译配置 | `BUILD.gn`（各目录） | Chromium 用 GN 构建，每个目录有 BUILD.gn |
| 新增 OH 适配器 | `ohos/adapter/` 目录结构 | 参考已有 adapter 的 BUILD.gn 和接口模式 |
| 字符串/资源 | `ohos_resources/` | GRIT 系统（.grd 文件），编译时生成头文件 |
| 编译产物部署 | `README_ZH.md` | build.sh + sign.sh，so 替换或 hap 替换两种调试方式 |
| 原文件遇到 `BUILDFLAG(IS_ARKWEB)` 或 `BUILDFLAG(ARKWEB_*)` | `arkweb/chromium_ext/` 下对应路径的 `_for_include` / `_ext` / `_utils` 文件 | 这是查找 ArkWeb 完整实现的关键路由；原文件中只有门控宏和 `#include`，实现代码在 chromium_ext/ |
| 新增 Chromium 功能定制 | `arkweb/build/features/features.gni` → `BUILD.gn` → `chromium_ext.gni` → `_for_include` 文件 | 四步走：定义 Flag → 注册到 features.h → 注册源文件到 chromium_ext.gni → 创建 _for_include 片段 |
| 理解某个 Feature Flag 的作用 | `arkweb/build/features/features.gni` | 命名规则 `arkweb_<模块>_<子功能>`，C++ 中用 `BUILDFLAG(ARKWEB_<UPPER>)` |

## 专家经验与约束

### 硬约束

- Chromium 上游代码（content/、base/、net/、cc/ 等目录）的侵入式修改遵循**最小化原则**：原文件仅添加 `#include "arkweb/chromium_ext/..."` 或 `#if BUILDFLAG(IS_ARKWEB)` / `#if BUILDFLAG(ARKWEB_*)` 条件编译块。不要直接修改 Chromium 原有逻辑，完整实现必须放在 `arkweb/chromium_ext/` 对应目录
- `ohos/adapter/` 中的适配器接口需与 OH 系统服务版本对齐
- 安全沙箱内的进程（渲染/Utility/GPU）不能直接访问系统资源，必须通过 browser 进程中转
- GN label 重定向（根目录 `ohos_*` 文件）指向 chromium_arkweb 仓库挂载的 `arkweb/` 目录，不是本仓库代码

### 依赖方向禁止

- ❌ 渲染进程代码不能直接调用 `ohos/adapter/`（必须通过 Mojo IPC 经由浏览器进程）
- ❌ `content/` 内部头文件不应被 `ohos/adapter/` 直接引用
- ❌ 不要在 `chrome/` 目录中添加 OH 特有功能（应放在 `ohos/` 或 `arkweb/chromium_ext/` 中）

### `_for_include` 编码规范

- `_for_include` 文件**必须**在顶部添加 `#error` 守卫，使用宿主文件的 include guard 宏名，防止被错误地独立编译
- `_for_include` 文件**不应**加入任何 BUILD.gn 的 `sources`，它们通过 `#include` 随宿主文件编译
- `#if` / `#else` 双分支**必须都能编译通过**
- Feature Flag 的声明和实现**必须使用同一个 flag**
- `_for_include_private.h` 中添加的成员变量也必须门控，且与使用它的方法使用同一个 flag
- Chromium BUILD.gn 中的 `import` 语句放在 `if (is_arkweb)` 内部，避免非 ArkWeb 构建时解析

### 反模式

- 不要绕过 `ohos/adapter/` 直接在 Chromium 代码中调用 OH 系统服务
- 不要用 `base::Thread` 创建非托管线程，Chromium 有 `content::BrowserThread::IO` 等托管线程
- 不要在 IO 线程做阻塞操作（文件 IO、同步等待）
- 不要忽略 `ohos/adapter/BUILD.gn` 中的 `is_ohos` 条件编译
- 不要在 Chromium 原文件中直接编写大量 ArkWeb 逻辑，应拆分到 `_for_include` 文件
- 不要将 `_for_include` 文件当作独立源文件使用（它依赖宿主文件的命名空间和头文件上下文）

### 关键黑话

| 术语 | 含义 |
|------|------|
| WebContents | 浏览器进程中的"一个标签页"，持有页面完整状态 |
| RenderFrameHost | 浏览器进程侧的 Frame 代表，对应渲染进程中的 RenderFrameImpl |
| RFH / RFHImpl | RenderFrameHost / RenderFrameHostImpl 的缩写 |
| Mojo | Chromium 进程间通信框架，替代旧版 IPC |
| compositor | 合成器（cc/），将渲染结果合成到屏幕 |
| GRIT | Google Resource and Internationalization Tool（.grd 资源文件系统） |
| adapter | OH 系统适配层（`ohos/adapter/`），将 Chromium 需要的系统调用映射到 OH API |
| `_for_include` | ArkWeb 自创的源码注入机制：代码片段通过 `#include` 嵌入 Chromium 原文件，继承宿主的命名空间和上下文 |
| `_ext` | 独立扩展类文件，如 `render_thread_impl_ext.cc`，不依赖宿主文件上下文 |
| `chromium_ext/` | ArkWeb 定制代码目录（在 chromium_arkweb 仓库中），镜像 Chromium 目录结构 |
| `chromium_ext.gni` | 所有扩展源文件的中央注册表 |

## 编译

### 从 OH 源码根目录

```sh
# 编译 rk3568（含 hap）
./build.sh -t w -A rk3568

# 仅编译 so
./build.sh -A rk3568

# 编译 rk3568_64
./build.sh -t w -A rk3568_64
./build.sh -A rk3568_64

# 签名
./sign.sh rk3568
```

提交使用 `git commit -s`，保留 DCO Signed-off-by 签名。

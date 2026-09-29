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
    ...                      按 chromium 子目录一一镜像
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
- `BUILDFLAG(ARKWEB_*)` 具体功能 flag — 涉及大量功能开关
- `BUILDFLAG(IS_ARKWEB)` 平台级 flag
- `#include "arkweb/chromium_ext/*_for_include*"` 注入点

读取原文件时，遇到 `BUILDFLAG(ARKWEB_*)` 或 `BUILDFLAG(IS_ARKWEB)` 即说明有定制修改，必须查看 `arkweb/chromium_ext/` 下对应的 `_for_include` / `_ext` / `_utils` 文件，才能获得完整实现。

首次编写定制代码时，参考以下范例对齐写法：
- `_for_include` 片段注入范式：`arkweb/chromium_ext/content/renderer/render_thread_impl_for_include.cc`（片段随宿主文件编译，顶部带 `#error` 守卫）
- `_ext` 独立扩展类范式：`arkweb/chromium_ext/content/renderer/render_thread_impl_ext.cc`（不依赖宿主上下文）
- `_utils` 工具类范式：`arkweb/chromium_ext/base/sys_info_utils_ext.h`

若 `arkweb/` 未挂载（单仓 checkout），无法读到 `_for_include` / `_ext` / `_utils` 实现时，先声明该实现缺失并向用户确认，不要仅凭原文件的门控宏推断定制逻辑的实现。

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

### 嵌套指令文件

本文件为仓库根级默认指引。子目录若含自身 `AGENTS.md` / `CLAUDE.md` / skill，以就近文件为准，其规则优先于本文件。进入 `ohos/adapter/`、`arkweb/chromium_ext/` 等子目录作业前，先检查是否存在更具体的指令文件。

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

### 修改前须先声明

修改 Chromium 原文件前，先说明以下三点，再动手：

1. **命中的路由**：命中哪个 `BUILDFLAG(IS_ARKWEB)` / `BUILDFLAG(ARKWEB_*)` 或注入点（`#include "arkweb/chromium_ext/..."`）
2. **已读的实现**：已查阅的 `arkweb/chromium_ext/` 对应 `_for_include` / `_ext` / `_utils` 文件
3. **采用的形态**：本次改动用哪种注入形态（`_for_include` 片段 / `_ext` 独立类 / `_utils` 工具类），以及涉及的 Feature Flag

改 `ohos/adapter/` 时同样先声明：对接的 OH 系统服务、依赖的接口与版本、以及数据是否需经 Mojo IPC 由浏览器进程中转（渲染进程不得直连 adapter）。

## 专家经验与约束

### 硬约束

- Chromium 上游代码（content/、base/、net/、cc/ 等目录）的侵入式修改遵循**最小化原则**：原文件仅添加 `#include "arkweb/chromium_ext/..."` 或 `#if BUILDFLAG(IS_ARKWEB)` / `#if BUILDFLAG(ARKWEB_*)` 条件编译块。不要直接修改 Chromium 原有逻辑，完整实现必须放在 `arkweb/chromium_ext/` 对应目录
- `ohos/adapter/` 中的适配器接口需与 OH 系统服务版本对齐
- 安全沙箱内的进程（渲染/Utility/GPU）不能直接访问系统资源，必须通过 browser 进程中转
- GN label 重定向（根目录 `ohos_*` 文件）指向 chromium_arkweb 仓库挂载的 `arkweb/` 目录，不是本仓库代码

### 不可手改（生成物）

以下为构建产物，只改数据源，勿直接编辑产物：

- `features.h`（由 `arkweb/build/features/features.gni` + `BUILD.gn` 生成）：改 Feature Flag 只动 `features.gni`
- GRIT 生成的头文件（由 `ohos_resources/**/*.grd` 生成）：改字符串/资源只动 `.grd`
- GN 构建缓存与 `out/` 目录产物：不纳入版本控制，不手改

### 改动前须确认（升级）

触及以下边界时，先说明兼容性影响并与用户确认，再动手：

- `content/public/` 等公共接口的签名、语义、生命周期、错误码——破坏兼容会波及所有调用方
- 沙箱 / 权限 / 信任边界（`sandbox/`、进程隔离策略）
- 序列化 / 协议格式 / 持久化数据结构 / 跨版本行为
- 第三方依赖版本或 license 敏感变更

### 真机与破坏性操作

- 真机部署（so 替换 / hap 替换 / 刷机）前，先确认目标设备与架构（rk3568 vs rk3568_64），设备型号不匹配的 so 会导致启动崩溃
- 不执行会清空设备数据或抹除分区的命令（如 `hdc` 全量擦除、`rm -rf` 系统目录）——需要时先说明影响并等用户确认
- 签名证书（`ohos_sdk/` 下）不提交、不外传、不改动

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
- 不要新增未分类的日志标签或改动崩溃归因（DFX）逻辑，须沿用现有 hilog / 日志标签规范；涉及故障归因、可观测性行为变更时先说明影响并确认

### 模块级修改约束

- arkweb渲染合成：涉及缓存清理或内存管理逻辑时，必须验证在高频分配场景（如动画帧循环）下内存申请与释放速率的平衡性，确认不会因缓存策略变更导致内存块堆积触碰上限（如PA 65536块）
- arkweb渲染引擎：子进程/子模块被外部机制（内核查杀、系统回收等）销毁时，必须设计"启动阶段被杀"与"运行中被杀"两条路径的死亡通知与逃生恢复逻辑，且自验必须覆盖两者

### 领域术语

| 术语 | 含义 |
|------|------|
| RFH / RFHImpl | RenderFrameHost / RenderFrameHostImpl 的缩写 |
| Mojo | Chromium 进程间通信框架，替代旧版 IPC |
| compositor | 合成器（cc/），将渲染结果合成到屏幕 |
| GRIT | Google Resource and Internationalization Tool（.grd 资源文件系统） |
| `adapter` | OH 系统适配层（`ohos/adapter/`），将 Chromium 需要的系统调用映射到 OH API |
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

## 验证与完成标准

### 最小验证

- 编译：改动涉及的 target 能编译通过 —— `./build.sh -A rk3568`（或对应架构 `rk3568_64`）
- 依赖检查：涉及 `BUILD.gn` 依赖变更时跑 `gn check`（本机 gn 在 `/opt/gn/gn`，ninja 在 `/opt/ninja/ninja`）
- 代码风格：C++ 改动用仓库根 `.clang-format` 格式化 —— `/opt/llvm/bin/clang-format -i --style=file <改动文件>`
- 提交前检查：本仓库带 Chromium 标准 `PRESUBMIT.py`，如本机装有 depot_tools，用 `git cl presubmit --upload` 跑；无 depot_tools 时至少人工核对本节 checklist

### 关于单元测试

本仓库以「目标架构编译通过」为主验证手段，`ohos/adapter/` 下暂无标准独立单测入口。若改动位于带 Chromium 单测的目录（如 `base/`、`net/`），可用 gn 生成对应 `*_unittests` target 并经 ninja 编译运行；否则以编译 + 真机冒烟为准，并在汇报中注明未跑单测的原因。

可直接运行的单测示例（以 `base/` 为例，按实际改动目录替换 target 与用例过滤）：

```sh
/opt/gn/gn gen out/rk3568
/opt/ninja/ninja -C out/rk3568 base_unittests
./out/rk3568/base_unittests --gtest_filter='<相关用例>'
```

无对应单测目录时以「编译通过 + 真机冒烟」为准，并在汇报中注明未跑单测的原因。

### 任务特定检查

- 改 `_for_include` 片段：确认 `#if` / `#else` 双分支都能编译通过（ArkWeb 与非 ArkWeb 构建各验证一次）
- 改 Feature Flag：确认声明与实现使用同一个 flag，`_for_include_private.h` 成员变量与使用它的方法门控一致
- 改 `ohos/adapter/`：确认 `BUILD.gn` 的 `is_ohos` 条件编译正确
- 改字符串/资源：确认从 `.grd` 重新生成，未手改生成头文件

### 完成定义（Done）

- 目标架构编译通过
- `_for_include` 双分支均可编译，门控 flag 前后一致
- C++ 改动已过 `.clang-format`，`BUILD.gn` 依赖变更已过 `gn check`
- 无遗留的直接改 Chromium 原有逻辑（改动限于 `#include` 注入或条件编译块）

### 汇报要求

完成后说明：改了哪些文件、命中哪个 Feature Flag / 注入点、跑了哪些验证命令，并附上每条命令的通过/失败结果（编译是否通过、单测 pass/fail 数、`gn check` / `clang-format` 是否干净）。命令失败或被跳过时如实说明，不得以「已完成」概括未验证项。

### 无法验证时

若因环境限制无法编译或运行检查，明确说明受阻原因和已完成/未完成的部分，不得谎报完成。


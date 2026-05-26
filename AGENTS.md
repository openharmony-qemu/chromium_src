# Chromium Src（ArkWeb 内核层）指引

## 项目定位

本仓库对应 Chromium 源码的 `src/` 目录，是 ArkWeb 引擎的内核层。与 `chromium_cef`（CEF 封装层）、`chromium_arkweb`（ArkWeb 适配层）联合编译为 NWeb.hap。本仓库在上游 Chromium 基础上新增 `ohos/`、`ohos_resources/` 等目录，承载 OpenHarmony 平台的 Chromium 补丁和扩展。

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

### OpenHarmony 扩展（本仓库维护重点）

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
```

### GN Label 重定向文件

根目录下的 `ohos_*` 文件是 GN label 重定向，内容为实际路径：

```
ohos_nweb       → arkweb/ohos_nweb
ohos_nweb_ex    → arkweb/ohos_nweb_ex
ohos_nweb_turbo → arkweb/ohos_nweb_ex/ohos_nweb_turbo
ohos_glue       → arkweb/glue
ohos_browser_shell → arkweb/ohos_nweb_ex/ohos_browser_shell
ohos_build      → arkweb/build
```

### 多仓库编译关系

```
chromium_src (本仓库)         ← Chromium 内核 + OH 适配器
  + chromium_cef              ← CEF 封装 + OH 扩展 (ohos_cef_ext)
  + chromium_arkweb           ← ArkWeb 适配层
  + chromium_third_party      ← 第三方库 (blink/v8/skia/icu 等)
  ↓ 联合编译
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

## 专家经验与约束

### 硬约束

- Chromium 上游代码（content/、base/、net/、cc/ 等目录）**不要直接修改**，修改必须通过 `ohos/patch/` 提交补丁或通过 Chromium 社区贡献
- `ohos/adapter/` 中的适配器接口需与 OH 系统服务版本对齐
- 安全沙箱内的进程（渲染/Utility/GPU）不能直接访问系统资源，必须通过 browser 进程中转
- GN label 重定向文件（根目录 `ohos_*` 文件）内容为路径引用，不是代码

### 依赖方向禁止

- ❌ 渲染进程代码不能直接调用 `ohos/adapter/`（必须通过 Mojo IPC 经由浏览器进程）
- ❌ `content/` 内部头文件不应被 `ohos/adapter/` 直接引用
- ❌ 不要在 `chrome/` 目录中添加 OH 特有功能（应放在 `ohos/` 中）

### 反模式

- 不要绕过 `ohos/adapter/` 直接在 Chromium 代码中调用 OH 系统服务
- 不要用 `base::Thread` 创建非托管线程，Chromium 有 `content::BrowserThread::IO` 等托管线程
- 不要在 IO 线程做阻塞操作（文件 IO、同步等待）
- 不要忽略 `ohos/adapter/BUILD.gn` 中的 `is_ohos` 条件编译

### 关键黑话

| 术语 | 含义 |
|------|------|
| WebContents | 浏览器进程中的"一个标签页"，持有页面完整状态 |
| RenderFrameHost | 浏览器进程侧的 Frame 代表，对应渲染进程中的 RenderFrameImpl |
| RFH / RFHImpl | RenderFrameHost / RenderFrameHostImpl 的缩写 |
| Mojo | Chromium 进程间通信框架，替代旧版 IPC |
| compositor | 合成器（cc/），将渲染结果合成到屏幕 |
| GRIT | Google Resource and Internationalization Tool（.grd 资源文件系统） |
| adapter | OH 系统适配层，将 Chromium 需要的系统调用映射到 OH API |
| GN | Generate Ninja，Chromium 的构建配置系统 |
| is_ohos | GN 编译变量，标识是否为 OH 平台构建 |

## 编译和测试

### 编译（从 OH 源码根目录）

```sh
# 下载代码
repo init -u https://gitcode.com/openharmony-tpc/manifest -b master -m developer.xml --no-repo-verify
repo sync -c
repo forall -c 'git lfs pull'

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

### 最小验证

```sh
# 方法一：替换 so
hdc shell "mount -o remount,rw /"
hdc file send libnweb_render.so /data/app/el1/bundle/public/com.ohos.nweb/libs/arm
hdc file send libweb_engine.so /data/app/el1/bundle/public/com.ohos.nweb/libs/arm
hdc shell reboot

# 方法二：替换 hap
hdc shell "mount -o remount,rw /"
hdc file send NWeb-rk3568.hap /system/app/com.ohos.nweb/NWeb.hap
hdc shell "rm /data/* -rf"
hdc shell reboot
```

### 调试

```sh
hdc hilog | grep -i arkweb     # ArkWeb 日志
hdc hilog | grep -i nweb       # NWeb 日志
```

提交使用 `git commit -s`，保留 DCO Signed-off-by 签名。

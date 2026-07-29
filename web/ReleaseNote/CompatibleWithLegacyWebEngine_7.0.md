# M132内核在OpenHarmony 7.0系统上的适配指导

## 1. Web内核切换API使用说明

OpenHarmony 7.0系统ArkWebCore内核默认升级到了M144版本，同时系统提供了双内核方案，保留老的M132版本内核，以便生态应用自主选择M132升级到M144的节奏和策略，降低应用因Web内核升级而导致的兼容性问题。

两种Web内核类型说明：

| **内核类型** | **英文**   | **说明**  |
| ----------- | ---------- | -------- |
| 常青内核     | EVERGREEN WebCore | 当前系统的最新版Web内核，系统基于此类型的内核进行完整的功能实现，推荐应用使用。|
| 遗留内核     | LEGACY WebCore    | 复用上一个商用版本的内核。遗留内核仅作为兼容性回滚使用，不包含后续安全补丁修复，存在安全风险；新的OpenHarmony系统发布时，不一定必选支持；且遗留内核的支持有时间限制，一般在系统发布后半年后会完全禁用掉。 |

双内核相关API:

```
enum ArkWebEngineVersion {
    SYSTEM_DEFAULT = 0,
    M114 = 1,
    M132 = 2,
    M144 = 3,
    ARKWEB_EVERGREEN = 99999
}
static setActiveWebEngineVersion(engineVersion: ArkWebEngineVersion): void;
static getActiveWebEngineVersion(): ArkWebEngineVersion;
static isActiveWebEngineEvergreen(): boolean;
```

ArkWebEngineVersion枚举值定义：

| **枚举值** | **内核类型** | **说明** |
| :------: | ------ | ------ |
| M144 | 7.0版本的常青内核 | 7.0版本上的默认内核。如果后续OpenHarmony系统版本上不存在此内核则设置无效，使用系统默认内核。 |
| M132 | 7.0版本的遗留内核 | 开发者可选择此遗留内核。如果后续OpenHarmony系统版本上不存在此内核则设置无效，使用系统默认内核。 计划在2027-Q2禁用此内核。 |
| SYSTEM_DEFAULT | 系统默认 | 使用系统上默认内核，7.0版本上默认为M144。 |
| ARKWEB_EVERGREEN | 常青内核，系统的最新内核 | 开发者可选择在每个系统版本上都使用最新的内核，6.1以及之后所有系统版本都生效。<br>**说明：** 从API version 23开始支持|

下文调用方式可参考完整示例工程 [DualWebCore](https://gitcode.com/openharmony/applications_app_samples/blob/master/code/DocsSample/ArkWeb/DualWebCore)，包含 ArkTS 调用（方式一）与 native dlopen 实现（方式三），方式二（直接调用 NDK）仅以注释提示。示例中的内核版本号仅为演示，按业务需要选择即可。

### 切换内核前的准备：清理 Web 缓存

切换到低版本（遗留）内核前，需要先清理应用沙箱中 `/data/storage/el2/base/cache/web` 目录下的 Web 缓存数据，避免新旧内核缓存不兼容导致异常（详见第 2 章「使用遗留内核的风险说明」的数据一致性说明）。该清理操作通过 NAPI 实现，与下文三种内核切换方式相互独立；下文各方式示例中的 `testNapi.deleteWebCache()` 即调用此实现，以下函数与下文各方式的 napi 函数位于同一个 `napi_init.cpp`（`libentry.so`）中。

```
// napi_init.cpp

static void DeleteDirectoryRecursivelyImpl(const std::string& path)
{
    try {
        // 检查路径是否存在
        if (!fs::exists(path)) {
            OH_LOG_ERROR(LOG_APP, "Directory does not exist: %{public}s", path.c_str());
            return;
        }
        // 递归删除目录及其内容
        fs::remove_all(path);
        OH_LOG_INFO(LOG_APP, "Successfully deleted directory: %{public}s", path.c_str());
    } catch (const fs::filesystem_error& e) {
        OH_LOG_ERROR(LOG_APP, "Filesystem error: %{public}s", e.what());
    } catch (const std::exception& e) {
        OH_LOG_ERROR(LOG_APP, "Error: %{public}s", e.what());
    }
}


static void DeleteWebCacheImpl()
{
    DeleteDirectoryRecursivelyImpl("/data/storage/el2/base/cache/web");
}

static napi_value DeleteWebCache(napi_env env, napi_callback_info info)
{
    DeleteWebCacheImpl();
    return nullptr;
}
```

### 方式一：ArkTS 接口（需 SDK 20 及以上）

应用在Web组件加载之前，可以通过接口 [setActiveWebEngineVersion](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#setactivewebengineversion20) 指定ArkWebCore内核的版本：

```
// EntryAbility.ets

import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { webview } from '@kit.ArkWeb';
import { hilog } from '@kit.PerformanceAnalysisKit';
import testNapi from 'libentry.so';

const DOMAIN = 0x0000;

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {

    // 设置低版本 web内核之前推荐清理web缓存
    testNapi.deleteWebCache();

    // 设置web内核为M132
    webview.WebviewController.setActiveWebEngineVersion(webview.ArkWebEngineVersion.M132);

    // 查询并打印内核版本
    hilog.info(DOMAIN, 'testTag', 'webVersion = %{public}d', webview.WebviewController.getActiveWebEngineVersion());
  }
}
```

### 方式二：NDK 接口直接调用（需 SDK 20 及以上）

也可以通过 NDK 接口 [OH_NativeArkWeb_SetActiveWebEngineVersion](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/capi-native-interface-arkweb-h.md#oh_nativearkweb_setactivewebengineversion) 实现（需包含头文件 `web/native_interface_arkweb.h` 并链接 `libohweb.so`）：

```
// napi_init.cpp

static napi_value GetWebVersion(napi_env env, napi_callback_info info)
{
    // 查询内核版本
    int version = static_cast<int>(OH_NativeArkWeb_GetActiveWebEngineVersion());

    napi_value ret;
    napi_create_int32(env, version, &ret);
    return ret;
}

static napi_value SetWebVersion(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = {nullptr};

    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    napi_valuetype valuetype0;
    napi_typeof(env, args[0], &valuetype0);

    int32_t value0;
    napi_get_value_int32(env, args[0], &value0);

    // 设置内核版本
    OH_NativeArkWeb_SetActiveWebEngineVersion(static_cast<ArkWebEngineVersion>(value0));

    return nullptr;
}
```

### 方式三：NDK 接口动态调用（dlopen，兼容未适配 SDK 20 的应用）

适用场景：应用未适配 SDK 20，编译期没有 `web/native_interface_arkweb.h` 头文件，通过运行时 `dlopen` 加载 `libohweb.so` 并 `dlsym` 获取接口指针来调用同一组接口：

```
// EntryAbility.ets

import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import testNapi from 'libentry.so';

const DOMAIN = 0x0000;

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {

    // 设置低版本 web内核之前推荐清理web缓存
    testNapi.deleteWebCache();

    // 设置132 web内核
    testNapi.setWebVersion(2);

    // 打印当前web内核信息
    hilog.info(DOMAIN, 'testTag', 'webVersion = %{public}d', testNapi.getWebVersion());
  }
}
```

```
// CMakeLists.txt

add_library(entry SHARED napi_init.cpp)
target_link_libraries(entry PUBLIC libhilog_ndk.z.so)  # 本示例使用 OH_LOG_* 打印日志
# libohweb.so 由 dlopen 在运行时加载，无需在此链接
```

```
// napi_init.cpp

static void SetWebVersionImpl(int version)
{
    void* handle = dlopen("libohweb.so", RTLD_LAZY);
    if (!handle) {
        OH_LOG_ERROR(LOG_APP, "Failed to load library: libohweb.so");
        return;
    }
    
    typedef void (*FuncPtr)(int a);
    FuncPtr func = (FuncPtr)dlsym(handle, "OH_NativeArkWeb_SetActiveWebEngineVersion");
    if (!func) {
        OH_LOG_ERROR(LOG_APP, "Failed to find symbol: OH_NativeArkWeb_SetActiveWebEngineVersion");
        dlclose(handle);
        return;
    }
    
    func(version); // 调用目标函数
    dlclose(handle);
}


static napi_value SetWebVersion(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = {nullptr};

    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    napi_valuetype valuetype0;
    napi_typeof(env, args[0], &valuetype0);

    int32_t value0;
    napi_get_value_int32(env, args[0], &value0);
    
    SetWebVersionImpl(value0);

    return nullptr;
}

static int GetWebVersionImpl()
{
    void* handle = dlopen("libohweb.so", RTLD_LAZY);
    if (!handle) {
        OH_LOG_ERROR(LOG_APP, "Failed to load library: libohweb.so");
        return 0;
    }
    
    typedef int (*FuncPtr)(void);
    FuncPtr func = (FuncPtr)dlsym(handle, "OH_NativeArkWeb_GetActiveWebEngineVersion");
    if (!func) {
        OH_LOG_ERROR(LOG_APP, "Failed to find symbol: OH_NativeArkWeb_GetActiveWebEngineVersion");
        dlclose(handle);
        return 0;
    }
    
    int ret = func(); // 调用目标函数
    dlclose(handle);
    return ret;
}

static napi_value GetWebVersion(napi_env env, napi_callback_info info)
{
    int version = GetWebVersionImpl();

    napi_value ret;
    napi_create_int32(env, version, &ret);
    return ret;
}
```

> **注意：**
> 如果调用该接口有以下可能失败原因：
> 
> * **内核已经初始化**：此接口只能在内核初始化前调用才能生效，初始化后调用不会生效。
> * **系统没有预置指定版本的内核**：系统版本发布后，一些产品可能不支持双内核，此时此接口调用不会生效。
> * **指定版本的内核已经失效**：遗留内核的生命周期通常在系统发布半年后，生命周期结束后，指定遗留内核版本不会生效。
> * 本接口调用是否生效，可以通过[getActiveWebEngineVersion](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#getactivewebengineversion20)或[OH_NativeArkWeb_GetActiveWebEngineVersion](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/capi-native-interface-arkweb-h.md#oh_nativearkweb_getactivewebengineversion)接口查询实际生效的内核版本来判断。

## 2. 使用遗留内核的风险说明

ArkWeb常青内核是新系统默认的配套内核，在功能、标准遵循度、安全性、性能方面都有全方位的增强。应用需首选使用常青内核，遗留内核仅作为兼容性问题的应急回滚方案。遗留内核不包含后续的安全补丁修复，长期使用存在安全风险。

OH7.0版本ArkWeb由M132内核升级到M144内核，详细变化及收益参考《[ArkWeb版本的差异总结](./ArkWeb_132_144.md)》。

以下ArkWeb接口从API version 26开始支持，且仅在M144常青内核上实现：

| **类型** | **接口** |
| -------- | -------- |
| WebviewController | [enableAdvancedSecurityMode](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#enableadvancedsecuritymode) |
| WebviewController | [executeAIPageCommand](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#executeaipagecommand) |
| Web组件 | [aiSessionOptions](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-attributes.md#aisessionoptions) |
| Web组件 | [enableDrag](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-attributes.md#enabledrag) |
| Web组件 | [enableFullscreenVideoOverlay](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-attributes.md#enablefullscreenvideooverlay) |
| Web组件 | [keyboardAppearance](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-attributes.md#keyboardappearance) |
| Web组件 | [onInputmethodAttached](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-events.md#oninputmethodattached) |
| Web组件 | [scrollbarLayoutPolicy](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-attributes.md#scrollbarlayoutpolicy) |

应用如需使用上述接口，请使用M144常青内核。在M132遗留内核上调用上述接口可能不生效或无法获得预期结果，存在业务功能和使用体验的兼容性风险。

应用使用遗留内核前，需要评估以下信息：

* 双内核兼容性：部分ArkWeb新增API仅支持常青内核，应用开发者需评估这些API在遗留内核上的行为，做好兼容性保障。
* 数据一致性：应用在由常青内核降级回滚到遗留内核时，WEB相关的缓存数据可能不被遗留内核支持；在降级回滚时，必须先清理应用沙箱中/data/storage/el2/base/cache/web目录下的WEB缓存数据，确保回滚后可正常工作。

## 3. 使用遗留内核的代码隔离方式

应用如果支持切换至M132遗留内核，需要在调用仅M144常青内核支持的接口前判断当前内核类型。

以[executeAIPageCommand](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#executeaipagecommand)接口为例，该接口仅在M144常青内核上实现，可按以下方式进行代码隔离：

<!-- @[call_executeaipagecommand_based_on_engine_version](https://gitcode.com/openharmony/applications_app_samples/blob/master/code/DocsSample/ArkWeb/DualWebCore/entry/src/main/ets/pages/ExecuteAIPageDemo.ets) -->

```ts
import { webview } from '@kit.ArkWeb';
import { BusinessError } from '@kit.BasicServicesKit';

interface AIPageCommand {
  method: string;
}

@Entry
@Component
struct ExecuteAIPageDemo {
  controller: webview.WebviewController = new webview.WebviewController();

  build() {
    Column() {
      Button('executeAIPageCommand')
        .onClick(async () => {
          try {
            let commandObj: AIPageCommand = { method: 'getFullDom' };
            let command: string = JSON.stringify(commandObj);
            // 判断当前应用是否使用了M132遗留内核
            if (webview.WebviewController.getActiveWebEngineVersion() > webview.ArkWebEngineVersion.M132) {
                // 仅在M132版本之上的内核才可以调用executeAIPageCommand接口
                let result: string = await this.controller.executeAIPageCommand(command);
                console.info(`executeAIPageCommand result: ${result}`);
            }
          } catch (error) {
            console.error(`ErrorCode: ${(error as BusinessError).code},  Message: ${(error as BusinessError).message}`);
          }
        })
      Web({ src: 'about:blank', controller: this.controller })
    }
  }
}
```

## 4. 内核navigator标识信息变化说明

应用会使用W3C中navigator的[userAgent](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/userAgent)和[platform](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/platform)属性进行业务隔离，这些字段的值示例如下：

| **类型**      | **[platform](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/platform)** | **[userAgent](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/userAgent)**                                                       |
| - | - | - |
| M132 on OH6.0 | Linux x86_64 | Mozilla/5.0 (Phone; OpenHarmony 6.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/**132.0.0.0** Safari/537.36 ArkWeb/*6.0.0.44* Mobile  |
| M132 on OH6.1 | Linux x86_64 | Mozilla/5.0 (Phone; OpenHarmony 6.1; Android 10) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/**132.0.0.0** Safari/537.36 ArkWeb/*6.1.0.125* Mobile |
| M132 on OH7.0 | Linux x86_64 | Mozilla/5.0 (Phone; OpenHarmony 7.0; Android 10) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/**132.0.0.0** Safari/537.36  ArkWeb/*6.1.0.28* Mobile |
| M144 on OH7.0 | Linux x86_64 | Mozilla/5.0 (Phone; OpenHarmony 7.0; Android 10) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/**144.0.0.0** Safari/537.36 ArkWeb/*7.0.0.30* Mobile  |


> 注意：不推荐使用[platform](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/platform)属性，该属性已废弃。

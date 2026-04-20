# M132内核在OpenHarmony7.0系统上的适配指导

## 1. Web内核切换API使用说明

OpenHarmony 7.0系统ArkWebCore内核默认升级到了M144版本，同时系统提供了双内核方案，保留老的M132版本内核，以便生态应用自主选择M132升级到M144的节奏和策略，降低应用因Web内核升级而导致的兼容性问题。

两种Web内核类型说明：

| **内核类型** | **英文**   | **说明**  |
| ----------- | ---------- | -------- |
| 常青内核     | EVERGREEN WebCore | 当前系统的最新版Web内核，系统基于此类型的内核进行完整的功能实现，推荐应用使用。|
| 遗留内核     | LEGACY WebCore    | 复用上一个商用版本的内核，只做安全补丁及舆情问题修复。遗留内核仅作为兼容性回滚使用，新的OpenHarmony系统发布时，不一定必选支持；且遗留内核的支持有时间限制，一般在系统发布后半年后会完全禁用掉。 |

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
| M144 | 7.0版本的常青内核 | 7.0版本上的默认内核。如果后续OpenHarmony系统版本上不存在此内核则设置无效。 |
| M132 | 7.0版本的遗留内核 | 开发者可选择此遗留内核。如果后续OpenHarmony系统版本上不存在此内核则设置无效。 计划在2027-Q2禁用此内核。 |
| SYSTEM_DEFAULT | 系统默认 | 使用系统上默认内核，7.0版本上默认为M144 |
| ARKWEB_EVERGREEN | 常青内核，系统的最新内核 | 开发者可选择在每个系统版本上都使用最新的内核，6.1以及之后所有系统版本都生效。<br>**说明：** 从API version 23开始支持|

应用在Web组件加载之前，可以通过SDK 20的setActiveWebEngineVersion接口，指定ArkWebCore内核的版本。[示例代码](https://gitcode.com/openharmony/applications_app_samples/blob/master/code/DocsSample/ArkWeb/DualWebCore)：

```
// EntryAbility.ets

import { AbilityConstant, ConfigurationConstant, UIAbility, Want } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';
import webview from '@ohos.web.webview';
import { ArkWebEngineType } from '@ohos.web.webview';
import testNapi from 'libentry.so';

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {

    // 设置低版本web内核之前清理web缓存
    testNapi.deleteWebCache();

    // 设置web内核为M132
    webview.WebViewController.setActiveWebEngineVersion(ArkWebEngineVersion::M132);

    // 查询并打印内核版本
    hilog.info(DOMAIN, 'testTag', 'webVersion = %{public}d', webview.WebviewController.getActiveWebEngineVersion());
  }
}
```

也可以通过NDK接口来实现：

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

    return 0;
}
```

如果应用未适配SDK 20也可以通过NDK方式调用：

```
// EntryAbility.ets

import { AbilityConstant, ConfigurationConstant, UIAbility, Want } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';
import webview from '@ohos.web.webview';
import testNapi from 'libentry.so';

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {

    // 设置低版本web内核之前清理web缓存
    testNapi.deleteWebCache();

    // 设置132 web内核
    testNapi.setWebVersion(2);

    // 打印当前web内核信息
    hilog.info(DOMAIN, 'testTag', 'webVersion = %{public}d', testNapi.getWebVersion());
  }
}
```

```
// CMakeList.txt

add\_library(entry SHARED napi\_init.cpp)
```

```
// napi_init.cpp

static void deleteDirectoryRecursivelyImpl(const std::string& path) {
    try {
        // 检查路径是否存在
        if (!fs::exists(path)) {
            std::cerr << "Directory does not exist: " << path << std::endl;
            return;
        }
        // 递归删除目录及其内容
        fs::remove_all(path);
        std::cout << "Successfully deleted directory: " << path << std::endl;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}


static void deleteWebCacheImpl()
{
    deleteDirectoryRecursivelyImpl("/data/storage/el2/base/cache/web");
}

static napi_value deleteWebCache(napi_env env, napi_callback_info info)
{
    deleteWebCacheImpl();
    return 0;
}

static void setWebVersionImpl(int version) {
    void* handle = dlopen("libohweb.so", RTLD_LAZY);
    if (!handle) {
        // 处理错误：dlerror()
        return;
    }
  
    typedef void (*func_ptr)(int a);
    func_ptr func = (func_ptr)dlsym(handle, "OH_NativeArkWeb_SetActiveWebEngineVersion");
    if (!func) {
        // 处理符号未找到
        dlclose(handle);
        return;
    }
  
    func(version); // 调用目标函数
    dlclose(handle);
}


static napi_value setWebVersion(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = {nullptr};

    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);

    napi_valuetype valuetype0;
    napi_typeof(env, args[0], &valuetype0);

    int32_t value0;
    napi_get_value_int32(env, args[0], &value0);
  
    setWebVersionImpl(value0);
    return 0;
}

static int getWebVersionImpl()
{
    void* handle = dlopen("libohweb.so", RTLD_LAZY);
    if (!handle) {
        // 处理错误：dlerror()
        return 0;
    }
  
    typedef int (*func_ptr)(void);
    func_ptr func = (func_ptr)dlsym(handle, "OH_NativeArkWeb_GetActiveWebEngineVersion");
    if (!func) {
        // 处理符号未找到
        dlclose(handle);
        return 0;
    }
  
    int ret = func(); // 调用目标函数
    dlclose(handle);
    return ret;
}

static napi_value getWebVersion(napi_env env, napi_callback_info info)
{
    int version = getWebVersionImpl();
  
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
> * 本接口调用是否生效可以通过getActiveWebEngineVersion接口查询实际生效的内核版本

## 2. 使用遗留内核的风险说明

ArkWeb常青内核是新系统默认的配套内核，在功能、标准遵循度、安全性、性能方面都有全方位的增强。应用需首选使用常青内核，遗留内核仅作为兼容性的阶段性回滚内核。

OH7.0版本ArkWeb由M132内核升级到M144内核，详细变化及收益参考《[ArkWeb版本的差异总结](./ArkWeb_132_144.md)》。

应用使用遗留内核前，需要评估以下信息：

* 双内核兼容性：ArkWeb新增的API依赖常青内核，应用开发者需结合下列章节新增API在遗留内核上的行为进行兼容性保障。
* 数据一致性：应用在由常青内核降级回滚到遗留内核时，WEB相关的缓存数据可能不被遗留内核支持；在降级回滚时，必须先清理应用沙箱中/data/storage/el2/base/cache/web目录下的WEB缓存数据，确保回滚后可正常工作。

## 3. 使用遗留内核的代码隔离方式

## 4. M132遗留内核API兼容指南

以下是OpenHarmony 7.0新增依赖M144内核的ArkWeb API，如果应用需要在OpenHarmony 7.0上兼容M132遗留内核，可参考以下接口说明，做好代码适配。

### 4.1 内核navigator标识信息变化说明

应用会使用W3C中navigator的[userAgent](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/userAgent)和[platform](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/platform)属性进行业务隔离，这些字段的值如下所示：

| **类型**      | **[platform](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/platform)** | **[userAgent](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/userAgent)**                                                       |
| ------------- | ----------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------- |
| M114 on OH5.1 | Linux x86_64                                                                        | Mozilla/5.0 (Phone; OpenHarmony 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/**114.0.0.0** Safari/537.36 ArkWeb/*5.1.0.207* Mobile |
| M114 on OH6.0 | Linux x86_64                                                                        | Mozilla/5.0 (Phone; OpenHarmony 6.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/**114.0.0.0** Safari/537.36 ArkWeb/*6.0.0.44* Mobile  |
| M132 on OH6.0 | Linux x86_64                                                                        | Mozilla/5.0 (Phone; OpenHarmony 6.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/**132.0.0.0** Safari/537.36 ArkWeb/*6.0.0.44* Mobile  |

> 注意：不推荐使用[platform](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/platform)属性，该属性已废弃。

### 4.2 webview接口

### 4.3 web组件接口

### 4.4 NDK接口

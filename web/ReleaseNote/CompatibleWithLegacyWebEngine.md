# M114适配说明

## 1. Web内核切换API使用说明

OpenHarmony 6.0系统ArkWebCore内核默认升级到了M132版本，同时系统提供了双内核方案，保留老的M114版本内核，以便生态应用使用自主选择M114升级到M132的节奏和策略，降低应用因WEB内核升级的兼容性影响。

两种web内核类型说明：

| **内核类型** | 英文              | **说明**                                                                                                                                                                                        |
| ------------ | ----------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 常青内核     | EVERGREEN webCore | 当前系统的最新版web内核，系统基于此版本的内核进行完整的功能实现，推荐应用使用。                                                                                                                 |
| 遗留内核     | LEGACY webcore    | 复用上一个商用版本的内核，只做安全补丁及舆情问题修复。遗留内核仅作为兼容性回滚使用，新的OpenHarmony系统发布时，不一定必选支持；且遗留内核的支持有时间限制，一般在系统发布后半年后会完全禁用掉。 |

双内核相关API:

```
enum ArkWebEngineVersion {
    SYSTEM_DEFAULT = 0,
    M114 = 1,
    M132 = 2,
    SYSTEM_EVERGREEN = 99999
}
static setActiveWebEngineVersion(engineVersion: ArkWebEngineVersion): void;
static getActiveWebEngineVersion(): ArkWebEngineVersion;
static isActiveWebEngineEvergreen(): boolean;
```

ArkWebEngineVersion枚举值定义：

|    **枚举值**    | **内核类型**             | **说明**                                                                                                               |
| :--------------: | ------------------------ | ---------------------------------------------------------------------------------------------------------------------- |
|       M132       | 6.0版本的常青内核        | 6.0版本上的默认内核。如果后续oh系统版本上不存在此内核则设置无效。                                                      |
|       M114       | 6.0版本的遗留内核        | 开发者可选择此遗留内核。如果后续oh系统版本上不存在此内核则设置无效。                                                   |
| SYSTEM_EVERGREEN | 常青内核，系统的最新内核 | 开发者可选择在每个系统版本上都使用最新的内核，6.0以及之后所有系统版本都生效，比如7.0系统上常青内核可能是最新的其他内核 |
|  SYSTEM_DEFAULT  | 系统默认                 | 使用系统上默认内核，6.0版本上默认为M132                                                                                |

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

    // 设置web内核为M114
    webview.WebViewController.setActiveWebEngineVersion(ArkWebEngineVersion::M114);

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

    // 设置114 web内核
    testNapi.setWebVersion(1);

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
> * **指定版本的内核已经失效**：系统刚发布时如果支持M114遗留内核，半年后应用灰度升级到M132后，系统侧也会禁用掉M114遗留内核，此时此接口调用也不会生效。
> * 本接口调用是否生效可以通过getActiveWebEngineVersion接口查询实际生效的内核版本

## 2. 使用遗留内核的风险说明

ArkWeb常青内核是新系统默认的配套内核，在功能、标准遵循度、安全性、性能方面都有全方位的增强。应用需首选使用常青内核，遗留内核仅作为兼容性的阶段性回滚内核。

OH6.0版本ArkWeb由M114内核升级到M132内核，详细变化及收益参考《[ArkWeb版本的差异总结](./ArkWeb_114_132.md)》。

应用使用遗留内核前，需要评估以下信息：

* 双内核兼容性：ArkWeb新增的API依赖常青内核，应用开发者需结合下列章节新增API在遗留内核上的行为进行兼容性保障。
* 数据一致性：应用在由常青内核降级回滚到遗留内核时，WEB相关的缓存数据可能不被遗留内核支持；在降级回滚时，必须先清理应用沙箱中/data/storage/el2/base/cache/web目录下的WEB缓存数据，确保回滚后可正常工作。

## 3. 使用遗留内核的代码隔离方式

OpenHarmony每个版本都有新特性和新增API基于常青内核开发，应用如果切换使用遗留内核，使用这些新增API会失效或报错。

* 报错接口的兼容方式：
  如下所示，getProress接口仅在M132及之后的内核版本提供，应用如果兼容支持遗留M114内核，在调用前需进行内核版本的判断隔离：

```
/* 判断当前应用是否使用了M114遗留内核 */
if (getActiveWebEngineVersion() > M114) {
  /* 仅在M114版本之上的内核才调用getProgress接口 */ 
  getProgress();
}
```

* 失效接口的兼容方式：
  很多设置类接口在遗留内核上不生效，也不返回错误码。应用可选择忽略该调用，如果该功能影响其它业务代码，可参考上述代码进行隔离。

## 4. M114遗留内核API兼容指南

以下是OpenHarmony 6.0新增依赖M132内核的ArkWeb API，如果应用需要在OpenHarmony 6.0上兼容M114遗留内核，可参考以下接口说明，做好代码适配。

### 4.1 内核navigator标识信息变化说明

应用会使用W3C中navigator的[userAgent](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/userAgent)和[platform](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/platform)屬性进行业务隔离，这些字段的值如下所示：

| **类型**      | **[platform](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/platform)** | **[userAgent](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/userAgent)**                                                       |
| ------------- | ----------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------- |
| M114 on OH5.1 | Linux x86_64                                                                        | Mozilla/5.0 (Phone; OpenHarmony**5.1**) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/**114.0.0.0** Safari/537.36 ArkWeb/*5.1.0.207* Mobile |
| M114 on OH6.0 | Linux x86_64                                                                        | Mozilla/5.0 (Phone; OpenHarmony**6.0**) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/**114.0.0.0** Safari/537.36 ArkWeb/*6.0.0.44* Mobile  |
| M132 on OH6.0 | Linux x86_64                                                                        | Mozilla/5.0 (Phone; OpenHarmony**6.0**) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/**132.0.0.0** Safari/537.36 ArkWeb/*6.0.0.44* Mobile  |

> 注意：不推荐使用[platform](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/platform)属性，该属性已废弃。

### 4.2 webview接口

#### ErrorPage处理接口

```
/**
      * Set whether enable the error page. onOverrideErrorPage will be triggered when the page error.
      *
      * @param { boolean } enable - Whether to enable the default error page feature.
      * @throws { BusinessError } 17100001 - Init error.
      *                           The WebviewController must be associated with a Web component.
      * @syscap SystemCapability.Web.Webview.Core
      * @since 20
      */
    setErrorPageEnabled(enable: boolean): void;

    /**
      * Get whether default error page feature is enabled.
      *
      * @returns  { boolean } -  True if enable the default error page feature; else false.
      * @throws { BusinessError } 17100001 - Init error.
      *                           The WebviewController must be associated with a Web component.
      * @syscap SystemCapability.Web.Webview.Core
      * @since 20
      */
    getErrorPageEnabled(): boolean;
```

* **接口作用说明**
  设置和查询默认错误页配置。
* **接口在M114遗留内核上的行为**
  [setErrorPageEnabled](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#seterrorpageenabled20)在M114遗留内核上设置不生效，不会抛异常，也不会返回错误码；应用调用后，通过[getErrorPageEnabled](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#getErrorPageEnabled20)接口获取值始终为false。
  不建议开发者在M114内核中使用以上接口。

#### PrivateNetworkAccess接口

```
/**
 * After enable PrivateNetworkAccess feature, ArkWeb will send a CORS preflight request before issuing any
 * sub-resource private network requests to request explicit permission from the target server.
 * After disable PrivateNetworkAccess, ArkWeb will no longer check whether the private network request
 * is legitimate.
 * 
 * @param {boolean} enable - {@code true} enable the private network access check; {@code false} otherwise.
 * @static
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
static enablePrivateNetworkAccess(enable: boolean): void;

/**
 * Get whether PrivateNetworkAccess is enabled.
 * 
 * @returns {boolean} True is enable the ability to check private network access else false.
 * @static
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
static isPrivateNetworkAccessEnabled(): boolean;
```

* **接口作用说明**
  设置和查询Web组件设置私有网络访问检查功能（Private Network Access）的启用状态。
* **接口在M114遗留内核上的行为**
  [enablePrivateNetworkAccess]([GitCode - 全球开发者的开源社区,开源代码托管平台](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#enablePrivateNetworkAccess20))设置不生效，不会抛异常，也不会返回错误码；[isPrivateNetworkAccessEnabled]([GitCode - 全球开发者的开源社区,开源代码托管平台](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#isPrivateNetworkAccessEnabled20))接口获取值始终为false。
  不建议开发者在M114内核中使用以上接口。

#### UserAgent新增接口

```
/**
 * Set the default User-Agent for the application.
 *
 * <p><strong>API Note</strong>:<br>
 * Unlike setCustomUserAgent, which only takes effect in the current web context, the
 * priority for pages loaded in the web is as follows:
 * 1. The User-Agent set by setCustomUserAgent is used first.
 * 2. If not set, it will check whether a specific User-Agent has been
 *    assigned to the current page via setUserAgentForHosts.
 * 3. If no specific User-Agent is assigned, the application will fall back
 *    to using the User-Agent set by setAppCustomUserAgent.
 * 4. If the app's default User-Agent is also not specified, the web's default
 *    User-Agent will be used as the final fallback.
 * </p>
 *
 * @param { string } userAgent - The User-Agent string.
 * @static
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
static setAppCustomUserAgent(userAgent: string) : void;


/**
 * Set the User-Agent to be used for specified hosts, with a maximum of 20,000 hosts.
 * <p><strong>API Note</strong>:<br>
 * Setting the same host list multiple times for the same User-Agent will override
 * the previous settings. That is, if you want to cancel certain hosts from using
 * the specified User-Agent, you need to reset the host list for that User-Agent.
 * </p>
 *
 * @param { string } userAgent - The User-Agent string.
 * @param { Array<string> } hosts - The hosts to which the User-Agent apply.
 * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified.
 * <br>2. Incorrect parameter types. 3. Parameter verification failed.
 * @static
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
static setUserAgentForHosts(userAgent: string, hosts : Array<string>) : void;
```

* **接口作用说明**
  设置应用或应用的特定网站设置自定义用户代理，会覆盖系统的用户代理，应用内所有Web组件生效。
* **接口在M114遗留内核上的行为**
  [setAppCustomUserAgent](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#setAppCustomUserAgent20)在M114内核中使用该接口不生效，不会抛异常，也不会返回错误码。
  [setUserAgentForHosts ]([项目文件预览 - docs:OpenHarmony documentation | OpenHarmony开发者文档 - GitCode](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#setuseragentforhosts20))设置不生效，不会抛异常，也不会返回错误码。
  不建议开发者在M114内核中使用以上接口。

#### getProgress获取网页加载进度接口

```
/**
 * Gets the loading progress for the current page.
 *
 * @returns { number } The loading progress for the current page.
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
getProgress() : number;
```

* **接口作用说明**
  获取当前网页加载进度。
* **接口在M114遗留内核上的行为**
  [getProgress ](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#getProgress20)在M114内核中调用该接口抛801异常。

#### setWebDebuggingAccess接口

```
/**
 * Enables debugging of web contents.
 * <p><strong>API Note</strong>:<br>
 * The port numbers from 0 to 1024 are prohibited. Ports less than 0 or greater than 65535 are considered invalid.
 * If an attempt is made to set these disabled or invalid ports, an exception will be thrown.
 * </p>
 *
 * @param { boolean } webDebuggingAccess {@code true} enables debugging of web contents; {@code false} otherwise.
 * @param { number } port Indicates the port of the devtools server. After the port is specified, a tcp server
 * socket is created instead of a unix domain socket.
 * @throws { BusinessError } 17100023 - The port number is not within the allowed range.
 * @static
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
static setWebDebuggingAccess(webDebuggingAccess: boolean, port: number): void;
```

* **接口作用说明**
  设置是否启用无线网页调试功能，默认不开启。
* **接口在M114遗留内核上的行为**
  若开发者在M114内核中使用该[接口]([项目文件预览 - docs:OpenHarmony documentation | OpenHarmony开发者文档 - GitCode](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#setwebdebuggingaccess20))，仅会启用网页调试功能，而端口设置无效，接口效果与 `static setWebDebuggingAccess(webDebuggingAccess: boolean): void`一致。
  不建议开发者在M114内核中使用以上接口，建议开发者通过 `static setWebDebuggingAccess(webDebuggingAccess: boolean): void`[接口]([项目文件预览 - docs:OpenHarmony documentation | OpenHarmony开发者文档 - GitCode](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#setwebdebuggingaccess))替代。

#### setWebDestroyMode接口

```
/**
 * Set web destroy mode.
 * @param { WebDestroyMode } mode web destroy mode, default NORMAL_MODE.
 * @static
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
static setWebDestroyMode(mode: WebDestroyMode): void;

/**
 * Enum type supplied to {@link SetWebDestroyMode} for indicating the web component destroy mode.
 * @enum { number }
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
enum WebDestroyMode {  
  /**
   * The normal destroy mode, when the web component triggers destroy,
   * the resources will be released at the appropriate time.
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  NORMAL_MODE = 0,

  /**
   * The fast destroy mode, when the web component triggers destroy, the resources will be immediately released.
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  FAST_MODE = 1
}
```

* **接口作用说明**
  通过setWebDestroyMode设置Web的析构模式，当WebDestroyMode设置成FAST_MODE为立即析构，当Web组件触发销毁时，会立即释放相关资源；设置成NORMAL_MODE为正常模式，当Web组件触发销毁时会延迟析构，默认值为NORMAL_MODE。
* **接口在M114遗留内核上的行为**
  setWebDestroyMode在M114遗留内核上设置不生效，不会抛异常，也不会返回错误码。  Web组件将维持延迟析构的模式运行。
  不建议开发者在M114内核中使用以上接口。

#### onActivateContent接口

```
/**
 * Triggered when the web page is activated for window.open called by other web component.
 *
 * @param { Callback<void> } callback the triggered function when the web page is activated for window.open called by other web component.
 * @returns { WebAttribute }
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
onActivateContent(callback: Callback<void>): WebAttribute;
```

* **接口作用说明**
  当Web页面触发window.open(url, name)时，会根据name查找是否存在已绑定的Web实例。若存在，则触发onActivateContent回调并通知应用需将其展示至前端；若不存在，则通过onWindowNew通知应用创建新Web实例。
* **接口在M114遗留内核上的行为**
  由于在M114遗留内核上并未注册onActivateContent回调，所以会通过onWindowNew创建新Web实例，不会抛异常，也不会返回错误码。

#### getPageOffset 接口

```
/**
 * Get the page offset of the webpage in view port, the coordinates of the top left corner of the view port are X: 0, Y: 0.  
 * And the unit is virtual pixel.
 *
 * @returns { ScrollOffset } page offset
 * @throws { BusinessError } 801 - Capability not supported.
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
getPageOffset(): ScrollOffset;
```

* **接口作用说明**
  获取页面滑动偏移量，与getScrollOffset相比，不包含过滚动的偏移。
* **接口在M114遗留内核上的行为**
  在M114内核中使用 `getPageOffset`接口，将返回错误码801。
  不建议开发者在M114内核中使用该接口。

#### avoidVisibleViewportBottom 接口

```
/**
     * Sets the bottom avoidance height of the web visible viewport.
     * When setting non-zero height, the position and size of the web component remain unchanged,
     * <br>and the visible viewport upward avoids avoidHeight, as manifested by the web page content raising avoidHeight.
     * <br>This interface is generally used for customizing the bottom avoidance area, and it is not recommended for
     * <br>simultaneous use with clicking the editable area of the web page showing the keyboard.
     * <br>In this case, the keyboardAvoidMode will be OVERLAYS_CONTENT.
     * When setting zero, web page content can be restored and the keyboardAvoidMode will be the value set by keyboardAvoidMode().
     *
     * @param { number } avoidHeight - the height value of the visible viewport avoidance. Unit: vp.
     * <br>The valid interval of avoidHeight is [0, the height of web component].
     * <br>When avoidHeight is out of the valid interval, it takes the boundary value of the interval.
     * @throws { BusinessError } 17100001 - Init error.
     *                           The WebviewController must be associated with a Web component.
     * @syscap SystemCapability.Web.Webview.Core
     * @since 20
     */
    avoidVisibleViewportBottom(avoidHeight: number): void;
```

* **接口作用说明**
  设置网页底部避让高度
* **接口在M114遗留内核上的行为**
  `avoidVisibleViewportBottom`在M114遗留内核上设置不生效，不会抛异常。
  不建议开发者在M114内核中使用以上接口。

#### getBlanklessInfoWithKey接口

```
/**
  * Defines the blankless information.
  *
  * @typedef BlanklessInfo
  * @syscap SystemCapability.Web.Webview.Core
  * @since 20
  */
 interface BlanklessInfo {
   /**
    * Defines the error codes of blankless.
    * @type { WebBlanklessErrorCode }
    * @syscap SystemCapability.Web.Webview.Core
    * @since 20
    */
   errCode: WebBlanklessErrorCode;

   /**
    * Obtains the similarity of snapshots generated by the blankless loading solution based on the last
    * several snapshots. The value ranges from 0 to 1.0. The value 1.0 indicates that the snapshots are the same. A
    * value closer to 1 indicates higher similarity. This value has a certain lag, meaning that the similarity of
    * the current loading will only be reflected in the next loading. It is recommended that the frame interpolation
    * functionality be disabled when the similarity is 0.
    * @type { number }
    * @syscap SystemCapability.Web.Webview.Core
    * @since 20
    */
   similarity: number;

   /**
    * Obtains the loading time based on the historical loading time. The unit is ms and the value is greater
    * than 0.
    *
    * @type { number }
    * @syscap SystemCapability.Web.Webview.Core
    * @since 20
    */
   loadingTime: number;
 }

/**
 * Obtains the prediction information about the blankless loading solution and enables the generation of
 * the transition frame for the current loading. The application determines whether to enable the blankless
 * loading solution based on the information.
 *
 * @param { string } key  The key value that uniquely identifies the page.
 * Default value: N/A.
 * The value cannot be empty or exceed 2048 characters.
 * When an invalid value is set, this API does not take effect.
 * @returns { BlanklessInfo } The prediction information about the blankless loading solution.
 * @throws { BusinessError } 801 This functionality is not supported.
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
getBlanklessInfoWithKey(key: string) : BlanklessInfo;
```

* **接口作用说明**
  获取页面首屏加载预测信息（具体见****WebBlanklessInfo ****），并使能本次加载过渡帧生成，应用根据此信息确定是否需要使能Blankless加载
* **接口在M114遗留内核上的行为**
  接口功能在M114内核不支持，返回错误码 `801`

#### setBlanklessLoadingWithKey接口

```
/**
     * Enumerates the error codes of blankless. For details, see {@link setBlanklessLoadingWithKey} or {@link
     * BlanklessInfo}.
     *
     * @enum { number }
     * @syscap SystemCapability.Web.Webview.Core
     * @since 20
     */
    enum WebBlanklessErrorCode {
      /**
       * The operation is successful.
       *
       * @syscap SystemCapability.Web.Webview.Core
       * @since 20
       */
      SUCCESS = 0,

      /**
       * Unknown error.
       *
       * @syscap SystemCapability.Web.Webview.Core
       * @since 20
       */
      ERR_UNKNOWN = -1,

      /**
       * Invalid parameter.
       *
       * @syscap SystemCapability.Web.Webview.Core
       * @since 20
       */
      ERR_INVALID_PARAM = -2,

      /**
       * The web controller is not bound to any component.
       *
       * @syscap SystemCapability.Web.Webview.Core
       * @since 20
       */
      ERR_CONTROLLER_NOT_INITED = -3,

      /**
       * The key value is not matched. This error code is returned when the key values of
       * setBlanklessLodingWithKey and getBlanklessInfoWithKey are not matched.
       *
       * @syscap SystemCapability.Web.Webview.Core
       * @since 20
       */
      ERR_KEY_NOT_MATCH = -4,

      /**
       * The system determines that the change is too large when the similarity is less than 0.33. As a result,
       * the setBlanklessLodingWithKey API fails to enable frame interpolation.
       *
       * @syscap SystemCapability.Web.Webview.Core
       * @since 20
       */
      ERR_SIGNIFICANT_CHANGE = -5
  }

/**
    * Sets whether to enable blankless page loading. This API must be used in pair with the
    * getBlanklessInfoWithKey API.
    *
    * @param { string } key  The key value that uniquely identifies the current page. It must be the same as
    * the key value of the getBlanklessInfoWithKey API.
    * Default value: N/A.
    * The value cannot be empty or exceed 2048 characters.
    * When an invalid value is set, the error code WebBlanklessErrorCode is returned, and the API does not
    * take effect.
    * @param { boolean } is_start  Whether to enable frame interpolation. The value true indicates to enable
    * frame interpolation, and the value false indicates the opposite.
    * The default value is false.
    * The value can be true or false.
    * Action for setting an invalid value: N/A.
    * @returns { WebBlanklessErrorCode } WebBlanklessErrorCode.
    * @throws { BusinessError } 801 This functionality is not supported.
    * @syscap SystemCapability.Web.Webview.Core
    * @since 20
    */
setBlanklessLoadingWithKey(key: string, is_start: boolean) : WebBlanklessErrorCode;
```

* **接口作用说明**
  设置Blankless加载是否使能
* **接口在M114遗留内核上的行为**
  接口功能在M114内核不支持，返回错误码 `801`

#### clearBlanklessLoadingCache接口

```
/**
 * Clears the blankless loading cache of the page with a specified key value.
 *
 * @param { Array<string> } [keys]  The list of key values of pages cached in the blankless loading
 * solution. These key values are specified in getBlanklessInfoKey.
 * The default value is the list of key values of all pages cached in the blankless loading solution.
 * The key length cannot exceed 2048 characters, and the number of keys must be less than or equal to 100.
 * The URL is the same as that input to the Web component during page loading.
 * When the key length exceeds 2048 characters, the key does not take effect. When the number of keys
 * exceeds 100, the first 100 keys are used. If these parameters are left empty, the default values are used.
 * @throws { BusinessError } 801 This functionality is not supported.
 * @static
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
static clearBlanklessLoadingCache(keys?: Array<string>) : void;
```

* **接口作用说明**
  清除指定key值页面Blankless优化缓存，本接口只清除缓存。
* **接口在M114遗留内核上的行为**
  接口功能在M114内核不支持，返回错误码 `801`

#### setBlanklessLoadingCacheCapacity接口

```
/**
 * Sets the cache capacity of the blankless loading solution and returns the value that takes effect. If
 * this API is not called, the default capacity 30 MB is used. The maximum capacity cannot exceed 100 MB.
 *
 * @param { number } capacity  Cache capacity, in MB. The maximum value is 100 MB.
 * The default value is 30 MB.
 * The value ranges from 0 to 100. If this parameter is set to 0, no cache capacity is available and the
 * functionality is disabled globally.
 * When the value is set to a number smaller than 0, the value 0 takes effect. When the value is set to a
 * number greater than 100, the value 100 takes effect.
 * @returns { number } The effective value that ranges from 0 MB to 100 MB.
 * @throws { BusinessError } 801 This functionality is not supported.
 * @static
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
static setBlanklessLoadingCacheCapacity(capacity: number) : number;
```

* **接口作用说明**
  设置Blankless加载方案持久缓存容量，返回实际生效的值
* **接口在M114遗留内核上的行为**
  接口功能在M114内核不支持，返回错误码 `801`

### 4.3 web组件接口

#### mediaOptions 接口

```
/**
 * Arkweb audio session Type
 *
 * @enum { number }
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
declare enum AudioSessionType {
  /**
   * Ambient audio, which is mixable with other types of audio.
   * This is useful in some special cases such as when the user wants to mix audios from multiple pages.
   *
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  AMBIENT=3
}

declare interface WebMediaOptions {
 resumeInterval?: number;

 audioExclusive?: boolean;
/**
 * The type for audio sessions.
 *
 * @type { ?AudioSessionType }
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
 audioSessionType?: AudioSessionType;
}

...
mediaOptions(options: WebMediaOptions): WebAttribute;
```

* **接口作用说明**
  应用中Web音频类型。默认值对应系统音频流类型 STREAM\_USAGE\_MUSIC。设置该参数会改变组件音频类型与系统音频类型映射关系，进而影响Web音频焦点策略。
* **接口在M114遗留内核上的行为**
  audioSessionType 参数在M114遗留内核上设置不生效，不会抛异常，也不会返回错误码。在M114上，mediaOptions接口参数 WebMediaOptions 仅 resumeInterval 和 audioExclusive 参数生效；
  不建议开发者在M114内核中使用audioSessionType 参数。

#### 页面加载回调新增接口

```
declare interface OnLoadStartedEvent {
   /**
    * The url to be loaded.
    *
    * @type { string }
    * @syscap SystemCapability.Web.Webview.Core
    * @since 20
    */
   url: string;
}

/**
 * Triggered at the begin of web page loading. This method is called once for each main frame load.  Embedded frame
 * changes, i.e. clicking a link whose target is an iframe and fragment navigations (navigations to #fragment_id)
 * will not trigger this callback.
 *
 * <p><strong>API Note</strong>:<br>
 * Different from onPageBegin, onLoadStarted is triggered only once if the mainframe is automatically redirected
 * before the page is completely loaded. OnPageBegin is triggered every navigation.
 * </p>
 *
 * @param { Callback<OnLoadStartedEvent> } callback The triggered function at the begin of web page loading.
 * @returns { WebAttribute }
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
onLoadStarted(callback: Callback<OnLoadStartedEvent>): WebAttribute;

declare interface OnLoadFinishedEvent {
  /**
   * The url to be loaded.
   *
   * @type { string }
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  url: string;
}
/**
 * Notify the host application that a page has finished loading. This method is called only for main frame.
 *
 * <p><strong>API Note</strong>:<br>
 * Different from onPageEnd, fragment navigation also triggers onLoadFinished.
 * </p>
 *
 * @param { Callback<OnLoadFinishedEvent> } callback The triggered function at the end of web page loading.
 * @returns { WebAttribute }
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
onLoadFinished(callback: Callback<OnLoadFinishedEvent>): WebAttribute;

/**
 * The callback of onOverrideErrorPage.
 *
 * @typedef { function } OnOverrideErrorpageCallback
 * @param { WebResourceRequest } webResourceRequest - Information about the failed request.
 * @param { error } WebResourceError - The information of error.
 * @returns { string } - Return an HTML text content encoded in Base64.
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
type OnOverrideErrorPageCallback= (webResourceRequest: WebResourceRequest, error: WebResourceError) => string;

/**
 * Triggered when the web page's document resource error.
 * <p><strong>API Note</strong>:<br>
 * This only triggered for main frame.
 * </p>
 *
 * @param { OnOverrideErrorPageCallback } callback The triggered function when the
 * web page's document resource error.
 * @returns { WebAttribute }
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
onOverrideErrorPage(callback: OnOverrideErrorPageCallback): WebAttribute;

declare interface OnTitleReceiveEvent {
  title: string;

  /**
   * Mark the source of the title. If it is true, the title is derived from the H5 title element;
   * If it is false, it is calculated from the URL. By default, it is calculated from the URL.
   * 
   * @type { ?boolean }
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  isRealTitle?: boolean;
}
onTitleReceive(callback: Callback<OnTitleReceiveEvent>): WebAttribute;

declare interface OnBeforeUnloadEvent {
  url: string;
  message: string;
  result: JsResult;

  /**
   * The isReload parameter is set to true when the page is refreshed;
   * otherwise, it remains false. Default is false.
   *
   * @type { ?boolean }
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  isReload?: boolean;
}
onBeforeUnload(callback: Callback<OnBeforeUnloadEvent, boolean>): WebAttribute;
```

* **接口作用说明**
  OH6.0上新增了通知宿主应用页面开始加载、加载完成、页面标题以及自定义错误页回调接口。
* **接口在M114遗留内核上的行为**
  在M114内核上，下述三个新增回调函数将都不会生效，为组件定义这些回调时，都可以成功调用，但设置后系统不会触发此回调。
  
  * [onLoadStarted](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-events.md#onloadstarted20)
  * [onLoadFinished](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-events.md#onloadfinished20)
  * [onOverrideErrorPage](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-events.md#onoverrideerrorpage20)
  
  应用业务如果依赖此类回调的执行，需要做好兼容适配，确保不回调时，业务也可以降级完成。
  
  下述两个接口在M114行也会毁掉，只是……
  [onTitleReceive](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-events.md#ontitlereceive)接口在回调时，isRealTitle始终为false。
  [onBeforeUnload](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-events.md#onBeforeUnload)接口在回调时，isReload始终为false。

#### SslErrorHandler相关接口

```
declare class SslErrorHandler {
...
/**
 * ArkWeb has encountered an SSL certificate error, and this interface indicates whether to terminate or
 * continue displaying the error to users.
 *
 * @param { boolean } abortLoading If abortLoading is true, the current request will be canceled and the
 *                                 user will remain on the current page. If it is false, the SSL error
 *                                 will not be ignored, and a blank page will be displayed. If a default
 *                                 error page is enabled, the default error page will be shown instead.
 *                                 The default value is false.
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
handleCancel(abortLoading: boolean): void;
}

declare interface SslErrorEvent {
  handler: SslErrorHandler;
  ...
  /**
   * Certificate chain data in DER format.
   *
   * @type { ?Array<Uint8Array> }
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  certChainData?: Array<Uint8Array>;
}
type OnSslErrorEventCallback = (sslErrorEvent: SslErrorEvent) => void;

onSslErrorEvent(callback: OnSslErrorEventCallback): WebAttribute;

declare interface OnSslErrorEventReceiveEvent {
  /**
   * Notifies the user of the operation behavior of the web component.
   *
   * @type { SslErrorHandler }
   * @syscap SystemCapability.Web.Webview.Core
   * @atomicservice
   * @since 12
   */
  handler: SslErrorHandler;
  ...
}
onSslErrorEventReceive(callback: Callback<OnSslErrorEventReceiveEvent>): WebAttribute;
```

* **接口作用说明**
  网页加载过程中，发生SSL错误回调时，OH6.0新增了以下两个接口：
  * certChainData：SSL证书链数据
  * handleCancel：可通知Web组件取消此请求，并根据参数abortLoading决定是否停止加载。
* **接口在M114遗留内核上的行为**
  在M114内核上：
  * SslErrorEvent的certChainData为null。
  * [handleCancel](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-SslErrorHandler.md#handlecancel20)的abortLoading配置不生效。

#### bypassVsyncCondition 接口

```
/**
 * Enum type supplied to {@link bypassVsyncCondition} for setting the bypass vsync condition.
 *
 * @enum { number }
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
declare enum WebBypassVsyncCondition {
  /**
   * Not bypass vsync.
   *
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  NONE = 0,

  /**
   * bypass vsync.
   *
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  SCROLLBY_FROM_ZERO_OFFSET = 1
}

/**
 * Set up a condition that bypass vsync
 * If the condition is matched, the drawing schedule does not reply on Vsync scheduling 
 * and directly rendering and drawing
 *
 * @param { WebBypassVsyncCondition } condition - The condition to bypass render vsync.
 * @returns { WebAttribute }
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
bypassVsyncCondition(condition: WebBypassVsyncCondition): WebAttribute;
```

* **接口作用说明**
  [WebBypassVsyncCondition](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-e.md#webbypassvsynccondition20)枚举只提供给bypassVsyncCondition接口传参使用，用以设置是否跳过渲染vsync的条件。| 名称                      | 值 | 说明                                                                                        |
  | ------------------------- | -- | ------------------------------------------------------------------------------------------- |
  | NONE                      | 0  | 默认值，按vsync调度流程绘制。                                                               |
  | SCROLLBY_FROM_ZERO_OFFSET | 1  | 在使用scrollby（只支持带滚动偏移量）且Web页面滚动偏移量为0，渲染流程跳过vsync调度直接绘制。 |

当开发者调用scrollBy接口进行页面滚动时，可以通过bypassVsyncCondition接口设置渲染流程跳过vsync（垂直同步）调度，直接触发绘制。

* **接口在M114遗留内核上的行为**
  [ bypassVsyncCondition](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-attributes.md#bypassvsynccondition20)在M114遗留内核上设置不生效，不会抛异常，也不会返回错误码。
  不建议开发者在M114内核中使用以上接口。

#### onContextMenuShow相关新增接口

```
declare class WebContextMenuResult {
  ...
  /**
   * Executes the redo operation related to this context menu.
   *
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  redo(): void;

  /**
   * Executes the undo operation related to this context menu.
   *
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  undo(): void;

  /**
   * Executes the paste and match style operation related to this context menu.
   *
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  pasteAndMatchStyle(): void;
}

declare enum ContextMenuMediaType {
  None = 0,
  Image = 1,

  /**
   * Video.
   *
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  VIDEO = 2,

  /**
   * Audio.
   *
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  AUDIO = 3
}

declare class WebContextMenuParam {
  ...
  getMediaType(): ContextMenuMediaType;
}

declare interface OnContextMenuShowEvent {
  param: WebContextMenuParam;
  result: WebContextMenuResult;
}

onContextMenuShow(callback: Callback<OnContextMenuShowEvent, boolean>): WebAttribute;
```

* **接口作用说明**
  WebContextMenuParam用于获取定制上下文菜单的相关参数，新增了以下媒体类型的识别：
  
  * VIDEO：上下文菜单识别为视频内容
  * AUDIO：上下文菜单识别为音频内容
  
  WebContextMenuResult用于响应在编辑区上下文菜单操作，新增了以下接口：
  
  * undo：在编辑区调用该接口会重做用户上一步的修改
  * redo：在编辑区调用该接口会重做用户上一步的修改
  * pasteAndMatchStyle：在编辑区调用该接口会将剪贴板中的数据粘贴为纯文本
* **接口在M114遗留内核上的行为**
  在M114上，上下文菜单无法识别VIDEO和AUDIO类型， getMediaType始终不会返回上述两种类型。
  WebContextMenuResult新增的接口调用后不生效，且系统不报错、不抛异常、无错误码。

#### bindSelectionMenu新增LINK枚举支持

```
declare enum WebElementType {
  IMAGE = 1,

  /**
   * Link,corresponding link type.
   *
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
  */
  LINK = 2
}

bindSelectionMenu(elementType: WebElementType, content: CustomBuilder, responseType: WebResponseType,
    options?: SelectionMenuOptionsExt): WebAttribute;
```

* **接口作用说明**
  如果设置bindSelectionMenu下的WebElementType为LINK时，开发者可自定义超链接菜单功能；长按元素类型为超链接时，会展示开发者自定义的菜单。
* **接口在M114遗留内核上的行为**
  开发者无法自定义超链接菜单，若开发者配置WebElementType为LINK，自定义超链接菜单功能不生效，且系统不报错、不抛异常、无错误码，不执行自定义超链接菜单展示。

#### DataDetector相关接口

```
/**
 * Enable data detector.
 *
 * @param { boolean } enable - {@code true} means enable data detector in Web;{@code false} otherwise.
 *    The default value is false.
 * @returns { WebAttribute } The attribute of the web.
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
enableDataDetector(enable: boolean): WebAttribute;

/**
 * Data detector with config.
 *
 * @param { TextDataDetectorConfig } config - The config of text data detector.
 * @returns { WebAttribute } The attribute of the web.
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
dataDetectorConfig(config: TextDataDetectorConfig): WebAttribute;
```

* **接口作用说明**
  DataDetector用于AI提供智能分词能力，支持5种类型（电话号码、链接、邮箱、地址、时间）的智能分词识别。dataDetectorConfig和enableDataDetector接口可以配置文本识别的类型、颜色、样式，以及是否识别长按显示AI菜单能力，并提供点击、长按、选中出AI菜单的能力。
* **接口在M114遗留内核上的行为**
  调用此接口无效，且系统不报错、不抛异常、无错误码，不执行，系统保持默认无AI智能分词能力。
  不建议开发者在M114内核中使用以上接口。

#### gestureFocusMode接口

```
/**
 * Set the gesture focus acquisition mode.
 * When users interact with the web using different gestures,
 * this determines whether and when focus is acquired based on the configured mode.
 * Default value: DEFAULT, where all gestures acquire focus on touch down.
 *
 * @param { GestureFocusMode } mode - The gesture focus mode, which can be {@link GestureFocusMode}.
 *    The default value is FocusMode.DEFAULT.
 * @returns { WebAttribute }
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
gestureFocusMode(mode: GestureFocusMode): WebAttribute;

/**
 * Enum type supplied to {@link gestureFocusMode} for setting the web gesture focus mode.
 *
 * @enum { number }
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 * @arkts 1.1&1.2
 */
 declare enum GestureFocusMode {
   /**
    * Any action on a web component, such as tapping, long-pressing, scrolling, zooming, etc.,
    * will cause the web component to acquire focus on touch down.
    *
    * @syscap SystemCapability.Web.Webview.Core
    * @since 20
    * @arkts 1.1&1.2
    */
   DEFAULT = 0,

   /**
    * Tap and long-press gestures will cause the web component to acquire focus after touch up,
    * while gestures such as scrolling, zooming, etc., do not request focus.
    *
    * @syscap SystemCapability.Web.Webview.Core
    * @since 20
    * @arkts 1.1&1.2
    */
   GESTURE_TAP_AND_LONG_PRESS = 1
 }
```

* **接口作用说明**
  支持配置手势获焦模式。DEFAULT模式下，任何手势操作都可在Touch Down时使web获焦；GESTURE_TAP_AND_LONG_PRESS模式下，仅长按和点击手势可使web获焦；默认为DEFAULT模式。
* **接口在M114遗留内核上的行为**
  开发者设置gestureFocusMode属性接口配置不生效，且系统不报错、不抛异常、无错误码，Web组件默认为DEFAULT获焦模式。
  不建议开发者在M114内核中使用以上接口。

#### Pdf加载滚动回调接口

```
/**
 * Triggered when the PDF in web page scrolling at bottom with pdf scroll event.
 * @param { Callback<OnPdfScrollEvent> } callback Function Triggered when the scrolling to bottom.
 * @returns { WebAttribute }
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 * @arkts 1.1&1.2
 */
onPdfScrollAtBottom(callback: Callback<OnPdfScrollEvent>): WebAttribute;

/**
 * Triggered when the PDF page load finish.
 * @param { Callback<OnPdfLoadEvent> } callback
 * @returns { WebAttribute }
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 * @arkts 1.1&1.2
 */
onPdfLoadEvent(callback: Callback<OnPdfLoadEvent>): WebAttribute;
```

* **接口作用说明**
  上述新增接口用于通知宿主应用PDF页面已加载到底部以及加载完成事件。
* **接口在M114遗留内核上的行为**
  应用在M114内核上为组件设置上述回调不生效，不报错、不抛异常、无错误码；设置后系统不会触发此回调。

#### 同层渲染新增接口

```
declare interface EmbedOptions {
  supportDefaultIntrinsicSize?: boolean;

  /**
   * Whether the {@link onNativeEmbedVisibilityChange} event supports display-related attributes
   * of the embed element.
   * <br>Default value is false. If true, the changes of the display-related attributes of the
   * embed element will be reported through the {@link onNativeEmbedVisibilityChange} event.
   *
   * @type { ?boolean }
   * @default false
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  supportCssDisplayChange?: boolean;
}

nativeEmbedOptions(options?: EmbedOptions): WebAttribute;

/**
 * Defines the user mouse info on embed layer.
 *
 * @typedef NativeEmbedMouseInfo
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
declare interface NativeEmbedMouseInfo {
  /**
   * The native embed id.
   *
   * @type { ?string }
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  embedId?: string;

  /**
   * An event sent when the state of contacts with a mouse-sensitive surface changes.
   *
   * @type { ?MouseEvent }
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  mouseEvent?: MouseEvent;

  /**
   * Handle the user's mouse result.
   *
   * @type { ?EventResult }
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  result?: EventResult;
}

/**
 * The callback when mouse event is triggered in native embed area
 *
 * @typedef { function } MouseInfoCallback
 * @param { NativeEmbedMouseInfo } event - callback information of mouse event in native embed area.
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
type MouseInfoCallback = (event: NativeEmbedMouseInfo) => void;

/**
 * Triggered when mouse effect on embed tag.
 *
 * @param { MouseInfoCallback } callback - callback Triggered when mouse effect on embed tag.
 * @returns { WebAttribute }
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
onNativeEmbedMouseEvent(callback: MouseInfoCallback): WebAttribute;

declare class EventResult {
  ...
  /**
   * Sets the mouse event consumption result.
   *
   * @param { boolean } result -  Whether to consume the mouse event.
   *    {@code true} Indicates the consumption of the mouse event.
   *    {@code false} Indicates the non-consumption of the mouse event.
   *    Default value: true.
   * @param { boolean } stopPropagation - Whether to stop propagation.
   *    This parameter is valid only when result is set to true. 
   *    {@code true} Indicates stops the propagation of events farther along.
   *    {@code false} Indicates the propagation of events farther along.
   *    Default value: true.
   * @syscap SystemCapability.Web.Webview.Core
   * @since 20
   */
  setMouseEventResult(result: boolean, stopPropagation?: boolean): void;
}
declare interface NativeEmbedTouchInfo {
  embedId?: string;
  touchEvent?: TouchEvent;
  result?: EventResult;
}
onNativeEmbedGestureEvent(callback: (event: NativeEmbedTouchInfo) => void): WebAttribute;
```

* **接口作用说明**
  同层组件新增了以下两个接口：
  * supportCssDisplayChange：可见性支持display属性
  * onNativeEmbedMouseEvent：同层组件支持鼠标事件
  * setMouseEventResult：onNativeEmbedGestureEvent回调时，设置鼠标事件的消费结果
* **接口在M114遗留内核上的行为**
  * `supportCssDisplayChange`在M114遗留内核上设置不生效，不会抛异常，也不会返回错误码。
  * `onNativeEmbedMouseEvent`在M114遗留内核上设置后会不生效，内核不会触发鼠标事件，只触发 `onNativeEmbedGestureEvent`
  * `setMouseEventResult`在M114遗留内核上设置不生效，不会抛异常，也不会返回错误码。

### 4.4 NDK接口

#### OH_NativeArkWeb_SetBlanklessLoadingWithKey接口

```
typedef enum ArkWeb_BlanklessErrorCode {
/*
 * @brief Success.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
ARKWEB_BLANKLESS_SUCCESS = 0,

/*
 * @brief Unknown error.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
ARKWEB_BLANKLESS_ERR_UNKNOWN = -1,

/*
 * @brief Invalid args.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
ARKWEB_BLANKLESS_ERR_INVALID_ARGS = -2,

/*
 * @brief Init error. The web controller is not binded with the component.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
ARKWEB_BLANKLESS_ERR_CONTROLLER_NOT_INITED = -3,

/*
 * @brief The key of blankless was not matched.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
ARKWEB_BLANKLESS_KEY_NOT_MATCH = -4,

/*
 * @brief There are significant changes for the loading page.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
ARKWEB_BLANKLESS_SIGNIFICANT_CHANGE = -5,

/*
 * @brief Device not support.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
ARKWEB_BLANKLESS_ERR_DEVICE_NOT_SUPPORT = 801,
} ArkWeb_BlanklessErrorCode;

/**
 * @brief Sets whether to enable blankless page loading. This API must be used in pair with the
 * OH_NativeArkWeb_GetBlanklessInfoWithKey API.
 *
 * @param webTag webTag used when the webviewController is created.
 * @param key Key value that uniquely identifies the current page. It must be the same as the key value of the
 * OH_NativeArkWeb_GetBlanklessInfoWithKey API.
 * @param isStarted Whether to enable frame interpolation. The value true indicates to enable frame
 * interpolation, and the value false indicates the opposite.
 * The default value is false.
 * The value can be true or false.
 * Action for setting an invalid value: N/A.
 * @return Whether the API is successfully called. For details, see ArkWeb_BlanklessErrorCode.
 * @since 20
 */
ArkWeb_BlanklessErrorCode OH_NativeArkWeb_SetBlanklessLoadingWithKey(const char* webTag, const char* key, bool isStarted);
```

* **接口作用说明**
  设置Blankless加载是否使能
* **接口在M114遗留内核上的行为**
  接口功能在M114内核不支持，返回错误码 `801`

#### OH_NativeArkWeb_ClearBlanklessLoadingCache接口

```
/**
 * @brief Clears the blankless loading cache of the page with a specified key value.
 *
 * @param key The list of key values of pages cached in the blankless loading solution. These key values are
 * specified in OH_NativeArkWeb_GetBlanklessInfoWithKey.
 * The default value is the list of key values of all pages cached in the blankless loading solution.
 * The key length cannot exceed 2048 characters, and the number of keys must be less than or equal to 100. The
 * URL is the same as that input to the Web component during page loading.
 * When the key length exceeds 2048 characters, the key does not take effect. When the number of keys exceeds
 * 100, the first 100 keys are used. If this parameter is set to NULL, the default value is used.
 * @param size Size of the key list.
 * @since 20
 */
void OH_NativeArkWeb_ClearBlanklessLoadingCache(const char* key[], uint32_t size);
```

* **接口作用说明**
  清除指定key值页面Blankless优化缓存，本接口只清除缓存。
* **接口在M114遗留内核上的行为**
  接口功能在M114内核不支持，无效果

#### OH_NativeArkWeb_GetBlanklessInfoWithKey接口

```
/**
 * @brief Defines the blankless information.
 *
 * @since 20
 */
typedef struct {
    /** The errCode of the blankless. */
    ArkWeb_BlanklessErrorCode errCode;
    /** The estimated similarity of the history snapshots. */
    double similarity;
    /** The loadingTime of the history loading. */
    int32_t loadingTime;
} ArkWeb_BlanklessInfo;

/**
 * @brief Obtains the prediction information about the blankless loading solution and enables the generation
 * of the transition frame for the current loading. The application determines whether to enable the blankless
 * loading solution based on the information.
 * This API applies to pages in an applet or web application whose URLs are not fixed or cannot be uniquely
 * identified.
 *
 * @param webTag webTag used when the webviewController is created.
 * Default value: N/A.
 * The value cannot be empty.
 * When an invalid value is set, the error code is returned, and the API does not take effect.
 * @param key Key value that uniquely identifies the current page.
 * @return Return value of the ArkWeb_BlanklessInfo type.
 * @since 20
 */
ArkWeb_BlanklessInfo OH_NativeArkWeb_GetBlanklessInfoWithKey(const char* webTag, const char* key);
```

* **接口作用说明**
  获取页面首屏加载预测信息，并使能本次加载过渡帧生成，应用根据此信息确定是否需要使能Blankless加载
* **接口在M114遗留内核上的行为**
  接口功能在M114内核不支持，返回ArkWeb_BlanklessInfo类型变量的ArkWeb_BlanklessErrorCode错误码是 `801`

#### OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity接口

```
/**
 * @brief Sets the cache capacity of the blankless loading solution and returns the value that takes effect.
 *
 * @param capacity Cache capacity, in MB. The maximum value is 100 MB.
 * The default value is 30 MB.
 * The value ranges from 0 to 100. If this parameter is set to 0, no cache capacity is available and the
 * functionality is disabled globally.
 * When the value is set to a number smaller than 0, the value 0 takes effect. When the value is set to a
 * number greater than 100, the value 100 takes effect.
 * @return The effective value that ranges from 0 MB to 100 MB.
 * @since 20
 */
uint32_t OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity(uint32_t capacity);
```

* **接口作用说明**
  设置Blankless加载方案持久缓存容量，返回实际生效的值
* **接口在M114遗留内核上的行为**
  接口功能在M114内核不支持，无效果，返回值是0

#### OH_ArkWebResourceHandler_DidFailWithErrorV2接口

```
/*
 * @brief Notify the ArkWeb that this request should be failed.
 * @param resourceHandler The ArkWeb_ResourceHandler for the request.
 * @param errorCode The error code for this request. Refer to arkweb_net_error_list.h.
 * @param completeIfNoResponse If true, will construct a response when haven't received a response.
 * @return {@link ARKWEB_NET_OK} 0 - Success.
 *         {@link ARKWEB_INVALID_PARAM} 17100101 - Invalid param.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
int32_t OH_ArkWebResourceHandler_DidFailWithErrorV2(const ArkWeb_ResourceHandler* resourceHandler,
                                                    ArkWeb_NetError errorCode,
                                                    bool completeIfNoResponse);
```

* **接口作用说明**
  通知ArkWeb内核，被拦截请求应返回失败。若completeIfNoResponse为false，调用前需优先调用didReceiveResponse，将构造的响应头传递给被拦截的请求。若completeIfNoResponse为true，且调用前未调用didReceiveResponse，则自动生成一个响应头，网络错误码为 `-104`。
* **接口在M114遗留内核上的行为**
  接口行为跟 `OH_ArkWebResourceHandler_DidFailWithError`一致，completeIfNoResponse参数不生效。

#### OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy 接口

```
/**
 * @brief Register a JavaScript object with callback methods, which may return values. This object will be injected
 *        into all frames of the current page, including all iframes, and will be accessible using the specified
 *        name in ArkWeb_ProxyObjectWithResult. The object will only be available in JavaScript after the next
 *        load or reload.
 *        These methods will be executed in the ArkWeb worker thread.
 *
 * @param webTag The name of the web component.
 * @param proxyObject The JavaScript object to register, the object has callback functions with return value.
 * @param permission The JSON string, which defaults to null, is used to configure the permission control for
 *                   JSBridge, allowing for the definition of URL whitelists at the object and method levels.
 *
 * @since 20
 */
void OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy(const char* webTag,
    const ArkWeb_ProxyObjectWithResult* proxyObject, const char* permission);
```

* **接口作用说明**
  ndk接口支持在异步线程注册JavaScriptProxy，避免ui线程繁忙导致的阻塞。
* **接口在M114遗留内核上的行为**
  设置不生效，不会抛异常，也不会返回错误码。
  不建议开发者在M114内核中使用以上接口。

#### ArkWebHttpBodyStream AsyncRead 接口

```
/**
* @brief Callback when the read operation done.
* @param httpBodyStream The ArkWeb_HttpBodyStream.
* @param buffer The buffer to receive data.
* @param bytesRead Callback after OH_ArkWebHttpBodyStream_AsyncRead. bytesRead greater than 0 means that
* the buffer is filled with data of bytesRead size. Caller can read from the buffer, and if
* OH_ArkWebHttpBodyStream_IsEOF is false, caller can continue to read the remaining data.
* 
* @syscap SystemCapability.Web.Webview.Core
* @since 20
  */

  typedef void (*ArkWeb_HttpBodyStreamAsyncReadCallback)(const ArkWeb_HttpBodyStream *httpBodyStream,
uint8_t *buffer,
int bytesRead);
/**
* @brief Set the callback for OH_ArkWebHttpBodyStream_AsyncRead.
* 
* The result of OH_ArkWebHttpBodyStream_AsyncRead will be notified to caller through the\n
* readCallback. The callback will runs in the ArkWeb worker thread.\n
* 
* @param httpBodyStream The ArkWeb_HttpBodyStream.
* @param readCallback The callback of read function.
* @return {@link ARKWEB_NET_OK} 0 - Success.
* {@link ARKWEB_INVALID_PARAM} 17100101 - Invalid param.
* 
* @syscap SystemCapability.Web.Webview.Core
* @since 20
  */

  int32_t OH_ArkWebHttpBodyStream_SetAsyncReadCallback(ArkWeb_HttpBodyStream *httpBodyStream,
  ArkWeb_HttpBodyStreamAsyncReadCallback readCallback);

/**

* @brief Read the http body to the buffer.
* 
* The buffer must be larger than the bufLen. We will read data from a worker thread to the buffer,\n
* so should not use the buffer in other threads before the callback to avoid concurrency issues.\n
* 
* @param httpBodyStream The ArkWeb_HttpBodyStream.
* @param buffer The buffer to receive data.
* @param bufLen The size of bytes to read.
* 
* @syscap SystemCapability.Web.Webview.Core
* @since 20
  */
  void OH_ArkWebHttpBodyStream_AsyncRead(const ArkWeb_HttpBodyStream *httpBodyStream, uint8_t *buffer, int bufLen);
```

* **接口作用说明**
  ArkWebHttpBodyStream_AsyncRead 支持异步异步数据，常用于性能优化 。
* **接口在M114遗留内核上的行为**
  OH_ArkWebHttpBodyStream_SetAsyncReadCallback 设置不生效，返回错误码17100100。
  OH_ArkWebHttpBodyStream_AsyncRead 不执行操作。
  ArkWeb_HttpBodyStreamAsyncReadCallback 不会触发回调。
  不建议开发者在M114内核中使用以上接口。


# M114内核在OpenHarmony6.0系统上的适配指导

## 1. Web内核切换API使用说明

OpenHarmony 6.0系统ArkWebCore内核默认升级到了M132版本，同时系统提供了双内核方案，保留老的M114版本内核，以便生态应用自主选择M114升级到M132的节奏和策略，降低应用因Web内核升级而导致的兼容性问题。

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
    ARKWEB_EVERGREEN = 99999
}
static setActiveWebEngineVersion(engineVersion: ArkWebEngineVersion): void;
static getActiveWebEngineVersion(): ArkWebEngineVersion;
static isActiveWebEngineEvergreen(): boolean;
```

ArkWebEngineVersion枚举值定义：

|    **枚举值**    | **内核类型**             | **说明**                                                                                                               |
| :--------------: | ------------------------ | ---------------------------------------------------------------------------------------------------------------------- |
|       M132       | 6.0版本的常青内核        | 6.0版本上的默认内核。如果后续OpenHarmony系统版本上不存在此内核则设置无效，使用系统默认内核。                                                      |
|       M114       | 6.0版本的遗留内核        | 开发者可选择此遗留内核。如果后续OpenHarmony系统版本上不存在此内核则设置无效，使用系统默认内核。 计划在2026-Q2禁用此内核。                                |
|  SYSTEM_DEFAULT  | 系统默认                 | 使用系统上默认内核，6.0版本上默认为M132 |
| ARKWEB_EVERGREEN | 常青内核，系统的最新内核 | 开发者可选择在每个系统版本上都使用最新的内核，6.1以及之后所有系统版本都生效，比如7.0系统上常青内核可能是最新的其他内核。<br>**说明：** 从API version 23开始支持|

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
> * **指定版本的内核已经失效**：遗留内核的生命周期通常在系统发布半年后，生命周期结束后，指定遗留内核版本不会生效。
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

应用会使用W3C中navigator的[userAgent](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/userAgent)和[platform](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/platform)属性进行业务隔离，这些字段的值如下所示：

| **类型**      | **[platform](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/platform)** | **[userAgent](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/userAgent)**                                                       |
| ------------- | ----------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------- |
| M114 on OH5.1 | Linux x86_64                                                                        | Mozilla/5.0 (Phone; OpenHarmony 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/**114.0.0.0** Safari/537.36 ArkWeb/*5.1.0.207* Mobile |
| M114 on OH6.0 | Linux x86_64                                                                        | Mozilla/5.0 (Phone; OpenHarmony 6.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/**114.0.0.0** Safari/537.36 ArkWeb/*6.0.0.44* Mobile  |
| M132 on OH6.0 | Linux x86_64                                                                        | Mozilla/5.0 (Phone; OpenHarmony 6.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/**132.0.0.0** Safari/537.36 ArkWeb/*6.0.0.44* Mobile  |

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

* **接口作用说明**:
  设置和查询默认错误页配置。
* **接口在M114遗留内核上的行为**:
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

* **接口作用说明**:
  设置和查询Web组件设置私有网络访问检查功能（Private Network Access）的启用状态。
* **接口在M114遗留内核上的行为**:
  [enablePrivateNetworkAccess](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#enablePrivateNetworkAccess20)设置不生效，不会抛异常，也不会返回错误码；[isPrivateNetworkAccessEnabled](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#isPrivateNetworkAccessEnabled20)接口获取值始终为false。
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

* **接口作用说明**:
  设置应用或应用的特定网站设置自定义用户代理，会覆盖系统的用户代理，应用内所有Web组件生效。
* **接口在M114遗留内核上的行为**:
  [setAppCustomUserAgent](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#setAppCustomUserAgent20)在M114内核中使用该接口不生效，不会抛异常，也不会返回错误码。
  [setUserAgentForHosts ](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#setuseragentforhosts20)设置不生效，不会抛异常，也不会返回错误码。
  不建议开发者在M114内核中使用以上接口。

#### getProgress获取网页加载进度接口

```
/**
 * Gets the loading progress for the current page.
 *
 * @returns { number } The loading progress for the current page.
 * @throws { BusinessError } 801 - Capability not supported.
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
getProgress() : number;
```

* **接口作用说明**:
  获取当前网页加载进度。
* **接口在M114遗留内核上的行为**:
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

* **接口作用说明**:
  设置是否启用无线网页调试功能，默认不开启。
* **接口在M114遗留内核上的行为**:
  [setWebDebuggingAccess(webDebuggingAccess: boolean, port: number)](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#setwebdebuggingaccess20)的`port: number`端口设置无效，建议开发者直接使用[static setWebDebuggingAccess(webDebuggingAccess: boolean): void](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-apis-webview-WebviewController.md#setwebdebuggingaccess)。

#### WebResourceHandler didFail接口

```
/**
 class WebResourceHandler {
/**
 * Notify that this request should be failed.
 *
 * @param { WebNetErrorList } code - Set response error code to intercept.
 * @param { boolean } completeIfNoResponse - If completeIfNoResponse is true, when DidFailWithError is called, if
 *                                           DidReceiveResponse has not been called, a response is automatically
 *                                           constructed and the current request is terminated.
 * @throws { BusinessError } 17100101 - The errorCode is either ARKWEB_NET_OK or outside the range of error codes
 *                                      in WebNetErrorList.
 * @throws { BusinessError } 17100021 - The resource handler is invalid.
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
didFail(code: WebNetErrorList, completeIfNoResponse: boolean): void;
}
```

* **接口作用说明**:
  通知ArkWeb内核，被拦截请求应返回失败。若completeIfNoResponse为false，调用前需优先调用didReceiveResponse，将构造的响应头传递给被拦截的请求。若completeIfNoResponse为true，且调用前未调用didReceiveResponse，则自动生成一个响应头，网络错误码为`-104`。
* **接口在M114遗留内核上的行为**:
  接口行为跟``didFail(code: WebNetErrorList): void;``一致，completeIfNoResponse参数不生效。

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

* **接口作用说明**:
  通过setWebDestroyMode设置Web的析构模式，当WebDestroyMode设置成FAST_MODE为立即析构，当Web组件触发销毁时，会立即释放相关资源；设置成NORMAL_MODE为正常模式，当Web组件触发销毁时会延迟析构，默认值为NORMAL_MODE。
* **接口在M114遗留内核上的行为**:
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

* **接口作用说明**:
  当Web页面触发window.open(url, name)时，会根据name查找是否存在已绑定的Web实例。若存在，则触发onActivateContent回调并通知应用需将其展示至前端；若不存在，则通过onWindowNew通知应用创建新Web实例。
* **接口在M114遗留内核上的行为**:
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

* **接口作用说明**:
  获取页面滑动偏移量，与getScrollOffset相比，不包含过滚动的偏移。
* **接口在M114遗留内核上的行为**:
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
	 *                           801 - Capability not supported.
     *                           The WebviewController must be associated with a Web component.
     * @syscap SystemCapability.Web.Webview.Core
     * @since 20
     */
    avoidVisibleViewportBottom(avoidHeight: number): void;
```

* **接口作用说明**:
  设置网页底部避让高度
* **接口在M114遗留内核上的行为**:
  `avoidVisibleViewportBottom`在M114遗留内核上设置不生效，将返回错误码801。
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

* **接口作用说明**:
  获取页面首屏加载预测信息（具体见****WebBlanklessInfo ****），并使能本次加载过渡帧生成，应用根据此信息确定是否需要使能Blankless加载
* **接口在M114遗留内核上的行为**:
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

* **接口作用说明**:
  设置Blankless加载是否使能
* **接口在M114遗留内核上的行为**:
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

* **接口作用说明**:
  清除指定key值页面Blankless优化缓存，本接口只清除缓存。
* **接口在M114遗留内核上的行为**:
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

* **接口作用说明**:
  设置Blankless加载方案持久缓存容量，返回实际生效的值
* **接口在M114遗留内核上的行为**:
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

* **接口作用说明**:
  应用中Web音频类型。默认值对应系统音频流类型 STREAM\_USAGE\_MUSIC。设置该参数会改变组件音频类型与系统音频类型映射关系，进而影响Web音频焦点策略。
* **接口在M114遗留内核上的行为**:
  audioSessionType 参数在M114遗留内核上设置不生效，不会抛异常，也不会返回错误码。在M114上，mediaOptions接口参数 WebMediaOptions 仅 resumeInterval 和 audioExclusive 参数生效；
  不建议开发者在M114内核中使用audioSessionType 参数。

#### 页面加载回调接口

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
 * @param { OnErrorReceiveEvent } errorPageEvent - The information of error.
 * @returns { string } - Return an HTML text content encoded in Base64.
 * @syscap SystemCapability.Web.Webview.Core
 * @since 20
 */
type OnOverrideErrorPageCallback = (errorPageEvent: OnErrorReceiveEvent) => string;

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

* **接口作用说明**:
  * onLoadStarted：应用页面开始加载时触发该回调
  * onLoadFinished:：应用页面加载完成时触发该回调
  * onOverrideErrorPage：页面加载遇到错误时触发该回调, 可使用该接口自定义错误展示页
  * onTitleReceive: 应用页面文档标题发生变化时触发该回调
  * onBeforeUnload: 即将完成页面刷新或关闭当前页面时触发该回调
* **接口在M114遗留内核上的行为**:
  在M114内核上，下述三个新增回调函数将失效，为组件定义这些回调时，都可以成功调用，但设置后系统不会触发此回调。
  
  * [onLoadStarted](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-events.md#onloadstarted20)
  * [onLoadFinished](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-events.md#onloadfinished20)
  * [onOverrideErrorPage](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-events.md#onoverrideerrorpage20)
  
  应用业务如果依赖此类回调的执行，需要做好兼容适配，确保不回调时，业务也可以降级完成。
  
  下述两个接口在M114行也会回调，只是行为与132内核有所区别:
  * [onTitleReceive](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-events.md#ontitlereceive)接口在回调时，isRealTitle始终为false。
  * [onBeforeUnload](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkweb/arkts-basic-components-web-events.md#onBeforeUnload)接口在回调时，isReload始终为false。


/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BASE_OHOS_NWEB_ENGINE_EVENT_LOGGER_CODE_H_
#define BASE_OHOS_NWEB_ENGINE_EVENT_LOGGER_CODE_H_

#include <stdint.h>
#include <string>

#include "base/base_export.h"

namespace base {
namespace ohos {

/********************************************
  *错误码*     *描述*
102_01****  网络请求错误
102_021001  资源加载错误
102_031001  html解析错误
102_041001  DOM树
102_051001  网页layer tree创建错误
102_061001  Js解析错误
102_071001  css解析错误
102_08****  渲染
102_091001  媒体错误（音视频播放等）
102_101001  硬件加速错误
102_111001  网页滑动错误
102_121001  网页输入事件捕捉处理错误
102_131001  下载业务模块
102_141001  广告拦截模块
102_142001  进程启动错误
102_151001  密码管理模块
**********************************************/

BASE_EXPORT extern const char kDefaultUrl[];

// Module Name Value
BASE_EXPORT extern const char kModuleComponent[];
BASE_EXPORT extern const char kModuleContentBrowser[];
BASE_EXPORT extern const char kModuleContentRender[];
BASE_EXPORT extern const char kModuleNet[];

// 网络请求错误
BASE_EXPORT extern const char kNetworkSSLError[];
BASE_EXPORT extern const char kNetworkHTTPError[];
BASE_EXPORT extern const char kReceivedHttpError[];
BASE_EXPORT extern const char kEmptyContent[];
BASE_EXPORT extern const char kNetworkNetError[];
BASE_EXPORT extern const char kNetworkDNSError[];
BASE_EXPORT extern const char kMultiIpConnect[];
BASE_EXPORT extern const char kSecureDnsFallbackResult[];
BASE_EXPORT extern const char kSecureDnsRetryResult[];
BASE_EXPORT extern const char kDnsTransactionPartialFailedResult[];
BASE_EXPORT extern const char kHostDidUseQUIC[];
BASE_EXPORT extern const char kDnsHijackingResult[];
BASE_EXPORT extern const char kSbOverlayErrorPage[];
BASE_EXPORT extern const char kErrorPageOverlaySb[];
BASE_EXPORT extern const char kUseSslCipher[];
BASE_EXPORT extern const char kNetworkChangeError[];
BASE_EXPORT extern const char kErrorPageReloadInfo[];
BASE_EXPORT extern const char kNotifyChannelLagResult[];
BASE_EXPORT extern const char kErrorWithWirelesskit[];

// 资源加载错误
BASE_EXPORT extern const char kResourceLoadError[];
// html解析错误
BASE_EXPORT extern const char kHtmlParseError[];
// DOM树
BASE_EXPORT extern const char kDomTreeCreateError[];
// 网页layer tree创建错误
BASE_EXPORT extern const char kLayerTreeCreateError[];
// Js解析错误
BASE_EXPORT extern const char kJsParseError[];
// css解析错误
BASE_EXPORT extern const char kCssParseError[];

// 渲染
BASE_EXPORT extern const char kLoadEvent[];
BASE_EXPORT extern const char kContentLoadEvent[];
BASE_EXPORT extern const char kFirstScreenEvent[];
BASE_EXPORT extern const char kErrorPage[];

// 媒体错误（音视频播放等）
BASE_EXPORT extern const char kMediaError[];
// 硬件加速错误
BASE_EXPORT extern const char kHardwareAccelerateError[];
// 网页滑动错误
BASE_EXPORT extern const char kWebpageSlideError[];
// 网页输入事件捕捉处理错误
BASE_EXPORT extern const char kWebpageInputEventError[];
// 下载业务模块
BASE_EXPORT extern const char kDownloadError[];
// 广告拦截模块
BASE_EXPORT extern const char kAdblockError[];
// 进程启动错误
BASE_EXPORT extern const char kLauchProcessError[];
// 密码管理模块
BASE_EXPORT extern const char kPasswordManagerError[];

}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_NWEB_ENGINE_EVENT_LOGGER_CODE_H_
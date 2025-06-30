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

#include "base/ohos/nweb_engine_event_logger_code.h"

namespace base {
namespace ohos {

const char kDefaultUrl[] = "***";

// Module Name Value.
const char kModuleContentBrowser[] = "conent_browser";
const char kModuleContentRender[] = "conent_render";
const char kModuleNet[] = "net";

// 网络请求错误
const char kNetworkSSLError[]                   = "102_011001"; // SSL错误
const char kNetworkHTTPError[]                  = "102_012001"; // HTTP错误
const char kReceivedHttpError[]                 = "102_012002"; // 收到http错误
const char kEmptyContent[]                      = "102_012204"; // 服务器返回204
const char kNetworkNetError[]                   = "102_013001"; // Net错误
const char kNetworkDNSError[]                   = "102_014001"; // DNS错误
const char kMultiIpConnect[]                    = "102_014002"; // 多IP链接
const char kSecureDnsFallbackResult[]           = "102_014003"; // https dns fallback解析结果
const char kSecureDnsRetryResult[]              = "102_014004"; // 使用https dns的重试结果
const char kDnsTransactionPartialFailedResult[] = "102_014005"; // DNS查询部分失败
const char kHostDidUseQUIC[]                    = "102_014006"; // 使用QUIC打点
const char kDnsHijackingResult[]                = "102_014007"; // 可疑DNS劫持打点
const char kSbOverlayErrorPage[]                = "102_014008"; // Error Page Hit Safe Browsing Code
const char kErrorPageOverlaySb[]                = "102_014009"; // Errorpage Overlay SB Blocking page.
const char kUseSslCipher[]                      = "102_014010"; // Did use ssl cipher
const char kNetworkChangeError[]                = "102_015001"; // 网络切换错误
const char kErrorPageReloadInfo[]               = "102_015002"; // Reload失败
const char kNotifyChannelLagResult[]            = "102_015003"; // WirelessKitChanne切换
const char kErrorWithWirelesskit[]              = "102_015004"; // WirelessKit错误

// 资源加载错误
const char kResourceLoadError[]    = "102_021001";
// html解析错误
const char kHtmlParseError[]       = "102_031001";
// DOM树
const char kDomTreeCreateError[]   = "102_041001";
// 网页layer tree创建错误
const char kLayerTreeCreateError[] = "102_051001";
// Js解析错误
const char kJsParseError[]         = "102_061001";
// css解析错误
const char kCssParseError[]        = "102_071001";

// 渲染
const char kLoadEvent[]        = "102_081001"; // Load加载完成
const char kErrorPage[]        = "102_081002";
const char kContentLoadEvent[] = "102_082001"; // ContentLoad完成
const char kFirstScreenEvent[] = "102_083001"; // 首屏绘制完成

// 媒体错误（音视频播放等）
const char kMediaError[] = "102_091001";
// 硬件加速错误
const char kHardwareAccelerateError[] = "102_101001";
// 网页滑动错误
const char kWebpageSlideError[] = "102_111001";
// 网页输入事件捕捉处理错误
const char kWebpageInputEventError[] = "102_121001";
// 下载业务模块
const char kDownloadError[] = "102_131001";
// 广告拦截模块
const char kAdblockError[] = "102_141001";
// 进程启动错误
const char kLauchProcessError[] = "102_142001";
// 密码管理模块
const char kPasswordManagerError[] = "102_151001";

}  // namespace ohos
}  // namespace base
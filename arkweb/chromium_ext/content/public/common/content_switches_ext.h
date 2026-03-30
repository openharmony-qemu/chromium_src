// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on content_switches.h originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ARKWEB_CONTENT_PUBLIC_COMMON_CONTENT_SWITCHES_EXT_H_
#define ARKWEB_CONTENT_PUBLIC_COMMON_CONTENT_SWITCHES_EXT_H_

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#include "content/common/content_export.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

namespace switches {

#if BUILDFLAG(IS_ARKWEB)
CONTENT_EXPORT extern const char kEnableMultiRendererProcess[];
CONTENT_EXPORT extern const char kOhosEnableDrDc[];
CONTENT_EXPORT extern const char kOhosHanceSurface[];
CONTENT_EXPORT extern const char kOhosDisableTouchEvent[];
CONTENT_EXPORT extern const char kPasswordWaitUsername[];
CONTENT_EXPORT extern const char kMaxNumberOfSavedFrames[];
// CONTENT_EXPORT extern const char kNumRasterThreads[];
CONTENT_EXPORT extern const char kIsMultiRenderProcess[];
CONTENT_EXPORT extern const char kSimpleBackendIsDefault[];
CONTENT_EXPORT extern const char kOHOSPersistSessionCookies[];
CONTENT_EXPORT extern const char kEnableEmbedMode[];
CONTENT_EXPORT extern const char kWebViewImplForLargeScreen[];
CONTENT_EXPORT extern const char kEnableDeleteUnusedResourcesDelay[];
CONTENT_EXPORT extern const char kHttpCacheMaxSize[];
CONTENT_EXPORT extern const char kCookieConfigPersistSession[];
CONTENT_EXPORT extern const char kDoubleTapSupportForPlatformEnabled[];
CONTENT_EXPORT extern const char kUserAgentValue[];
CONTENT_EXPORT extern const char kIgnoreLockdownMode[];
CONTENT_EXPORT extern const char kEnablePrinting[];
CONTENT_EXPORT extern const char kOhosHttpCacheSimple[];
CONTENT_EXPORT extern const char kCalcTabletMode[];
CONTENT_EXPORT extern const char kDisableMobileStyleSheet[];
CONTENT_EXPORT extern const char kDisableNonAdvancedSecurityMode[];
CONTENT_EXPORT extern const char kDisableNextPreviousFlag[];
#endif

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
CONTENT_EXPORT extern const char kOhSchemeHandlerCustomScheme[];
#endif

#if BUILDFLAG(ARKWEB_HAP_DECOMPRESSED)
CONTENT_EXPORT extern const char kOhosHapPath[];
#endif

#if BUILDFLAG(ARKWEB_BUGFIX_CRASH)
CONTENT_EXPORT extern const char kOhosHanceSurface[];
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
CONTENT_EXPORT extern const char kOhosSubresourceFilter[];
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
CONTENT_EXPORT extern const char kOhosCustomScheme[];
#endif

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
CONTENT_EXPORT extern const char kEnableNwebExHttpDnsFallback[];
CONTENT_EXPORT extern const char kEnableNwebExDownload[];
#endif

#if BUILDFLAG(IS_ARKWEB)
CONTENT_EXPORT extern const char kEnableNwebEx[];
CONTENT_EXPORT extern const char kEnableMediaAvsession[];
CONTENT_EXPORT extern const char kEnableNwebExReportCrashpad[];
CONTENT_EXPORT extern const char kEnableNwebExTopControls[];
CONTENT_EXPORT extern const char kEnableNwebExFreeCopy[];
CONTENT_EXPORT extern const char kEnableNwebExPermission[];
CONTENT_EXPORT extern const char kEnableNwebExExceptionList[];
CONTENT_EXPORT extern const char kEnableNwebExPassword[];
CONTENT_EXPORT extern const char kEnableNwebExUa[];
CONTENT_EXPORT extern const char kEnableNwebExNetworkLoad[];
CONTENT_EXPORT extern const char kEnableNwebExGetZoomLevel[];
CONTENT_EXPORT extern const char kEnableNwebExPullToRefresh[];
#endif

#if BUILDFLAG(ARKWEB_BFCACHE)
extern const char kEnableBFCache[];
#endif

#if BUILDFLAG(ARKWEB_BFCACHE)
extern const char kEnableBFCache[];
extern const char kEnableCacheNativeEmbed[];
extern const char kEnableCacheMediaTakeOver[];
#endif

#if BUILDFLAG(ARKWEB_VULKAN)
CONTENT_EXPORT extern const char kOhosEnableVulkan[];
#endif

#if BUILDFLAG(ARKWEB_ADVANCED_SECURITY_MODE)
CONTENT_EXPORT extern const char kAdSecValue[];
CONTENT_EXPORT extern const char kDisableMathml[];
CONTENT_EXPORT extern const char kDisablePdfViewer[];
CONTENT_EXPORT extern const char kDisableServiceWorker[];
CONTENT_EXPORT extern const char kDisableNonProxyUDP[];
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
CONTENT_EXPORT extern const char kSetExtensionName[];
#endif
#if BUILDFLAG(ARKWEB_GWP_ASAN)
CONTENT_EXPORT extern const char  kOhosEnableGwpAsanType[];
#endif
#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
CONTENT_EXPORT extern const char  kEnableWebAudioBackgroundTask[];
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
CONTENT_EXPORT extern const char  kEnableReportCookieMonsterClient[];
#endif // BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)

#if BUILDFLAG(IS_ARKWEB)
CONTENT_EXPORT extern const char  kEnableReportThreadPoolForeg[];
#endif
}  // namespace switches

#endif  // ARKWEB_CONTENT_PUBLIC_COMMON_CONTENT_SWITCHES_EXT_H_

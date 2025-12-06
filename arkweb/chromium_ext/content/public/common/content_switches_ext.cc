// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on content_switches.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content_switches_ext.h"

#include "build/build_config.h"

namespace switches {

#if BUILDFLAG(IS_ARKWEB)
const char kEnableMultiRendererProcess[] = "enable-multi-renderer-process";
const char kOhosEnableDrDc[] = "ohos-enable-drdc";
const char kOhosDisableTouchEvent[] = "disable-touch-event-register";
const char kPasswordWaitUsername[] = "ohos-enable-wait-for-username";
const char kMaxNumberOfSavedFrames[] = "ohos-enable-max-number-of-saved-frames";
// const char kNumRasterThreads[] = "ohos-enable-num-raster-threads";
const char kIsMultiRenderProcess[] = "ohos-enable-single-render-process";
const char kSimpleBackendIsDefault[] = "ohos-enable-simple-backend-is-default";
const char kOHOSPersistSessionCookies[] = "ohos-enable-persist-session-cookies";
const char kEnableEmbedMode[] = "ohos-enable-embed-mode";
const char kWebViewImplForLargeScreen[] =
    "ohos-enable-web-view-impl-for-large-screen";
const char kEnableDeleteUnusedResourcesDelay[] =
    "ohos-enable-delete-unused-resources-delay";
const char kHttpCacheMaxSize[] = "ohos-enable-set-http-cache-max-size";
const char kCookieConfigPersistSession[] =
    "ohos-enable-cookie-config-persist-session";
const char kDoubleTapSupportForPlatformEnabled[] =
    "ohos-enable-double-tap-for-platform";
const char kUserAgentValue[] = "ohos-user-agent-value";
const char kIgnoreLockdownMode[] = "ohos-enable-ignore-lockdown-mode";
const char kEnablePrinting[] = "ohos-enable-printing";
const char kOhosHttpCacheSimple[] = "ohos-enable-http-cache-simple";
const char kCalcTabletMode[] = "ohos-enable-calc-tablet-mode";
const char kDisableMobileStyleSheet[] = "ohos-disable-mobile-style-sheet";
const char kDisableNonAdvancedSecurityMode[] =
    "disable-non-advanced-security-mode";
const char kDisableNextPreviousFlag[] = "ohos-disable-next-previous-flag";
const char kUserCacheDir[] = "user-cache-dir";
const char kArkwebAppDataDir[] = "arkweb-app-data-dir";
const char kCacheDirExists[] = "ohos-cache-dir-exists";
const char kUserDataDirExists[] = "ohos-user-data-dir-exists";

#endif

#if BUILDFLAG(ARKWEB_HAP_DECOMPRESSED)
const char kOhosHapPath[] = "user-hap-path";
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
const char kOhosSubresourceFilter[] = "ohos-subresource-filter";
#endif

#if BUILDFLAG(ARKWEB_BUGFIX_CRASH)
const char kOhosHanceSurface[] = "ohos-enhance-surface";
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
const char kOhosCustomScheme[] = "ohos-custom-scheme";
#endif
#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
const char kOhSchemeHandlerCustomScheme[] = "ohos-scheme-handler-custom-scheme";
#endif

#if BUILDFLAG(IS_ARKWEB)
const char kEnableMediaAvsession[] = "enable-nweb-media-avsession";
const char kEnableNwebEx[] = "enable-nweb-ex";
const char kEnableNwebExReportCrashpad[] = "enable-nweb-ex-report-crashpad";
const char kEnableNwebExTopControls[] = "enable-nweb-ex-top-controls";
const char kEnableNwebExFreeCopy[] = "enable-nweb-ex-free-copy";
const char kEnableNwebExPermission[] = "enable-nweb-ex-permission";
const char kEnableNwebExHttpDnsFallback[] = "enable-nweb-ex-http-dns-fallback";
const char kEnableNwebExExceptionList[] = "enable-nweb-ex-exception-list";
const char kEnableNwebExPassword[] = "enable-nweb-ex-password";
const char kEnableNwebExUa[] = "enable-nweb-ex-ua";
const char kEnableNwebExNetworkLoad[] = "enable-nweb-ex-network-load";
const char kEnableNwebExDownload[] = "enable-nweb-ex-download";
const char kEnableNwebExGetZoomLevel[] = "enable-nweb-ex-get-zoom-level";
#endif
#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
const char kEnableNwebExPullToRefresh[] = "enable-nweb-ex-pull-to-refresh";
#endif

#if BUILDFLAG(ARKWEB_BFCACHE)
const char kEnableBFCache[] = "enable-bfcache";
const char kEnableCacheNativeEmbed[] = "enable-cache-native-embed";
const char kEnableCacheMediaTakeOver[] = "enable-cache-media-take-over";
#endif

#if BUILDFLAG(ARKWEB_VULKAN)
const char kOhosEnableVulkan[] = "ohos-enable-vulkan";
#endif

#if BUILDFLAG(ARKWEB_ADVANCED_SECURITY_MODE)
const char kAdSecValue[] = "advanced_sec_value";
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
const char kSetExtensionName[] = "set-extension-name";
#endif

#if BUILDFLAG(ARKWEB_GWP_ASAN)
const char  kOhosEnableGwpAsanType[] = "ohos-enable-gwp-asan-type";
#endif
#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
const char  kEnableWebAudioBackgroundTask[] = "enable-web-audio-background-task";
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
const char  kEnableReportCookieMonsterClient[] = "ohos-enable-report-cookie-monster-client";
#endif // BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)

#if BUILDFLAG(IS_ARKWEB)
const char  kEnableReportThreadPoolForeg[] = "ohos-enable-report-thread-pool-foreg";
#endif
}  // namespace switches

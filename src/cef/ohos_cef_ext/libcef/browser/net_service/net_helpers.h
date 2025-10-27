// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_NET_SERVICE_NET_HELPERS_H_
#define CEF_LIBCEF_BROWSER_NET_SERVICE_NET_HELPERS_H_

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
#include <map>
#include <vector>
#endif
#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
#include "net/dns/public/secure_dns_mode.h"

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
#include "cef/include/cef_download_handler.h"
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include <mutex>
#include <optional>
#endif

class GURL;

namespace net_service {

#define NETHELPERS_EXPORT __attribute__((visibility("default")))

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
enum class FileAccessType {
  kFileAccessEmpty,
  kFileAccessPass,
  kFileAccessBlock,
};

struct NetHelperSetting {
  bool file_access = false;
  bool block_network = false;
  int cache_mode = 0;
#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
  bool disallow_sandbox_file_access_from_file_url = false;
#endif
  FileAccessType file_access_status = FileAccessType::kFileAccessEmpty;
};
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
struct CustomDnsEntry {
  std::vector<std::string> address;
  int32_t ttl;
};
#endif

#if BUILDFLAG(ARKWEB_NETWORK_SERVICE)
constexpr static int32_t kDefaultSocketIdleTimeout = 300;
#endif

class NETHELPERS_EXPORT NetHelpers {
 public:
  static bool ShouldBlockContentUrls();
  static bool ShouldBlockFileUrls();
  static bool IsAllowAcceptCookies();
  static bool IsThirdPartyCookieAllowed();
#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  static bool ShouldBlockFileUrls(struct NetHelperSetting setting);
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
  static bool GetNoUseCache();
#endif

#if BUILDFLAG(ARKWEB_HTTP_DNS)
  static net::SecureDnsMode DnsOverHttpMode();
  static std::string DnsOverHttpServerConfig();
  static bool HasValidDnsOverHttpConfig();
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
  static void SetHostIP(const std::string host_name,
                        const std::string address,
                        int32_t ttl);
  static std::map<std::string, struct CustomDnsEntry> GetHostIP();
  static std::vector<std::string> GetHostIP(const std::string host_name);
  static void ClearHostIP(const std::string host_name);
  static void ClearHostIP();
  static std::map<std::string, struct CustomDnsEntry> custom_dns;
#endif

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  static void SetDownloadHandler(
      CefRefPtr<CefDownloadHandler> download_handler);
  static CefRefPtr<CefDownloadHandler> GetDownloadHandler();
#endif

  static bool allow_content_access;
  static bool allow_file_access;
  static bool is_network_blocked;
  static bool accept_cookies;
  static bool third_party_cookies;
  static int cache_mode;
  static int connection_timeout;

#if BUILDFLAG(ARKWEB_HTTP_DNS)
  static int doh_mode;
  static std::string doh_config;
#endif
#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
  static bool no_use_cache;
#endif
#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
  static int network;
#endif
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  static CefRefPtr<CefDownloadHandler> global_download_handler;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  static std::optional<bool> enable_private_network_check;
  static std::mutex enable_private_network_check_mutex;
  static bool ShouldAllowInsecurePrivateNetworkRequests();
  static void SetPrivateNetworkAccess(bool enable);
  static bool GetPrivateNetworkAccess();
#endif

#if BUILDFLAG(ARKWEB_NETWORK_SERVICE)
  static int32_t socket_idle_timeout;
  static int32_t GetDefaultSocketIdleTimeout();
  static void SetSocketIdleTimeout(int32_t timeout);
  static int32_t GetSocketIdleTimeout();
#endif
};

bool IsSpecialFileUrl(const GURL& url);

// Update request's |load_flags| based on the settings.
int UpdateLoadFlags(int load_flags
#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
,
                    struct NetHelperSetting setting
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
);

// Returns true if the given URL should be aborted with
// net::ERR_ACCESS_DENIED.
bool IsURLBlocked(const GURL& url
#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
,
                  struct NetHelperSetting setting
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
);
}  // namespace net_service

#endif  // CEF_LIBCEF_BROWSER_NET_SERVICE_NET_HELPERS_H_

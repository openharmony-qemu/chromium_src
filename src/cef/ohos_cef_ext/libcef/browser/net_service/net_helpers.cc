// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/net_service/net_helpers.h"

#include "base/base_paths_ohos.h"
#include "base/files/file_path.h"
#include "base/path_service.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "include/base/cef_logging.h"
#include "net/base/load_flags.h"
#include "url/gurl.h"

#if BUILDFLAG(ARKWEB_HTTP_DNS)
#include <string>

#include "build/build_config.h"
#include "net/dns/public/secure_dns_mode.h"
#endif

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
#include "base/base_switches.h"
#include "base/command_line.h"
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include "base/files/file_util.h"
#endif

namespace net_service {

namespace {

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
static const std::string APP_READ_ONLY_PATH =
    "/data/storage/el1/bundle/entry/resources/resfile/";
constexpr int32_t APPLICATION_API_12 = 12;
#endif

#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
static const std::string APP_STORAGE_SANDBOX_PATH =
    "/data/storage";
#endif

int UpdateCacheLoadFlags(int load_flags, int cache_control_flags) {
  const int all_cache_control_flags =
      net::LOAD_BYPASS_CACHE | net::LOAD_VALIDATE_CACHE |
      net::LOAD_SKIP_CACHE_VALIDATION | net::LOAD_ONLY_FROM_CACHE;
  DCHECK_EQ((cache_control_flags & all_cache_control_flags),
            cache_control_flags);
  load_flags &= ~all_cache_control_flags;
  load_flags |= cache_control_flags;
  return load_flags;
}
}  // namespace

bool NetHelpers::allow_content_access = false;
bool NetHelpers::allow_file_access = false;
bool NetHelpers::is_network_blocked = false;
bool NetHelpers::accept_cookies = true;
bool NetHelpers::third_party_cookies = false;
int NetHelpers::cache_mode = 0;
int NetHelpers::connection_timeout = 30;
#if BUILDFLAG(ARKWEB_NETWORK_SERVICE)
int32_t NetHelpers::socket_idle_timeout = kDefaultSocketIdleTimeout;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
std::optional<bool> NetHelpers::enable_private_network_check = std::nullopt;
std::mutex NetHelpers::enable_private_network_check_mutex;
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
std::map<std::string, struct CustomDnsEntry> NetHelpers::custom_dns = {};
#endif

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
CefRefPtr<CefDownloadHandler> NetHelpers::global_download_handler = nullptr;
void NetHelpers::SetDownloadHandler(
    CefRefPtr<CefDownloadHandler> download_handler) {
  NetHelpers::global_download_handler = download_handler;
}
CefRefPtr<CefDownloadHandler> NetHelpers::GetDownloadHandler() {
  return NetHelpers::global_download_handler;
}
#endif

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
bool NetHelpers::no_use_cache = false;
#endif

#if BUILDFLAG(ARKWEB_HTTP_DNS)
int NetHelpers::doh_mode = -1;
std::string NetHelpers::doh_config = "";
#endif

bool NetHelpers::ShouldBlockContentUrls() {
  return !allow_content_access;
}

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
bool NetHelpers::ShouldBlockFileUrls(struct NetHelperSetting setting) {
  return !setting.file_access;
}
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)

bool NetHelpers::ShouldBlockFileUrls() {
  return !allow_file_access;
}

bool NetHelpers::IsAllowAcceptCookies() {
  return accept_cookies;
}

bool NetHelpers::IsThirdPartyCookieAllowed() {
  return third_party_cookies;
}

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
bool NetHelpers::GetNoUseCache() {
  return no_use_cache;
}
#endif

#if BUILDFLAG(ARKWEB_HTTP_DNS)
net::SecureDnsMode NetHelpers::DnsOverHttpMode() {
  net::SecureDnsMode enum_sec_dns_mode = net::SecureDnsMode::kAutomatic;
  switch (doh_mode) {
    case static_cast<int>(net::SecureDnsMode::kSecure):
      enum_sec_dns_mode = net::SecureDnsMode::kSecure;
      break;
    case static_cast<int>(net::SecureDnsMode::kAutomatic):
      enum_sec_dns_mode = net::SecureDnsMode::kAutomatic;
      break;
    case static_cast<int>(net::SecureDnsMode::kOff):
      enum_sec_dns_mode = net::SecureDnsMode::kOff;
      break;
    default:
      LOG(WARNING) << __func__ << " User input mal mode:" << doh_mode;
      break;
  }

  return enum_sec_dns_mode;
}

std::string NetHelpers::DnsOverHttpServerConfig() {
  return doh_config;
}

bool NetHelpers::HasValidDnsOverHttpConfig() {
  if (doh_config.empty()) {
    return false;
  }

  if (doh_mode > static_cast<int>(net::SecureDnsMode::kSecure) ||
      doh_mode < static_cast<int>(net::SecureDnsMode::kOff)) {
    return false;
  }

  return true;
}
#endif  // BUILDFLAG(ARKWEB_HTTP_DNS)

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
int32_t GetApplicationApiVersion() {
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kOhosAppApiVersion)) {
    LOG(ERROR) << "kOhosAppApiVersion not exist";
    return -1;
  }
  std::string apiVersion =
      base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
          switches::kOhosAppApiVersion);
  if (apiVersion.empty()) {
    return -1;
  }
  return std::stoi(apiVersion);
}
#endif

bool IsSpecialFileUrl(const GURL& url) {
  if (!url.is_valid() || !url.SchemeIsFile() || !url.has_path()) {
    return false;
  }

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  if (GetApplicationApiVersion() >= APPLICATION_API_12) {
    if (base::StartsWith(url.path(), APP_READ_ONLY_PATH,
                         base::CompareCase::SENSITIVE)) {
      LOG(INFO) << "Is special file url, the path is in: "
                << APP_READ_ONLY_PATH;
      return true;
    }
    LOG(INFO) << "Is not special file url, the path is not in: "
              << APP_READ_ONLY_PATH;
    return false;
  }
#endif  // BUILDFLAG(ARKWEB_NETWORK_BASE)

  base::FilePath app_data_path;
  if (!base::PathService::Get(base::DIR_OHOS_APP_DATA, &app_data_path)) {
    return false;
  }
  base::FilePath app_bundle_path;
  if (!base::PathService::Get(base::DIR_OHOS_APP_INSTALLATION,
                              &app_bundle_path)) {
    return false;
  }
  LOG(INFO) << "app_data_path:" << app_data_path.value()
            << ", app_bundle_path:" << app_bundle_path.value();

  if (base::StartsWith(url.path(), app_data_path.value(),
                       base::CompareCase::SENSITIVE)) {
    return true;
  }

  if (base::StartsWith(url.path(), app_bundle_path.value(),
                       base::CompareCase::SENSITIVE)) {
    return true;
  }
  return false;
}

#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
bool IsAppStorageSandboxUrl(const GURL& url) {
  if (!url.is_valid() || !url.SchemeIsFile() || !url.has_path()) {
    return false;
  }
 
  return base::StartsWith(url.path(), APP_STORAGE_SANDBOX_PATH,
                          base::CompareCase::SENSITIVE);
}
#endif

bool IsURLBlocked(const GURL& url
#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
,
                  struct NetHelperSetting setting
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
) {
  // Part of implementation of NWebPreference.allowContentAccess.
  if (url.SchemeIs(url::kContentScheme) &&
      NetHelpers::ShouldBlockContentUrls()) {
    return true;
  }

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  if (url.SchemeIsFile() && (setting.file_access_status != FileAccessType::kFileAccessEmpty)) {
    if (setting.file_access_status == FileAccessType::kFileAccessBlock) {
      LOG(WARNING) << "Blocked by file access list";
      return true;
    }
    return false;
  }
#endif  // BUILDFLAG(ARKWEB_NETWORK_LOAD)

#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
  if (setting.file_access &&
      setting.disallow_sandbox_file_access_from_file_url) {
      LOG(WARNING) << "Blocked by sandbox file access from file url";
    return IsAppStorageSandboxUrl(url) && !IsSpecialFileUrl(url);
  }
#endif  // BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)

  // Part of implementation of NWebPreference.allowFileAccess.
#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  if (url.SchemeIsFile() && NetHelpers::ShouldBlockFileUrls(setting)) {
#else
  if (url.SchemeIsFile() && NetHelpers::ShouldBlockFileUrls()) {
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
    // Appdatas are always available.
    return !IsSpecialFileUrl(url);
  }

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  return setting.block_network && url.SchemeIs(url::kFtpScheme);
#else
  return NetHelpers::is_network_blocked && url.SchemeIs(url::kFtpScheme);
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
}

int UpdateLoadFlags(int load_flags
#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
,
                    NetHelperSetting setting
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
) {
#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  if (setting.block_network) {
#else
  if (NetHelpers::is_network_blocked) {
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
    LOG(INFO) << "Update cache control flag to block network.";
    return UpdateCacheLoadFlags(
        load_flags,
        net::LOAD_ONLY_FROM_CACHE | net::LOAD_SKIP_CACHE_VALIDATION);
  }

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  if (!setting.cache_mode) {
#else
  if (!NetHelpers::cache_mode) {
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
    return load_flags;
  }

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  return UpdateCacheLoadFlags(load_flags, setting.cache_mode);
#else
  return UpdateCacheLoadFlags(load_flags, NetHelpers::cache_mode);
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
}

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
void NetHelpers::SetHostIP(const std::string host_name,
                           const std::string address,
                           int32_t ttl) {
  if (host_name != "" && address != "") {
    auto it = NetHelpers::custom_dns.find(host_name);
    if (it != NetHelpers::custom_dns.end()) {
      it->second.address.push_back(address);
      it->second.ttl = ttl;
    } else {
      CustomDnsEntry node;
      node.address.push_back(address);
      node.ttl = ttl;
      NetHelpers::custom_dns.insert(
          std::pair<std::string, CustomDnsEntry>(host_name, node));
    }
  }
}

std::map<std::string, struct CustomDnsEntry> NetHelpers::GetHostIP() {
  return NetHelpers::custom_dns;
}

std::vector<std::string> NetHelpers::GetHostIP(const std::string host_name) {
  std::vector<std::string> dns = {};
  if (host_name != "") {
    auto it = NetHelpers::custom_dns.find(host_name);
    if (it != NetHelpers::custom_dns.end()) {
      return it->second.address;
    }
  }
  return dns;
}

void NetHelpers::ClearHostIP(const std::string host_name) {
  if (host_name != "") {
    NetHelpers::custom_dns.erase(host_name);
  }
}

void NetHelpers::ClearHostIP() {
  NetHelpers::custom_dns.clear();
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
bool NetHelpers::ShouldAllowInsecurePrivateNetworkRequests() {
  bool allow = false;
  std::lock_guard<std::mutex> lock(enable_private_network_check_mutex);
  if (enable_private_network_check.has_value()) {
    allow = !enable_private_network_check.value();
  }
  return allow;
}

void NetHelpers::SetPrivateNetworkAccess(bool enable) {
  std::lock_guard<std::mutex> lock(enable_private_network_check_mutex);
  enable_private_network_check = enable;
}

bool NetHelpers::GetPrivateNetworkAccess() {
  std::lock_guard<std::mutex> lock(enable_private_network_check_mutex);
  return enable_private_network_check.value_or(true);
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_SERVICE)
int32_t NetHelpers::GetDefaultSocketIdleTimeout() {
  return kDefaultSocketIdleTimeout;
}

void NetHelpers::SetSocketIdleTimeout(int32_t timeout) {
  socket_idle_timeout = timeout;
}

int32_t NetHelpers::GetSocketIdleTimeout() {
  int32_t timeout = kDefaultSocketIdleTimeout;
  if (socket_idle_timeout != kDefaultSocketIdleTimeout) {
      timeout = socket_idle_timeout;
  } else {
      const base::CommandLine *command_line = base::CommandLine::ForCurrentProcess();
      if (command_line && command_line->HasSwitch(::switches::kSocketIdleTimeout)) {
          int time;
          if (base::StringToInt(command_line->GetSwitchValueASCII(::switches::kSocketIdleTimeout), &time)) {
              timeout = static_cast<int32_t>(time);
          }
      }
  }
  return timeout;
}
#endif

}  // namespace net_service

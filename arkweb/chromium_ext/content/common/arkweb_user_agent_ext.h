// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CONTENT_COMMON_ARKWEB_USER_AGENT_EXT_H_
#define CONTENT_COMMON_ARKWEB_USER_AGENT_EXT_H_

#include "content/public/common/user_agent.h"

#include <stdint.h>

#include "arkweb/build/features/features.h"
#include "base/containers/contains.h"
#include "base/logging.h"
#include "base/strings/strcat.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "build/buildflag.h"
#include "build/build_config.h"
#include "build/util/chromium_git_revision.h"

#if BUILDFLAG(IS_MAC)
#include "base/mac/mac_util.h"
#endif

#if BUILDFLAG(IS_IOS)
#include "ui/base/device_form_factor.h"
#endif

#if BUILDFLAG(IS_WIN)
#include "base/win/windows_version.h"
#elif BUILDFLAG(IS_POSIX) && !BUILDFLAG(IS_MAC)
#include <sys/utsname.h>
#endif

#if BUILDFLAG(ARKWEB_USERAGENT)
#include <atomic>

#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/command_line.h"
#include "components/embedder_support/arkweb_version.h"
#include "content/public/common/content_switches.h"
#include "third_party/blink/public/common/user_agent/user_agent_brand_version_type.h"
#include "third_party/blink/public/common/user_agent/user_agent_metadata.h"
#include "third_party/bounds_checking_function/include/securec.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

namespace content {
#if BUILDFLAG(ARKWEB_TEST)
extern std::atomic<bool> is_compatible_type_setted;
extern void SetArkwebUserAgentExtStateForTest(bool);
extern void ResetArkwebUserAgentExtStateForTest();
#endif
#if BUILDFLAG(ARKWEB_USERAGENT)
std::string GetDistVersion();
std::string GetOhosFullname();
void SetProductString(std::string& user_agent);
void UpdateHighEntropyCh(blink::UserAgentMetadata& metadata);
void UpdateLowEntropyCh(blink::UserAgentMetadata& metadata);

CONTENT_EXPORT std::string GetUserAgentByHost(const std::string& host);
#endif

}  // namespace content

#endif  // CONTENT_COMMON_ARKWEB_USER_AGENT_EXT_H_
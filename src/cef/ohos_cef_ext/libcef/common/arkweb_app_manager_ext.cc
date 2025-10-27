// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cef/libcef/common/app_manager.h"
#include "cef/ohos_cef_ext/libcef/common/arkweb_app_manager_ext.h"

#include "base/command_line.h"
#include "base/logging.h"
#include "cef/libcef/common/net/scheme_info.h"
#include "cef/libcef/common/scheme_registrar_impl.h"
#include "content/public/browser/child_process_security_policy.h"
#include "content/public/common/content_switches.h"

#if BUILDFLAG(ARKWEB_CUSTOM_SCHEME_CODECACHE)
#include "libcef/common/net/scheme_registration.h"
#endif

#if BUILDFLAG(IS_WIN)
#include <windows.h>

#include "base/path_service.h"
#endif

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
#include "base/json/json_reader.h"
#include "base/values.h"
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
std::vector<std::string> ArkWebAppManagerExt::CustomSchemeCmdLineSplit(
    std::string str,
    const char split) {
  std::istringstream inStream(str);
  std::vector<std::string> ret;
  std::string token;
  while (getline(inStream, token, split)) {
    if (!token.empty()) {
      ret.push_back(token);
      token.clear();
    }
  }
  return ret;
}

void ArkWebAppManagerExt::RenderAddCustomSchemes() {
  CefRefPtr<CefCommandLine> cmdLine = CefCommandLine::GetGlobalCommandLine();
  if (cmdLine->HasSwitch(switches::kProcessType)) {
    LOG(INFO) << "render Add custom schemes";
    if (cmdLine->HasSwitch(switches::kOhosCustomScheme)) {
      std::string cmdlineSchemes =
          cmdLine->GetSwitchValue(switches::kOhosCustomScheme).ToString();
      LOG(INFO) << "render cmdlineScheme:" << cmdlineSchemes;
      std::vector<std::string> schemesInfo =
          CustomSchemeCmdLineSplit(cmdlineSchemes, ';');
      for (auto it = schemesInfo.begin(); it != schemesInfo.end(); ++it) {
        std::string schemeSplit = *it;
        std::vector<std::string> scheme =
            CustomSchemeCmdLineSplit(schemeSplit, ',');
        if (scheme.size() != 3) {
          break;
        }
        CefSchemeInfo regScheme = {"",
                                   false,
                                   false,
                                   false,
                                   false,
                                   false,
                                   false,
                                   false
#if BUILDFLAG(ARKWEB_NETWORK_BASE) || BUILDFLAG(ARKWEB_CUSTOM_SCHEME_CODECACHE)
                                   ,
                                   false
#endif
        };
        regScheme.scheme_name = scheme[0];
        if (scheme[1] == std::string("1")) {
          regScheme.is_cors_enabled = true;
        }
        if (scheme[2] == std::string("1")) {
          regScheme.is_fetch_enabled = true;
        }
        LOG(INFO) << "render add custom scheme name:" << regScheme.scheme_name
                  << " is_cors:" << regScheme.is_cors_enabled
                  << " is_fetch:" << regScheme.is_fetch_enabled;
        AddCustomScheme(&regScheme);
      }
    }
  }
}

void ArkWebAppManagerExt::AddSchemeCodeCache(const CefSchemeInfo* scheme_info) {
  if (!scheme_info) {
    LOG(INFO) << "Scheme_info is null.";
    return;
  }
  if (scheme_info->is_code_cache_enabled) {
    LOG(DEBUG) << "App manager register the scheme:"
               << scheme_info->scheme_name.c_str() << " supported code cache.";
    scheme::AddSchemesSupportCodeCache(scheme_info->scheme_name);
  }
}

void ArkWebAppManagerExt::SchemeHandlerAddCustomScheme(content::ContentClient::Schemes* schemes) {
  CefSchemeRegistrarImpl schemeRegistrar;
  base::CommandLine* cmdLine = base::CommandLine::ForCurrentProcess();
  if (cmdLine && cmdLine->HasSwitch(switches::kOhSchemeHandlerCustomScheme)) {
    std::string cmdlineSchemes =
        cmdLine->GetSwitchValueASCII(switches::kOhSchemeHandlerCustomScheme);
    LOG(INFO) << "scheme_handler cmdlineScheme:" << cmdlineSchemes;
    std::optional<base::Value> schemeInfos =
        base::JSONReader::Read(cmdlineSchemes);
    if (schemeInfos && schemeInfos->is_dict()) {
      for (auto kv : schemeInfos->GetDict()) {
        schemeRegistrar.AddCustomScheme(kv.first, kv.second.GetInt());
      }
    }
  }
  schemeRegistrar.GetSchemes(schemes);
}

#endif  // BUILDFLAG(ARKWEB_NETWORK_LOAD)

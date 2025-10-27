// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef ARKWEB_LIBCEF_COMMON_APP_MANAGER_EXT_H_
#define ARKWEB_LIBCEF_COMMON_APP_MANAGER_EXT_H_
#pragma once

#include <list>

#include "arkweb/build/features/features.h"
#include "base/functional/callback.h"
#include "build/build_config.h"
#include "cef/include/cef_app.h"
#include "cef/include/cef_request_context.h"
#include "content/public/common/content_client.h"

class CefBrowserContext;
struct CefSchemeInfo;

// Exposes global application state in the main and render processes.
class ArkWebAppManagerExt : public CefAppManager {
 public:
  friend CefAppManager;
  ArkWebAppManagerExt *AsArkWebAppManagerExt() override { return this; }
  ArkWebAppManagerExt(const ArkWebAppManagerExt&) = delete;
  ArkWebAppManagerExt& operator=(const ArkWebAppManagerExt&) = delete;
  void AddSchemeCodeCache(const CefSchemeInfo* scheme_info);
  void SchemeHandlerAddCustomScheme(content::ContentClient::Schemes* schemes);

 private:
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  std::vector<std::string> CustomSchemeCmdLineSplit(std::string str,
                                                    const char split);
  void RenderAddCustomSchemes();
#endif
};

#endif  // ARKWEB_LIBCEF_COMMON_APP_MANAGER_EXT_H_

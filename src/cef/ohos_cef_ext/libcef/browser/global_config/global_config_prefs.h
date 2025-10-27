// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
 
#ifndef CEF_LIBCEF_BROWSER_GLOBAL_CONFIG_GLOBAL_CONFIG_PREFS_H_
#define CEF_LIBCEF_BROWSER_GLOBAL_CONFIG_GLOBAL_CONFIG_PREFS_H_
 
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"
 
namespace global_config {
#if BUILDFLAG(IS_ARKWEB_EXT)
  extern const char kGlobalConfigFeaturesSwitches[];
 
  void RegisterGlobalConfigPrefs(PrefRegistrySimple* registry);
 
  bool OnGlobalConfigResult(const std::string& path, PrefService* localState);
#endif
}
#endif
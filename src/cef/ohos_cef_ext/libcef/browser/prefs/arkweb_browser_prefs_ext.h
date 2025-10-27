// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_PREFS_ARKWEB_BROWSER_PREFS_H_
#define CEF_LIBCEF_BROWSER_PREFS_ARKWEB_BROWSER_PREFS_H_

#include <memory>

#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_CLOUD_CONTROL)
#include "components/pref_registry/pref_registry_syncable.h"
#include "components/prefs/pref_name_set.h"
#endif

class CefBrowserContext;
class CefBrowserHostBase;
class PrefRegistrySimple;
class PrefService;
class Profile;

namespace arkweb_browser_prefs_ext {

// Create the PrefService used to manage pref registration and storage.
// |profile| will be nullptr for the system-level PrefService. Used with the
// Alloy runtime only.
std::unique_ptr<PrefService> CreatePrefService(Profile* profile,
                                               const base::FilePath& cache_path,
                                               bool persist_user_preferences);
#if BUILDFLAG(ARKWEB_CLOUD_CONTROL)
void RegisterCloudControlProfilePrefs(
    user_prefs::PrefRegistrySyncable* registry);

void RegisterCloudControlPersistentPrefs(PrefNameSet& pref_name_set);
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
void RegisterSubresourceFilterPersistentPrefs(PrefNameSet& pref_name_set);
void RegisterUserSubresourceFilterPersistentPrefs(PrefNameSet& pref_name_set);
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

void RegisterProfilePrefs(user_prefs::PrefRegistrySyncable* registry);
}  // namespace arkweb_browser_prefs_ext

#endif  // CEF_LIBCEF_BROWSER_PREFS_ARKWEB_BROWSER_PREFS_H_

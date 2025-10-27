/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#if BUILDFLAG(ARKWEB_PREFS)
#include "cef/ohos_cef_ext/libcef/browser/prefs/arkweb_browser_prefs_ext.h"
#include "chrome/browser/prefs/pref_service_syncable_util.h"
#include "components/sync_preferences/pref_service_syncable.h"
#endif  // ARKWEB_PREFS

#if BUILDFLAG(ARKWEB_EXT_UA)
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#include "ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_ua_config.h"
#endif

#if BUILDFLAG(ARKWEB_USERAGENT)
#include "cef/ohos_cef_ext/libcef/browser/useragent/ua_push_config.h"
#endif

#if BUILDFLAG(ARKWEB_CLOUD_CONTROL) && BUILDFLAG(IS_ARKWEB_EXT)
#include "ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_engine_global_config.h"
#endif

namespace browser_prefs {

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
const char kMigratePasswordsReady[] = "migrate_passwords_ready";
const char kMigratePasswordsToPasswordVault[] =
    "migrate_passwords_to_password_vault";
const char kMigrationCount[] = "migration_count";
const char kMigrationDataBackupCompletion[] = "migration_data_backup_completion";
const char kMigrationQueryAssetfailure[] = "migration_query_asset_failure";
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
void RegisterMigratePasswordsPrefs(PrefRegistrySimple* registry) {
  registry->RegisterBooleanPref(browser_prefs::kMigratePasswordsReady, false);
  registry->RegisterBooleanPref(browser_prefs::kMigratePasswordsToPasswordVault,
                                false);
  registry->RegisterIntegerPref(browser_prefs::kMigrationCount, 0);
  registry->RegisterBooleanPref(browser_prefs::kMigrationDataBackupCompletion, false);
  registry->RegisterBooleanPref(browser_prefs::kMigrationQueryAssetfailure, false);
}
#endif

void UpdateCloudUAConfigAfterBrowserContextInitForInclude(Profile* profile) {
#if BUILDFLAG(ARKWEB_EXT_UA)
  auto* prefs = profile->GetPrefs();
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExUa)) {
    nweb_ex::AlloyBrowserUAConfig::GetInstance()->Init(prefs);
    std::string path;
    uint64_t version = 0;
    (void)nweb_ex::AlloyBrowserUAConfig::GetInstance()
        ->ReadCloudConfigInfoFromPrefs(path, version);
    nweb_ex::AlloyBrowserUAConfig::GetInstance()
        ->UpdateCloudUAConfigAfterBrowserContextInit(path, version);
  }
#endif
}

#if BUILDFLAG(ARKWEB_CLOUD_CONTROL) && BUILDFLAG(IS_ARKWEB_EXT)
void UpdateBrowserEngineGlobalConfigAfterBrowserContextInitForInclude(Profile* profile) {
  auto* prefs = profile->GetPrefs();
  nweb_ex::AlloyBrowserEngineGlobalConfig::GetInstance()->Init(prefs);
  std::string path;
  uint64_t version = 0;
  (void)nweb_ex::AlloyBrowserEngineGlobalConfig::GetInstance()
      ->ReadCloudConfigInfoFromPrefs(path, version);
  nweb_ex::AlloyBrowserEngineGlobalConfig::GetInstance()
      ->UpdateBrowserEngineGlobalConfigAfterBrowserContextInit(path, version);
}
#endif

void RegisterProfilePrefsForInclude(Profile* profile) {
#if BUILDFLAG(ARKWEB_PREFS)
  auto* pref_service_syncable = PrefServiceSyncableFromProfile(profile);
  arkweb_browser_prefs_ext::RegisterProfilePrefs(
      pref_service_syncable->GetPrefRegistrySyncable());
#endif  // ARKWEB_PREFS
}

}  // namespace browser_prefs

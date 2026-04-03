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

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_HAP_DECOMPRESSED)
#include "base/command_line.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "ohos_nweb/src/nweb_common.h"
#endif

namespace ui {

static std::unordered_set<std::string> supportLocaleList = {
  "resources",
  "chrome_100_percent",
  "chrome_200_percent",
#ifdef WEARABLE_SUPPORT_MULTI_LANGUAGE
  "ar",
  "be",
  "bg",
  "bn",
  "cs",
  "da",
  "de",
  "el",
  "en-GB",
  "en-US",
  "es",
  "es-419",
  "et",
  "fa",
  "fi",
  "fil",
  "fr",
  "he",
  "hi",
  "hr",
  "hu",
  "id",
  "it",
  "ja",
  "ka",
  "kk",
  "km",
  "ko",
  "lt",
  "lv",
  "mk",
  "ms",
  "my",
  "nb",
  "nl",
  "pl",
  "pt-BR",
  "pt-PT",
  "ro",
  "ru",
  "sk",
  "sl",
  "sr",
  "sv",
  "th",
  "tr",
  "uk",
  "uz",
  "vi",
  "zh-CN",
  "zh-TW",
  "zh-HK",
#else
  "ar",
  "bo-CN",
  "de",
  "en-GB",
  "en-US",
  "es",
  "es-419",
  "fr",
  "id",
  "it",
  "ja",
  "ko",
  "lo",
  "ms",
  "my",
  "pl",
  "pt-BR",
  "pt-PT",
  "ru",
  "th",
  "tr",
  "ug",
  "vi",
  "zh-CN",
  "zh-TW",
  "zh-HK",
#endif
};

#if BUILDFLAG(ARKWEB_HAP_DECOMPRESSED)
bool LocaleDataPakExistsExt(const std::string& locale) {
  if (locale.empty()) {
    return false;
  }
  const auto path = ResourceBundle::GetLocaleFilePath(locale);
  // If the hap package is not decompressed, the directory does not exist.
  ScopedAllowBlockingForNwebInit allow_blocking_for_using_path;
  if (path.empty() || !base::PathExists(path)) {
    std::string pakLocale;
    if (supportLocaleList.count(locale) > 0) {
      return true;
    } else if (!l10n_util::CheckAndResolveLocale(locale, &pakLocale, false)) {
      LOG(ERROR) << "CheckAndResolveLocale false, locale:" << locale;
      return false;
    }
    return supportLocaleList.count(pakLocale) > 0;
  } else {
    return true;
  }
}
#endif

// static
base::FilePath ResourceBundle::GetLocaleFilePath(
    const std::string& locale) {
  if (locale.empty()) {
    return base::FilePath();
  }
  std::string pakLocale;
  if (supportLocaleList.count(locale) > 0) {
    pakLocale = locale;
  } else if (!l10n_util::CheckAndResolveLocale(locale, &pakLocale, false)) {
    LOG(ERROR) << "CheckAndResolveLocale false, locale:" << locale;
    return base::FilePath();
  }

  base::FilePath locale_file_path;
  if (base::PathService::Get(base::DIR_ASSETS, &locale_file_path)) {
    locale_file_path = locale_file_path.AppendASCII(
        std::string("locales/") + pakLocale + ".pak");
  }

  // Note: The delegate GetPathForLocalePack() override is currently only used
  // by CastResourceDelegate, which does not call this function prior to
  // initializing the ResourceBundle. This called earlier than that by the
  // variations code which also has a CHECK that an inconsistent value does not
  // get returned via VariationsService::EnsureLocaleEquals().
  if (HasSharedInstance() && GetSharedInstance().delegate_) {
    locale_file_path = GetSharedInstance().delegate_->GetPathForLocalePack(
        locale_file_path, pakLocale);
  }

  LOG(INFO) << "GetLocaleFilePath pakLocale:" << pakLocale;
  // Don't try to load from paths that are not absolute.
  return locale_file_path.IsAbsolute() ? locale_file_path : base::FilePath();
}

}  // namespace ui

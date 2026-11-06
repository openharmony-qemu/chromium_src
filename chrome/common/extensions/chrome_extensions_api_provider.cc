// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/common/extensions/chrome_extensions_api_provider.h"

#include <string_view>

#include "chrome/common/extensions/api/api_features.h"
#include "chrome/common/extensions/api/generated_schemas.h"
#include "chrome/common/extensions/api/manifest_features.h"
#include "chrome/common/extensions/api/permission_features.h"
#include "chrome/common/extensions/chrome_manifest_handlers.h"
#include "chrome/common/extensions/permissions/chrome_api_permissions.h"
#include "chrome/grit/common_resources.h"
#include "extensions/common/features/json_feature_provider_source.h"
#include "extensions/common/permissions/permissions_info.h"

#if defined(IS_ARKWEB_TRUE)
#include "build/build_config.h"
#endif  // IS_ARKWEB_TRUE

#if BUILDFLAG(IS_ARKWEB) && BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/build/features/features.h"
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "arkweb/chromium_ext/chrome/common/extensions/api/generated_schemas.h"
#endif

namespace extensions {

ChromeExtensionsAPIProvider::ChromeExtensionsAPIProvider() = default;
ChromeExtensionsAPIProvider::~ChromeExtensionsAPIProvider() = default;

void ChromeExtensionsAPIProvider::AddAPIFeatures(FeatureProvider* provider) {
  AddChromeAPIFeatures(provider);
}

void ChromeExtensionsAPIProvider::AddManifestFeatures(
    FeatureProvider* provider) {
  AddChromeManifestFeatures(provider);
}

void ChromeExtensionsAPIProvider::AddPermissionFeatures(
    FeatureProvider* provider) {
  AddChromePermissionFeatures(provider);
}

void ChromeExtensionsAPIProvider::AddBehaviorFeatures(
    FeatureProvider* provider) {
  // Note: No chrome-specific behavior features.
}

void ChromeExtensionsAPIProvider::AddAPIJSONSources(
    JSONFeatureProviderSource* json_source) {
  json_source->LoadJSON(IDR_CHROME_EXTENSION_API_FEATURES);
}

bool ChromeExtensionsAPIProvider::IsAPISchemaGenerated(
    const std::string& name) {
#if BUILDFLAG(ARKWEB_EXT_ACTOR)
  if (api::ChromeGeneratedSchemas::IsGenerated(name)) {
    return true;
  }
  return api::ArkWebGeneratedSchemas::IsGenerated(name);
#else
  return api::ChromeGeneratedSchemas::IsGenerated(name);
#endif
}

std::string_view ChromeExtensionsAPIProvider::GetAPISchema(
    const std::string& name) {
#if BUILDFLAG(ARKWEB_EXT_ACTOR)
  std::string_view schema = api::ChromeGeneratedSchemas::Get(name);
  if (!schema.empty()) {
    return schema;
  }
  return api::ArkWebGeneratedSchemas::Get(name);
#else
  return api::ChromeGeneratedSchemas::Get(name);
#endif
}

void ChromeExtensionsAPIProvider::RegisterPermissions(
    PermissionsInfo* permissions_info) {
  permissions_info->RegisterPermissions(
      chrome_api_permissions::GetPermissionInfos(),
      chrome_api_permissions::GetPermissionAliases());
}

void ChromeExtensionsAPIProvider::RegisterManifestHandlers() {
  RegisterChromeManifestHandlers();
}

}  // namespace extensions

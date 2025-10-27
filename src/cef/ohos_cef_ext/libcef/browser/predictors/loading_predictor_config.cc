// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_cef_ext/libcef/browser/predictors/loading_predictor_config.h"

namespace ohos_predictors {

// Returns whether the speculative preconnect feature is enabled.
bool IsPreconnectFeatureEnabled() {
  return true;
}

bool IsLoadingPredictorEnabled(content::BrowserContext* context) {
  return IsPreconnectFeatureEnabled();
}

bool IsPreconnectAllowed(content::BrowserContext* context) {
  if (!IsPreconnectFeatureEnabled()) {
    return false;
  }

  return true;
}

LoadingPredictorConfig::LoadingPredictorConfig()
    : max_navigation_lifetime_seconds(60),
      max_hosts_to_track(100),
      max_origins_per_entry(50),
      max_consecutive_misses(3),
      max_redirect_consecutive_misses(5),
      flush_data_to_disk_delay_seconds(30) {}

LoadingPredictorConfig::LoadingPredictorConfig(
    const LoadingPredictorConfig& other) = default;

LoadingPredictorConfig::~LoadingPredictorConfig() = default;

}  // namespace ohos_predictors

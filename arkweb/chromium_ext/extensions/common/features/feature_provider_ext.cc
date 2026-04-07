/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#include "base/logging.h"
#include "extensions/common/extension_urls.h"
#include "extensions/common/features/feature.h"
#include "extensions/common/features/feature_provider.h"
 
namespace extensions {
 
void FeatureProvider::UpdateFeature(const std::string& name,
                                    const std::string& url,
                                    std::string_view pattern) const {
  auto feature = features_.find(name);
  if (feature != features_.end()) {
    feature->second->add_matches(GURL(url), pattern);
  }
}
 
void FeatureProvider::UpdateFeatures() {
  LOG(INFO) << "begin to update feature";
  auto features = FeatureProvider::GetAPIFeatures();
  if (features) {
    auto url = GURL(extension_urls::kNewChromeWebstoreBaseURL);
    auto pattern = extension_urls::GetDefaultWebstoreLaunchURLPattern();
    features->UpdateFeature("webstorePrivate", url.spec(), pattern.spec());
  }
}
 
}  // namespace extensions
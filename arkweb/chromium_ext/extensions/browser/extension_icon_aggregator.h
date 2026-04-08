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

#ifndef EXTENSIONS_BROWSER_EXTENSION_ICON_AGGREGATOR_H_
#define EXTENSIONS_BROWSER_EXTENSION_ICON_AGGREGATOR_H_

#include <map>

#include "base/functional/callback.h"
#include "base/scoped_multi_source_observation.h"
#include "extensions/browser/extension_icon_image.h"
#include "ohos_nweb/src/capi/nweb_extension_action_icon.h"

namespace extensions {

class ExtensionIconAggregator : public IconImage::Observer {
 public:
  using IconReadyCallback = base::OnceCallback<void(NWebExtensionActionIconV2)>;

  ExtensionIconAggregator();
  explicit ExtensionIconAggregator(float icon_scale);
  ~ExtensionIconAggregator() override;

  ExtensionIconAggregator(const ExtensionIconAggregator&) = delete;
  ExtensionIconAggregator& operator=(const ExtensionIconAggregator&) = delete;

  void AddIcon(IconImage* icon_image, IconReadyCallback callback);

  void SetOnAllReadyCallback(base::OnceClosure callback);

  void StartLoading();

 private:
  void OnExtensionIconImageChanged(IconImage* image) override;

  void TryTriggerAllReady();

  std::map<IconImage*, IconReadyCallback> pending_icons_;
  base::ScopedMultiSourceObservation<IconImage, IconImage::Observer>
      icon_observations_{this};
  base::OnceClosure on_all_ready_callback_;
  bool started_ = false;
  float icon_scale_ = 1.0f;
};

}  // namespace extensions

#endif  // EXTENSIONS_BROWSER_EXTENSION_ICON_AGGREGATOR_H_

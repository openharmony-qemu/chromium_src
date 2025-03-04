/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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


#ifndef EXTENSIONS_BROWSER_EXTENSION_ICON_IMAGE_OBSERVER_H_
#define EXTENSIONS_BROWSER_EXTENSION_ICON_IMAGE_OBSERVER_H_

#include "extensions/browser/extension_icon_image.h"
#include "ui/gfx/image/image_skia.h"

namespace extensions {
 
class ExtensionIconImageObserver : public IconImage::Observer {
 public:
  ExtensionIconImageObserver(const std::string& extension_id, int32_t tab_id);

  ExtensionIconImageObserver(const ExtensionIconImageObserver&) = delete;
  ExtensionIconImageObserver& operator=(const ExtensionIconImageObserver&) = delete;

  ~ExtensionIconImageObserver() override;

  void OnExtensionIconImageChanged(IconImage* icon) override;
 private:
  std::string extension_id_;
  int32_t tab_id_;
};

void SetExtensionIconImageObserver(const std::string& extension_id, int32_t tab_id);
ExtensionIconImageObserver* GetExtensionIconImageObserver(const std::string& extension_id);
void DestroyExtensionIconImageObserver(const std::string& extension_id);

}
 
#endif  // EXTENSIONS_BROWSER_EXTENSION_ICON_IMAGE_OBSERVER_H_
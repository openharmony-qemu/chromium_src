/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NAVIGATION_STATE_SERIALIZER_H
#define NAVIGATION_STATE_SERIALIZER_H
#pragma once

#include <memory>
#include <vector>

#include "base/pickle.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/navigation_entry_restore_context.h"
#include "content/public/browser/web_contents.h"
#include "include/cef_values.h"

class NavigationStateSerializer {
 public:
  static CefRefPtr<CefBinaryValue> WriteNavigationStatus(
      content::WebContents& web_contents);
  static bool RestoreNavigationStatus(content::WebContents& web_contents,
                                      const CefRefPtr<CefBinaryValue>& state);

 private:
  static void WriteNavigationEntry(content::NavigationEntry& entry,
                                   base::Pickle* pickle);
  static bool RestoreNavigationEntry(
      base::PickleIterator* iterator,
      std::unique_ptr<content::NavigationEntry>& entry,
      content::NavigationEntryRestoreContext* context);
};

#endif

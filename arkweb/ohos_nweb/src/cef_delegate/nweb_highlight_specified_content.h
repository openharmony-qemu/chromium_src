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

#ifndef NWEB_HIGHLIGHT_SPECIFIED_CONTENT_H
#define NWEB_HIGHLIGHT_SPECIFIED_CONTENT_H

#include "nweb_delegate.h"

namespace OHOS::NWeb {

class NWebHighlightSpecifiedContent {
 public:
  explicit NWebHighlightSpecifiedContent(
      base::WeakPtr<NWebDelegate> nweb_delegate);
  void SetHighlightSpecifiedContentEnable(bool enable);

 private:
  FRIEND_TEST_ALL_PREFIXES(NWebHighlightSpecifiedContentTest, SetHighlightSpecifiedContentEnable_NoDelegate);
  FRIEND_TEST_ALL_PREFIXES(NWebHighlightSpecifiedContentTest, SetHighlightSpecifiedContentEnable_001);
  FRIEND_TEST_ALL_PREFIXES(NWebHighlightSpecifiedContentTest, SetHighlightSpecifiedContentEnable_002);
  FRIEND_TEST_ALL_PREFIXES(NWebHighlightSpecifiedContentTest, RegisterHighlight_NoDelegate);
  FRIEND_TEST_ALL_PREFIXES(NWebHighlightSpecifiedContentTest, RegisterHighlight_001);
  FRIEND_TEST_ALL_PREFIXES(NWebHighlightSpecifiedContentTest, RegisterHighlight_002);
  FRIEND_TEST_ALL_PREFIXES(NWebHighlightSpecifiedContentTest, RemoveHighlightFunctions_NoDelegate);
  FRIEND_TEST_ALL_PREFIXES(NWebHighlightSpecifiedContentTest, RemoveHighlightFunctions_001);
  FRIEND_TEST_ALL_PREFIXES(NWebHighlightSpecifiedContentTest, RemoveHighlightFunctions_002);
  void RegisterHighlight();
  void RemoveHighlightFunctions();

  bool enable_ = false;

  base::WeakPtr<NWebDelegate> nweb_delegate_;
  base::WeakPtrFactory<NWebHighlightSpecifiedContent> weak_factory_;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_HIGHLIGHT_SPECIFIED_CONTENT_H
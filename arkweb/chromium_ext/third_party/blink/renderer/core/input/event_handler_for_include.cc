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

namespace blink {

void SendContextMenuEventExt(const WebMouseEvent& event,
                             HitTestRequest& request) {
  int hit_test_flag = 0;
  if (event.menu_source_type == kMenuSourceShowFreeCopyMenu) {
    hit_test_flag |= HitTestRequest::kReadOnly;
  } else {
    hit_test_flag |= HitTestRequest::kActive;
  }
  request = hit_test_flag;
}

void ShowNonLocatedContextMenuExt(WebMenuSourceType source_type,
                                  HitTestRequest& request) {
  HitTestRequest::HitTestRequestType hit_test_request_type = 0;
  if (source_type == kMenuSourceShowFreeCopyMenu) {
    hit_test_request_type |= HitTestRequest::kReadOnly;
  } else {
    hit_test_request_type |= HitTestRequest::kActive;
  }
  request = hit_test_request_type;
}

}  // namespace blink

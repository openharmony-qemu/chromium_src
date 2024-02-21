/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "nweb_load_committed_details_impl.h"

namespace OHOS::NWeb {
NWebLoadCommittedDetailsImpl::NWebLoadCommittedDetailsImpl(
    const std::string& current_url,
    NavigationType type,
    bool is_main_frame,
    bool is_same_document,
    bool did_replace_entry)
    : current_url_(current_url),
      type_(type),
      is_main_frame_(is_main_frame),
      is_same_document_(is_same_document),
      did_replace_entry_(did_replace_entry) {}

bool NWebLoadCommittedDetailsImpl::IsMainFrame() {
  return is_main_frame_;
}

bool NWebLoadCommittedDetailsImpl::IsSameDocument() {
  return is_same_document_;
}

bool NWebLoadCommittedDetailsImpl::DidReplaceEntry() {
  return did_replace_entry_;
}

NWebLoadCommittedDetails::NavigationType
NWebLoadCommittedDetailsImpl::GetNavigationType() {
  return type_;
}

std::string NWebLoadCommittedDetailsImpl::GetURL() {
  return current_url_;
}

}


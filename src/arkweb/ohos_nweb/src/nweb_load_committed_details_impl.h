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

#ifndef NWEB_LOAD_COMMITTED_DETAILS_IMPL_H
#define NWEB_LOAD_COMMITTED_DETAILS_IMPL_H

#include "nweb_load_committed_details.h"

namespace OHOS::NWeb {
class NWebLoadCommittedDetailsImpl : public NWebLoadCommittedDetails {
 public:
  NWebLoadCommittedDetailsImpl(const std::string& current_url,
                               NavigationType type,
                               bool is_main_frame,
                               bool is_same_document,
                               bool did_replace_entry);
  ~NWebLoadCommittedDetailsImpl() = default;

  bool IsMainFrame() override;
  bool IsSameDocument() override;
  bool DidReplaceEntry() override;
  NavigationType GetNavigationType() override;
  std::string GetURL() override;

 private:
  std::string current_url_;
  NavigationType type_;
  bool is_main_frame_;
  bool is_same_document_;
  bool did_replace_entry_;
};
}  // namespace OHOS::NWeb
#endif  // NWEB_LOAD_COMMITTED_DETAILS_IMPL_H

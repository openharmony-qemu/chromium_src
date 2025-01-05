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

#include "nweb_file_selector_params_impl.h"

#include "base/logging.h"

namespace OHOS::NWeb {
FileSelectorParamsImpl::FileSelectorParamsImpl(
    FileSelectorMode mode,
    const std::string& title,
    const std::vector<CefString>& accept_type,
    const std::string& default_filename,
    bool is_capture,
    const std::vector<CefString>& mime_type)
    : mode_(mode),
      title_(title),
      default_filename_(default_filename),
      is_capture_(is_capture) {
  for (auto& c : accept_type) {
    accept_type_.push_back(c.ToString());
  }
  for (auto& c : mime_type) {
    mime_type_.push_back(c.ToString());
  }
}

const std::string FileSelectorParamsImpl::Title() {
  return title_;
}

NWebFileSelectorParams::FileSelectorMode FileSelectorParamsImpl::Mode() {
  return mode_;
}

const std::string FileSelectorParamsImpl::DefaultFilename() {
  return default_filename_;
}

const AcceptTypeList FileSelectorParamsImpl::AcceptType() {
  return accept_type_;
}

bool FileSelectorParamsImpl::IsCapture() {
  return is_capture_;
}

const AcceptTypeList FileSelectorParamsImpl::MimeType() {
  return mime_type_;
}

void FileSelectorCallbackImpl::OnReceiveValue(const std::vector<std::string>& value) {
  if (callback_ == nullptr || is_used_) {
    LOG(ERROR) << "FileSelectorCallbackImpl is null or already used";
    return;
  }
  is_used_ = true;
  if (value.size() == 0) {
    callback_->Cancel();
  } else {
    for (auto& c : value) {
      if (c.empty()) {
        continue;
      }
      file_path_.push_back(CefString(c));
    }
    callback_->Continue(file_path_);
  }
}
}  // namespace OHOS::NWeb
                          
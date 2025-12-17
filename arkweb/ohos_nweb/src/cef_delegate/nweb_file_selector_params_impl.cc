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

#include "base/datashare_uri_utils.h"
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

FileSelectorParamsImpl::FileSelectorParamsImpl(
    FileSelectorMode mode,
    const std::string& title,
    const std::vector<CefString>& accept_type,
    const std::string& default_filename,
    bool is_capture)
    : mode_(mode),
      title_(title),
      default_filename_(default_filename),
      is_capture_(is_capture) {
  for (auto& c : accept_type) {
    accept_type_.push_back(c.ToString());
  }
}

FileSelectorParamsImpl::FileSelectorParamsImpl(
    FileSelectorMode mode,
    const std::string& title,
    const std::vector<CefString>& accept_type,
    const std::string& default_filename,
    bool is_capture,
    const std::vector<CefString>& mime_type,
    const std::string& accepts,
    const std::string& default_path,
    const std::vector<CefString>& descriptions,
    bool is_exclude_accept_all_options)
    : mode_(mode),
      title_(title),
      default_filename_(default_filename),
      is_capture_(is_capture),
      accepts_(accepts),
      default_path_(default_path),
      is_exclude_accept_all_options_(is_exclude_accept_all_options) {
  for (auto& c : accept_type) {
    accept_type_.push_back(c.ToString());
  }
  for (auto& c : mime_type) {
    mime_type_.push_back(c.ToString());
  }
  for (auto& c : descriptions) {
    descriptions_.push_back(c.ToString());
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

const std::string FileSelectorParamsImpl::DefaultPath() {
  return default_path_;
}
 
const DescriptionsList FileSelectorParamsImpl::Descriptions() {
  return descriptions_;
}
 
bool FileSelectorParamsImpl::IsAcceptAllOptionExcluded() {
  return is_exclude_accept_all_options_;
}

const std::string FileSelectorParamsImpl::Accepts() {
  return accepts_;
}

void FileSelectorCallbackImpl::OnReceiveValue(
    const std::vector<std::string>& value) {
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
      file_path_.push_back(base::GetRealPath(base::FilePath(c)));
    }
    callback_->Continue(file_path_);
  }
}

void FileSelectorCallbackImplNativeApi::OnReceiveValue(const char** value) {
  if (callback_ == nullptr || is_used_) {
    LOG(ERROR) << "NWebFileSelectorCallbackImplNew is null or already used";
    return;
  }
  std::vector<std::string> vec;
  ConverToVector(value, vec);

  is_used_ = true;
  if (vec.size() == 0) {
    callback_->Cancel();
  } else {
    for (auto& c : vec) {
      if (c.empty()) {
        continue;
      }
      file_path_.push_back(CefString(c));
    }
    callback_->Continue(file_path_);
  }
}

void FileSelectorCallbackImplNativeApi::ConverToVector(
    const char** arr,
    std::vector<std::string>& vec) {
  if (!arr) {
    return;
  }

  while (*arr) {
    vec.push_back(*arr);
    ++arr;
  }
}
}  // namespace OHOS::NWeb

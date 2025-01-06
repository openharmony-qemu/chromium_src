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

#ifndef NWEB_FILE_SELECTOR_PARAMS_IMPL_H
#define NWEB_FILE_SELECTOR_PARAMS_IMPL_H

#include <string>
#include <vector>
#include "cef_dialog_handler.h"
#include "nweb_file_selector_params.h"
#include "nweb_handler.h"

namespace OHOS::NWeb {
class FileSelectorParamsImpl : public NWebFileSelectorParams {
 public:
  FileSelectorParamsImpl(FileSelectorMode mode,
                         const std::string& title,
                         const std::vector<CefString>& accept_type,
                         const std::string& default_filename,
                         bool is_capture,
                         const std::vector<CefString>& mime_type);

  const std::string Title() override;
  NWebFileSelectorParams::FileSelectorMode Mode() override;
  const std::string DefaultFilename() override;
  const AcceptTypeList AcceptType() override;
  bool IsCapture() override;
  const MimeTypeList MimeType() override;

 private:
  NWebFileSelectorParams::FileSelectorMode mode_;
  std::string title_;
  std::string default_filename_;
  AcceptTypeList accept_type_;
  bool is_capture_;
  MimeTypeList mime_type_;
};

class FileSelectorCallbackImpl : public NWebStringVectorValueCallback {
 public:
  explicit FileSelectorCallbackImpl(CefRefPtr<CefFileDialogCallback> callback)
      : callback_(callback),
        file_path_(std::vector<CefString>()),
        is_used_(false) {}
  ~FileSelectorCallbackImpl() = default;

  void OnReceiveValue(const std::vector<std::string>& value) override;

 private:
  CefRefPtr<CefFileDialogCallback> callback_;
  std::vector<CefString> file_path_;
  bool is_used_;
};
}  // namespace OHOS::NWeb

#endif

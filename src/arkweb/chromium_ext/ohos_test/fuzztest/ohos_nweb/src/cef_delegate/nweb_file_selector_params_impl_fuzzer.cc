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

#include "ohos_nweb/src/cef_delegate/nweb_file_selector_params_impl.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstring>

#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"

using namespace OHOS::NWeb;

void FileSelectorParamsImplFuzzTest(FuzzedDataProvider* fdp) {
    const std::string title = fdp->ConsumeRandomLengthString(256);
    std::vector<CefString> accept_type;
    accept_type.push_back(fdp->ConsumeRandomLengthString(256));
    accept_type.push_back(fdp->ConsumeRandomLengthString(256));
    accept_type.push_back(fdp->ConsumeRandomLengthString(256));
    const std::string default_filename = fdp->ConsumeRandomLengthString(256);
    bool is_capture = fdp->ConsumeBool();
    std::vector<CefString> mime_type;
    mime_type.push_back(fdp->ConsumeRandomLengthString(256));
    mime_type.push_back(fdp->ConsumeRandomLengthString(256));
    mime_type.push_back(fdp->ConsumeRandomLengthString(256));
    FileSelectorParamsImpl impl(OHOS::NWeb::NWebFileSelectorParams::FILE_OPEN_MODE, title, accept_type, default_filename, is_capture, mime_type);

    impl.Title();

    impl.Mode();

    impl.DefaultFilename();

    impl.AcceptType();

    impl.IsCapture();

    impl.MimeType();
}

class MyFileDialogCallbackImpl : public CefFileDialogCallback {
public:
    void Continue(const std::vector<CefString>& file_paths) override {}

    void Cancel() override {}

    IMPLEMENT_REFCOUNTING(MyFileDialogCallbackImpl);
};

void FileSelectorCallbackImplFuzzTest(FuzzedDataProvider* fdp) {
  CefRefPtr<CefFileDialogCallback> callback = new MyFileDialogCallbackImpl();
  std::vector<CefString> files;
  files.push_back(fdp->ConsumeRandomLengthString(256));
  callback->Continue(files);
  FileSelectorCallbackImpl impl(callback);

  std::vector<std::string> value;
  value.push_back(fdp->ConsumeRandomLengthString(256));
  value.emplace_back(fdp->ConsumeRandomLengthString(256));
  value.emplace_back(fdp->ConsumeRandomLengthString(256));
  impl.OnReceiveValue(value);
}

void FileSelectorCallbackImplNativeApiFuzzTest(FuzzedDataProvider* fdp) {
  CefRefPtr<CefFileDialogCallback> callback = new MyFileDialogCallbackImpl();
  std::vector<CefString> files;
  files.push_back(fdp->ConsumeRandomLengthString(256));
  callback->Continue(files);
  FileSelectorCallbackImplNativeApi impl(callback);

  const char** value = new const char*[4];
  for (size_t i = 0; i < 3; ++i) {
    value[i] = (fdp->ConsumeRandomLengthString(256)).c_str();
  }
  value[3] = nullptr;
  impl.OnReceiveValue(value);
  delete[] value;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
   if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  FileSelectorParamsImplFuzzTest(&fdp);

  FileSelectorCallbackImplFuzzTest(&fdp);

  FileSelectorCallbackImplNativeApiFuzzTest(&fdp);
  
  return 0;
}

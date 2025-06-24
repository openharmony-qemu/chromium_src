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

#ifndef NWEB_REOURCE_HANDLER_H
#define NWEB_REOURCE_HANDLER_H

#include <condition_variable>
#include <mutex>
#include <string>
#include <vector>
#include "cef/include/cef_resource_handler.h"
#include "nweb_url_resource_response.h"

namespace OHOS::NWeb {
class NWebResourceHandler : public CefResourceHandler {
 public:
  NWebResourceHandler(std::shared_ptr<NWebUrlResourceResponse> web_response,
                      std::string& str);
  ~NWebResourceHandler() = default;

  /* CefResourceHandler method begin */
  bool Open(CefRefPtr<CefRequest> request,
            bool& handle_request,
            CefRefPtr<CefCallback> callback) override;

  bool Read(void* data_out,
            int bytes_to_read,
            int& bytes_read,
            CefRefPtr<CefResourceReadCallback> callback) override;

  void GetResponseHeaders(CefRefPtr<CefResponse> response,
                          int64& response_length,
                          CefString& redirectUrl) override;

  const std::string GetResponseData() override;

  size_t GetResponseDataBuffer(char* data) override;

  size_t GetResponseDataBufferSize() override;

#ifdef OHOS_NETWORK_LOAD
  bool Skip(int64 bytes_to_skip,
            int64& bytes_skipped,
            CefRefPtr<CefResourceSkipCallback> callback) override;
#endif
  void Cancel() override;
  /* CefResourceHandler method end */

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(NWebResourceHandler);

 private:
  bool ReadStringData(void* data_out, int bytes_to_read, int& bytes_read);
  bool ReadFileData(void* data_out, int bytes_to_read, int& bytes_read);
#ifdef OHOS_NETWORK_LOAD
  int64_t GetFileSizeByFd();
  bool ReadResourceDataByHap();
  bool ReadResourceData(void* data_out, int bytes_to_read, int& bytes_read);
#endif
  std::string data_;
  size_t offset_ = 0;
  std::shared_ptr<NWebUrlResourceResponse> response_;
#ifdef OHOS_NETWORK_LOAD
  std::unique_ptr<uint8_t[]> resource_data_;
  size_t resource_data_offset_ = 0;
  size_t resource_data_len_ = 0;
#endif
};
}  // namespace OHOS::NWeb

#endif

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

#ifndef OHOS_NWEB_SRC_NWEB_PIPE_RESOURCE_HANDLER_H
#define OHOS_NWEB_SRC_NWEB_PIPE_RESOURCE_HANDLER_H

#include <string>
#include <vector>

#include "base/memory/raw_ptr.h"
#include "cef/include/cef_resource_handler.h"
#include "net/base/io_buffer.h"
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"
#include "ohos_nweb/src/cef_delegate/nweb_scheme_handler_factory.h"
#include "url/gurl.h"

namespace OHOS::NWeb {

class NWebPipeResourceHandler : public CefResourceHandler {
 public:
  NWebPipeResourceHandler(
      const ArkWeb_ResourceRequest* resource_request,
      const ArkWeb_ResourceHandler* resource_handler,
      CefRefPtr<OHOS::NWeb::NWebSchemeHandlerFactory> factory,
      const std::string& web_tag,
      bool from_service_worker);
  ~NWebPipeResourceHandler() = default;

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

  void Cancel() override;

  bool Skip(int64 bytes_to_skip,
            int64& bytes_skipped,
            CefRefPtr<CefResourceSkipCallback> callback) override;
  /* CefResourceHandler method end */

  void DidReceiveResponse(CefRefPtr<CefResponse> response);
  void DidReceiveData(const uint8_t* buffer, int64_t buf_len);
  void DidFinish();
  void DidFailWithError(int error_code);

  void AddRef() const override { ref_count_.AddRef(); }
  bool Release() const override;
  bool HasOneRef() const override { return ref_count_.HasOneRef(); }
  bool HasAtLeastOneRef() const override {
    return ref_count_.HasAtLeastOneRef();
  }

 private:
  int UnSafeReadTrunkData(bool flush);
  int UnSafeReadTrunkData(void* data_out,
                          int bytes_to_read,
                          int& bytes_read,
                          bool flush);
  void CallOnRequestStop() const;

  bool finished_{false};
  bool finished_with_error_{false};
  bool remain_read_{false};
  bool canceled_{false};
  int error_code_{0};
  CefRefPtr<CefResponse> response_;
  CefRefPtr<CefCallback> response_ready_callback_;
  CefRefPtr<CefResourceReadCallback> resource_ready_callback_;
  void* last_data_out_{nullptr};
  int last_bytes_to_read_{0};
  scoped_refptr<net::GrowableIOBuffer> data_buffer_;
  raw_ptr<const ArkWeb_ResourceRequest> resource_request_ = nullptr;
  raw_ptr<const ArkWeb_ResourceHandler> resource_handler_ = nullptr;
  CefRefPtr<OHOS::NWeb::NWebSchemeHandlerFactory> factory_;
  std::string web_tag_;
  GURL url_;
  bool needs_sniff_mimetype_{false};
  bool from_service_worker_{false};
  mutable base::Lock lock_;
  CefRefCount ref_count_;

  // IMPLEMENT_REFCOUNTING(NWebPipeResourceHandler);
};

}  // namespace OHOS::NWeb

#endif  // OHOS_NWEB_SRC_NWEB_PIPE_RESOURCE_HANDLER_H

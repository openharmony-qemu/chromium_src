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

#ifndef OHOS_NWEB_SRC_CAPI_SCHEME_HANDLER_POST_DATA_STREAM_H
#define OHOS_NWEB_SRC_CAPI_SCHEME_HANDLER_POST_DATA_STREAM_H

#include "cef/include/cef_request.h"
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"

struct ArkWeb_PostDataStream_ : public CefPostDataStreamInitCallback,
                                CefPostDataStreamReadCallback {
  ArkWeb_PostDataStream_(const ArkWeb_ResourceRequest* resource_request);
  ~ArkWeb_PostDataStream_();

  void SetReadCallback(ArkWeb_PostDataReadCallback read_callback_in);
  void Init(ArkWeb_PostDataStreamInitCallback stream_init_callback_in);
  void Read(void* buffer, int64_t buf_len) const;
  int64_t GetSize() const;
  int64_t GetPosition() const;
  bool IsChunked() const;
  bool IsEOF() const;
  bool IsInMemory() const;
  void SetUserData(void* user_data_in);
  void* GetUserData() const;
  void OnInitComplete(int rv) override;
  void OnReadComplete(char* buffer, int bytes_read) override;

  ArkWeb_PostDataReadCallback read_callback{nullptr};
  ArkWeb_PostDataStreamInitCallback stream_init_callback{nullptr};
  CefRefPtr<CefPostDataStream> post_data_stream;
  void* user_data{nullptr};

  IMPLEMENT_REFCOUNTING(ArkWeb_PostDataStream_);
};

#endif  // OHOS_NWEB_SRC_CAPI_SCHEME_HANDLER_POST_DATA_STREAM_H

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

#include "ohos_nweb/src/ndk/scheme_handler/http_body_stream.h"

#include "base/logging.h"
#include "cef/include/cef_request.h"
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"

#include "cef/libcef/browser/thread_util.h"

ArkWeb_HttpBodyStream_::ArkWeb_HttpBodyStream_(
    const ArkWeb_ResourceRequest* resource_request) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return;
  }

  if (!resource_request->cef_request) {
    LOG(ERROR) << "scheme_handler cef_request is nullptr";
    return;
  }

  if (!resource_request->cef_request->GetUploadStream()) {
    LOG(ERROR) << "scheme_handler upload stream is nullptr";
    return;
  }

  post_data_stream = resource_request->cef_request->GetUploadStream();
}

ArkWeb_HttpBodyStream_::~ArkWeb_HttpBodyStream_() {}

void ArkWeb_HttpBodyStream_::SetReadCallback(
    ArkWeb_HttpBodyStreamReadCallback read_callback_in) {
  read_callback = read_callback_in;
}

void ArkWeb_HttpBodyStream_::Init(
    ArkWeb_HttpBodyStreamInitCallback stream_init_callback_in) {
  if (!CEF_CURRENTLY_ON_IOT()) {
    CEF_POST_TASK(CEF_IOT,
      base::BindOnce(&ArkWeb_HttpBodyStream_::Init,
        this, stream_init_callback_in));
    return;
  }

  this->stream_init_callback = stream_init_callback_in;
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return;
  }
 
  post_data_stream->Init(this);
}
 
void ArkWeb_HttpBodyStream_::SetUserData(void* user_data_in) {
  if (!user_data_in) {
    LOG(ERROR) << "scheme_handler set a nullptr.";
    return;
  }
  user_data = user_data_in;
}
 
void* ArkWeb_HttpBodyStream_::GetUserData() const {
  return user_data;
}
 
void ArkWeb_HttpBodyStream_::Read(void* buffer, int64_t buf_len) const {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return;
  }
 
  if (!buffer) {
    LOG(ERROR) << "scheme_hadnler read buffer is nullptr.";
  }
 
  post_data_stream->Read(buffer, buf_len,
                         const_cast<ArkWeb_HttpBodyStream*>(this));
}
 
int64_t ArkWeb_HttpBodyStream_::GetSize() const {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return -1;
  }
 
  return post_data_stream->GetSize();
}
 
int64_t ArkWeb_HttpBodyStream_::GetPosition() const {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return -1;
  }
 
  return post_data_stream->GetPosition();
}
 
bool ArkWeb_HttpBodyStream_::IsChunked() const {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return false;
  }
 
  return post_data_stream->IsChunked();
}
 
bool ArkWeb_HttpBodyStream_::IsEOF() const {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return false;
  }
 
  return post_data_stream->IsEOF();
}
 
bool ArkWeb_HttpBodyStream_::IsInMemory() const {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return false;
  }
 
  return post_data_stream->IsInMemory();
}
 
NO_SANITIZE("cfi-icall") void ArkWeb_HttpBodyStream_::OnInitComplete(int rv) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return;
  }
 
  if (!stream_init_callback) {
    LOG(ERROR) << "scheme_handler stream_init_callback is nullptr.";
    return;
  }

  if (!CEF_CURRENTLY_ON_UIT() && is_ets_) {
    CEF_POST_TASK(CEF_UIT,
      base::BindOnce(&ArkWeb_HttpBodyStream_::OnInitComplete,
        this, rv));
    return;
  }
 
  stream_init_callback(this, static_cast<ArkWeb_NetError>(rv));
}
 
NO_SANITIZE("cfi-icall") void ArkWeb_HttpBodyStream_::OnReadComplete(
    char* buffer, int bytes_read) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
  }
 
  if (!read_callback) {
    LOG(ERROR) << "scheme_handler read callback is nullptr.";
    return;
  }
  
  if (!CEF_CURRENTLY_ON_UIT() && is_ets_) {
    CEF_POST_TASK(CEF_UIT,
      base::BindOnce(&ArkWeb_HttpBodyStream_::OnReadComplete,
        this, buffer, bytes_read));
    return;
  }

  read_callback(this, reinterpret_cast<uint8_t*>(buffer), bytes_read);
}

void ArkWeb_HttpBodyStream_::Reset() {
  if (!post_data_stream) {
    return;
  }

  post_data_stream->Reset();
  post_data_stream->Release();
}

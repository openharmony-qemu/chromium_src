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

#include "ohos_nweb/src/ndk/scheme_handler/post_data_stream.h"

#include "base/logging.h"
#include "cef/include/cef_request.h"
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"

ArkWeb_PostDataStream_::ArkWeb_PostDataStream_(
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

ArkWeb_PostDataStream_::~ArkWeb_PostDataStream_() {}

void ArkWeb_PostDataStream_::SetReadCallback(
    ArkWeb_PostDataReadCallback read_callback_in) {
  read_callback = read_callback_in;
}

void ArkWeb_PostDataStream_::Init(
    ArkWeb_PostDataStreamInitCallback stream_init_callback_in) {
  this->stream_init_callback = stream_init_callback_in;
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return;
  }

  post_data_stream->Init(this);
}

void ArkWeb_PostDataStream_::SetUserData(void* user_data_in) {
  if (!user_data_in) {
    LOG(ERROR) << "scheme_handler set a nullptr.";
    return;
  }
  user_data = user_data_in;
}

void* ArkWeb_PostDataStream_::GetUserData() const {
  return user_data;
}

void ArkWeb_PostDataStream_::Read(void* buffer, int64_t buf_len) const {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return;
  }

  if (!buffer) {
    LOG(ERROR) << "scheme_hadnler read buffer is nullptr.";
  }

  post_data_stream->Read(buffer, buf_len,
                         const_cast<ArkWeb_PostDataStream*>(this));
}

int64_t ArkWeb_PostDataStream_::GetSize() const {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return -1;
  }

  return post_data_stream->GetSize();
}

int64_t ArkWeb_PostDataStream_::GetPosition() const {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return -1;
  }

  return post_data_stream->GetPosition();
}

bool ArkWeb_PostDataStream_::IsChunked() const {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return false;
  }

  return post_data_stream->IsChunked();
}

bool ArkWeb_PostDataStream_::IsEOF() const {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return false;
  }

  return post_data_stream->IsEOF();
}

bool ArkWeb_PostDataStream_::IsInMemory() const {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return false;
  }

  return post_data_stream->IsInMemory();
}

void ArkWeb_PostDataStream_::OnInitComplete(int rv) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
    return;
  }

  if (!stream_init_callback) {
    LOG(ERROR) << "scheme_handler stream_init_callback is nullptr.";
    return;
  }

  stream_init_callback(this, static_cast<ArkWeb_NetError>(rv));
}

void ArkWeb_PostDataStream_::OnReadComplete(char* buffer, int bytes_read) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post_data_stream is nullptr.";
  }

  if (!read_callback) {
    LOG(ERROR) << "scheme_handler read callback is nullptr.";
    return;
  }

  read_callback(this, reinterpret_cast<uint8_t*>(buffer), bytes_read);
}

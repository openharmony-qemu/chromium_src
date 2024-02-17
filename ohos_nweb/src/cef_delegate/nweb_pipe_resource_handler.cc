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

#include "nweb_pipe_resource_handler.h"

#include "base/logging.h"
#include "ohos_adapter_helper.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"

namespace OHOS::NWeb {

namespace {

// A buffer spare capacity limits the amount of times we need to resize the
// buffer when copying over data, which involves reallocating memory.
constexpr int kReadBufferSpareCapacity = 512;

// The minimum number of bytes to process.
constexpr int kMiniumBytesToProcess = 2;

}  // namespace

NWebPipeResourceHandler::NWebPipeResourceHandler(
    const ArkWeb_ResourceRequest* resource_request,
    const ArkWeb_ResourceHandler* resource_handler,
    CefRefPtr<OHOS::NWeb::NWebSchemeHandlerFactory> factory,
    const std::string& web_tag,
    bool from_service_worker)
    : resource_request_(resource_request),
      resource_handler_(resource_handler),
      factory_(factory),
      web_tag_(web_tag),
      from_service_worker_(from_service_worker) {}

bool NWebPipeResourceHandler::Open(CefRefPtr<CefRequest> request,
                                   bool& handle_request,
                                   CefRefPtr<CefCallback> callback) {
  if (response_) {
    handle_request = true;
    LOG(INFO)
        << "scheme_handler resource handler open, process data immediately.";
    return true;
  } else {
    LOG(INFO)
        << "scheme_handler resource handler open, will process data later.";
    handle_request = false;
    response_ready_callback_ = callback;
    return true;
  }
}

bool NWebPipeResourceHandler::Read(
    void* data_out,
    int bytes_to_read,
    int& bytes_read,
    CefRefPtr<CefResourceReadCallback> callback) {
  base::AutoLock scoped_lock_(lock_);
  LOG(INFO) << "scheme_handler read data called " << bytes_to_read;
  if (remain_read_) {
    LOG(ERROR) << "scheme_handler there is still a read operation that has not "
                  "been run.";
    bytes_read = 0;
    return false;
  }

  if (finished_with_error_) {
    LOG(INFO) << "scheme_handler this request has beed failed with error code "
              << error_code_;
    bytes_read = error_code_;
    return false;
  }

  if (!data_buffer_) {
    LOG(INFO) << "scheme_handler donn't have valid buffer process data later.";
    bytes_read = 0;
    last_bytes_to_read_ = bytes_to_read;
    resource_ready_callback_ = callback;
    last_data_out_ = data_out;
    remain_read_ = true;
    return true;
  }

  int bytes_available = data_buffer_->offset();
  LOG(INFO) << "scheme_handler bytes_available " << bytes_available
            << " bytes_to_read: " << bytes_to_read << " finished:" << finished_;
  if (finished_ && bytes_available <= 0) {
    bytes_read = 0;
    return false;
  }

  LOG(INFO) << "scheme_handler will read chrunk data.";
  int bytes_consumed =
      UnSafeReadTrunkData(data_out, bytes_to_read, bytes_read, finished_);
  if (bytes_consumed > 0) {
    LOG(INFO) << "scheme_handler consumed " << bytes_consumed << " bytes";
    bytes_read = bytes_consumed;
    return true;
  }

  LOG(INFO) << "scheme_handler consumed: " << bytes_consumed
            << " bytes finished: " << finished_
            << " bytes_read: " << bytes_read;
  if (finished_) {
    LOG(INFO) << "scheme_handler this request has been finished.";
    bytes_read = 0;
    return false;
  }

  LOG(INFO) << "scheme_handler donn't have enough buffer process data later.";
  bytes_read = 0;
  last_bytes_to_read_ = bytes_to_read;
  resource_ready_callback_ = callback;
  last_data_out_ = data_out;
  remain_read_ = true;
  return true;
}

void NWebPipeResourceHandler::GetResponseHeaders(
    CefRefPtr<CefResponse> response,
    int64& response_length,
    CefString& redirectUrl) {
  base::AutoLock scoped_lock_(lock_);
  LOG(INFO) << "scheme_handler get response headers url: "
            << response_->GetURL().ToString()
            << " status: " << response_->GetStatus()
            << " error: " << response_->GetError();
  if (response_->GetURL().ToString() != "") {
    redirectUrl = response_->GetURL();
  }

  if (response_->GetError() != 0) {
    response->SetError(response_->GetError());
    response_length = 0;
  } else {
    response->SetMimeType(response_->GetMimeType());
    response->SetStatus(response_->GetStatus());
    response->SetStatusText(response_->GetStatusText());
    response->SetCharset(response_->GetCharset());
    CefRequest::HeaderMap cef_response_headers;
    response_->GetHeaderMap(cef_response_headers);
    response->SetHeaderMap(cef_response_headers);
    response_length = -1;
  }
}

void NWebPipeResourceHandler::Cancel() {
  LOG(INFO) << "scheme_handler resource canceld.";
  canceled_ = true;
}

void NWebPipeResourceHandler::DidReceiveResponse(
    CefRefPtr<CefResponse> response) {
  base::AutoLock scoped_lock_(lock_);
  if (!response) {
    LOG(ERROR) << "scheme_handler did receve response nullptr";
    return;
  }

  response_ = response;
  if (response_ready_callback_) {
    response_ready_callback_->Continue();
  }
}

void NWebPipeResourceHandler::DidReceiveData(const uint8_t* buffer,
                                             int64_t buf_len) {
  base::AutoLock scoped_lock_(lock_);
  LOG(INFO) << "scheme_handler did receive data buf_len: " << buf_len
            << " finished: " << finished_
            << " finished_with_error: " << finished_with_error_
            << " reamain_read: " << remain_read_;
  if (finished_ || finished_with_error_ || canceled_) {
    LOG(ERROR) << "scheme_handler had finished should not append data.";
    return;
  }

  if (!data_buffer_) {
    data_buffer_ = base::MakeRefCounted<net::GrowableIOBuffer>();
    data_buffer_->SetCapacity(buf_len + kReadBufferSpareCapacity);
  } else if (data_buffer_->RemainingCapacity() < buf_len) {
    data_buffer_->SetCapacity(data_buffer_->offset() + buf_len +
                              kReadBufferSpareCapacity);
  }

  memcpy(data_buffer_->data(), buffer, buf_len);
  data_buffer_->set_offset(data_buffer_->offset() + buf_len);

  if (remain_read_ && !canceled_) {
    int bytes_consumed = UnSafeReadTrunkData(false);
    LOG(INFO) << "scheme_handler consumed data " << bytes_consumed;
  }
}

void NWebPipeResourceHandler::DidFinish() {
  base::AutoLock scoped_lock_(lock_);
  LOG(INFO) << "scheme_handler did finish.";
  finished_ = true;
  if (remain_read_) {
    UnSafeReadTrunkData(true);
  }
}

void NWebPipeResourceHandler::DidFailWithError(int error_code) {
  base::AutoLock scoped_lock_(lock_);
  finished_ = true;
  finished_with_error_ = true;
  error_code_ = error_code;
  if (remain_read_) {
    remain_read_ = false;
    resource_ready_callback_->Continue(error_code);
  }
}

bool NWebPipeResourceHandler::Skip(
    int64 bytes_to_skip,
    int64& bytes_skipped,
    CefRefPtr<CefResourceSkipCallback> callback) {
  LOG(INFO) << "scheme_handler skip";
  bytes_skipped = -2;
  return false;
}

int NWebPipeResourceHandler::UnSafeReadTrunkData(bool flush) {
  LOG(INFO) << "scheme_handler ReadTrunkData flush " << flush
            << " data_buffer_ " << data_buffer_;
  if (!data_buffer_) {
    if (flush && resource_ready_callback_) {
      resource_ready_callback_->Continue(0);
    }
    return 0;
  }

  int bytes_available = data_buffer_->offset();
  if (bytes_available < kMiniumBytesToProcess && !flush) {
    LOG(INFO) << "scheme_handler donn't have enough data.";
    return 0;
  }

  LOG(INFO) << "scheme_handler ReadTrunkData last_bytes_to_read_ "
            << last_bytes_to_read_ << " bytes_available " << bytes_available;
  int bytes_consumed = std::min(last_bytes_to_read_, bytes_available);
  memcpy(last_data_out_, data_buffer_->StartOfBuffer(), bytes_consumed);
  int bytes_not_consumed = bytes_available - bytes_consumed;
  LOG(INFO) << "scheme_handler bytes not consumed " << bytes_not_consumed;
  memmove(data_buffer_->StartOfBuffer(),
          data_buffer_->StartOfBuffer() + bytes_consumed, bytes_not_consumed);
  LOG(INFO) << "scheme_handler ReadTrunkData set offset " << bytes_not_consumed;
  data_buffer_->set_offset(bytes_not_consumed);
  resource_ready_callback_->Continue(bytes_consumed);
  remain_read_ = false;
  return bytes_consumed;
}

int NWebPipeResourceHandler::UnSafeReadTrunkData(void* data_out,
                                                 int bytes_to_read,
                                                 int& bytes_read,
                                                 bool flush) {
  int bytes_available = data_buffer_->offset();
  if (bytes_available < kMiniumBytesToProcess && !flush) {
    LOG(INFO) << "scheme_handler lower than minium bytes to process flush "
              << flush;
    return 0;
  }

  int bytes_consumed = std::min(bytes_to_read, bytes_available);
  memcpy(data_out, data_buffer_->StartOfBuffer(), bytes_consumed);
  int bytes_not_consumed = bytes_available - bytes_consumed;

  memmove(data_buffer_->StartOfBuffer(),
          data_buffer_->StartOfBuffer() + bytes_consumed, bytes_not_consumed);
  data_buffer_->set_offset(bytes_not_consumed);
  return bytes_consumed;
}

void NWebPipeResourceHandler::CallOnRequestStop() const {
  if (!factory_) {
    LOG(ERROR) << "scheme_handler factory is nullptr.";
    return;
  }

  if (resource_handler_ && resource_request_) {
    factory_->OnRequestStop(resource_request_, web_tag_, from_service_worker_);
  } else {
    LOG(ERROR) << "scheme_handler resource_handler_ " << resource_handler_
               << " resource_request_ " << resource_request_;
  }
}

bool NWebPipeResourceHandler::Release() const {
  if (ref_count_.Release()) {
    delete static_cast<const NWebPipeResourceHandler*>(this);
    return true;
  }

  if (HasOneRef() && (canceled_ || finished_ || finished_with_error_)) {
    CallOnRequestStop();
  }
  return false;
}

}  // namespace OHOS::NWeb

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

#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#include "base/logging.h"
#include "base/strings/string_util.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/mime_sniffer.h"
#include "net/http/http_util.h"
#include "ohos_adapter_helper.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"
#include "third_party/bounds_checking_function/include/securec.h"

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
      from_service_worker_(from_service_worker) {
  char* url = nullptr;
  resource_request->GetUrl(&url);
  if (url) {
    url_ = GURL(url);
    delete url;
  }
}

bool NWebPipeResourceHandler::Open(CefRefPtr<CefRequest> request,
                                   bool& handle_request,
                                   CefRefPtr<CefCallback> callback) {
  base::AutoLock scoped_lock_(lock_);
  if (response_) {
    handle_request = true;
    LOG(DEBUG)
        << "scheme_handler resource handler open, process data immediately.";
    return true;
  } else {
    LOG(DEBUG)
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
  LOG(DEBUG) << "scheme_handler read data called " << bytes_to_read;
  if (remain_read_) {
    LOG(ERROR) << "scheme_handler there is still a read operation that has not "
                  "been run.";
    bytes_read = 0;
    return false;
  }

  if (finished_with_error_) {
    LOG(DEBUG) << "scheme_handler this request has beed failed with error code "
               << error_code_;
    bytes_read = error_code_;
    return false;
  }

  if (!data_buffer_) {
    if (finished_) {
      bytes_read = 0;
      return false;
    }
    LOG(DEBUG) << "scheme_handler donn't have valid buffer process data later.";
    bytes_read = 0;
    last_bytes_to_read_ = bytes_to_read;
    resource_ready_callback_ = callback;
    last_data_out_ = data_out;
    remain_read_ = true;
    return true;
  }

  int bytes_available = data_buffer_->offset();
  LOG(DEBUG) << "scheme_handler bytes_available " << bytes_available
             << " bytes_to_read: " << bytes_to_read
             << " finished:" << finished_;
  if (finished_ && bytes_available <= 0) {
    bytes_read = 0;
    return false;
  }

  LOG(DEBUG) << "scheme_handler will read chrunk data.";
  int bytes_consumed =
      UnSafeReadTrunkData(data_out, bytes_to_read, bytes_read, finished_);
  if (bytes_consumed > 0) {
    LOG(DEBUG) << "scheme_handler consumed " << bytes_consumed << " bytes";
    bytes_read = bytes_consumed;
    return true;
  }

  LOG(DEBUG) << "scheme_handler consumed: " << bytes_consumed
             << " bytes finished: " << finished_
             << " bytes_read: " << bytes_read;
  if (finished_) {
    LOG(DEBUG) << "scheme_handler this request has been finished.";
    bytes_read = 0;
    return false;
  }

  LOG(DEBUG) << "scheme_handler donn't have enough buffer process data later.";
  bytes_read = 0;
  last_bytes_to_read_ = bytes_to_read;
  resource_ready_callback_ = callback;
  last_data_out_ = data_out;
  remain_read_ = true;
  return true;
}

void NWebPipeResourceHandler::GetResponseHeaders(
    CefRefPtr<CefResponse> response,
    int64_t& response_length,
    CefString& redirectUrl) {
  base::AutoLock scoped_lock_(lock_);
  if (!response_) {
    LOG_FEEDBACK(ERROR, kNetwork) << __func__ << " message:responseIsNull";
  }

  LOG(DEBUG) << "scheme_handler get response headers url: "
             << url::LogUtils::ConvertUrlWithMask(response_->GetURL().ToString())
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

    int64_t content_length;
    if (base::StringToInt64(
            response_->GetHeaderByName("Content-Length").ToString(),
            &content_length)) {
      response_length = content_length < 0 ? -1 : content_length;
    }
  }
}

void NWebPipeResourceHandler::Cancel() {
  LOG(DEBUG) << "scheme_handler resource canceld.";
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
  if (response_->GetMimeType().ToString() == "") {
    std::string name = "content-type";
    std::string value;
    std::string mime_type_from_content_type;
    std::string charset;

    bool had_charset = false;
    CefRequest::HeaderMap extra_headers;
    response_->GetHeaderMap(extra_headers);
    CefRequest::HeaderMap::const_iterator it = extra_headers.begin();
    for (; it != extra_headers.end(); ++it) {
      if (base::EqualsCaseInsensitiveASCII(it->first.ToString(), name)) {
        net::HttpUtil::ParseContentType(it->second.ToString(),
                                        &mime_type_from_content_type, &charset,
                                        &had_charset, nullptr);
        LOG(DEBUG) << "content-type: " << it->second.ToString()
                   << " mime_type: " << mime_type_from_content_type;

        response_->SetMimeType(mime_type_from_content_type);
        if (had_charset) {
          response_->SetCharset(charset);
        }
        break;
      }
    }

    if (mime_type_from_content_type == "" && response_->GetError() == 0) {
      needs_sniff_mimetype_ = true;
      return;
    }
  }

  if (response_ready_callback_) {
    response_ready_callback_->Continue();
    response_ready_callback_ = nullptr;
  }
}

void NWebPipeResourceHandler::DidReceiveData(const uint8_t* buffer,
                                             int64_t buf_len) {
  base::AutoLock scoped_lock_(lock_);
  LOG(DEBUG) << "scheme_handler did receive data buf_len: " << buf_len
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

  if (memcpy_s(data_buffer_->data(), data_buffer_->RemainingCapacity(), buffer,
               buf_len) != EOK) {
    LOG(ERROR) << "DidReceiveData memcpy_s failed.";
    return;
  }
  data_buffer_->set_offset(data_buffer_->offset() + buf_len);

  if (needs_sniff_mimetype_) {
    size_t data_length = data_buffer_->offset();
    if (data_length > net::kMaxBytesToSniff) {
      data_length = net::kMaxBytesToSniff;
    }
    std::string new_type;
    net::SniffMimeType(
        std::string_view(
            base::as_writable_chars(data_buffer_->everything()).data(),
            data_length),
        url_, std::string(), net::ForceSniffFileUrlsForHtml::kDisabled,
        &new_type);
    response_->SetMimeType(new_type);
    needs_sniff_mimetype_ = false;
    if (response_ && response_ready_callback_) {
      response_ready_callback_->Continue();
      response_ready_callback_ = nullptr;
    }
  }

  if (remain_read_ && !canceled_) {
    int bytes_consumed = UnSafeReadTrunkData(false);
    LOG(DEBUG) << "scheme_handler consumed data " << bytes_consumed;
  }
}

void NWebPipeResourceHandler::DidFinish() {
  base::AutoLock scoped_lock_(lock_);
  LOG(DEBUG) << "scheme_handler did finish.";
  finished_ = true;
  if (response_ && response_ready_callback_) {
    response_ready_callback_->Continue();
    response_ready_callback_ = nullptr;
  }
  if (remain_read_) {
    UnSafeReadTrunkData(true);
  }
}

void NWebPipeResourceHandler::DidFailWithError(int error_code, bool completeIfNoResponse) {
  LOG(DEBUG) << "scheme_handler NWebPipeResourceHandler::DidFailWithError error_code: " << error_code
             << " completeIfNoResponse: " << completeIfNoResponse;
  base::AutoLock scoped_lock_(lock_);
  finished_ = true;
  finished_with_error_ = true;
  error_code_ = error_code;
  if (!response_ && completeIfNoResponse) {
    response_ = CefResponse::Create();
    response_->SetError(static_cast<cef_errorcode_t>(ARKWEB_ERR_CONNECTION_FAILED));
  }
  if (response_ && response_ready_callback_) {
    response_ready_callback_->Continue();
    response_ready_callback_ = nullptr;
  }
  if (remain_read_) {
    remain_read_ = false;
    resource_ready_callback_->Continue(error_code);
  }
}

bool NWebPipeResourceHandler::Skip(
    int64_t bytes_to_skip,
    int64_t& bytes_skipped,
    CefRefPtr<CefResourceSkipCallback> callback) {
  LOG(DEBUG) << "scheme_handler skip";
  bytes_skipped = bytes_to_skip;
  return true;
}

int NWebPipeResourceHandler::UnSafeReadTrunkData(bool flush) {
  LOG(DEBUG) << "scheme_handler ReadTrunkData flush " << flush
             << " data_buffer_ " << data_buffer_;
  if (!data_buffer_) {
    if (flush && resource_ready_callback_) {
      resource_ready_callback_->Continue(0);
    }
    return 0;
  }

  int bytes_available = data_buffer_->offset();
  if (bytes_available < kMiniumBytesToProcess && !flush) {
    LOG(DEBUG) << "scheme_handler donn't have enough data.";
    return 0;
  }

  LOG(DEBUG) << "scheme_handler ReadTrunkData last_bytes_to_read_ "
             << last_bytes_to_read_ << " bytes_available " << bytes_available;
  int bytes_consumed = std::min(last_bytes_to_read_, bytes_available);
  if (memcpy_s(last_data_out_, last_bytes_to_read_,
               data_buffer_->span_before_offset().data(),
               bytes_consumed) != EOK) {
    LOG(ERROR) << "UnSafeReadTrunkData memcpy_s failed.";
    return 0;
  }
  int bytes_not_consumed = bytes_available - bytes_consumed;
  LOG(DEBUG) << "scheme_handler bytes not consumed " << bytes_not_consumed;
  if (memmove_s(data_buffer_->span_before_offset().data(),
                data_buffer_->capacity(),
                data_buffer_->span_before_offset().data() + bytes_consumed,
                bytes_not_consumed) != EOK) {
    LOG(ERROR) << "UnSafeReadTrunkData memmove_s failed.";
    return 0;
  }
  LOG(DEBUG) << "scheme_handler ReadTrunkData set offset "
             << bytes_not_consumed;
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
    LOG(DEBUG) << "scheme_handler lower than minium bytes to process flush "
               << flush;
    return 0;
  }

  int bytes_consumed = std::min(bytes_to_read, bytes_available);
  if (memcpy_s(data_out, bytes_to_read,
               data_buffer_->span_before_offset().data(),
               bytes_consumed) != EOK) {
    LOG(ERROR) << "UnSafeReadTrunkData memcpy_s failed.";
    return 0;
  }
  int bytes_not_consumed = bytes_available - bytes_consumed;

  if (memmove_s(data_buffer_->span_before_offset().data(),
                data_buffer_->capacity(),
                data_buffer_->span_before_offset().data() + bytes_consumed,
                bytes_not_consumed) != EOK) {
    LOG(ERROR) << "UnSafeReadTrunkData memmove_s failed.";
    return 0;
  }
  data_buffer_->set_offset(bytes_not_consumed);
  return bytes_consumed;
}

void NWebPipeResourceHandler::CallOnRequestStop() const {
  if (!factory_) {
    LOG(ERROR) << "scheme_handler factory is nullptr.";
    return;
  }

  if (resource_handler_ && resource_request_) {
    if(resource_request_->is_ets_) {
      factory_->OnRequestStop(resource_request_, web_tag_,
                              from_service_worker_);
      return;
    }
    content::GetIOThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(&NWebSchemeHandlerFactory::OnRequestStop, factory_,
                       resource_request_, web_tag_, from_service_worker_));
  } else {
    LOG(ERROR) << "scheme_handler resource_handler_ " << resource_handler_
               << " resource_request_ " << resource_request_;
  }
}

bool NWebPipeResourceHandler::Release() const {
  lock_.Acquire();
  if (ref_count_.Release()) {
    lock_.Release();
    delete static_cast<const NWebPipeResourceHandler*>(this);
    return true;
  }

  if (HasOneRef() && (canceled_ || finished_ || finished_with_error_)) {
    CallOnRequestStop();
  }
  lock_.Release();
  return false;
}

}  // namespace OHOS::NWeb

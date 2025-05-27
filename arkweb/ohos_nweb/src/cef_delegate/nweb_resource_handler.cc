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

#include "nweb_resource_handler.h"

#include <unistd.h>

#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_SAFE_FUNCTION)
#include <securec.h>
#endif
#include "base/command_line.h"
#include "base/logging.h"
#include "content/public/common/content_switches.h"
#if BUILDFLAG(ARKWEB_SAFE_FUNCTION)
#include "net/base/net_errors.h"
#endif
#include "nweb_touch_handle_hot_zone_impl.h"
#include "ohos_adapter_helper.h"

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include <sys/stat.h>
#endif

namespace OHOS::NWeb {

class NWebResourceReadyCallbackImpl : public NWebResourceReadyCallback {
 public:
  NWebResourceReadyCallbackImpl(CefRefPtr<CefCallback> callback)
      : callback_(callback) {}
  ~NWebResourceReadyCallbackImpl() = default;
  void Continue() override {
    // LOG(DEBUG) << "intercept NWebResourceReadyCallbackImpl::Continue";
    callback_->Continue();
  }

  void Cancel() override {
    // LOG(DEBUG) << "intercept NWebResourceReadyCallbackImpl::Cancel";
    callback_->Cancel();
  }

 private:
  CefRefPtr<CefCallback> callback_;
};

void ConvertMapToHeaderMap(
    CefRequest::HeaderMap& headers,
    const std::map<std::string, std::string> response_map) {
  for (auto iter = response_map.begin(); iter != response_map.end(); iter++) {
    const std::string& key = iter->first;
    const std::string& value = iter->second;
    headers.emplace(key, value);
  }
}

NWebResourceHandler::NWebResourceHandler(
    std::shared_ptr<NWebUrlResourceResponse> web_response,
    std::string& str)
    : data_(str) {
  response_ = web_response;
  offset_ = 0;
}

/* CefResourceHandler method begin */
bool NWebResourceHandler::Open(CefRefPtr<CefRequest> request,
                               bool& handle_request,
                               CefRefPtr<CefCallback> callback) {
  LOG(DEBUG) << "intercept NWebResourceHandler::Open";
  if (response_ == nullptr) {
    return false;
  }
  if (response_->ResponseDataStatus()) {
    // LOG(DEBUG) << "intercept open reponse sync";
    handle_request = true;
    data_ = response_->ResponseData();
    return true;
  }
  // LOG(DEBUG) << "intercept open async";
  handle_request = false;
  std::shared_ptr<NWebResourceReadyCallbackImpl> nwebCb =
      std::make_shared<NWebResourceReadyCallbackImpl>(callback);
  response_->PutResponseReadyCallback(nwebCb);
  return true;
}

bool NWebResourceHandler::ReadStringData(void* data_out,
                                         int bytes_to_read,
                                         int& bytes_read) {
  // LOG(DEBUG) << "intercept ReadStringData";
  bool has_data = false;
  bytes_read = 0;

  if (data_.empty()) {
    data_ = response_->ResponseData();
  }
  if (offset_ < data_.length()) {
    // Copy the next block of data into the buffer.
    int transfer_size =
        std::min(bytes_to_read, static_cast<int>(data_.length() - offset_));
#if BUILDFLAG(ARKWEB_SAFE_FUNCTION)
    if (memcpy_s(data_out, static_cast<size_t>(bytes_to_read),
                 data_.c_str() + offset_, transfer_size) != EOK) {
      LOG(WARNING)
          << "intercept NWebResourceHandler::ReadStringData memcpy failed";
      bytes_read = net::ERR_FAILED;
      return false;
    }
#else
    memcpy(data_out, data_.c_str() + offset_, transfer_size);
#endif
    offset_ += transfer_size;

    bytes_read = transfer_size;
    has_data = true;
  }

  return has_data;
}

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
int64_t NWebResourceHandler::GetFileSizeByFd() {
  if (resource_data_len_ == 0) {
    int fd = response_->ResponseFileHandle();
    if (fd <= 0) {
      LOG(ERROR) << "intercept get fd invalid : " << fd;
      return -1;
    }
    struct stat st;
    if (fstat(fd, &st) == -1) {
      return -1;
    }
    resource_data_len_ = st.st_size;
  }
  return resource_data_len_;
}
#endif

bool NWebResourceHandler::ReadFileData(void* data_out,
                                       int bytes_to_read,
                                       int& bytes_read) {
  // LOG(DEBUG) << "intercept ReadFileData";
  int fd = response_->ResponseFileHandle();
  if (fd <= 0) {
    bytes_read = fd;
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
    LOG(ERROR) << "intercept get fd invalid : " << fd;
#endif
    return false;
  }
  int ret = read(fd, data_out, bytes_to_read);
  // read fail
  if (ret < 0) {
    LOG(ERROR) << "intercept read fail : " << ret;
    bytes_read = ret;
    return false;
  }
  // completed read
  if (ret == 0) {
    // LOG(DEBUG) << "intercept ReadFileData completed";
    bytes_read = 0;
    return false;
  }
  // LOG(DEBUG) << "intercept contiunue to read:" << ret;
  bytes_read = ret;
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  resource_data_offset_ += bytes_read;
#endif
  return true;
}

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
bool NWebResourceHandler::ReadResourceDataByHap() {
  if (resource_data_len_ == 0) {
    std::string resourceUrlHead("resource:/RAWFILE");
    std::string resourceUrl = response_->ResponseResourceUrl();
    if (resourceUrl.find(resourceUrlHead) == std::string::npos) {
      LOG(ERROR) << "intercept find resource head fail : " << resourceUrl;
      return false;
    }
    resourceUrl.erase(0, resourceUrlHead.length());
    std::string resourcePath = "resources/rawfile" + resourceUrl;
    // LOG(DEBUG) << "intercept Read Resource path : " << resourcePath;
    std::string hapPath =
        base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
            switches::kOhosHapPath);
    auto resourceInstance =
        OHOS::NWeb::OhosAdapterHelper::GetInstance().GetResourceAdapter(
            hapPath);
    uint8_t* data;
    if (!resourceInstance->GetRawFileData(resourcePath, resource_data_len_,
                                          &data, false)) {
      LOG(ERROR) << "intercept Read Resource path fail : " << resourcePath;
      return false;
    }
    resource_data_.reset(data);
  }
  return true;
}

bool NWebResourceHandler::ReadResourceData(void* data_out,
                                           int bytes_to_read,
                                           int& bytes_read) {
  bool has_data = false;
  if (ReadResourceDataByHap() == false) {
    return has_data;
  }

  unsigned char* dataPtr =
      reinterpret_cast<unsigned char*>(resource_data_.get());
  if (resource_data_offset_ < resource_data_len_) {
    int transfer_size =
        std::min(bytes_to_read,
                 static_cast<int>(resource_data_len_ - resource_data_offset_));
#if BUILDFLAG(ARKWEB_SAFE_FUNCTION)
    if (memcpy_s(data_out, static_cast<size_t>(bytes_to_read),
                 dataPtr + resource_data_offset_, transfer_size) != EOK) {
      LOG(WARNING)
          << "intercept NWebResourceHandler::ReadResourceData memcpy failed";
    }
#else
    memcpy(data_out, dataPtr + resource_data_offset_, transfer_size);
#endif
    resource_data_offset_ += transfer_size;
    bytes_read = transfer_size;
    has_data = true;
  }
  return has_data;
}

bool NWebResourceHandler::Skip(int64_t bytes_to_skip,
                               int64_t& bytes_skipped,
                               CefRefPtr<CefResourceSkipCallback> callback) {
  if (response_ == nullptr) {
    return CefResourceHandler::Skip(bytes_to_skip, bytes_skipped, callback);
  }
  if (response_->ResponseDataType() ==
      NWebResponseDataType::NWEB_RESOURCE_URL_TYPE) {
    resource_data_offset_ += bytes_to_skip;
    bytes_skipped = resource_data_offset_;
  } else if (response_->ResponseDataType() ==
             NWebResponseDataType::NWEB_FILE_TYPE) {
    int fd = response_->ResponseFileHandle();
    if (fd <= 0 || lseek(fd, bytes_to_skip, SEEK_CUR) == -1) {
      LOG(ERROR) << "intercept Skip get fd invalid : " << fd;
      return CefResourceHandler::Skip(bytes_to_skip, bytes_skipped, callback);
    }
    resource_data_offset_ += bytes_to_skip;
    bytes_skipped = resource_data_offset_;
  } else {
    return CefResourceHandler::Skip(bytes_to_skip, bytes_skipped, callback);
  }
  // LOG(DEBUG) << "intercept Skip bytes_to_skip: " << bytes_to_skip
  //            << ", resource_data_offset: " << resource_data_offset_;
  return true;
}
#endif

bool NWebResourceHandler::Read(void* data_out,
                               int bytes_to_read,
                               int& bytes_read,
                               CefRefPtr<CefResourceReadCallback> callback) {
  if (response_ == nullptr) {
    bytes_read = -1;
    return false;
  }
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  LOG(DEBUG) << "intercept NWebResourceHandler::Read, responseDataType:"
             << static_cast<int32_t>(response_->ResponseDataType());
  switch (response_->ResponseDataType()) {
    case NWebResponseDataType::NWEB_RESOURCE_URL_TYPE:
      return ReadResourceData(data_out, bytes_to_read, bytes_read);
    case NWebResponseDataType::NWEB_FILE_TYPE:
      return ReadFileData(data_out, bytes_to_read, bytes_read);
    case NWebResponseDataType::NWEB_STRING_TYPE:
    case NWebResponseDataType::NWEB_BUFFER_TYPE:
      return ReadStringData(data_out, bytes_to_read, bytes_read);
    default:
      break;
  }
  return false;
#else
  if (response_->ResponseIsFileHandle()) {
    return ReadFileData(data_out, bytes_to_read, bytes_read);
  } else {
    return ReadStringData(data_out, bytes_to_read, bytes_read);
  }
#endif
}

void NWebResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
                                             int64_t& response_length,
                                             CefString& redirectUrl) {
  // LOG(DEBUG) << "intercept NWebResourceHandler::GetResponseHeaders";
  if (response_ && response) {
    response->SetMimeType(response_->ResponseMimeType());
    response->SetStatus(response_->ResponseStatusCode());
    response->SetStatusText(response_->ResponseStatus());
    response->SetCharset(response_->ResponseEncoding());
    CefRequest::HeaderMap cef_request_headers;
    std::map<std::string, std::string> request_headers =
        response_->ResponseHeaders();
    ConvertMapToHeaderMap(cef_request_headers, request_headers);
    response->SetHeaderMap(cef_request_headers);
  }

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  if (response_->ResponseDataType() == NWebResponseDataType::NWEB_STRING_TYPE) {
	if (data_.empty()) {
	  data_ = response_->ResponseData();
	}
    response_length = data_.length();
    LOG(DEBUG) << "intercept NWEB_STRING_TYPE response_length="
               << response_length;
  } else if (response_->ResponseDataType() ==
             NWebResponseDataType::NWEB_BUFFER_TYPE) {
    response_length = response_->GetResponseDataBufferSize();
    LOG(DEBUG) << "intercept NWEB_BUFFER_TYPE response_length="
               << response_length;
  } else if (response_->ResponseDataType() ==
             NWebResponseDataType::NWEB_RESOURCE_URL_TYPE) {
    if (ReadResourceDataByHap() == false) {
      response_length = -1;
      return;
    }
    response_length = resource_data_len_ - resource_data_offset_;
  } else {
    if (GetFileSizeByFd() == -1) {
      response_length = -1;
      return;
    }
    response_length = resource_data_len_ - resource_data_offset_;
  }
#else
  if (response_->ResponseIsFileHandle()) {
    response_length = -1;
  } else {
    response_length = data_.length();
  }
#endif
}

#if BUILDFLAG(ARKWEB_RESOURCE_INTERCEPTION)
const std::string NWebResourceHandler::GetResponseData() {
  if (response_ == nullptr ||
      response_->ResponseDataType() != NWebResponseDataType::NWEB_STRING_TYPE) {
    static const std::string data;
    return data;
  }
  return response_->ResponseData();
}

size_t NWebResourceHandler::GetResponseDataBuffer(char* data, size_t dest_size) {
  if (data == nullptr || response_ == nullptr
    || response_->ResponseDataType() != NWebResponseDataType::NWEB_BUFFER_TYPE) {
    return 0;
  }
  size_t buffer_size = response_->GetResponseDataBufferSize();
#if BUILDFLAG(ARKWEB_SAFE_FUNCTION)
  if (memcpy_s(data, dest_size, response_->GetResponseDataBuffer(),
               buffer_size) != EOK) {
    LOG(WARNING)
        << "intercept NWebResourceHandler::GetResponseDataBuffer memcpy failed";
    return 0;
  }
#else
  memcpy(data, response_->GetResponseDataBuffer(), buffer_size);
#endif
  return buffer_size;
}

size_t NWebResourceHandler::GetResponseDataBufferSize() {
  if (response_ == nullptr) {
    return 0;
  }
  return response_->GetResponseDataBufferSize();
}
#endif

void NWebResourceHandler::Cancel() {
  // LOG(DEBUG) << "intercept NWebResourceHandler::Cancel";
  if (response_ == nullptr) {
    return;
  }
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  resource_data_len_ = 0;
  resource_data_offset_ = 0;
#endif
  int fd = response_->ResponseFileHandle();
  if (fd <= 0) {
    return;
  }
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  response_->PutResponseFileHandle(-1);
#endif
  close(fd);
}
/* CefResourceHandler method end */
}  // namespace OHOS::NWeb

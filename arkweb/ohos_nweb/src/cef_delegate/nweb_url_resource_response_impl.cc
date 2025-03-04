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

#include "nweb_url_resource_response_impl.h"

namespace OHOS::NWeb {

NWebUrlResourceResponseImpl::NWebUrlResourceResponseImpl(
    const std::string& mime_type,
    const std::string& encoding,
    const int status_code,
    const std::string& reason_phrase,
    const std::map<std::string, std::string>& response_headers,
    std::string& input_stream)
    : mime_type_(mime_type),
      encoding_(encoding),
      status_code_(status_code),
      reason_phrase_(reason_phrase),
      response_headers_(response_headers),
      input_stream_(input_stream) {}

std::string NWebUrlResourceResponseImpl::ResponseData() {
  return input_stream_;
}

void NWebUrlResourceResponseImpl::PutResponseData(
    const std::string& input_stream) {
  input_stream_ = input_stream;
  fd_ = 0;
  isFileFd_ = false;
  dataType_ = NWebResponseDataType::NWEB_STRING_TYPE;
}

void NWebUrlResourceResponseImpl::PutResponseEncoding(
    const std::string& encoding) {
  encoding_ = encoding;
}

std::string NWebUrlResourceResponseImpl::ResponseEncoding() {
  return encoding_;
}

void NWebUrlResourceResponseImpl::PutResponseMimeType(
    const std::string& mime_type) {
  mime_type_ = mime_type;
}

std::string NWebUrlResourceResponseImpl::ResponseMimeType() {
  return mime_type_;
}

void NWebUrlResourceResponseImpl::PutResponseHeaders(
    const std::map<std::string, std::string>& response_headers) {
  response_headers_ = response_headers;
}

std::map<std::string, std::string>
NWebUrlResourceResponseImpl::ResponseHeaders() {
  return response_headers_;
}

void NWebUrlResourceResponseImpl::PutResponseStateAndStatuscode(
    int status_code,
    const std::string& reason_phrase) {
  status_code_ = status_code;
  reason_phrase_ = reason_phrase;
}

int NWebUrlResourceResponseImpl::ResponseStatusCode() {
  return status_code_;
}

std::string NWebUrlResourceResponseImpl::ResponseStatus() {
  return reason_phrase_;
}

void NWebUrlResourceResponseImpl::PutResponseDataStatus(bool isDataReady) {
  isDataReady_ = isDataReady;
  if (isDataReady_ == true && readyCallback_ != nullptr) {
    readyCallback_->Continue();
    readyCallback_ = nullptr;
  }
}

bool NWebUrlResourceResponseImpl::ResponseDataStatus() {
  return isDataReady_;
}

bool NWebUrlResourceResponseImpl::ResponseIsFileHandle() {
  return isFileFd_;
}

void NWebUrlResourceResponseImpl::PutResponseFileHandle(int fd) {
  dataType_ = NWebResponseDataType::NWEB_FILE_TYPE;
  fd_ = fd;
  isFileFd_ = true;
  input_stream_.clear();
}

int NWebUrlResourceResponseImpl::ResponseFileHandle() {
  return fd_;
}

void NWebUrlResourceResponseImpl::PutResponseResourceUrl(
    const std::string& url) {
  resource_url_ = url;
  dataType_ = NWebResponseDataType::NWEB_RESOURCE_URL_TYPE;
}

std::string NWebUrlResourceResponseImpl::ResponseResourceUrl() {
  return resource_url_;
}

NWebResponseDataType NWebUrlResourceResponseImpl::ResponseDataType() {
  return dataType_;
}

void NWebUrlResourceResponseImpl::PutResponseReadyCallback(
    std::shared_ptr<NWebResourceReadyCallback> readyCallback) {
  readyCallback_ = readyCallback;
  if (isDataReady_ && readyCallback_ != nullptr) {
    readyCallback_->Continue();
    readyCallback_ = nullptr;
  }
}

void NWebUrlResourceResponseImpl::PutResponseDataBuffer(char* buffer,
                                                        size_t bufferSize) {
  buffer_ = buffer;
  bufferSize_ = bufferSize;
  fd_ = 0;
  isFileFd_ = false;
  dataType_ = NWebResponseDataType::NWEB_BUFFER_TYPE;
}

char* NWebUrlResourceResponseImpl::GetResponseDataBuffer() {
  return buffer_;
}

size_t NWebUrlResourceResponseImpl::GetResponseDataBufferSize() {
  return bufferSize_;
}

}  // namespace OHOS::NWeb

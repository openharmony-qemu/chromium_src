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

#ifndef NWEB_URL_RESOURCE_RESPONSE_IMPL_H
#define NWEB_URL_RESOURCE_RESPONSE_IMPL_H

#include "nweb_url_resource_response.h"

namespace OHOS::NWeb {

class NWebUrlResourceResponseImpl : public NWebUrlResourceResponse {
public:
  /**
   * @brief Construct a resource response with the given parameters.
   *
   * @param mime_type the resource response's MIME type, for example {
   * "text/html"}.
   * @param encoding the resource response's character encoding, for example
   * {"utf-8"}.
   * @param status_code the status code needs to be in the ranges [100, 299],
   * [400, 599]. Causing a redirect by specifying a 3xx code is not supported.
   * @param reason_phrase the phrase describing the status code, for example
   * "OK". Must be non-empty.
   * @param response_headers the resource response's headers represented as a
   * mapping of header name -> header value.
   * @param input_stream the input stream that provides the resource response's
   * data.
   */
  NWebUrlResourceResponseImpl(
      const std::string &mime_type, const std::string &encoding,
      const int status_code, const std::string &reason_phrase,
      const std::map<std::string, std::string> &response_headers,
      std::string &input_stream);

  NWebUrlResourceResponseImpl() = default;
  ~NWebUrlResourceResponseImpl() = default;

  /**
   * @brief get input stream
   *
   * @retval inputstream string
   */
  std::string ResponseData() override;

  /**
   * @brief set input stream
   *
   * @param input_stream set inputstream for example: fread(buf, 1, sizeof(buf),
   * file)
   */
  void PutResponseData(const std::string &input_stream) override;

  /**
   * @brief Construct a resource response with the given parameters.
   *
   * @param encoding encoding { "utf-8" }
   */
  void PutResponseEncoding(const std::string &encoding) override;

  /**
   * @brief get encoding
   *
   * @retval encoding the resource response's encoding
   */
  std::string ResponseEncoding() override;

  /**
   * @brief Construct a resource response with the given parameters.
   *
   * @param mime_type mime_type{ "text/html" }
   */
  void PutResponseMimeType(const std::string &mime_type) override;

  /**
   * @brief Get mimetype
   *
   * @retval mimetype The resource response's MIME type
   */
  std::string ResponseMimeType() override;

  /**
   * @brief Set ResponseHeaders
   *
   * @param response_headers response header
   */
  void PutResponseHeaders(
      const std::map<std::string, std::string> &response_headers) override;

  /**
   * @brief Get ResponseHeaders
   *
   * @retval response headers
   */
  std::map<std::string, std::string> ResponseHeaders() override;

  /**
   * @brief Set StatusCode And ReasonPhrase
   *
   * @param status_code status code
   * @param reasonphrase reason phrase
   */
  void PutResponseStateAndStatuscode(int status_code,
                                     const std::string &reason_phrase) override;

  /**
   * @brief Get status code
   *
   * @retval status code
   */
  int ResponseStatusCode() override;

  /**
   * @brief Get ReasonPhrase
   *
   * @retval errorcode reason
   */
  std::string ResponseStatus() override;

  void PutResponseDataStatus(bool isDataReady) override;

  bool ResponseDataStatus() override;

  bool ResponseIsFileHandle() override;

  void PutResponseFileHandle(int fd) override;

  int ResponseFileHandle() override;

  void PutResponseResourceUrl(const std::string &url) override;

  std::string ResponseResourceUrl() override;

  NWebResponseDataType ResponseDataType() override;

  void PutResponseReadyCallback(
      std::shared_ptr<NWebResourceReadyCallback> readyCallback) override;

  void PutResponseDataBuffer(char* buffer, size_t bufferSize) override;

  char* GetResponseDataBuffer() override;

  size_t GetResponseDataBufferSize() override;

private:
  static bool IsSupportedStatusCode(int status_code);

private:
  std::string mime_type_;
  std::string encoding_;
  int status_code_ = 200;
  std::string reason_phrase_;
  std::map<std::string, std::string> response_headers_;
  std::string input_stream_;
  int fd_ = -1;
  std::string resource_url_;
  NWebResponseDataType dataType_ = NWebResponseDataType::NWEB_STRING_TYPE;
  bool isFileFd_ = false;
  bool isDataReady_ = true;
  std::shared_ptr<NWebResourceReadyCallback> readyCallback_ = nullptr;
  char* buffer_;
  size_t bufferSize_ = 0;
  const int net_failed_ = -2;
  static const int kMinStatusCode_ = 100;
  static const int kMaxStatusCode_ = 599;
};

} // namespace OHOS::NWeb

#endif // NWEB_URL_RESOURCE_RESPONSE_IMPL_H

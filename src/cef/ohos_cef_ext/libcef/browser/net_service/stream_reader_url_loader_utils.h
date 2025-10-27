/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef STREAM_READER_URL_LOADER_UTILS_H_
#define STREAM_READER_URL_LOADER_UTILS_H_

#include <regex>

#include "base/memory/raw_ptr.h"
#include "arkweb/build/features/features.h"
#include "libcef/browser/net_service/stream_reader_url_loader.h"
#include "net/base/features.h"
#include "net/http/http_response_headers.h"
#include "services/network/public/cpp/features.h"
#include "services/network/public/mojom/url_response_head.mojom.h"

#if BUILDFLAG(ARKWEB_PERFORMANCE_NETWORK_TRACE)
#include "base/trace_event/trace_event.h"
#endif

namespace net_service {
class StreamReaderURLLoader;
class ResourceResponse;

static const char kResponseDataID[] = "ResponseDataID";
// length of unix timestamp accurate to milliseconds
static const int kResponseDataIDMaxLength = 13;

class StreamReaderURLLoaderUtils {
 public:
  friend class StreamReaderURLLoader;
  explicit StreamReaderURLLoaderUtils(
      StreamReaderURLLoader* stream_reader_url_loader);
  ~StreamReaderURLLoaderUtils();

#if BUILDFLAG(ARKWEB_API_PER)
  void HandleResponseDataID(
      network::mojom::URLResponseHeadPtr& pending_response,
      const typename ResourceResponse::HeaderMap extra_headers);
  void CreatePipeOptions(MojoCreateDataPipeOptions& options);
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  void CheckStatusCode(int status_code,
                       const std::string& mime_type,
                       scoped_refptr<net::HttpResponseHeaders> headers);
#endif

#if BUILDFLAG(ARKWEB_RESOURCE_INTERCEPTION)
  bool TryTransferDataWithSharedMemory();
#endif

 private:
  raw_ptr<StreamReaderURLLoader> url_loader_;
};
}  // namespace net_service
#endif

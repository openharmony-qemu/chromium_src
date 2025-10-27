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
#include "ohos_cef_ext/libcef/browser/net_service/stream_reader_url_loader_utils.h"

namespace net_service {

StreamReaderURLLoaderUtils::StreamReaderURLLoaderUtils(
    StreamReaderURLLoader* stream_reader_url_loader)
{
  this->url_loader_ = stream_reader_url_loader;
}

StreamReaderURLLoaderUtils::~StreamReaderURLLoaderUtils() = default;

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
constexpr int32_t NET_STATUS_CODE_400 = 400;
#endif

#if BUILDFLAG(ARKWEB_API_PER)
bool CheckResponseDataID(const std::string& identity)
{
  if (identity.empty() || identity.length() > kResponseDataIDMaxLength) {
    return false;
  }
  static std::regex pattern(R"(\d+)");
  return std::regex_match(identity, pattern);
}

void StreamReaderURLLoaderUtils::HandleResponseDataID(
    network::mojom::URLResponseHeadPtr& pending_response,
    const typename ResourceResponse::HeaderMap extra_headers)
{
  // When user custom resource responses via 'onInterceptRequest',
  // they can set 'ResponseDataID' value in the reponse header.
  // This value will be utilized to generate codecache for interupt js resource.
  //
  // 'ResponseDataID': A string of 13 pure digits representing response data ID.
  // When response data changes, a different 'ResponseDataID' must be set.
  const auto& it = extra_headers.find(kResponseDataID);
  if (it != extra_headers.end()) {
    auto identity = it->second;
    if (CheckResponseDataID(identity)) {
      pending_response->response_time =
          base::Time::FromMillisecondsSinceUnixEpoch(std::stod(identity));
      LOG(INFO) << "ResponseDataID have set";
    } else {
      LOG(WARNING) << "ResponseDataID not a reasonable value!";
    }
  }
}

void StreamReaderURLLoaderUtils::CreatePipeOptions(
    MojoCreateDataPipeOptions& options)
{
  options.struct_size = sizeof(MojoCreateDataPipeOptions);
  options.flags = MOJO_CREATE_DATA_PIPE_FLAG_NONE;
  options.element_num_bytes = 1;
  options.capacity_num_bytes =
      network::features::GetDataPipeDefaultAllocationSize(
          network::features::DataPipeAllocationSize::kLargerSizeIfPossible);
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void StreamReaderURLLoaderUtils::CheckStatusCode(
    int status_code,
    const std::string& mime_type,
    scoped_refptr<net::HttpResponseHeaders> headers)
{
  if (!headers) {
    LOG(DEBUG) << "CheckStatusCode headers is null";
    return;
  }
  if (status_code >= NET_STATUS_CODE_400) {
    if (!mime_type.empty()) {
      headers->SetHeader(net::HttpRequestHeaders::kContentType, mime_type);
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_RESOURCE_INTERCEPTION)
bool StreamReaderURLLoaderUtils::TryTransferDataWithSharedMemory()
{
  size_t bufferSize =
      url_loader_->response_delegate_->GetResponseDataBufferSize();
  if (bufferSize <= 0) {
    LOG(DEBUG) << "shared-memory buffer size <= 0";
    return false;
  }

  auto buffer = mojo::SharedBufferHandle::Create(bufferSize);
  if (!buffer.is_valid()) {
    LOG(ERROR) << "shared-memory create buffer err";
    return false;
  }
  base::WritableSharedMemoryRegion writable_region =
      mojo::UnwrapWritableSharedMemoryRegion(std::move(buffer));
  base::WritableSharedMemoryMapping shared_memory_mapping =
      writable_region.Map();
  if (!shared_memory_mapping.IsValid()) {
    LOG(ERROR) << "shared-memory mapping err";
    return false;
  }

  char* memory = shared_memory_mapping.GetMemoryAs<char>();

  size_t size = url_loader_->response_delegate_->GetResponseDataBuffer(
      memory, bufferSize);
  LOG(DEBUG) << "shared-memory+++ GetResponseDataBuffer buffer size=" << size;
  if (size <= 0) {
    LOG(ERROR) << "shared-memory GetResponseDataBuffer size <= 0";
    return false;
  }

  base::ReadOnlySharedMemoryRegion read_only_region =
      base::WritableSharedMemoryRegion::ConvertToReadOnly(
          std::move(writable_region));
  if (!read_only_region.IsValid()) {
    LOG(ERROR) << "shared-memory: convert to read only err";
    return false;
  }
  url_loader_->client_->OnTransferDataWithSharedMemory(
      std::move(read_only_region), bufferSize);
  LOG(DEBUG) << "shared-memory--- GetResponseDataBuffer buffer size="
             << bufferSize;

  return true;
}
#endif
}  // namespace net_service

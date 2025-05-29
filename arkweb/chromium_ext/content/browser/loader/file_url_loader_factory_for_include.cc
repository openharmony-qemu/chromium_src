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

#include "arkweb/build/features/features.h"
#include "base/base_switches.h"
#include "net/base/url_util.h"
#if BUILDFLAG(ARKWEB_RECOURCE_SCHEME)
#include "base/datashare_uri_utils.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

namespace content {
namespace {

#if BUILDFLAG(ARKWEB_RECOURCE_SCHEME)
constexpr int32_t APPLICATION_API_10 = 10;

int32_t GetApplicationApiVersion() {
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kOhosAppApiVersion)) {
    LOG(ERROR) << "kOhosAppApiVersion not exist";
    return -1;
  }
  std::string apiVersion =
      base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
          switches::kOhosAppApiVersion);
  if (apiVersion.empty()) {
    return -1;
  }
  return std::stoi(apiVersion);
}

class ResourceURLLoader : public network::mojom::URLLoader {
 public:
  static void CreateAndStart(
      const base::FilePath& profile_path,
      const network::ResourceRequest& request,
      network::mojom::FetchResponseType response_type,
      mojo::PendingReceiver<network::mojom::URLLoader> loader,
      mojo::PendingRemote<network::mojom::URLLoaderClient> client_remote,
      std::unique_ptr<FileURLLoaderObserver> observer,
      scoped_refptr<net::HttpResponseHeaders> extra_response_headers) {
    auto* resource_url_loader = new ResourceURLLoader;
    resource_url_loader->Start(profile_path, request, response_type,
                               std::move(loader), std::move(client_remote),
                               std::move(observer),
                               std::move(extra_response_headers));
  }

  ResourceURLLoader(const ResourceURLLoader&) = delete;
  ResourceURLLoader& operator=(const ResourceURLLoader&) = delete;

  // network::mojom::URLLoader:
  void FollowRedirect(
      const std::vector<std::string>& removed_headers,
      const net::HttpRequestHeaders& modified_headers,
      const net::HttpRequestHeaders& modified_cors_exempt_headers,
      const std::optional<GURL>& new_url) override {}
  void SetPriority(net::RequestPriority priority,
                   int32_t intra_priority_value) override {}
  void PauseReadingBodyFromNet() override {}
  void ResumeReadingBodyFromNet() override {}

 private:
  ResourceURLLoader() = default;
  ~ResourceURLLoader() override = default;

  void Start(const base::FilePath& profile_path,
             const network::ResourceRequest& request,
             network::mojom::FetchResponseType response_type,
             mojo::PendingReceiver<network::mojom::URLLoader> loader,
             mojo::PendingRemote<network::mojom::URLLoaderClient> client_remote,
             std::unique_ptr<FileURLLoaderObserver> observer,
             scoped_refptr<net::HttpResponseHeaders> extra_response_headers) {
    auto head = network::mojom::URLResponseHead::New();
    head->request_start = base::TimeTicks::Now();
    head->response_start = base::TimeTicks::Now();
    head->response_type = response_type;
    head->headers = extra_response_headers;
    receiver_.Bind(std::move(loader));
    receiver_.set_disconnect_handler(base::BindOnce(
        &ResourceURLLoader::OnMojoDisconnect, base::Unretained(this)));
    client_.Bind(std::move(client_remote));

    if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
            switches::kOhosHapPath)) {
      OnClientComplete(net::ERR_FAILED, std::move(observer));
      return;
    }
    std::string resourcesPath;
    if (request.url.SchemeIs(url::kResourcesScheme)) {
      resourcesPath = "resources/";
      resourcesPath += request.url.host() +
                       net::UnescapePercentEncodedUrl(request.url.path());
    } else {
      LOG(ERROR) << "url scheme error";
      OnClientComplete(net::ERR_FAILED, std::move(observer));
      return;
    }

    LOG(INFO) << "ResourceURLLoader url: " << request.url.spec()
              << ", path: " << resourcesPath;
    std::string hapPath =
        base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
            switches::kOhosHapPath);
    mojo::ScopedDataPipeProducerHandle producer_handle;
    mojo::ScopedDataPipeConsumerHandle consumer_handle;
    // Request the larger size data pipe for resource:// URL loading.
    uint32_t data_pipe_size =
        network::features::GetDataPipeDefaultAllocationSize(
            network::features::DataPipeAllocationSize::kLargerSizeIfPossible);
    // This should already be static_asserted in network::features, but good
    // to double-check.
    DCHECK(data_pipe_size >= net::kMaxBytesToSniff)
        << "Default file data pipe size must be at least as large as a "
           "MIME-type sniffing buffer.";
    if (mojo::CreateDataPipe(data_pipe_size, producer_handle,
                             consumer_handle) != MOJO_RESULT_OK) {
      OnClientComplete(net::ERR_FAILED, std::move(observer));
      return;
    }
    if (observer) {
      observer->OnStart();
    }
    size_t length = 0;
    uint8_t* rawData;
    auto resourceInstance =
        OHOS::NWeb::OhosAdapterHelper::GetInstance().GetResourceAdapter(
            hapPath);
    mojo::DataPipeProducer::DataSource::ReadResult read_result;

    if (!resourceInstance->GetRawFileData(resourcesPath, length, &rawData,
                                          false)) {
      LOG(ERROR) << "ResourceURLLoader GetRawFileData failed";
      read_result.result = MOJO_RESULT_NOT_FOUND;
      if (observer) {
        observer->OnRead(base::span<char>(), &read_result);
        observer->OnDone();
      }
      client_->OnComplete(network::URLLoaderCompletionStatus(
          ConvertMojoResultToNetError(read_result.result)));
      client_.reset();
      MaybeDeleteSelf();
      return;
    }
    LOG(INFO) << "GetRawFileData length: " << length;
    read_result.result = MOJO_RESULT_OK;
    read_result.bytes_read =
        length > net::kMaxBytesToSniff ? net::kMaxBytesToSniff : length;
    std::unique_ptr<uint8_t[]> data;
    data.reset(rawData);
    char* dataPtr = reinterpret_cast<char*>(data.get());
    std::vector<char> initial_read_buffer(net::kMaxBytesToSniff);
    for (size_t i = 0; i < read_result.bytes_read; ++i) {
      initial_read_buffer[i] = *(dataPtr + i);
    }
    if (observer) {
      observer->OnRead(base::span<char>(dataPtr, length), &read_result);
    }

    uint64_t initial_read_size = read_result.bytes_read;
    net::HttpByteRange byte_range;
    if (std::optional<std::string> range_header =
            request.headers.GetHeader(net::HttpRequestHeaders::kRange);
        range_header) {
      // Handle a simple Range header for a single range.
      std::vector<net::HttpByteRange> ranges;
      bool fail = false;
      if (net::HttpUtil::ParseRangeHeader(*range_header, &ranges) &&
          ranges.size() == 1) {
        byte_range = ranges[0];
        if (!byte_range.ComputeBounds(length)) {
          fail = true;
        }
      } else {
        fail = true;
      }
      if (fail) {
        OnClientComplete(net::ERR_REQUEST_RANGE_NOT_SATISFIABLE,
                         std::move(observer));
        return;
      }
    }
    uint64_t first_byte_to_send = 0;
    uint64_t total_bytes_to_send = length;
    if (byte_range.IsValid()) {
      first_byte_to_send = byte_range.first_byte_position();
      total_bytes_to_send =
          byte_range.last_byte_position() - first_byte_to_send + 1;
    }
    total_bytes_written_ = total_bytes_to_send;
    head->content_length = base::saturated_cast<int64_t>(total_bytes_to_send);
    if (first_byte_to_send < initial_read_size) {
      base::span<const uint8_t> bytes_to_write =
          base::as_byte_span(initial_read_buffer)
              .subspan(base::checked_cast<size_t>(first_byte_to_send));
      bytes_to_write = bytes_to_write.first(base::checked_cast<size_t>(std::min(
          static_cast<uint64_t>(bytes_to_write.size()), total_bytes_to_send)));

      size_t actually_written_bytes = 0;
      MojoResult result = producer_handle->WriteData(
          bytes_to_write, MOJO_WRITE_DATA_FLAG_NONE, actually_written_bytes);
      if (result != MOJO_RESULT_OK) {
        OnFileWritten(std::move(observer), nullptr, result);
        LOG(ERROR) << "ResourceURLLoader WriteData failed";
        return;
      }
      // Discount the bytes we just sent from the total range.
      first_byte_to_send = initial_read_size;
      total_bytes_to_send -= actually_written_bytes;
    }
    const base::FilePath::CharType* resource_file_path =
        FILE_PATH_LITERAL(resourcesPath.c_str());
    if (!net::GetMimeTypeFromFile(base::FilePath(resource_file_path),
                                  &head->mime_type)) {
      std::string new_type;
      net::SniffMimeType(
          std::string_view(initial_read_buffer.data(), read_result.bytes_read),
          request.url, head->mime_type,
          GetContentClient()->browser()->ForceSniffingFileUrlsForHtml()
              ? net::ForceSniffFileUrlsForHtml::kEnabled
              : net::ForceSniffFileUrlsForHtml::kDisabled,
          &new_type);
      head->mime_type.assign(new_type);
      head->did_mime_sniff = true;
    }
    if (!head->headers) {
      head->headers =
          base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 200 OK");
    }
    LOG(INFO) << "ResourceURLLoader AddHeader mime_type " << head->mime_type;
    head->headers->AddHeader(net::HttpRequestHeaders::kContentType,
                             head->mime_type);
    client_->OnReceiveResponse(std::move(head), std::move(consumer_handle),
                               std::nullopt);
    LOG(INFO) << "total_bytes_to_send: " << total_bytes_to_send;
    if (total_bytes_to_send == 0) {
      // There's definitely no more data, so we're already done.
      OnFileWritten(std::move(observer), nullptr, MOJO_RESULT_OK);
      return;
    }

    // In case of a range request, seek to the appropriate position before
    // sending the remaining bytes asynchronously. Under normal conditions
    // (i.e., no range request) this Seek is effectively a no-op.
    if (observer) {
      observer->OnSeekComplete(first_byte_to_send);
    }

    data_producer_ =
        std::make_unique<mojo::DataPipeProducer>(std::move(producer_handle));
    std::string_view remaining_buffer(dataPtr + first_byte_to_send,
                                      total_bytes_to_send);
    data_producer_->Write(
        std::make_unique<mojo::FilteredDataSource>(
            std::make_unique<mojo::StringDataSource>(
                remaining_buffer, mojo::StringDataSource::AsyncWritingMode::
                                      STRING_STAYS_VALID_UNTIL_COMPLETION),
            std::move(observer)),
        base::BindOnce(&ResourceURLLoader::OnFileWritten,
                       base::Unretained(this), nullptr, std::move(data)));
  }

  void OnMojoDisconnect() {
    data_producer_.reset();
    receiver_.reset();
    client_.reset();
    MaybeDeleteSelf();
  }

  void OnClientComplete(net::Error net_error,
                        std::unique_ptr<FileURLLoaderObserver> observer) {
    client_->OnComplete(network::URLLoaderCompletionStatus(net_error));
    client_.reset();
    if (observer) {
      if (net_error != net::OK) {
        mojo::DataPipeProducer::DataSource::ReadResult result;
        result.result = ConvertNetErrorToMojoResult(net_error);
        observer->OnRead(base::span<char>(), &result);
      }
      observer->OnDone();
    }
    MaybeDeleteSelf();
  }

  void MaybeDeleteSelf() {
    if (!receiver_.is_bound() && !client_.is_bound()) {
      delete this;
    }
  }

  void OnFileWritten(std::unique_ptr<FileURLLoaderObserver> observer,
                     std::unique_ptr<uint8_t[]> write_data,
                     MojoResult result) {
    data_producer_.reset();
    if (observer) {
      observer->OnDone();
    }

    if (result == MOJO_RESULT_OK) {
      network::URLLoaderCompletionStatus status(net::OK);
      status.encoded_data_length = total_bytes_written_;
      status.encoded_body_length = total_bytes_written_;
      status.decoded_body_length = total_bytes_written_;
      client_->OnComplete(status);
    } else {
      LOG(ERROR) << "write data failed, result: " << result;
      client_->OnComplete(network::URLLoaderCompletionStatus(net::ERR_FAILED));
    }
    client_.reset();
    MaybeDeleteSelf();
  }

  std::unique_ptr<mojo::DataPipeProducer> data_producer_;
  mojo::Receiver<network::mojom::URLLoader> receiver_{this};
  mojo::Remote<network::mojom::URLLoaderClient> client_;

  uint64_t total_bytes_written_ = 0;
};

class FileURLLoaderUtils {
 public:
#if BUILDFLAG(ARKWEB_RECOURCE_SCHEME) || BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  static std::unique_ptr<mojo::FileDataSource> FileDataSourceExt(
      base::FilePath path) {
    std::unique_ptr<mojo::FileDataSource> file_data_source;
    if (path.IsDataShareUri()) {
      file_data_source = std::make_unique<mojo::FileDataSource>(
          base::OpenDatashareUriForRead(path));
    } else {
      file_data_source = std::make_unique<mojo::FileDataSource>(
          base::File(path, base::File::FLAG_OPEN | base::File::FLAG_READ));
    }
    return file_data_source;
  }
#endif
};
#endif

}  // namespace

}  // namespace content

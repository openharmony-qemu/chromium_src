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

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/media/video_common_data_provider.h"

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "net/http/http_byte_range.h"
#include "net/http/http_request_headers.h"
#include "services/network/public/cpp/cors/cors.h"
#include "third_party/blink/public/mojom/fetch/fetch_api_request.mojom.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/platform/web_url_error.h"
#include "third_party/blink/public/platform/web_url_request.h"
#include "third_party/blink/public/platform/web_url_response.h"
#include "third_party/blink/renderer/platform/media/cache_util.h"
#include "third_party/blink/renderer/platform/media/resource_fetch_context.h"
#include "third_party/blink/renderer/platform/media/url_index.h"
#include "third_party/blink/renderer/platform/weborigin/security_origin.h"
#include "third_party/bounds_checking_function/include/securec.h"

namespace blink {

const base::TimeDelta kLoaderFailedPerTimeoutSeconds = base::Seconds(20);
const base::TimeDelta kLoaderFailedTotalTimeoutSeconds = base::Seconds(60);
const int kReportPerRequestTimeout = 1;
const int kReportTotalRequestTimeout = 2;

VideoCommonDataProvider::VideoCommonDataProvider(
    UrlData* url_data,
    MultiBufferBlockId pos,
    std::string id,
    bool is_client_audio_element,
    scoped_refptr<base::SingleThreadTaskRunner> task_runner)
    : pos_(pos),
      id_(id),
      url_data_(url_data),
      cors_mode_(url_data->cors_mode()),
      origin_(url_data->url()),
      is_client_audio_element_(is_client_audio_element),
      task_runner_(std::move(task_runner)) {
  DCHECK(url_data_) << " pos = " << pos;
  DCHECK_GE(pos, 0);
}

void VideoCommonDataProvider::SetUrlData(UrlData* ud) {
  url_data_ = ud;
}

UrlData* VideoCommonDataProvider::urlData() const {
  return url_data_;
}

const KURL& VideoCommonDataProvider::Origin() const {
  return origin_;
}

UrlData::CorsMode VideoCommonDataProvider::CorsMode() const {
  return cors_mode_;
}

MultiBufferBlockId VideoCommonDataProvider::Tell() const {
  return pos_;
}

void VideoCommonDataProvider::Fallback() {
  url_data_->SetNeedFallback();
  url_data_->multibuffer()->Fallback(this);
}

WebURLRequest VideoCommonDataProvider::PrepareRequest(int64_t start, int64_t end) {
  WebURLRequest request(url_data_->url());
  request.SetRequestContext(is_client_audio_element_
                                ? mojom::RequestContextType::AUDIO
                                : mojom::RequestContextType::VIDEO);
  request.SetRequestDestination(
      is_client_audio_element_ ? network::mojom::RequestDestination::kAudio
                               : network::mojom::RequestDestination::kVideo);

  if (end == -1) {
    request.SetHttpHeaderField(
        WebString::FromUTF8(net::HttpRequestHeaders::kRange),
        WebString::FromUTF8(
            net::HttpByteRange::RightUnbounded(start).GetHeaderValue()));
  } else {
    request.SetHttpHeaderField(
        WebString::FromUTF8(net::HttpRequestHeaders::kRange),
        WebString::FromUTF8(
            net::HttpByteRange::Bounded(start, end).GetHeaderValue()));
  }

  // We would like to send an if-match header with the request to
  // tell the remote server that we really can't handle files other
  // than the one we already started playing. Unfortunately, doing
  // so will disable the http cache, and possibly other proxies
  // along the way. See crbug/504194 and crbug/689989 for more information.

  // Disable compression, compression for audio/video doesn't make sense...
  request.SetHttpHeaderField(
      WebString::FromUTF8(net::HttpRequestHeaders::kAcceptEncoding),
      WebString::FromUTF8("identity;q=1, *;q=0"));

  if (url_data_->cors_mode() != UrlData::CORS_UNSPECIFIED) {
    request.SetMode(network::mojom::RequestMode::kCors);
    if (url_data_->cors_mode() != UrlData::CORS_USE_CREDENTIALS) {
      request.SetCredentialsMode(network::mojom::CredentialsMode::kSameOrigin);
    }
  }

  return request;
}

int64_t VideoCommonDataProvider::block_size() const {
  int64_t ret = 1;
  return ret << block_size_shift();
}

int32_t VideoCommonDataProvider::block_size_shift() const {
  return url_data_->multibuffer()->block_size_shift();
}

VideoRangeURLLoaderClient::VideoRangeURLLoaderClient(
    VideoCommonDataProvider* provider,
    int64_t start,
    int64_t end,
    std::string id,
    scoped_refptr<base::SingleThreadTaskRunner> task_runner)
    : provider_(provider),
      start_(start),
      end_(end),
      id_(id),
      byte_pos_(start),
      task_runner_(std::move(task_runner)) {}

VideoRangeURLLoaderClient::~VideoRangeURLLoaderClient() {
  ResetLoader();
}

void VideoRangeURLLoaderClient::ResetLoader() {
  if (active_loader_) {
    auto loader_auto_release = std::move(active_loader_);
    loader_auto_release->NotifyFinished();
  }
}

std::string VideoRangeURLLoaderClient::AsHumanReadableString() const {
  std::ostringstream s;
  s << " [(hash" << std::hex << base::FastHash(base::byte_span_from_ref(this)) << "): id=" 
    << id_ << ";start=" << start_
    << ";end=" << end_ << ";pos=" << byte_pos()
    << ";receive=" << received_ << "]";
  return s.str();
}

void VideoRangeURLLoaderClient::SetFifo(
    std::list<scoped_refptr<media::DataBuffer>>* p) {
  fifo_ptr_ = p;
}

bool VideoRangeURLLoaderClient::ProviderInThisRange() {
  MultiBufferBlockId pos = provider_->Tell();
  int64_t bp = pos << provider_->block_size_shift();
  if (bp < start_) {
    return false;
  }
  if (end_ == -1) {
    return true;
  }
  if (bp > end_) {
    return false;
  }
  return true;
}

std::pair<int64_t, int64_t> VideoRangeURLLoaderClient::Range() {
  return std::pair<int64_t, int64_t>(start_, end_);
}

int64_t VideoRangeURLLoaderClient::byte_pos() const {
  return byte_pos_;
}

void VideoRangeURLLoaderClient::Start() {
  if ((end_ != -1 && byte_pos() > end_) ||
      (provider_->urlData()->length() > 0 &&
       byte_pos() >= provider_->urlData()->length())) {
    LOG(INFO) << "VideoOpt: " << __func__ << AsHumanReadableString();
    // if does not use PostTask, iter in MultiBuffer::RemoveProvider
    // called by OnDataProviderEvent is end(), because in
    // MultiBuffer::AddReader, provider created by CreateWriter has not been
    // added into writer_index_
    task_runner_->PostTask(FROM_HERE,
                           base::BindOnce(&VideoRangeURLLoaderClient::Terminate,
                                          weak_factory_.GetWeakPtr()));
    return;
  }

  // Start resource loading.
  WebAssociatedURLLoaderOptions options;
  if (provider_->urlData()->cors_mode() != UrlData::CORS_UNSPECIFIED) {
    options.expose_all_response_headers = true;
    // The author header set is empty, no preflight should go ahead.
    options.preflight_policy =
        network::mojom::CorsPreflightPolicy::kPreventPreflight;
  }

  ResetLoader();
  active_loader_ =
      provider_->urlData()->url_index()->fetch_context()->CreateVideoUrlLoader(
          options);

  WebURLRequest req = provider_->PrepareRequest(byte_pos(), end_);

  active_loader_->Load(req, weak_factory_.GetWeakPtr(), id_, start_);
}

// WebAssociatedURLLoaderClient implementation.

bool VideoRangeURLLoaderClient::WillFollowRedirect(
    const WebURL& new_url,
    const WebURLResponse& redirect_response) {
  LOG(INFO) << "VideoOpt: " << __func__ << AsHumanReadableString();
  DVLOG(1) << "willFollowRedirect";
  redirects_to_ = new_url;
  provider_->urlData()->set_valid_until(base::Time::Now() +
                                        GetCacheValidUntil(redirect_response));

  // This test is vital for security!
  if (provider_->CorsMode() == UrlData::CORS_UNSPECIFIED) {
    // We allow the redirect if the origin is the same.
    if (!SecurityOrigin::AreSameOrigin(provider_->Origin(), redirects_to_)) {
      // We also allow the redirect if we don't have any data in the
      // cache, as that means that no dangerous data mixing can occur.
      if (provider_->urlData()->multibuffer()->map().empty() &&
          fifo_ptr_->empty()) {
        return true;
      }

      ResetLoader();
      LOG(INFO) << "VideoOpt: failed redirect with CORS" << AsHumanReadableString();
      provider_->urlData()->Fail();
      return false;  // "this" may be deleted now.
    }
  }

  return true;
}

void VideoRangeURLLoaderClient::DidSendData(uint64_t bytes_sent,
                                         uint64_t total_bytes_to_be_sent) {
  NOTIMPLEMENTED();
}

void VideoRangeURLLoaderClient::DidDownloadData(uint64_t data_length) {
  NOTIMPLEMENTED();
}

void VideoRangeURLLoaderClient::DidReceiveResponse(
    const blink::WebURLResponse& response) {
#if DCHECK_IS_ON()
  std::string version;
  switch (response.HttpVersion()) {
    case WebURLResponse::kHTTPVersion_0_9:
      version = "0.9";
      break;
    case WebURLResponse::kHTTPVersion_1_0:
      version = "1.0";
      break;
    case WebURLResponse::kHTTPVersion_1_1:
      version = "1.1";
      break;
    case WebURLResponse::kHTTPVersion_2_0:
      version = "2.1";
      break;
    case WebURLResponse::kHTTPVersionUnknown:
      version = "unknown";
      break;
  }
  DVLOG(1) << "didReceiveResponse: HTTP/" << version << " "
           << response.HttpStatusCode()
           << " cors_mode:" << provider_->CorsMode();
#endif
  DCHECK(active_loader_);

  scoped_refptr<UrlData> destination_url_data(provider_->urlData());

  if (!redirects_to_.IsEmpty()) {
    LOG(INFO) << "VideoOpt: redirects_to_ not empty";
    destination_url_data = provider_->urlData()->url_index()->GetByUrl(
        redirects_to_, provider_->CorsMode(), provider_->urlData()->cache_lookup_mode());
    redirects_to_ = KURL();
  }

  base::Time last_modified;
  if (base::Time::FromString(
          response.HttpHeaderField("Last-Modified").Utf8().data(),
          &last_modified)) {
    destination_url_data->set_last_modified(last_modified);
  }

  destination_url_data->set_etag(
      response.HttpHeaderField("ETag").Utf8().data());

  destination_url_data->set_valid_until(base::Time::Now() +
                                        GetCacheValidUntil(response));

  destination_url_data->set_cacheable(GetReasonsForUncacheability(response) ==
                                      0);

  bool end_of_file = false;
  // We get the response type here because aborting the loader may change it.
  const auto response_type = response.GetType();
  bytes_to_discard_ = 0;

  // We make a strong assumption that when we reach here we have either
  // received a response from HTTP/HTTPS protocol or the request was
  // successful (in particular range request). So we only verify the partial
  // response for HTTP and HTTPS protocol.
  if (destination_url_data->url().ProtocolIsInHTTPFamily()) {
    bool partial_response = (response.HttpStatusCode() == kHttpPartialContent);
    bool ok_response = (response.HttpStatusCode() == kHttpOK);
    // Check to see whether the server supports byte ranges.
    std::string accept_ranges =
        response.HttpHeaderField("Accept-Ranges").Utf8();
    if (accept_ranges.find("bytes") != std::string::npos) {
      destination_url_data->set_range_supported();
    }

    // If we have verified the partial response and it is correct.
    // It's also possible for a server to support range requests
    // without advertising "Accept-Ranges: bytes".
    if (partial_response &&
        VerifyPartialResponse(response, destination_url_data)) {
      destination_url_data->set_range_supported();
    } else if (ok_response) {
      // 200 ok represents not support range request,
      // fallback to ResourceMultiBufferDataProvider
      LOG(INFO) << "VideoOpt: " << __func__ << " receive 200 ok, fallback"
                << AsHumanReadableString();
      ResetLoader();
      provider_->Fallback();
      return;
    } else if (response.HttpStatusCode() == kHttpRangeNotSatisfiable) {
      // Unsatisfiable range
      // Really, we should never request a range that doesn't exist, but
      // if we do, let's handle it in a sane way.
      // Note, we can't just call OnDataProviderEvent() here, because
      // url_data_ hasn't been updated to the final destination yet.
      end_of_file = true;
      LOG(INFO) << "VideoOpt: " << __func__ << " receive 416"
                << AsHumanReadableString();
    } else {
      // Can't call fail until readers have been migrated to the new
      // url data below.
      LOG(INFO) << "VideoOpt: " << __func__ << "receive "
                << response.HttpStatusCode() << ", fallback"
                << AsHumanReadableString();
      ResetLoader();
      provider_->Fallback();
      return;
    }
  } else {
    // Shouldn't happen
    NOTREACHED();
  }

  destination_url_data =
      provider_->urlData()->url_index()->TryInsert(destination_url_data);

  // This is vital for security!
  destination_url_data->set_is_cors_cross_origin(
      network::cors::IsCorsCrossOriginResponseType(response_type));
  destination_url_data->SetMediaWebURLErrorCB(
      provider_->urlData()->GetMediaWebURLErrorCB());

  // Only used for metrics.
  {
    WebString access_control =
        response.HttpHeaderField("Access-Control-Allow-Origin");
    if (!access_control.IsEmpty() && !access_control.Equals("null")) {
      // Note: When |access_control| is not *, we should verify that it matches
      // the requesting origin. Instead we just assume that it matches, which is
      // probably accurate enough for metrics.
      destination_url_data->set_has_access_control();
    }

    destination_url_data->set_mime_type(response.MimeType().Utf8());
  }

  if (destination_url_data != provider_->urlData()) {
    LOG(INFO) << "VideoOpt: " << __func__ << " url_data != destination_url_data"
              << AsHumanReadableString();

    // At this point, we've encountered a redirect, or found a better url data
    // instance for the data that we're about to download.

    // First, let's take a ref on the current url data.
    scoped_refptr<UrlData> old_url_data(provider_->urlData());
    destination_url_data->Use();

    // Take ownership of ourselves. (From the multibuffer)
    std::unique_ptr<MultiBuffer::DataProvider> self(
        provider_->urlData()->multibuffer()->RemoveProvider(provider_));
    provider_->SetUrlData(destination_url_data.get());
    // Give the ownership to our new owner.
    provider_->urlData()->multibuffer()->AddProvider(std::move(self));

    // Call callback to let upstream users know about the transfer.
    // This will merge the data from the two multibuffers and
    // cause clients to start using the new UrlData.
    old_url_data->RedirectTo(destination_url_data);
  }

  // Get the response URL since it can differ from the request URL when a
  // service worker provided the response. Normally we would just use
  // ResponseUrl(), but ResourceMultiBufferDataProvider disallows mixing
  // constructed responses (new Response()) and native server responses, even if
  // they have the same response URL.
  KURL response_url;
  if (!response.WasFetchedViaServiceWorker() ||
      response.HasUrlListViaServiceWorker()) {
    response_url = response.ResponseUrl();
  }

  // This test is vital for security!
  if (!provider_->urlData()->ValidateDataOrigin(response_url)) {
    ResetLoader();
    provider_->urlData()->Fail();
    return;  // "this" may be deleted now.
  }

  if (end_of_file) {
    fifo_ptr_->push_back(media::DataBuffer::CreateEOSBuffer());
    provider_->urlData()->multibuffer()->OnDataProviderEvent(provider_);
  }
}

void VideoRangeURLLoaderClient::DidReceiveData(base::span<const char> data) {
  DVLOG(1) << "VideoOpt: didReceiveData: " << data.size() << " bytes";
  DCHECK(active_loader_);
  DCHECK_GT(data.size(), 0u);

  if (bytes_to_discard_) {
    uint64_t tmp = std::min<uint64_t>(bytes_to_discard_, data.size());
    data = data.subspan(static_cast<size_t>(tmp));
    bytes_to_discard_ -= tmp;
    if (data.empty()) {
      return;
    }
  }

  // When we receive data, we allow more retries.
  retries_ = 0;

  while (!data.empty()) {
    if (fifo_ptr_->empty() ||
        fifo_ptr_->back()->data_size() == provider_->block_size()) {
      fifo_ptr_->push_back(new media::DataBuffer(provider_->block_size()));
      fifo_ptr_->back()->set_data_size(0);
    }

    int last_block_size = fifo_ptr_->back()->data_size();
    auto to_append =
        std::min<int64_t>(data.size(), provider_->block_size() - last_block_size);
    DCHECK_GT(to_append, 0);
    errno_t result =
        memcpy_s(fifo_ptr_->back()->writable_data() + last_block_size,
                 static_cast<size_t>(to_append), data.data(), static_cast<size_t>(to_append));
    if (result != EOK) {
      LOG(INFO) << "VideoOpt: " << __func__
                << " memcpy_s failed, errno = " << result
                << AsHumanReadableString();
      return;
    }
    data = data.subspan(static_cast<size_t>(to_append));
    fifo_ptr_->back()->set_data_size(static_cast<int>(last_block_size + to_append));

    byte_pos_ += to_append;
    if (last_block_size + to_append == provider_->block_size()) {
      received_++;
    }
  }

  if (ProviderInThisRange()) {
    provider_->urlData()->multibuffer()->OnDataProviderEvent(provider_);
  }
}

void VideoRangeURLLoaderClient::DidFinishLoading() {
  DVLOG(1) << "didFinishLoading";
  DCHECK(active_loader_.get());

  // We're done with the loader.
  ResetLoader();

  // If we didn't know the |instance_size_| we do now.
  int64_t pos = byte_pos();

  // This request reports something smaller than what we've seen in the past,
  // Maybe it's transient error?
  if (provider_->urlData()->length() != kPositionNotSpecified &&
      ((end_ == -1 && pos < provider_->urlData()->length()) ||
       (end_ != -1 && end_ < provider_->urlData()->length() && pos <= end_))) {
    LOG(INFO) << "VideoOpt: " << __func__ << " pos = " << pos << "; end = " << end_
              << "; retries = " << retries_ << AsHumanReadableString();
    if (retries_ < kMaxRetries) {
      DVLOG(1) << " Partial data received.... @ pos = " << pos;
      retries_++;
      task_runner_->PostDelayedTask(
          FROM_HERE,
          base::BindOnce(&VideoRangeURLLoaderClient::Start,
                         weak_factory_.GetWeakPtr()),
          base::Milliseconds(kLoaderPartialRetryDelayMs));
      return;
    } else {
      provider_->urlData()->Fail();
      return;  // "this" may be deleted now.
    }
  }

  fifo_ptr_->push_back(media::DataBuffer::CreateEOSBuffer());
  provider_->urlData()->multibuffer()->OnDataProviderEvent(provider_);

  // Beware, this object might be deleted here.
}

void VideoRangeURLLoaderClient::SetDefersLoading(bool deferred) {
  if (active_loader_) {
    active_loader_->SetDefersLoading(deferred);
  }
}

bool VideoRangeURLLoaderClient::VerifyPartialResponse(
    const WebURLResponse& response,
    const scoped_refptr<UrlData>& url_data) {
  int64_t first_byte_position;
  int64_t last_byte_position;
  int64_t instance_size;
  if (!ParseContentRange(response.HttpHeaderField("Content-Range").Utf8(),
                         &first_byte_position, &last_byte_position,
                         &instance_size)) {
    return false;
  }

  if (url_data->length() == kPositionNotSpecified) {
    url_data->set_length(instance_size);
    provider_->NotifyLengthSet();
  }

  if (first_byte_position > byte_pos()) {
    return false;
  }
  if (last_byte_position + 1 < byte_pos()) {
    return false;
  }
  bytes_to_discard_ = byte_pos() - first_byte_position;

  return true;
}

bool VideoRangeURLLoaderClient::ParseContentRange(
    const std::string& content_range_str,
    int64_t* first_byte_position,
    int64_t* last_byte_position,
    int64_t* instance_size) {
  const char kUpThroughBytesUnit[] = "bytes ";
  if (!base::StartsWith(content_range_str, kUpThroughBytesUnit,
                        base::CompareCase::SENSITIVE)) {
    return false;
  }
  std::string range_spec =
      content_range_str.substr(sizeof(kUpThroughBytesUnit) - 1);
  size_t dash_offset = range_spec.find("-");
  size_t slash_offset = range_spec.find("/");

  if (dash_offset == std::string::npos || slash_offset == std::string::npos ||
      slash_offset < dash_offset || slash_offset + 1 == range_spec.length()) {
    return false;
  }
  if (!base::StringToInt64(range_spec.substr(0, dash_offset),
                           first_byte_position) ||
      !base::StringToInt64(
          range_spec.substr(dash_offset + 1, slash_offset - dash_offset - 1),
          last_byte_position)) {
    return false;
  }
  if (slash_offset == range_spec.length() - 2 &&
      range_spec[slash_offset + 1] == '*') {
    *instance_size = kPositionNotSpecified;
  } else {
    if (!base::StringToInt64(range_spec.substr(slash_offset + 1),
                             instance_size)) {
      return false;
    }
  }
  if (*last_byte_position < *first_byte_position ||
      (*instance_size != kPositionNotSpecified &&
       *last_byte_position >= *instance_size)) {
    return false;
  }

  return true;
}


void VideoRangeURLLoaderClient::Terminate() {
  fifo_ptr_->push_back(media::DataBuffer::CreateEOSBuffer());
  provider_->urlData()->multibuffer()->OnDataProviderEvent(provider_);
}

void VideoRangeURLLoaderClient::DidFail(const blink::WebURLError& error) {
  LOG(INFO) << "VideoOpt didFail: reason=" << error.reason();
  DCHECK(active_loader_.get());
  ResetLoader();

  if (retries_ < kMaxRetries && byte_pos() != 0) {
    retries_++;
    task_runner_->PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&VideoRangeURLLoaderClient::Start,
                       weak_factory_.GetWeakPtr()),
        base::Milliseconds(kLoaderFailedRetryDelayMs +
                           kAdditionalDelayPerRetryMs * retries_));
  } else {
    // We don't need to continue loading after failure.
    // Note that calling Fail() will most likely delete this object.
    provider_->urlData()->NotifyMediaWebURLError(error.reason());
    provider_->urlData()->Fail();
    LOG(INFO) << "VideoOpt" 
              << "(hash" << std::hex << base::FastHash(base::byte_span_from_ref(this)) << ")"
              << " call url_data Fail";
  }
}

void VideoRangeURLLoaderClient::DidReStart() {
  LOG(INFO) << "VideoOpt, VideoRangeURLLoaderClient DidReStart";
  task_runner_->PostTask(FROM_HERE,
                         base::BindOnce(&VideoRangeURLLoaderClient::Start,
                                        weak_factory_.GetWeakPtr()));
}
}  // namespace blink

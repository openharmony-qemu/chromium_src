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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_VIDEO_COMMON_DATA_PROVIDER_H
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_VIDEO_COMMON_DATA_PROVIDER_H

#include <memory>
#include <string>

#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "third_party/blink/public/platform/web_url_request.h"
#include "third_party/blink/public/web/web_associated_url_loader.h"
#include "third_party/blink/public/web/web_associated_url_loader_client.h"
#include "third_party/blink/public/web/web_frame.h"
#include "third_party/blink/renderer/platform/media/multi_buffer.h"
#include "third_party/blink/renderer/platform/media/url_index.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"

namespace blink {

// The number of milliseconds to wait before retrying when the server
// decides to not give us all the data at once.
const int kLoaderPartialRetryDelayMs = 25;

// The number of milliseconds to wait before retrying a failed load.
const int kLoaderFailedRetryDelayMs = 250;

// Each retry, add this many MS to the delay.
// total delay is:
// (kLoaderPartialRetryDelayMs +
//  kAdditionalDelayPerRetryMs * (kMaxRetries - 1) / 2) * kMaxretries = 29250 ms
const int kAdditionalDelayPerRetryMs = 50;

const int kHttpOK = 200;
const int kHttpPartialContent = 206;
const int kHttpRangeNotSatisfiable = 416;

class VideoCommonDataProvider : public MultiBuffer::DataProvider {
 public:
  VideoCommonDataProvider(UrlData* url_data,
                       MultiBufferBlockId pos,
                       std::string id,
                       bool is_client_audio_element,
                       scoped_refptr<base::SingleThreadTaskRunner> task_runner);

  virtual void Start() = 0;

  void Fallback();  // this function must be follwed by return;

  // Prepare the request.
  WebURLRequest PrepareRequest(int64_t start, int64_t end);

  void SetUrlData(UrlData*);
  UrlData* urlData() const;
  const KURL& Origin() const;
  UrlData::CorsMode CorsMode() const;
  virtual void NotifyLengthSet() {}

  int64_t block_size() const;
  int32_t block_size_shift() const;

  // MultiBuffer::DataProvider implementation
  MultiBufferBlockId Tell() const override;

 protected:
  friend class MultiBufferDataSourceTest;

  // Current Position.
  MultiBufferBlockId pos_;

  // video's id property
  std::string id_;

  // This is where we actually get read data from.
  // We don't need (or want) a scoped_refptr for this one, because
  // we are owned by it. Note that we may change this when we encounter
  // a redirect because we actually change ownership.
  raw_ptr<UrlData> url_data_;

  // Copy of url_data_->cors_mode()
  // const to make it obvious that redirects cannot change it.
  const UrlData::CorsMode cors_mode_;

  // The origin for the initial request.
  // const to make it obvious that redirects cannot change it.
  const KURL origin_;

  // Is the client an audio element?
  bool is_client_audio_element_ = false;

  const scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
};

class VideoRangeURLLoaderClient : public WebAssociatedURLLoaderClient {
 public:
  // NUmber of times we'll retry if the connection fails.
  enum { kMaxRetries = 30 };

  VideoRangeURLLoaderClient(
      VideoCommonDataProvider* provider,
      int64_t start,
      int64_t end,
      std::string id,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner);
  ~VideoRangeURLLoaderClient() override;

  std::string AsHumanReadableString() const;
  std::pair<int64_t, int64_t> Range();
  bool ProviderInThisRange();
  void SetFifo(std::list<scoped_refptr<media::DataBuffer>>*);
  void Start();
  int64_t byte_pos() const;
  void SetDefersLoading(bool deferred);
  void ResetLoader();

  // WebAssociatedURLLoaderClient implementation.
  bool WillFollowRedirect(const WebURL& new_url,
                          const WebURLResponse& redirect_response) override;
  void DidSendData(uint64_t bytesSent, uint64_t totalBytesToBeSent) override;
  void DidReceiveResponse(const WebURLResponse& response) override;
  void DidDownloadData(uint64_t data_length) override;
  void DidReceiveData(base::span<const char>) override;
  void DidFinishLoading() override;
  void DidFail(const WebURLError&) override;
  void DidReStart() override;

 private:
  // Callback used when we're asked to fetch data after the end of the file.
  void Terminate();

  // Parse a Content-Range header into its component pieces and return true if
  // each of the expected elements was found & parsed correctly.
  // |*instance_size| may be set to kPositionNotSpecified if the range ends in
  // "/*".
  // NOTE: only public for testing!  This is an implementation detail of
  // VerifyPartialResponse (a private method).
  static bool ParseContentRange(const std::string& content_range_str,
                                int64_t* first_byte_position,
                                int64_t* last_byte_position,
                                int64_t* instance_size);

  // If we have made a range request, verify the response from the server.
  bool VerifyPartialResponse(const WebURLResponse& response,
                             const scoped_refptr<UrlData>& url_data);

  raw_ptr<VideoCommonDataProvider> provider_;
  // Keeps track of an active WebAssociatedURLLoader.
  // Only valid while loading resource.
  std::unique_ptr<WebAssociatedURLLoader> active_loader_;
  uint16_t received_ = 0;
  int64_t start_;
  int64_t end_;
  std::string id_;
  int64_t byte_pos_;

  // Temporary storage for incoming data.
  std::list<scoped_refptr<media::DataBuffer>>* fifo_ptr_;

  // How many retries have we done at the current position.
  int retries_ = 0;

  // If the server tries to gives us more bytes than we want, this how
  // many bytes we need to discard before we get to the right place.
  uint64_t bytes_to_discard_ = 0;

  // When we encounter a redirect, this is the source of the redirect.
  KURL redirects_to_;

  const scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

  base::WeakPtrFactory<VideoRangeURLLoaderClient> weak_factory_{this};
};

}  // namespace blink

#endif // THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_VIDEO_COMMON_DATA_PROVIDER_H
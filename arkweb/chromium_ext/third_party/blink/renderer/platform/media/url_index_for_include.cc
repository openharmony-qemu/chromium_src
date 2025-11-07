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

namespace blink {

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
MediaWebURLErrorCB& UrlData::GetMediaWebURLErrorCB() {
  return media_url_error_cb_;
}

void UrlData::SetMediaWebURLErrorCB(MediaWebURLErrorCB cb) {
  media_url_error_cb_ = cb;
}

void UrlData::NotifyMediaWebURLError(int reason) {
  if (!media_url_error_cb_.is_null()) {
    media_url_error_cb_.Run(reason);
  }
}
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE


#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
std::unique_ptr<MultiBuffer::DataProvider> ResourceMultiBuffer::CreateWriter(
    const MultiBufferBlockId& pos,
    bool is_client_audio_element,
    int32_t preload_size,
    int32_t request_size,
    uint16_t byte_rate,
    std::string id) {
  if (!url_data_) {
    LOG(INFO) << "VideoOpt: CreateWriter url_data_=null";
    return nullptr;
  }
 
  LOG(INFO) << "VideoOpt: CreateWriter pos=" << pos
            << url_data_->CreateWriterInfo();
  if (url_data_->IsCreateSegmentationProvider()) {
    LOG(INFO) << "VideoOpt: CreateWriter SegmentationMultiBufferDataProvider"
              << ", pos:" << pos << ", preload_size:" << preload_size
              << ", request_size:" << request_size << ", byte_rate:" << byte_rate;
    auto writer = std::make_unique<SegmentationMultiBufferDataProvider>(
        url_data_, pos, preload_size, request_size, byte_rate, id,
        is_client_audio_element, task_runner_);
    writer->Start();
    return writer;
  }
 
  auto writer = std::make_unique<ResourceMultiBufferDataProvider>(
      url_data_, pos, is_client_audio_element, task_runner_);
  writer->Start();
  return writer;
}

bool UrlData::IsCreateSegmentationProvider() {
  bool fitted = false;
  if (url_index_) {
    fitted = url_index_->IsNewsFeedPageFitted();
  }

  if (fitted && !IsNeedFallback() && url().ProtocolIsInHTTPFamily()) {
    return true;
  }

  return false;
}

std::string UrlData::CreateWriterInfo() {
  std::ostringstream s;
 
  bool fitted = false;
  if (url_index_) {
    fitted = url_index_->IsNewsFeedPageFitted();
  }
 
  s << "; useLoadOptimization = " << fitted
    << "; IsNeedFallback = " << IsNeedFallback()
    << "; IsHttp(s)= " << url().ProtocolIsInHTTPFamily();
 
  return s.str();
}

bool UrlData::IsNeedFallback() {
  return need_fall_back_;
}

void UrlData::SetNeedFallback() {
  need_fall_back_ = true;
  LOG(INFO) << "VideoOpt: " << __func__
            << "(hash" << std::hex << base::FastHash(base::byte_span_from_ref(this)) << ")";
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

}
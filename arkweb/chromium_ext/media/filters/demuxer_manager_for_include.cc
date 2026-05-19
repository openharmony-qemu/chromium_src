/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
namespace media {
 
const std::string DemuxerManager::GetMimeType() const {
  std::string mime_type;
  if (data_source_) {
    auto* co_data_source = data_source_->GetAsCrossOriginDataSource();
    if (co_data_source) {
      mime_type = co_data_source->GetMimeType();
    }
  }
  return mime_type;
}
 
int32_t DemuxerManager::GetByteRate() const {
  if (media_info_) {
    return media_info_->byte_rate_;
  }
  return 0;
}
 
int64_t DemuxerManager::GetFileSize() const {
  if (media_info_) {
    return media_info_->file_size_;
  }
  return 0LL;
}
 
const std::vector<std::string> DemuxerManager::GetAudioCodecs() const {
  if (media_info_) {
    return media_info_->audio_codecs_;
  }
  return {};
}
 
const std::vector<std::string> DemuxerManager::GetVideoCodecs() const {
  if (media_info_) {
    return media_info_->video_codecs_;
  }
  return {};
}
 
} 
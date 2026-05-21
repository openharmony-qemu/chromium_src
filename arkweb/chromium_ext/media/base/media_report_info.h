
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
 
#ifndef MEDIA_BASE_MEDIA_REPORT_INFO_H_
#define MEDIA_BASE_MEDIA_REPORT_INFO_H_
 
#include <string>
#include <vector>
 
#include "base/memory/ref_counted.h"
#include "media/base/media_export.h"
 
namespace media {
 
struct MEDIA_EXPORT MediaReportInfo final : base::RefCounted<MediaReportInfo> {
    MediaReportInfo();
    MediaReportInfo(const MediaReportInfo&) = delete;
    MediaReportInfo operator=(const MediaReportInfo&) = delete;
 
    std::vector<std::string> audio_codecs_;
    std::vector<std::string> video_codecs_;
    int byte_rate_{0};
    int64_t file_size_{0};
 
private:
    friend class base::RefCounted<MediaReportInfo>;
    ~MediaReportInfo();
};
 
} // namespace media
#endif // MEDIA_BASE_MEDIA_REPORT_INFO_H_ 
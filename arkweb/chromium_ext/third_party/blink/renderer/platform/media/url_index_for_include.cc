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
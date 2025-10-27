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

void SetAcceptTypes(std::vector<std::u16string> types) override {
  accept_types_ = std::move(types);
}

void SetUseMediaCapture(bool use_media_capture) override {
  use_media_capture_ = use_media_capture;
}

void SetOpenWritable(bool open_writable) override {
  open_writable_ = open_writable;
}

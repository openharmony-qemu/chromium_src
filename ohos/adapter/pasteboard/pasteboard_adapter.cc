// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/pasteboard/pasteboard_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "aki/value.h"
#include "aki/value/array_buffer.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/permission_manager/permission_manager_adapter.h"

namespace ohos {
namespace adapter {
const int kFourChannel = 4;
const int kThreeChannel = 3;

template <class T>
T GetValue(aki::Value aki_value, T default_value) {
  if (!(aki_value.IsNull() || aki_value.IsUndefined())) {
    return aki_value.As<T>();
  }
  return default_value;
}

bool RequestPermission() {
  return permission::PermissionManagerAdapter::RequestPermission(
    permission::OHOSPermissionType::PASTEBOARD);
}

PasteBoardAdapter& PasteBoardAdapter::GetInstance() {
  static PasteBoardAdapter pasteboardAdapter;
  return pasteboardAdapter;
}

void PasteBoardAdapter::SetPasteData(PasteDataRecord record) {
  if (auto insert =
          ohos::adapter::GetJSFunction("PasteBoardAdapter.SetPasteData")) {
    aki::ArrayBuffer array_buffer((uint8_t*)record.bookmark_data.c_str(), record.bookmark_data.size());
    insert->Invoke<void>(record, array_buffer);
  }
}

std::string GetAkiResult(const std::string& function_name) {
  if (auto insert = ohos::adapter::GetJSFunction(function_name)) {
    std::promise<std::string> insert_promise;
    std::function<void(std::string)> callback =
        [&insert_promise](std::string message) {
          insert_promise.set_value(message);
        };
    insert->Invoke<void>(callback);
    std::string message = insert_promise.get_future().get();  // Sub-thread blocking
    return message;
  }
  return "";
}

int GetAkiNumberResult(const std::string& function_name) {
  if (auto insert = ohos::adapter::GetJSFunction(function_name)) {
    std::promise<int> insert_promise;
    std::function<void(int)> callback = [&insert_promise](int num) {
      insert_promise.set_value(num);
    };
    insert->Invoke<void>(callback);
    return insert_promise.get_future().get();
  }
  return 0;
}

std::string PasteBoardAdapter::ReadText() {
  if (!RequestPermission()) {
    LOGE("PasteBoardAdapter requestPermission denied");
    return "";
  }
  return GetAkiResult("PasteBoardAdapter.ReadPasteBoardText");
}

std::string PasteBoardAdapter::ReadHTML() {
  if (!RequestPermission()) {
    LOGE("PasteBoardAdapter requestPermission denied");
    return "";
  }
  return GetAkiResult("PasteBoardAdapter.ReadPasteBoardHTML");
}

std::vector<std::string> PasteBoardAdapter::ReadFilenames() {
  std::vector<std::string> result;
  if (!RequestPermission()) {
    LOGE("PasteBoardAdapter requestPermission denied");
    return result;
  }
  if (auto insert =
          ohos::adapter::GetJSFunction("PasteBoardAdapter.ReadPasteBoardURI")) {
    std::promise<std::vector<std::string>> insert_promise;
    std::function<void(std::vector<std::string>)> callback =
        [&insert_promise](std::vector<std::string> file_paths) {
          insert_promise.set_value(file_paths);
        };
    insert->Invoke<void>(callback);
    result = insert_promise.get_future().get();
  }
  return result;
}

void PasteBoardAdapter::Clear() {
  if (auto insert = ohos::adapter::GetJSFunction("PasteBoardAdapter.Clear")) {
    insert->Invoke<void>();
  }
}

bool PasteBoardAdapter::IsExistMimeType(const std::string& type) {
  if (!RequestPermission()) {
    LOGE("PasteBoardAdapter requestPermission denied");
    return false;
  }
  if (auto insert =
          ohos::adapter::GetJSFunction("PasteBoardAdapter.IsExistMimeType")) {
    std::promise<bool> insert_promise;
    std::function<void(bool)> callback = [&insert_promise](bool ret) {
      insert_promise.set_value(ret);
    };
    insert->Invoke<void>(callback, type);
    return insert_promise.get_future().get();
  }
  return false;
}

void ClipBoardImageData::ReadImage() {
  if (!RequestPermission()) {
    LOGE("PasteBoardAdapter requestPermission denied");
    return;
  }
  if (auto insert =
          ohos::adapter::GetJSFunction("PasteBoardAdapter.ReadImageInfo")) {
    std::function<void(aki::Value)> callback = [&](aki::Value result) {
      color_type = GetValue<ohos::adapter::ClipBoardImageColorType>(
          result["pixelMapFormat"],
          ohos::adapter::ClipBoardImageColorType::COLOR_TYPE_UNKNOWN);
      alpha_type = GetValue<ohos::adapter::ClipBoardImageAlphaType>(
          result["alphaType"],
          ohos::adapter::ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN);
      width = GetValue<int>(result["width"], 0);
      height = GetValue<int>(result["height"], 0);
      row_bytes = GetValue<int>(result["rowBytes"], 0);
    };
    insert->Invoke<void>(callback);
  }

  if (auto insert =
          ohos::adapter::GetJSFunction("PasteBoardAdapter.ReadImageBuff")) {
    std::promise<aki::ArrayBuffer> insert_promise;
    std::function<void(aki::ArrayBuffer)> callback =
        [&insert_promise](aki::ArrayBuffer buff) {
          insert_promise.set_value(buff);
        };
    insert->Invoke<void>(callback);
    aki::ArrayBuffer buffer = insert_promise.get_future().get();
    data = std::unique_ptr<uint8_t>(buffer.GetData());
  }
}

ClipBoardImageData PasteBoardAdapter::ReadImage() {
  ClipBoardImageData image_data;
  image_data.ReadImage();
  return image_data;
}

int PasteBoardAdapter::GetColorChannelNum(
    ClipBoardImageColorType pixel_format) {
  int channel_number = 0;
  switch (pixel_format) {
    case ohos::adapter::ClipBoardImageColorType::COLOR_TYPE_RGBA_8888:
    case ohos::adapter::ClipBoardImageColorType::COLOR_TYPE_BGRA_8888:
      channel_number = kFourChannel;
      return channel_number;
    case ohos::adapter::ClipBoardImageColorType::COLOR_TYPE_RGB_565:
    case ohos::adapter::ClipBoardImageColorType::COLOR_TYPE_RGB_888:
      channel_number = kThreeChannel;
      return channel_number;
    default:
      return channel_number;
  }
}

void PasteBoardAdapter::WriteImage(uint8_t* data,
                                   int32_t height,
                                   int32_t width,
                                   ClipBoardImageColorType pixel_format,
                                   ClipBoardImageAlphaType alpha_type) {
  if (data == nullptr) {
    LOGE("[PasteBoardAdapter] WriteImage data is null.");
    return;
  }
  if (GetColorChannelNum(pixel_format) == 0) {
    LOGE("[PasteBoardAdapter] WriteImage unsupported format.");
    return;
  }
  if (auto insert =
          ohos::adapter::GetJSFunction("PasteBoardAdapter.WriteImage")) {
    uint32_t buffer_size = height * width * GetColorChannelNum(pixel_format);
    aki::ArrayBuffer buff(data, buffer_size);
    insert->Invoke<void>(buff, height, width, pixel_format, alpha_type);
  }
}

void PasteBoardAdapter::WriteData(const std::string& format,
                                  const char* data_data,
                                  size_t data_len) {
  if (auto insert = ohos::adapter::GetJSFunction("PasteBoardAdapter.WriteData")) {
    if (data_data) {
      aki::ArrayBuffer array_buffer((uint8_t*)data_data, data_len);
      insert->Invoke<void>(array_buffer, format);
    } else {
      LOGE("[PasteBoardAdapter] WriteData data_data is null.");
    }
  }
}

std::string PasteBoardAdapter::ReadData(const std::string& type) {
  std::string str;
  if (!RequestPermission()) {
    LOGE("PasteBoardAdapter requestPermission denied");
    return str;
  }
  if (auto insert =
          ohos::adapter::GetJSFunction("PasteBoardAdapter.ReadPasteBoardData")) {
    std::promise<aki::ArrayBuffer> insert_promise;
    std::function<void(aki::ArrayBuffer)> callback =
        [&insert_promise](aki::ArrayBuffer buff) {
          insert_promise.set_value(buff);
        };
    insert->Invoke<void>(callback, type);
    aki::ArrayBuffer buffer = insert_promise.get_future().get();
    if (!buffer.GetData()) {
      LOGE("[PasteBoardAdapter] ReadData get buffer data is null.");
      return str;
    }
    uint8_t* data = reinterpret_cast<uint8_t*>(buffer.GetData());
    if (!data || buffer.GetLength() == 0) {
      LOGE("[PasteBoardAdapter] ReadData data is null.");
      return str;
    }
    str = std::string(reinterpret_cast<const char*>(data), buffer.GetLength());
  }
  return str;
}

JSBIND_CLASS(PasteDataRecord) {
  JSBIND_PROPERTY(html_text);
  JSBIND_PROPERTY(mime_type);
  JSBIND_PROPERTY(plain_text);
}

}  // namespace adapter
}  // namespace ohos

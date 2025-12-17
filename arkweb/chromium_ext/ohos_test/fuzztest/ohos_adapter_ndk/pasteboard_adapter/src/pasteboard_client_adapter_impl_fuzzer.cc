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
#define private public
#include "arkweb/ohos_adapter_ndk/pasteboard_adapter/include/pasteboard_client_adapter_impl.h"
#undef private

#include <fuzzer/FuzzedDataProvider.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>

using namespace OHOS::NWeb;

class MockClipBoardImageDataAdapter : public ClipBoardImageDataAdapter {
 public:
  MockClipBoardImageDataAdapter() = default;

  ClipBoardImageColorType GetColorType() override { return colorType; }

  ClipBoardImageAlphaType GetAlphaType() override { return alphaType; }

  uint32_t* GetData() override { return data; }

  size_t GetDataSize() override { return dataSize; }

  size_t GetRowBytes() override { return rowBytes; }

  int32_t GetWidth() override { return width; }

  int32_t GetHeight() override { return height; }

  void SetColorType(ClipBoardImageColorType color) override {
    colorType = color;
  }

  void SetAlphaType(ClipBoardImageAlphaType alpha) override {
    alphaType = alpha;
  }

  void SetData(uint32_t* d) override { data = d; }

  void SetDataSize(size_t size) override { dataSize = size; }

  void SetRowBytes(size_t r) override { rowBytes = r; }

  void SetWidth(int32_t w) override { width = w; }

  void SetHeight(int32_t h) override { height = h; }

  ClipBoardImageColorType colorType;
  ClipBoardImageAlphaType alphaType;
  uint32_t* data;
  size_t dataSize;
  size_t rowBytes;
  int32_t width;
  int32_t height;
};

class MockPasteboardObserver : public PasteboardObserverAdapter {
 public:
  MockPasteboardObserver() = default;
  void OnPasteboardChanged() override {}
};

void PasteDataRecordAdapterImplFuzzTest(FuzzedDataProvider* fdp) {
  std::string mimeType = fdp->ConsumeRandomLengthString(256);
  std::string html = fdp->ConsumeRandomLengthString(256);
  std::shared_ptr<std::string> htmlText =
      std::make_shared<std::string>(fdp->ConsumeRandomLengthString(256));
  std::shared_ptr<std::string> plainText =
      std::make_shared<std::string>(fdp->ConsumeRandomLengthString(256));
  std::string uriString = fdp->ConsumeRandomLengthString(256);
  std::string format = fdp->ConsumeRandomLengthString(256);
  std::vector<uint8_t> data = {fdp->ConsumeIntegralInRange<uint8_t>(0, 10),
                               fdp->ConsumeIntegralInRange<uint8_t>(0, 10),
                               fdp->ConsumeIntegralInRange<uint8_t>(0, 10)};
  PasteCustomData customData;
  uint32_t storage[][5] = {
      {0xCA, 0xDA, 0xCA, 0xC9, 0xA3}, {0xAC, 0xA8, 0x89, 0x47, 0x87},
      {0x4B, 0x25, 0x25, 0x25, 0x46}, {0x90, 0x1, 0x25, 0x41, 0x33},
      {0x75, 0x55, 0x44, 0x20, 0x00},
  };

  std::shared_ptr<MockClipBoardImageDataAdapter> image =
      std::make_shared<MockClipBoardImageDataAdapter>();
  std::shared_ptr<MockClipBoardImageDataAdapter> imageGot =
      std::make_shared<MockClipBoardImageDataAdapter>();
  ClipBoardImageColorType array[] = {
      ClipBoardImageColorType::COLOR_TYPE_RGBA_8888,
      ClipBoardImageColorType::COLOR_TYPE_BGRA_8888,
      ClipBoardImageColorType::COLOR_TYPE_UNKNOWN};
  ClipBoardImageAlphaType array2[] = {
      ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE,
      ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED,
      ClipBoardImageAlphaType::ALPHA_TYPE_POSTMULTIPLIED,
      ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN};

  std::shared_ptr<PasteDataRecordAdapter> impl =
      PasteDataRecordAdapter::NewRecord(mimeType, htmlText, plainText);

  impl->GetMimeType();

  impl->GetHtmlText();

  impl->GetPlainText();

  impl->GetUri();

  impl->GetCustomData();

  impl->SetHtmlText(htmlText);

  impl->SetPlainText(plainText);

  impl->SetUri("");

  impl->SetUri(uriString);

  impl->SetCustomData(customData);

  customData.insert(std::make_pair(format, data));
  impl->SetCustomData(customData);

  impl->SetImgData(nullptr);
  impl->GetImgData(imageGot);

  impl->SetImgData(image);
  impl->GetImgData(imageGot);

  image->SetWidth(5);
  impl->SetImgData(image);
  impl->GetImgData(imageGot);

  image->SetHeight(5);
  impl->SetImgData(image);
  impl->GetImgData(imageGot);

  image->SetColorType(fdp->PickValueInArray<ClipBoardImageColorType>(array));
  impl->SetImgData(image);
  impl->GetImgData(imageGot);

  image->SetAlphaType(fdp->PickValueInArray<ClipBoardImageAlphaType>(array2));
  impl->SetImgData(image);
  impl->GetImgData(imageGot);

  image->SetData(storage[0]);
  image->SetDataSize(sizeof(storage));
  image->SetRowBytes(5);
  image->SetHeight(5);
  impl->SetImgData(image);
  impl->GetImgData(imageGot);

  impl->GetMimeType();

  impl->GetHtmlText();

  impl->GetPlainText();

  impl->GetImgData(nullptr);

  impl->GetUri();

  impl->GetCustomData();

  PasteDataRecordAdapterImpl impl2(nullptr, std::shared_ptr<OH_UdmfData>());

  impl2.HtmlToPlainText(html);

  impl2.GetMimeType();

  impl2.GetHtmlText();

  impl2.GetPlainText();

  impl2.GetImgData(imageGot);

  impl2.GetUri();

  impl2.GetCustomData();

  impl2.Clear();

  impl2.GetRecord();
}

void PasteDataAdapterImplFuzzTest(FuzzedDataProvider* fdp) {
  std::string html = fdp->ConsumeRandomLengthString(256);
  std::string text = fdp->ConsumeRandomLengthString(256);
  std::size_t index = fdp->ConsumeIntegralInRange<std::size_t>(0, 10);

  std::shared_ptr<PasteDataAdapterImpl> impl =
      std::make_shared<PasteDataAdapterImpl>();

  impl->GetPrimaryMimeType();

  impl->GetRecordAt(index);

  impl->AllRecords();

  impl->AddHtmlRecord(html);

  impl->AddTextRecord(text);

  impl->GetMimeTypes();

  impl->GetPrimaryHtml();

  impl->GetPrimaryText();

  impl->GetRecordCount();

  std::shared_ptr<PasteDataAdapterImpl> impl2 =
      std::make_shared<PasteDataAdapterImpl>(nullptr);

  impl2->AddHtmlRecord(html);

  impl2->AddTextRecord(text);

  impl2->GetMimeTypes();

  impl2->GetPrimaryHtml();

  impl2->GetPrimaryText();

  impl2->GetPrimaryMimeType();

  impl2->GetRecordAt(index);

  impl2->GetRecordCount();

  impl2->AllRecords();
}

void PasteBoardClientAdapterImplFuzzTest(FuzzedDataProvider* fdp) {
  PasteRecordVector data;
  std::string mimeType = fdp->ConsumeRandomLengthString(256);
  std::shared_ptr<PasteDataRecordAdapter> record =
      PasteDataRecordAdapter::NewRecord(mimeType);
  std::shared_ptr<std::string> htmlText =
      std::make_shared<std::string>(fdp->ConsumeRandomLengthString(256));
  record->SetHtmlText(htmlText);
  CopyOptionMode array[] = {CopyOptionMode::IN_APP,
                            CopyOptionMode::LOCAL_DEVICE,
                            CopyOptionMode::CROSS_DEVICE,
                            CopyOptionMode::NONE};
  CopyOptionMode copyOption = fdp->PickValueInArray<CopyOptionMode>(array);
  std::string path = fdp->ConsumeRandomLengthString(256);
  std::shared_ptr<PasteboardObserverAdapter> observer =
      std::make_shared<MockPasteboardObserver>();

  PasteBoardClientAdapterImpl& impl =
      PasteBoardClientAdapterImpl::GetInstance();

  impl.TransitionCopyOption(copyOption);

  impl.GetPasteData(data);

  impl.HasPasteData();

  impl.Clear();

  data.push_back(record);
  data.push_back(nullptr);
  impl.SetPasteData(data, copyOption);

  impl.GetPasteData(data);

  impl.HasPasteData();

  impl.OpenRemoteUri(path);

  impl.IsLocalPaste();

  impl.GetTokenId();

  impl.AddPasteboardChangedObserver(nullptr);

  impl.RemovePasteboardChangedObserver(-1);

  int32_t callbackId = impl.AddPasteboardChangedObserver(observer);

  impl.RemovePasteboardChangedObserver(callbackId);

  impl.Clear();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  PasteDataRecordAdapterImplFuzzTest(&fdp);

  PasteDataAdapterImplFuzzTest(&fdp);

  PasteBoardClientAdapterImplFuzzTest(&fdp);

  return 0;
}

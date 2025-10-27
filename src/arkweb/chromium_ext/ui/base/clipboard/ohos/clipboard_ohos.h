// Copyright 2022 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_BASE_CLIPBOARD_CLIPBOARD_OHOS_H_
#define UI_BASE_CLIPBOARD_CLIPBOARD_OHOS_H_

#include <stddef.h>
#include <stdint.h>

#include <string_view>

#include "ui/base/clipboard/clipboard.h"
#include "ohos_nweb/include/nweb_preference.h"

namespace OHOS::NWeb {
class NWebSpanstringConvertHtmlCallback;
}  // namespace OHOS::NWeb

namespace ui {

class ClipboardData;
class ClipboardOHOSInternal;

class COMPONENT_EXPORT(UI_BASE_CLIPBOARD) ClipboardOHOS : public Clipboard {
 public:
  // Returns the in-memory clipboard for the current thread. This
  // method must *only* be used when the caller is sure that the clipboard for
  // the current thread is in fact an instance of ClipboardOHOS.
  static ClipboardOHOS* GetForCurrentThread();

  static void SetConvertHtmlCallback(
      std::shared_ptr<OHOS::NWeb::NWebSpanstringConvertHtmlCallback> callback);

  using CopyOptionCbFunc =
      base::RepeatingCallback<OHOS::NWeb::NWebPreference::CopyOptionMode()>;
  static void RegisterCopyOptionCb(CopyOptionCbFunc cb);

  // Clipboard overrides:
  std::optional<DataTransferEndpoint> GetSource(
      ClipboardBuffer buffer) const override;
  const ClipboardSequenceNumberToken& GetSequenceNumber(
      ClipboardBuffer buffer) const override;

 private:
  friend class Clipboard;
  ClipboardOHOS();
  ~ClipboardOHOS() override;

  // Clipboard overrides:
  void OnPreShutdown() override;
  std::vector<std::u16string> GetStandardFormats(
      ClipboardBuffer buffer,
      const DataTransferEndpoint* data_dst) const override;
  bool IsFormatAvailable(const ClipboardFormatType& format,
                         ClipboardBuffer buffer,
                         const DataTransferEndpoint* data_dst) const override;
  void Clear(ClipboardBuffer buffer) override;
  void ReadAvailableTypes(ClipboardBuffer buffer,
                          const DataTransferEndpoint* data_dst,
                          std::vector<std::u16string>* types) const override;
  void ReadText(ClipboardBuffer buffer,
                const DataTransferEndpoint* data_dst,
                std::u16string* result) const override;
  void ReadAsciiText(ClipboardBuffer buffer,
                     const DataTransferEndpoint* data_dst,
                     std::string* result) const override;
  void ReadHTML(ClipboardBuffer buffer,
                const DataTransferEndpoint* data_dst,
                std::u16string* markup,
                std::string* src_url,
                uint32_t* fragment_start,
                uint32_t* fragment_end) const override;
  void ReadSvg(ClipboardBuffer buffer,
               const DataTransferEndpoint* data_dst,
               std::u16string* result) const override;
  void ReadRTF(ClipboardBuffer buffer,
               const DataTransferEndpoint* data_dst,
               std::string* result) const override;
  void ReadPng(ClipboardBuffer buffer,
               const DataTransferEndpoint* data_dst,
               ReadPngCallback callback) const override;
  void ReadDataTransferCustomData(ClipboardBuffer buffer,
                                  const std::u16string& type,
                                  const DataTransferEndpoint* data_dst,
                                  std::u16string* result) const override;
  void ReadFilenames(ClipboardBuffer buffer,
                     const DataTransferEndpoint* data_dst,
                     std::vector<ui::FileInfo>* result) const override;
  void ReadBookmark(const DataTransferEndpoint* data_dst,
                    std::u16string* title,
                    std::string* url) const override;
  void ReadData(const ClipboardFormatType& format,
                const DataTransferEndpoint* data_dst,
                std::string* result) const override;
  bool IsSelectionBufferAvailable() const override;
  void WritePortableAndPlatformRepresentations(
      ClipboardBuffer buffer,
      const ObjectMap& objects,
      std::vector<Clipboard::PlatformRepresentation> platform_representations,
      std::unique_ptr<DataTransferEndpoint> data_src,
      uint32_t privacy_types) override;
  void WriteText(std::string_view text) override;
  void WriteHTML(std::string_view markup,
                 std::optional<std::string_view> source_url) override;
  void WriteSvg(std::string_view markup) override;
  void WriteRTF(std::string_view rtf) override;
  void WriteFilenames(std::vector<ui::FileInfo> filenames) override;
  void WriteBookmark(std::string_view title, std::string_view url) override;
  void WriteWebSmartPaste() override;
  void WriteBitmap(const SkBitmap& bitmap) override;
  void WriteData(const ClipboardFormatType& format,
                 base::span<const uint8_t> data) override;
  void WriteClipboardHistory() override;
  void WriteUploadCloudClipboard() override;
  void WriteConfidentialDataForPassword() override;
  bool HasPasteData() const override;
  void OnClipboardDataGuard(bool status) override;
  void UpdateClipboardData(UpdateClipboardDataCallback callback) override;

  const std::unique_ptr<ClipboardOHOSInternal> clipboard_internal_;
};

}  // namespace ui

#endif  // UI_BASE_CLIPBOARD_CLIPBOARD_OHOS_H_

// Copyright 2022 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_BASE_CLIPBOARD_CLIPBOARD_OHOS_H_
#define UI_BASE_CLIPBOARD_CLIPBOARD_OHOS_H_

#include "ui/base/clipboard/clipboard.h"
#include "ohos_nweb/include/nweb_spanstring_convert_html_callback.h"

namespace ui {
class ClipboardData;
class ClipboardOHOSInternal;

// In-memory clipboard implementation not backed by an underlying platform.
// This clipboard can be used where there's no need to sync the clipboard with
// an underlying platform, and can substitute platform clipboards like
// ClipboardWin on Windows or ClipboardMac on MacOS. As this isn't backed by an
// underlying platform, the clipboard data isn't persisted after an instance
// goes away.
class COMPONENT_EXPORT(UI_BASE_CLIPBOARD) ClipboardOHOS : public Clipboard {
 public:
  // Returns the in-memory clipboard for the current thread. This
  // method must *only* be used when the caller is sure that the clipboard for
  // the current thread is in fact an instance of ClipboardOHOS.
  static ClipboardOHOS* GetForCurrentThread();

  static void SetConvertHtmlCallback(std::shared_ptr<OHOS::NWeb::NWebSpanstringConvertHtmlCallback> callback);
  // Writes the current ClipboardData and returns the previous data.
  // The data source is expected to be set in `data`.
  std::unique_ptr<ClipboardData> WriteClipboardData(
      std::unique_ptr<ClipboardData> data);

  // Clipboard overrides:
  DataTransferEndpoint* GetSource(ClipboardBuffer buffer) const override;
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
  void OnClipboardDataGuard(bool status) override;
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
  void ReadCustomData(ClipboardBuffer buffer,
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
      std::unique_ptr<DataTransferEndpoint> data_src) override;
  void WriteText(const char* text_data, size_t text_len
#if defined(OHOS_CLIPBOARD)
                 ,
                 const CopyOptionMode copy_option
#endif // defined(OHOS_CLIPBOARD)
  ) override;
  void WriteHTML(const char* markup_data,
                 size_t markup_len,
                 const char* url_data,
                 size_t url_len
#if defined(OHOS_CLIPBOARD)
                 ,
                 const CopyOptionMode copy_option
#endif // defined(OHOS_CLIPBOARD)
                 ) override;
  void WriteUnsanitizedHTML(const char* markup_data,
                            size_t markup_len,
                            const char* url_data,
                            size_t url_len
#if defined(OHOS_CLIPBOARD)
                            ,
                            const CopyOptionMode copy_option
#endif // defined(OHOS_CLIPBOARD)
                            ) override;
  void WriteSvg(const char* markup_data, size_t markup_len) override;
  void WriteRTF(const char* rtf_data, size_t data_len) override;
  void WriteFilenames(std::vector<ui::FileInfo> filenames) override;
  void WriteBookmark(const char* title_data,
                     size_t title_len,
                     const char* url_data,
                     size_t url_len
#if defined(OHOS_CLIPBOARD)
                     ,
                     const CopyOptionMode copy_option
#endif // defined(OHOS_CLIPBOARD)
                     ) override;
  void WriteWebSmartPaste(
#if defined(OHOS_CLIPBOARD)
                 const CopyOptionMode copy_option
#endif // defined(OHOS_CLIPBOARD)
  ) override;
  void WriteBitmap(const SkBitmap& bitmap) override;
  void WriteData(const ClipboardFormatType& format,
                 const char* data_data,
                 size_t data_len) override;
  bool HasPasteData() const override;
  const std::unique_ptr<ClipboardOHOSInternal> clipboard_internal_;
  static std::shared_ptr<OHOS::NWeb::NWebSpanstringConvertHtmlCallback> convert_html_callback_;
};
}  // namespace ui

#endif  // UI_BASE_CLIPBOARD_CLIPBOARD_OHOS_H_

/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UI_BASE_CLIPBOARD_CLIPBOARD_OHOS_H_
#define UI_BASE_CLIPBOARD_CLIPBOARD_OHOS_H_

#include "ui/base/clipboard/clipboard.h"

namespace ui {
class ClipboardData;
class ClipboardOHOSInternal;

enum class ClipBoardImageColorType {
  COLOR_TYPE_UNKNOWN = 0,
  COLOR_TYPE_RGB_565,
  COLOR_TYPE_RGB_888,
  COLOR_TYPE_RGBA_8888,
  COLOR_TYPE_BGRA_8888
};
 
enum class ClipBoardImageAlphaType {
  ALPHA_TYPE_UNKNOWN = 0,
  ALPHA_TYPE_OPAQUE,
  ALPHA_TYPE_PREMULTIPLIED,
  ALPHA_TYPE_UNPREMUL
};


class COMPONENT_EXPORT(UI_BASE_CLIPBOARD) ClipboardOHOS : public Clipboard {
 public:
  static ClipboardOHOS* GetForCurrentThread();
  std::unique_ptr<ClipboardData> WriteClipboardData(
      std::unique_ptr<ClipboardData> data);

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
  const ClipboardOHOSInternal& GetInternalClipboard(
      ClipboardBuffer buffer) const;

  const std::unique_ptr<ClipboardOHOSInternal> clipboard_internal_;
  base::flat_map<ClipboardBuffer, std::unique_ptr<ClipboardOHOSInternal>>
      internal_clipboards_;
};
}  // namespace ui

#endif  // UI_BASE_CLIPBOARD_CLIPBOARD_OHOS_H_

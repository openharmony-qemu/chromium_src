/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#ifndef PRINTING_PRINTING_CONTEXT_OHOS_H_
#define PRINTING_PRINTING_CONTEXT_OHOS_H_

#include <memory>
#include <string>

#include "base/memory/raw_ptr.h"
#include "printing/mojom/print.mojom.h"
#include "printing/printing_context.h"
#include "ohos/adapter/print/ohos_print_common.h"

namespace ohos_print = ohos::adapter::print;

namespace printing {

class MetafilePlayer;

// PrintingContext with optional native UI for print dialog and pdf_paper_size.
class COMPONENT_EXPORT(PRINTING) PrintingContextOhos : public PrintingContext {
 public:
  PrintingContextOhos(Delegate* delegate,
                      OutOfProcessBehavior out_of_process_behavior);
  PrintingContextOhos(const PrintingContextOhos&) = delete;
  PrintingContextOhos& operator=(const PrintingContextOhos&) = delete;
  ~PrintingContextOhos() override;

  // PrintingContext implementation.
  void AskUserForSettings(int max_pages,
                          bool has_selection,
                          bool is_scripted,
                          PrintSettingsCallback callback) override;
  gfx::Size GetPdfPaperSizeDeviceUnits() override;
  mojom::ResultCode UseDefaultSettings() override;
  mojom::ResultCode UpdatePrinterSettings(
      const PrinterSettings& printer_settings) override;
  mojom::ResultCode NewDocument(const std::u16string& document_name) override;
  mojom::ResultCode PrintDocument(const MetafilePlayer& metafile,
                                  const PrintSettings& settings,
                                  uint32_t num_pages) override;
  mojom::ResultCode DocumentDone() override;
  void Cancel() override;
  void ReleaseContext() override;
  printing::NativeDrawingContext context() const override;

 private:
  std::unique_ptr<ohos_print::PrintJobData> print_job_data_ = nullptr;
};

}  // namespace printing

#endif  // PRINTING_PRINTING_CONTEXT_OHOS_H_

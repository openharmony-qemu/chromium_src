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

#include "printing/printing_context_ohos.h"

#include <memory>
#include <utility>

#include "base/check.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "ohos/adapter/print/ohos_print_adapter.h"
#include "printing/backend/ohos_helper.h"
#include "printing/backend/print_backend.h"
#include "printing/metafile.h"
#include "printing/mojom/print.mojom.h"
#include "printing/print_job_constants.h"
#include "printing/print_settings_initializer_ohos.h"
#include "printing/units.h"

namespace printing {

const std::string kPrintLogTag = "[OhosPrint] ";

// static
std::unique_ptr<PrintingContext> PrintingContext::CreateImpl(
    Delegate* delegate,
    OutOfProcessBehavior out_of_process_behavior) {
  return std::make_unique<PrintingContextOhos>(delegate,
                                               out_of_process_behavior);
}

PrintingContextOhos::PrintingContextOhos(
    Delegate* delegate,
    OutOfProcessBehavior out_of_process_behavior)
    : PrintingContext(delegate, out_of_process_behavior) {}

PrintingContextOhos::~PrintingContextOhos() {
  ReleaseContext();
}

void PrintingContextOhos::AskUserForSettings(int max_pages,
                                             bool has_selection,
                                             bool is_scripted,
                                             PrintSettingsCallback callback) {
  ohos::adapter::print::PrintAdapter::GetInstance().PrintPdfFile();
  std::move(callback).Run(mojom::ResultCode::kCanceled);
}

mojom::ResultCode PrintingContextOhos::UseDefaultSettings() {
  DCHECK(!in_print_job_);
  scoped_refptr<PrintBackend> backend =
      PrintBackend::CreateInstance(delegate_->GetAppLocale());
  std::string default_printer_name;
  mojom::ResultCode result =
      backend->GetDefaultPrinterName(default_printer_name);
  if (result != mojom::ResultCode::kSuccess)
    return result;
  Print_PrinterInfo* oh_printer_info = nullptr;
  Print_ErrorCode ret = OH_Print_QueryPrinterInfo(default_printer_name.c_str(), &oh_printer_info);
  if (ret != PRINT_ERROR_NONE || oh_printer_info == nullptr) {
    LOG(ERROR) << kPrintLogTag << __func__
               << " Get printer basic info failed: " << ret;
    return mojom::ResultCode::kFailed;
  }
  settings_->set_device_name(base::UTF8ToUTF16(default_printer_name));
  PrintSettingsInitializerOhos::InitPrintSettings(oh_printer_info->capability,
                                                  oh_printer_info->defaultValue,
                                                  settings_.get());
  OH_Print_ReleasePrinterInfo(oh_printer_info);
  return mojom::ResultCode::kSuccess;
}

gfx::Size PrintingContextOhos::GetPdfPaperSizeDeviceUnits() {
  return gfx::Size();
}

mojom::ResultCode PrintingContextOhos::UpdatePrinterSettings(
    const PrinterSettings& printer_settings) {
  DCHECK(!in_print_job_);
  print_job_data_ = std::make_unique<ohos_print::PrintJobData>();
  print_job_data_->printerId = base::UTF16ToUTF8(settings_->device_name());
  print_job_data_->copyNumber = settings_->copies();
  print_job_data_->isLandscape = settings_->landscape();
  if (settings_->color() == mojom::ColorModel::kColor) {
    print_job_data_->colorMode = ohos_print::PrintColorMode::COLOR_MODE_COLOR;
  } else {
    print_job_data_->colorMode = ohos_print::PrintColorMode::COLOR_MODE_MONOCHROME;
  }
  if (settings_->duplex_mode() == mojom::DuplexMode::kLongEdge) {
    print_job_data_->duplexMode = ohos_print::PrintDuplexMode::DUPLEX_MODE_LONG_EDGE;
  } else if (settings_->duplex_mode() == mojom::DuplexMode::kShortEdge) {
    print_job_data_->duplexMode = ohos_print::PrintDuplexMode::DUPLEX_MODE_SHORT_EDGE;
  } else {
    print_job_data_->duplexMode = ohos_print::PrintDuplexMode::DUPLEX_MODE_NONE;
  }
  print_job_data_->pageSize.id = settings_->requested_media().vendor_id;
  print_job_data_->pageSize.name = settings_->requested_media().vendor_id;
  int page_width = settings_->requested_media().size_microns.width();
  int page_height = settings_->requested_media().size_microns.height();
  print_job_data_->pageSize.width = ConvertUnit(page_width, kMicronsPerInch, kMicronsPerMm);
  print_job_data_->pageSize.height = ConvertUnit(page_height, kMicronsPerInch, kMicronsPerMm);
  print_job_data_->isCollate = settings_->collate();
  return mojom::ResultCode::kSuccess;
}

mojom::ResultCode PrintingContextOhos::NewDocument(
    const std::u16string& document_name) {
  DCHECK(!in_print_job_);
  in_print_job_ = true;
  if (print_job_data_ == nullptr) {
    LOG(ERROR) << kPrintLogTag << __func__ << "PrintJobData is not successfully initialized.";
    return mojom::ResultCode::kFailed;
  }
  print_job_data_->jobName = base::UTF16ToUTF8(document_name);
  return mojom::ResultCode::kSuccess;
}

mojom::ResultCode PrintingContextOhos::PrintDocument(
    const MetafilePlayer& metafile,
    const PrintSettings& settings,
    uint32_t num_pages) {
  if (abort_printing_)
    return mojom::ResultCode::kCanceled;
  DCHECK(in_print_job_);
  if (!IsPrintSupported()) {
    LOG(WARNING) << kPrintLogTag << __func__
               << " The SDK version is too early. ";
    return mojom::ResultCode::kFailed;
  }
  if (print_job_data_ == nullptr) {
    LOG(ERROR) << kPrintLogTag << __func__ << "PrintJobData is not successfully initialized.";
    return mojom::ResultCode::kFailed;
  }
  std::vector<char> data;
  bool success = metafile.GetDataAsVector(&data);
  if (!success) {
    LOG(ERROR) << kPrintLogTag << __func__ << " The data to be printed is empty.";
    return mojom::ResultCode::kFailed;
  }
  ohos_print::PrintAdapter::GetInstance().StartPrint(*print_job_data_, data);
  return mojom::ResultCode::kSuccess;
}

mojom::ResultCode PrintingContextOhos::DocumentDone() {
  if (abort_printing_)
    return mojom::ResultCode::kCanceled;
  DCHECK(in_print_job_);

  ResetSettings();
  return mojom::ResultCode::kSuccess;
}

void PrintingContextOhos::Cancel() {
  abort_printing_ = true;
  in_print_job_ = false;
}

void PrintingContextOhos::ReleaseContext() {
  print_job_data_ = nullptr;
  // Intentional No-op.
}

printing::NativeDrawingContext PrintingContextOhos::context() const {
  // Intentional No-op.
  return nullptr;
}

}  // namespace printing

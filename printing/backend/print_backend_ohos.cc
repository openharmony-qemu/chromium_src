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

#include "printing/backend/print_backend_ohos.h"

#include "base/logging.h"
#include "printing/backend/ohos_helper.h"

namespace printing {

const std::string kPrintLogTag = "[OhosPrint] ";

mojom::ResultCode PrintBackendOhos::EnumeratePrinters(
    PrinterList& printer_list) {
  DCHECK(printer_list.empty());
  if (!IsPrintSupported()) {
    LOG(WARNING) << kPrintLogTag << __func__
               << " The SDK version is too early. ";
    return mojom::ResultCode::kFailed;
  }
  Print_StringList printer_id_list = {0};
  Print_ErrorCode ret = OH_Print_QueryPrinterList(&printer_id_list);
  if (ret != PRINT_ERROR_NONE) {
    LOG(ERROR) << kPrintLogTag << __func__
               << " Get printer id failed: " << ret;
    return mojom::ResultCode::kFailed;
  }
  for (uint32_t i = 0; i < printer_id_list.count; i++) {
    char* printer_id = printer_id_list.list[i];
    Print_PrinterInfo* oh_printer_info = nullptr;
    ret = OH_Print_QueryPrinterInfo(printer_id, &oh_printer_info);
    if (ret != PRINT_ERROR_NONE || oh_printer_info == nullptr) {
      LOG(ERROR) << kPrintLogTag << __func__
                 << " Get printer info failed: " << ret;
      continue;
    }
    if (oh_printer_info->printerState == Print_PrinterState::PRINTER_UNAVAILABLE) {
      LOG(WARNING) << kPrintLogTag << __func__
                   << " Printer " << printer_id
                   << " is unavailable.";
      continue;
    }
    PrinterBasicInfo printer_info = ParseBasicPrinterInfo(*oh_printer_info);
    printer_list.push_back(printer_info);
    ReleasePrinterInfo(oh_printer_info);
  }
  OH_Print_ReleasePrinterList(&printer_id_list);
  return mojom::ResultCode::kSuccess;
}

mojom::ResultCode PrintBackendOhos::GetDefaultPrinterName(
    std::string& default_printer) {
  Print_StringList printer_id_list = {0};
  Print_ErrorCode ret = OH_Print_QueryPrinterList(&printer_id_list);
  if (ret != PRINT_ERROR_NONE) {
    LOG(ERROR) << kPrintLogTag << __func__
               << " Get printer id failed: " << ret;
    return mojom::ResultCode::kFailed;
  }
  for (uint32_t i = 0; i < printer_id_list.count; i++) {
    char* printer_id = printer_id_list.list[i];
    Print_PrinterInfo* oh_printer_info = nullptr;
    ret = OH_Print_QueryPrinterInfo(printer_id, &oh_printer_info);
    if (ret != PRINT_ERROR_NONE || oh_printer_info == nullptr) {
      LOG(ERROR) << kPrintLogTag << __func__
                 << " Get printer info failed: " << ret;
      continue;
    }
    if (oh_printer_info->printerState == Print_PrinterState::PRINTER_UNAVAILABLE) {
      LOG(WARNING) << kPrintLogTag << __func__
                   << " Printer " << printer_id
                   << " is unavailable.";
      continue;
    }
    if (oh_printer_info->isDefaultPrinter) {
      default_printer = oh_printer_info->printerId;
      ReleasePrinterInfo(oh_printer_info);
      break;
    }
    ReleasePrinterInfo(oh_printer_info);
  }
  OH_Print_ReleasePrinterList(&printer_id_list);
  return mojom::ResultCode::kSuccess;
}

mojom::ResultCode PrintBackendOhos::GetPrinterBasicInfo(
    const std::string& printer_name,
    PrinterBasicInfo* printer_info) {
  Print_PrinterInfo* oh_printer_info = nullptr;
  Print_ErrorCode ret = OH_Print_QueryPrinterInfo(printer_name.c_str(), &oh_printer_info);
  if (ret != PRINT_ERROR_NONE || oh_printer_info == nullptr) {
    LOG(ERROR) << kPrintLogTag << __func__
               << " Get printer basic info failed: " << ret;
    return mojom::ResultCode::kFailed;
  }
  *printer_info = ParseBasicPrinterInfo(*oh_printer_info);
  ReleasePrinterInfo(oh_printer_info);
  return mojom::ResultCode::kSuccess;
}

mojom::ResultCode PrintBackendOhos::GetPrinterSemanticCapsAndDefaults(
    const std::string& printer_name,
    PrinterSemanticCapsAndDefaults* printer_info) {
  Print_PrinterInfo* oh_printer_info = nullptr;
  Print_ErrorCode ret = OH_Print_QueryPrinterInfo(printer_name.c_str(), &oh_printer_info);
  if (ret != PRINT_ERROR_NONE || oh_printer_info == nullptr) {
    LOG(ERROR) << kPrintLogTag << __func__
               << " Get printer basic info failed: " << ret;
    ReleasePrinterInfo(oh_printer_info);
    return mojom::ResultCode::kFailed;
  }

  *printer_info = ParseCapabilities(oh_printer_info->capability,
                                    oh_printer_info->defaultValue);
  ReleasePrinterInfo(oh_printer_info);
  return mojom::ResultCode::kSuccess;
}

std::vector<std::string> PrintBackendOhos::GetPrinterDriverInfo(
    const std::string& printer_name) {
  std::vector<std::string> vec;
  return vec;
}

bool PrintBackendOhos::IsValidPrinter(const std::string& printer_name) {
  Print_PrinterInfo* oh_printer_info = nullptr;
  Print_ErrorCode ret = OH_Print_QueryPrinterInfo(printer_name.c_str(), &oh_printer_info);
  if (ret != PRINT_ERROR_NONE) {
    LOG(ERROR) << kPrintLogTag << __func__
               << " Get printer basic info failed: " << ret;
    return false;
  }
  bool res = oh_printer_info->printerState == Print_PrinterState::PRINTER_IDLE;
  ReleasePrinterInfo(oh_printer_info);
  return res;
}

void PrintBackendOhos::ReleasePrinterInfo(Print_PrinterInfo* oh_printer_info) {
  if (oh_printer_info) {
    OH_Print_ReleasePrinterInfo(oh_printer_info);
  }
}

scoped_refptr<PrintBackend> PrintBackend::CreateInstanceImpl(
    const std::string& locale) {
  return base::MakeRefCounted<PrintBackendOhos>();
}

}
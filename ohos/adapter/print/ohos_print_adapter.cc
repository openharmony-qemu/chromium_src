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

#include "ohos_print_adapter.h"
#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos::adapter::print {
PrintAdapter& PrintAdapter::GetInstance() {
  static PrintAdapter helper;
  return helper;
}

void PrintAdapter::OhosPrint(std::string printFile) {
  auto func = ohos::adapter::GetJSFunction(
      "PrintAdapter.ShowPrintDialog");
  if (func) {
    return func->Invoke<void>(printFile);
  }
}

void PrintAdapter::SystemPrinterSettings() {
  if (auto func = ohos::adapter::GetJSFunction(
          "PrintAdapter.StartSystemPrinterSettingsAbility")) {
    func->Invoke<void>();
  }
}
 
void PrintAdapter::PrintPdfFile() {
  if (auto func = ohos::adapter::GetJSFunction("PrintAdapter.PrintPdfFiles")) {
    aki::ArrayBuffer pdfArrayBuffer((uint8_t*)pdf_buffer.get(), pdf_length);
    func->Invoke<void>(std::move(pdfArrayBuffer));
  }
}
 
void PrintAdapter::PrintPdfFile(std::shared_ptr<char[]> pdf_data, uint32_t length) {
  if (auto func = ohos::adapter::GetJSFunction("PrintAdapter.PrintPdfFiles")) {
    aki::ArrayBuffer pdfArrayBuffer((uint8_t*)pdf_data.get(), length);
    func->Invoke<void>(std::move(pdfArrayBuffer));
  }
}
 
void PrintAdapter::SetPdfBuff(std::shared_ptr<char[]> pdf_data,
                              uint32_t length) {
  pdf_buffer = pdf_data;
  pdf_length = length;
}
 
void PrintAdapter::ShowPrintTipsDialog() {
  if (auto func =
          ohos::adapter::GetJSFunction("PrintAdapter.ShowPrintTipsDialog")) {
    func->Invoke<void>();
  }
}
 
void PrintAdapter::AddPrinters(PrinterInfo &printInfo,
                               std::shared_ptr<PrintCallback> printCallback) {}
}  // namespace ohos::adapter::print

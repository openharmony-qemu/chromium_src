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

#include "printing/print_settings_initializer_ohos.h"

#include <BasicServicesKit/ohprint.h>

#include "base/check.h"
#include "base/logging.h"
#include "printing/backend/ohos_helper.h"
#include "printing/print_settings.h"
#include "printing/printing_utils.h"
#include "printing/units.h"

namespace printing {

namespace {


}  // namespace

// static
void PrintSettingsInitializerOhos::InitPrintSettings(
    const Print_PrinterCapability& capability,
    const Print_DefaultValue& default_value,
    PrintSettings* print_settings) {
  DCHECK(print_settings);
  print_settings->SetOrientation(
      default_value.defaultOrientation == Print_OrientationMode::ORIENTATION_MODE_LANDSCAPE ||
      default_value.defaultOrientation == Print_OrientationMode::ORIENTATION_MODE_REVERSE_LANDSCAPE);
  Print_Resolution default_resolution = default_value.defaultResolution;
  if (default_resolution.horizontalDpi > 0 && default_resolution.verticalDpi > 0) {
    print_settings->set_dpi_xy(default_resolution.horizontalDpi, default_resolution.verticalDpi);
  } else {
    print_settings->set_dpi(kDefaultPdfDpi);
  }
  if (capability.supportedPageSizes != nullptr &&
      capability.supportedPageSizesCount > 0 &&
      default_value.defaultPageSizeId != nullptr) {
    for (uint32_t i = 0; i < capability.supportedPageSizesCount; i++) {
      Print_PageSize page_size = capability.supportedPageSizes[i];
      if (strcmp(page_size.id, default_value.defaultPageSizeId) == 0) {
        gfx::Size paper_size_um;
        gfx::Rect printable_area_um;
        std::tie(paper_size_um, printable_area_um) = GetPrintAreaInfo(page_size.width,
                                                                      page_size.height,
                                                                      default_value.defaultMargin);
        print_settings->SetPrinterPrintableArea(paper_size_um, printable_area_um, false);
        break;
      }
    }
  }
  print_settings->set_color(default_value.defaultColorMode == Print_ColorMode::COLOR_MODE_COLOR
                            ? mojom::ColorModel::kColor: mojom::ColorModel::kGray);
  print_settings->set_duplex_mode(ParseDuplexMode(default_value.defaultDuplexMode));
}

}  // namespace printing

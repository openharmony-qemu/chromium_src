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

#include "printing/backend/ohos_helper.h"

#include <algorithm>
#include <cmath>
#include <format>
#include <map>
#include <regex>
#include <tuple>
#include <vector>
#include <optional>

#include "base/logging.h"
#include "base/strings/string_util.h"
#include "deviceinfo.h"
#include "ohos/adapter/permission_manager/permission_manager_adapter.h"
#include "printing/backend/print_backend.h"
#include "printing/units.h"

namespace ohos_permission = ohos::adapter::permission;

namespace printing {

// This section contains helper code for OHOS printer capabilities
// parsing for semantic capabilities.
namespace {

const std::map<std::string, std::string> kPaperWriteList = {
    {"ISO_A0", "A0"},
    {"ISO_A1", "A1"},
    {"ISO_A2", "A2"},
    {"ISO_A3", "A3"},
    {"ISO_A4", "A4"},
    {"ISO_A5", "A5"},
    {"ISO_A6", "A6"},
    {"ISO_A7", "A7"},
    {"ISO_A8", "A8"},
    {"ISO_A9", "A9"},
    {"ISO_A10", "A10"},
    {"ISO_B0", "B0"},
    {"ISO_B1", "B1"},
    {"ISO_B2", "B2"},
    {"ISO_B3", "B3"},
    {"ISO_B4", "B4"},
    {"ISO_B5", "B5"},
    {"ISO_B6", "B6"},
    {"ISO_B7", "B7"},
    {"ISO_B8", "B8"},
    {"ISO_B9", "B9"},
    {"ISO_B10", "B10"},
    {"ISO_C0", "C0"},
    {"ISO_C1", "C1"},
    {"ISO_C2", "C2"},
    {"ISO_C3", "C3"},
    {"ISO_C4", "C4"},
    {"ISO_C5", "C5"},
    {"ISO_C6", "C6"},
    {"ISO_C7", "C7"},
    {"ISO_C8", "C8"},
    {"ISO_C9", "C9"},
    {"ISO_C10", "C10"},
    {"ISO_DL", "ISO_DL"},
    {"NA_LETTER", "Letter"},
    {"NA_GOVT_LETTER", "NA_GOVT_LETTER"},
    {"NA_LEGAL", "Legal"},
    {"NA_LEDGER", "Ledger"},
    {"NA_INDEX_4X6", "PHOTO_4x6"},
    {"NA_INDEX_5X7", "PHOTO_5x7"},
    {"ROC_8K", "ROC_8K"},
    {"ROC_16K", "ROC_16K"},
    {"JIS_B0", "B0"},
    {"JIS_B1", "B1"},
    {"JIS_B2", "B2"},
    {"JIS_B3", "B3"},
    {"JIS_B4", "B4"},
    {"JIS_B5", "B5"},
    {"JIS_B6", "B6"},
    {"JIS_B7", "B7"},
    {"JIS_B8", "B8"},
    {"JIS_B9", "B9"},
    {"JIS_B10", "B10"},
    {"JIS_EXEC", "JIS_EXEC"},
    {"JPN_HAGAKI", "Postcard"},
    {"OM_DSC_PHOTO", "OM_DSC_PHOTO"},
    {"OM_CARD", "OM_CARD"},
    {"OE_PHOTO_L", "OE_PHOTO_L"},
    {"NA_ARCH_A", "ARCH_A"},
    {"NA_ARCH_B", "ARCH_B"},
    {"NA_ARCH_C", "ARCH_C"},
    {"NA_ARCH_D", "ARCH_D"},
    {"NA_ARCH_E", "ARCH_E"},
    {"NA_EXECUTIVE", "Executive"},
    {"JPN_CHOU2", "CHOU2"},
    {"JPN_CHOU3", "CHOU3"},
    {"JPN_CHOU4", "CHOU4"},
    {"NA_FOOLSCAP", "FOOLSCAP"},
    {"NA_INDEX_3X5", "INDEX_3X5"},
    {"NA_INDEX_5X8", "INDEX_5X8"},
    {"PRC_1", "PRC1"},
    {"PRC_2", "PRC2"},
    {"PRC_4", "PRC4"},
    {"PRC_6", "PRC6"},
    {"PRC_7", "PRC7"},
    {"PRC_8", "PRC8"},
    {"PRC_16K", "PRC16"},
    {"INT_DL_ENVELOPE", "Envelope"},
    {"B_TABLOID", "Tabloid"},
    {"ISO_16K", "16K"},
    {"ISO_32K", "32K"}
};

using PrinterPaper = PrinterSemanticCapsAndDefaults::Paper;

uint32_t MilliInchToMilliMeter(uint32_t milli_inch) {
  const float kConvertUnit = 39.37f;
  uint32_t res = static_cast<uint32_t>(std::round(milli_inch / kConvertUnit));
  return res;
}

gfx::Rect LoadPaperPrintableAreaUm(const int paper_width,
                                   const int paper_height,
                                   const Print_Margin& paper_margin) {
  int left_margin_um =
      ConvertUnit(paper_margin.leftMargin, kMicronsPerMm, kMicronsPerInch);
  int bottom_margin_um =
      ConvertUnit(paper_margin.bottomMargin, kMicronsPerMm, kMicronsPerInch);
  int right_margin_um =
      ConvertUnit(paper_margin.rightMargin, kMicronsPerMm, kMicronsPerInch);
  int top_margin_um =
      ConvertUnit(paper_margin.topMargin, kMicronsPerMm, kMicronsPerInch);
  gfx::Rect printable_area_um(
      left_margin_um, bottom_margin_um,
      paper_width - left_margin_um - right_margin_um,
      paper_height - top_margin_um - bottom_margin_um);
  return printable_area_um;
}

std::string GetDisplayName(const std::string& id,
                           const uint32_t width,
                           const uint32_t height) {
  uint32_t real_width = MilliInchToMilliMeter(width);
  uint32_t real_height = MilliInchToMilliMeter(height);
  auto iter = kPaperWriteList.find(id);
  std::string label = id;
  if (iter != kPaperWriteList.end()) {
    label = iter->second;
  }
  return std::format("{} ({} × {} mm)", label, real_width, real_height);
}

std::pair<gfx::Size, gfx::Rect> GetPaperSizeInfo(const uint32_t& width,
                                                 const uint32_t& height,
                                                 const Print_Margin& default_margin) {
  int width_um = ConvertUnit(width, kMicronsPerMm, kMicronsPerInch);
  int height_um = ConvertUnit(height, kMicronsPerMm, kMicronsPerInch);
  const gfx::Size paper_size_um(width_um, height_um);
  gfx::Rect printable_area_um = LoadPaperPrintableAreaUm(width_um, height_um, default_margin);
  const gfx::Rect size_um_rect = gfx::Rect(paper_size_um);
  if (printable_area_um.IsEmpty() ||
      !size_um_rect.Contains(printable_area_um)) {
    printable_area_um = size_um_rect;
  }
  return std::make_pair(paper_size_um, printable_area_um);
}

std::optional<PrinterPaper> GetPrinterPaper(const Print_PageSize& page_size,
                                            const Print_Margin& default_margin) {
  // Calculate the paper size; the OHOS size unit is millimeters.
  if (page_size.width <= 0 || page_size.height <= 0) {
    LOG(ERROR) << __func__ << " Paper size is empty.";
    return std::nullopt;
  }
  gfx::Size paper_size_um;
  gfx::Rect printable_area_um;
  std::tie(paper_size_um, printable_area_um) = GetPaperSizeInfo(page_size.width,
                                                                page_size.height,
                                                                default_margin);
  std::string display_name = GetDisplayName(page_size.id, page_size.width, page_size.height);
  PrinterPaper paper(display_name, page_size.id, paper_size_um, printable_area_um);
  return paper;
}

std::pair<std::vector<PrinterPaper>, PrinterPaper> GetPaperSettings(
    const Print_PageSize* supported_page_sizes,
    const uint32_t supported_page_sizes_count,
    const char* default_page_size_id,
    const Print_Margin& default_margin) {
  std::vector<PrinterPaper> papers;
  PrinterPaper default_paper;
  if (supported_page_sizes != nullptr && supported_page_sizes_count > 0) {
    for (uint32_t i = 0; i < supported_page_sizes_count; i++) {
      Print_PageSize page_size = supported_page_sizes[i];
      std::optional<PrinterPaper> paper = GetPrinterPaper(page_size, default_margin);
      if (!paper.has_value()) {
        continue;
      }
      papers.push_back(paper.value());
      if (default_page_size_id != nullptr && strcmp(page_size.id, default_page_size_id) == 0) {
        default_paper = paper.value();
      }
    }
  }
  std::sort(papers.begin(), papers.end(),
            [](const PrinterPaper& paper1, const PrinterPaper& paper2) {
              return paper1.display_name() < paper2.display_name();
            });
  return std::make_pair(std::move(papers), default_paper);
}

void GetColorModelSettings(
    Print_ColorMode* supported_color_modes,
    uint32_t supported_color_modes_count,
    mojom::ColorModel& cm_black,
    mojom::ColorModel& cm_color) {
  if (supported_color_modes == nullptr || supported_color_modes_count <= 0) {
    LOG(ERROR) << __func__ << "The printer does not have color mode information.";
    return;
  }
  for (uint32_t i = 0; i < supported_color_modes_count; i++) {
    Print_ColorMode mode = supported_color_modes[i];
    if (mode == Print_ColorMode::COLOR_MODE_COLOR ||
        mode == Print_ColorMode::COLOR_MODE_AUTO) {
      cm_color = mojom::ColorModel::kColor;
    } else if (mode == Print_ColorMode::COLOR_MODE_MONOCHROME) {
      cm_black = mojom::ColorModel::kGray;
    }
  }
}

mojom::DuplexMode GetDuplexMode(const Print_DuplexMode& mode) {
    switch (mode) {
        case Print_DuplexMode::DUPLEX_MODE_ONE_SIDED:
            return mojom::DuplexMode::kSimplex;
        case Print_DuplexMode::DUPLEX_MODE_TWO_SIDED_LONG_EDGE:
            return mojom::DuplexMode::kLongEdge;
        case Print_DuplexMode::DUPLEX_MODE_TWO_SIDED_SHORT_EDGE:
            return mojom::DuplexMode::kShortEdge;
        default:
            return mojom::DuplexMode::kUnknownDuplexMode;
  }
}

std::pair<std::vector<mojom::DuplexMode>, mojom::DuplexMode> GetDuplexSettings(
    const Print_DuplexMode* supported_duplex_modes,
    const uint32_t supported_duplex_modes_count,
    const Print_DuplexMode& default_duplex_mode) {
  std::vector<mojom::DuplexMode> duplex_modes;

  if (supported_duplex_modes != nullptr && supported_duplex_modes_count > 0) {
    for (uint32_t i = 0; i < supported_duplex_modes_count; i++) {
      Print_DuplexMode mode = supported_duplex_modes[i];
      duplex_modes.push_back(GetDuplexMode(mode));
    }
  } else {
    LOG(ERROR) << __func__ << " The duplex modes of printer is null.";
  }

  mojom::DuplexMode duplex_default = GetDuplexMode(default_duplex_mode);
  return std::make_pair(std::move(duplex_modes), duplex_default);
}

std::optional<gfx::Size> ParseResolution(const Print_Resolution& resolution) {
    if (resolution.horizontalDpi <= 0 || resolution.verticalDpi <= 0) {
      LOG(ERROR) << __func__ << " Invalid resolution dimensions:"
                 << resolution.horizontalDpi << " " << resolution.verticalDpi;
      return std::nullopt;
    }
    return gfx::Size(resolution.horizontalDpi, resolution.verticalDpi);
}

std::pair<std::vector<gfx::Size>, gfx::Size> GetResolutionSettings(
    const Print_Resolution* supported_resolutions,
    const uint32_t supported_resolutions_count,
    const Print_Resolution& default_resolution) {
  constexpr gfx::Size kDefaultMissingDpi(kDefaultPdfDpi, kDefaultPdfDpi);
  std::vector<gfx::Size> dpis;
  if (supported_resolutions != nullptr && supported_resolutions_count > 0) {
    for (uint32_t i = 0; i < supported_resolutions_count; i++) {
      Print_Resolution resolution = supported_resolutions[i];
      std::optional<gfx::Size> parsed_size = ParseResolution(resolution);
      if (!parsed_size.has_value()) {
        continue;
      }
      dpis.push_back(parsed_size.value());
    }
  }
  if (dpis.empty()) {
    dpis.push_back(kDefaultMissingDpi);
  }

  std::optional<gfx::Size> default_parsed_size = ParseResolution(default_resolution);
  gfx::Size default_dpi;
  if (default_parsed_size.has_value()) {
    default_dpi = default_parsed_size.value();
  } else {
    default_dpi = kDefaultMissingDpi;
  }
  return std::make_pair(std::move(dpis), default_dpi);
}
}  // namespace

PrinterSemanticCapsAndDefaults ParseCapabilities(const Print_PrinterCapability& capability,
                                                 const Print_DefaultValue& default_value) {
  PrinterSemanticCapsAndDefaults caps;
  const uint32_t min_copies = 1;
  caps.collate_capable = true;
  caps.collate_default = true;
  caps.copies_max = std::max(min_copies, capability.supportedCopies);
  std::tie(caps.duplex_modes,
           caps.duplex_default) = GetDuplexSettings(capability.supportedDuplexModes,
                                                    capability.supportedDuplexModesCount,
                                                    default_value.defaultDuplexMode);
  std::tie(caps.dpis,
           caps.default_dpi) = GetResolutionSettings(capability.supportedResolutions,
                                                     capability.supportedResolutionsCount,
                                                     default_value.defaultResolution);
  mojom::ColorModel cm_black = mojom::ColorModel::kUnknownColorModel;
  mojom::ColorModel cm_color = mojom::ColorModel::kUnknownColorModel;
  GetColorModelSettings(capability.supportedColorModes,
                        capability.supportedColorModesCount,
                        cm_black, cm_color);
  caps.color_model = cm_color;
  caps.bw_model = cm_black;
  caps.color_default = default_value.defaultColorMode == Print_ColorMode::COLOR_MODE_COLOR;
  caps.color_changeable =
     ((cm_color != mojom::ColorModel::kUnknownColorModel) &&
      (cm_black != mojom::ColorModel::kUnknownColorModel) &&
      (cm_color != cm_black));
  std::tie(caps.papers,
           caps.default_paper) = GetPaperSettings(capability.supportedPageSizes,
                                                  capability.supportedPageSizesCount,
                                                  default_value.defaultPageSizeId,
                                                  default_value.defaultMargin);
  return caps;
}

PrinterBasicInfo ParseBasicPrinterInfo(
    const Print_PrinterInfo& oh_printer_info) {
  PrinterBasicInfo printer_info;
  printer_info.printer_name = oh_printer_info.printerId;
  printer_info.display_name = oh_printer_info.printerName;
  printer_info.printer_description = oh_printer_info.description;
  return printer_info;
}

std::pair<gfx::Size, gfx::Rect> GetPrintAreaInfo(const uint32_t& width,
                                                 const uint32_t& height,
                                                 const Print_Margin& default_margin) {
  return GetPaperSizeInfo(width, height, default_margin);
}

mojom::DuplexMode ParseDuplexMode(const Print_DuplexMode& mode) {
  return GetDuplexMode(mode);
}

bool IsPrintSupported() {
  const int kSupportedSdkVersion = 60100;
  int current_sdk_version = OH_GetDistributionOSApiVersion();
  if (current_sdk_version >= kSupportedSdkVersion) {
    return true;
  }
  return false;
}

}

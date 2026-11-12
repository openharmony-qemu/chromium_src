// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "pdf/metrics_handler.h"

#include <vector>

#include "base/metrics/histogram_functions.h"
#include "pdf/document_metadata.h"
#if BUILDFLAG(ARKWEB_PDF)
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif
namespace chrome_pdf {
#if BUILDFLAG(ARKWEB_PDF)
constexpr char PDF_VERSION[] = "PDF_VERSION";
constexpr char VERSION[] = "VERSION";
constexpr char PDF_PAGE_COUNT[] = "PDF_PAGE_COUNT";
constexpr char PAGE_COUNT[] = "PAGE_COUNT";
constexpr char PDF_HAS_ATTACHMENT[] = "PDF_HAS_ATTACHMENT";
constexpr char HAS_ATTACHMENT[] = "HAS_ATTACHMENT";
#endif
namespace {

// These values are persisted to logs. Entries should not be renumbered and
// numeric values should never be reused.
enum class PdfHasAttachment {
  kNo = 0,
  kYes = 1,
  kMaxValue = kYes,
};

}  // namespace

MetricsHandler::MetricsHandler() = default;

MetricsHandler::~MetricsHandler() = default;

void MetricsHandler::RecordAccessibilityIsDocTagged(bool is_tagged) {
  base::UmaHistogramBoolean("Accessibility.PDF.IsPDFTagged", is_tagged);
}

void MetricsHandler::RecordDocumentMetrics(const DocumentMetadata& metadata) {
  base::UmaHistogramEnumeration("PDF.Version", metadata.version);
  base::UmaHistogramCustomCounts("PDF.PageCount", metadata.page_count, 1,
                                 1000000, 50);
  base::UmaHistogramEnumeration(
      "PDF.HasAttachment", metadata.has_attachments ? PdfHasAttachment::kYes
                                                    : PdfHasAttachment::kNo);
  base::UmaHistogramEnumeration("PDF.FormType", metadata.form_type);
}

void MetricsHandler::RecordUrlSchemeIsFile(bool is_file) {
  base::UmaHistogramBoolean("PDF.SchemeIsFile", is_file);
}

}  // namespace chrome_pdf

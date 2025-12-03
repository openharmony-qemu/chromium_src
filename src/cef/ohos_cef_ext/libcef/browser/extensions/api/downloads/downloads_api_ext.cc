/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string>

#include "base/base64.h"
#include "base/i18n/time_formatting.h"
#include "base/logging.h"
#include "base/metrics/histogram_macros.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/extensions/api/downloads/downloads_api.h"
#include "chrome/common/extensions/api/downloads.h"
#include "components/download/public/common/download_item.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/web_contents.h"
#include "download_api_ext_router.h"
#include "extensions/browser/extension_function_dispatcher.h"
#include "extensions/common/extension.h"
#include "extensions/common/mojom/event_dispatcher.mojom-forward.h"
#include "extensions/common/permissions/permissions_data.h"
#include "net/base/filename_util.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_downloads_cef_delegate.h"
#include "third_party/bounds_checking_function/include/securec.h"
#include "ui/base/webui/web_ui_util.h"

using content::BrowserContext;
using download::DownloadItem;
using extensions::mojom::APIPermissionID;

namespace extensions {
namespace {
namespace downloads = api::downloads;
const char kOpenPermission[] = "The \"downloads.open\" permission is required";
const char kShelfPermission[] =
    "downloads.setShelfEnabled requires the "
    "\"downloads.shelf\" permission";
const char kUiPermission[] =
    "downloads.setUiOptions requires the "
    "\"downloads.ui\" permission";
const char kUserGesture[] = "User gesture required";

bool Fault(bool error, const char* message_in, std::string* message_out) {
  if (!error) {
    return false;
  }
  *message_out = message_in;
  return true;
}

extensions::api::downloads::DangerType ConvertDangerType(
    download::DownloadDangerType danger) {
  switch (danger) {
    case download::DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS:
      return extensions::api::downloads::DangerType::kSafe;
    case download::DOWNLOAD_DANGER_TYPE_DANGEROUS_FILE:
      return extensions::api::downloads::DangerType::kFile;
    case download::DOWNLOAD_DANGER_TYPE_DANGEROUS_URL:
      return extensions::api::downloads::DangerType::kUrl;
    case download::DOWNLOAD_DANGER_TYPE_DANGEROUS_CONTENT:
      return extensions::api::downloads::DangerType::kContent;
    case download::DOWNLOAD_DANGER_TYPE_MAYBE_DANGEROUS_CONTENT:
      return extensions::api::downloads::DangerType::kSafe;
    case download::DOWNLOAD_DANGER_TYPE_UNCOMMON_CONTENT:
      return extensions::api::downloads::DangerType::kUncommon;
    case download::DOWNLOAD_DANGER_TYPE_USER_VALIDATED:
      return extensions::api::downloads::DangerType::kAccepted;
    case download::DOWNLOAD_DANGER_TYPE_DANGEROUS_HOST:
      return extensions::api::downloads::DangerType::kHost;
    case download::DOWNLOAD_DANGER_TYPE_POTENTIALLY_UNWANTED:
      return extensions::api::downloads::DangerType::kUnwanted;
    case download::DOWNLOAD_DANGER_TYPE_ALLOWLISTED_BY_POLICY:
      return extensions::api::downloads::DangerType::kAllowlistedByPolicy;
    case download::DOWNLOAD_DANGER_TYPE_ASYNC_SCANNING:
      return extensions::api::downloads::DangerType::kAsyncScanning;
    case download::DOWNLOAD_DANGER_TYPE_ASYNC_LOCAL_PASSWORD_SCANNING:
      return extensions::api::downloads::DangerType::
          kAsyncLocalPasswordScanning;
    case download::DOWNLOAD_DANGER_TYPE_BLOCKED_PASSWORD_PROTECTED:
      return extensions::api::downloads::DangerType::kPasswordProtected;
    case download::DOWNLOAD_DANGER_TYPE_BLOCKED_TOO_LARGE:
      return extensions::api::downloads::DangerType::kBlockedTooLarge;
    case download::DOWNLOAD_DANGER_TYPE_SENSITIVE_CONTENT_WARNING:
      return extensions::api::downloads::DangerType::kSensitiveContentWarning;
    case download::DOWNLOAD_DANGER_TYPE_SENSITIVE_CONTENT_BLOCK:
      return extensions::api::downloads::DangerType::kSensitiveContentBlock;
    case download::DOWNLOAD_DANGER_TYPE_DEEP_SCANNED_SAFE:
      return extensions::api::downloads::DangerType::kDeepScannedSafe;
    case download::DOWNLOAD_DANGER_TYPE_DEEP_SCANNED_OPENED_DANGEROUS:
      return extensions::api::downloads::DangerType::
          kDeepScannedOpenedDangerous;
    case download::DOWNLOAD_DANGER_TYPE_PROMPT_FOR_SCANNING:
      return extensions::api::downloads::DangerType::kPromptForScanning;
    case download::DOWNLOAD_DANGER_TYPE_DANGEROUS_ACCOUNT_COMPROMISE:
      return extensions::api::downloads::DangerType::kAccountCompromise;
    case download::DOWNLOAD_DANGER_TYPE_DEEP_SCANNED_FAILED:
      return extensions::api::downloads::DangerType::kDeepScannedFailed;
    case download::DOWNLOAD_DANGER_TYPE_PROMPT_FOR_LOCAL_PASSWORD_SCANNING:
      return extensions::api::downloads::DangerType::
          kPromptForLocalPasswordScanning;
    case download::DOWNLOAD_DANGER_TYPE_BLOCKED_SCAN_FAILED:
      return extensions::api::downloads::DangerType::kBlockedScanFailed;
    case download::DOWNLOAD_DANGER_TYPE_MAX:
      NOTREACHED();
  }
}

/* queryinfo */
void GetEraseDownloadQueryInfoParams(
    std::optional<downloads::Erase::Params>& params,
    ExDownloadsQueryInfo& queryInfo) {
  queryInfo.bytesReceived = params->query.bytes_received;
  queryInfo.danger = static_cast<int32_t>(params->query.danger);
  queryInfo.endTime = params->query.end_time;
  queryInfo.endedAfter = params->query.ended_after;
  queryInfo.endedBefore = params->query.ended_before;
  queryInfo.error = static_cast<int32_t>(params->query.error);
  queryInfo.exists = params->query.exists;
  queryInfo.fileSize = params->query.file_size;
  queryInfo.filename = params->query.filename;
  queryInfo.filenameRegex = params->query.filename_regex;
  queryInfo.finalUrl = params->query.final_url;
  queryInfo.finalUrlRegex = params->query.final_url_regex;
  queryInfo.id = params->query.id;
  queryInfo.limit = params->query.limit;
  queryInfo.mime = params->query.mime;
  queryInfo.orderBy = params->query.order_by;
  queryInfo.paused = params->query.paused;
  queryInfo.query = params->query.query;
  queryInfo.startTime = params->query.start_time;
  queryInfo.startedAfter = params->query.started_after;
  queryInfo.startedBefore = params->query.started_before;
  queryInfo.state = static_cast<int32_t>(params->query.state);
  queryInfo.totalBytes = params->query.total_bytes;
  queryInfo.totalBytesGreater = params->query.total_bytes_greater;
  queryInfo.totalBytesLess = params->query.total_bytes_less;
  queryInfo.url = params->query.url;
  queryInfo.urlRegex = params->query.url_regex;
  return;
}

void GetSearchDownloadQueryInfoParams(
    std::optional<downloads::Search::Params>& params,
    ExDownloadsQueryInfo& queryInfo) {
  queryInfo.bytesReceived = params->query.bytes_received;
  queryInfo.danger = static_cast<int32_t>(params->query.danger);
  queryInfo.endTime = params->query.end_time;
  queryInfo.endedAfter = params->query.ended_after;
  queryInfo.endedBefore = params->query.ended_before;
  queryInfo.error = static_cast<int32_t>(params->query.error);
  queryInfo.exists = params->query.exists;
  queryInfo.fileSize = params->query.file_size;
  queryInfo.filename = params->query.filename;
  queryInfo.filenameRegex = params->query.filename_regex;
  queryInfo.finalUrl = params->query.final_url;
  queryInfo.finalUrlRegex = params->query.final_url_regex;
  queryInfo.id = params->query.id;
  queryInfo.limit = params->query.limit;
  queryInfo.mime = params->query.mime;
  queryInfo.orderBy = params->query.order_by;
  queryInfo.paused = params->query.paused;
  queryInfo.query = params->query.query;
  queryInfo.startTime = params->query.start_time;
  queryInfo.startedAfter = params->query.started_after;
  queryInfo.startedBefore = params->query.started_before;
  queryInfo.state = static_cast<int32_t>(params->query.state);
  queryInfo.totalBytes = params->query.total_bytes;
  queryInfo.totalBytesGreater = params->query.total_bytes_greater;
  queryInfo.totalBytesLess = params->query.total_bytes_less;
  queryInfo.url = params->query.url;
  queryInfo.urlRegex = params->query.url_regex;
  return;
}

base::Value::Dict DownloadItemsToJSON(ExDownloadsItem& download_item) {
  extensions::api::downloads::DownloadItem item;
  item.by_extension_id = download_item.byExtensionId;
  item.by_extension_name = download_item.byExtensionName;
  item.bytes_received = download_item.bytesReceived;
  item.can_resume = download_item.canResume;
  item.danger =
      static_cast<extensions::api::downloads::DangerType>(download_item.danger);
  item.end_time = download_item.endTime;
  item.error = static_cast<extensions::api::downloads::InterruptReason>(
      download_item.error);
  item.estimated_end_time = download_item.estimatedEndTime;
  item.exists = download_item.exists;
  item.file_size = download_item.fileSize;
  item.filename = download_item.filename;
  item.final_url = download_item.finalUrl;
  item.id = download_item.id;
  item.incognito = download_item.incognito;
  item.mime = download_item.mime;
  item.paused = download_item.paused;
  item.referrer = download_item.referrer;
  item.start_time = download_item.startTime;
  item.state =
      static_cast<extensions::api::downloads::State>(download_item.state);
  item.total_bytes = download_item.totalBytes;
  item.url = download_item.url;
  return item.ToValue();
}

/* download item change */
ExDownloadsItem GetExDownloadsItemByNwebItem(
    download::DownloadItem* item,
    int downloadId,
    content::BrowserContext* browser_context) {
  ExDownloadsItem download_item;
  download_item.id = downloadId;
  download_item.bytesReceived = static_cast<double>(item->GetReceivedBytes());
  download_item.canResume = item->CanResume();
  download_item.danger =
      static_cast<int>(ConvertDangerType(item->GetDangerType()));
  if (!item->GetEndTime().is_null()) {
    download_item.endTime = base::TimeFormatAsIso8601(item->GetEndTime());
  }
  if (item->GetState() == download::DownloadItem::INTERRUPTED) {
    download_item.error = static_cast<int>(item->GetLastReason());
  } else if (item->GetState() == download::DownloadItem::CANCELLED) {
    download_item.error =
        static_cast<int>(download::DOWNLOAD_INTERRUPT_REASON_USER_CANCELED);
  }

  base::TimeDelta time_remaining;
  if (item->TimeRemaining(&time_remaining)) {
    base::Time now = base::Time::Now();
    download_item.estimatedEndTime =
        base::TimeFormatAsIso8601(now + time_remaining);
  }
  download_item.exists = !item->GetFileExternallyRemoved();
  download_item.fileSize = static_cast<double>(item->GetTotalBytes());
  download_item.filename =
      base::UTF16ToUTF8(item->GetTargetFilePath().LossyDisplayName());
  download_item.finalUrl =
      item->GetURL().is_valid() ? item->GetURL().spec() : "";
  download_item.incognito = browser_context->IsOffTheRecord();
  download_item.mime = item->GetMimeType();
  download_item.paused = item->IsPaused();
  const GURL& referrer = item->GetReferrerUrl();
  download_item.referrer = referrer.is_valid() ? referrer.spec() : "";
  download_item.startTime = base::TimeFormatAsIso8601(item->GetStartTime());
  download_item.state = static_cast<int>(item->GetInsecureDownloadStatus());
  download_item.totalBytes = static_cast<double>(item->GetTotalBytes());
  const GURL& url = item->GetOriginalUrl();
  download_item.url = url.is_valid() ? url.spec() : "";

  return download_item;
}

std::optional<std::string> GetExtensionContextType(
    content::BrowserContext* browser_context) {
  if (!browser_context) {
    return std::nullopt;
  }

  if (browser_context->IsOffTheRecord()) {
    return "INCOGNITO";
  } else {
    return "REGULAR";
  }
}
}  // namespace

/* downloads.erase */
DownloadsEraseFunction::DownloadsEraseFunction() {}

DownloadsEraseFunction::~DownloadsEraseFunction() {}

ExtensionFunction::ResponseAction DownloadsEraseFunction::Run() {
  std::optional<downloads::Erase::Params> params =
      downloads::Erase::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }
  ExDownloadsQueryInfo queryInfo;
  GetEraseDownloadQueryInfoParams(params, queryInfo);
  queryInfo.contextType = GetExtensionContextType(browser_context());
  queryInfo.includeIncognitoInfo = include_incognito_information();

  call_downloads_erase_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance().Erase(
          queryInfo, base::BindRepeating(&DownloadsEraseFunction::EraseCallback,
                                         weak_ptr_factory_.GetWeakPtr()));

  call_downloads_erase_ = false;

  if (did_respond()) {
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    return RespondLater();
  }

  return RespondNow(Error("not support downloads.erase."));
}

void DownloadsEraseFunction::EraseCallback(
    const base::WeakPtr<DownloadsEraseFunction>& function,
    std::optional<std::string> error,
    std::vector<int32_t>& erase_id) {
  DCHECK(function);
  if (error.has_value()) {
    LOG(INFO) << "DownloadsEraseFunction EraseCallback error: "
              << error.value();
    std::string errorMessage = error.value();
    function->Respond(function->Error(errorMessage));
  } else {
    LOG(INFO) << "DownloadsEraseFunction EraseCallback has " << erase_id.size()
              << " erase_id.";
    base::Value::List json_results;
    for (uint32_t i = 0; i < erase_id.size(); i++) {
      json_results.Append(static_cast<int>(erase_id[i]));
    }
    function->Respond(function->WithArguments(std::move(json_results)));
  }

  if (!function->call_downloads_erase_) {
    LOG(INFO) << "DownloadsEraseFunction Release";
    function->Release();
  }
}

/* downloads.open */
DownloadsOpenFunction::DownloadsOpenFunction() {}

DownloadsOpenFunction::~DownloadsOpenFunction() {}

ExtensionFunction::ResponseAction DownloadsOpenFunction::Run() {
  std::optional<downloads::Open::Params> params =
      downloads::Open::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  // Extensions with debugger permission could fake user gestures and should
  // not be trusted.
  std::string error;
  if (Fault(!user_gesture(), download_extension_errors::kUserGesture, &error) ||
      Fault(!extension()->permissions_data()->HasAPIPermission(
                APIPermissionID::kDownloadsOpen),
            download_extension_errors::kOpenPermission, &error)) {
    return RespondNow(Error(std::move(error)));
  }

  // Prompt user for ack to open the download.
  // TODO(qinmin): check if user prefers to open all download using the same
  // extension, or check the recent user gesture on the originating webcontents
  // to avoid showing the prompt.
  if (!GetSenderWebContents() ||
      !GetSenderWebContents()->HasRecentInteraction() ||
      extension()->permissions_data()->HasAPIPermission(
          APIPermissionID::kDebugger)) {
    return RespondNow(NoArguments());
  }
  int downloadId = params->download_id;
  LOG(INFO) << "DownloadsOpenFunction::Run downloadId: " << downloadId;

  call_downloads_open_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance().Open(
          downloadId, GetExtensionContextType(browser_context()),
          include_incognito_information(),
          base::BindRepeating(&DownloadsOpenFunction::OpenCallback,
                              weak_ptr_factory_.GetWeakPtr()));

  call_downloads_open_ = false;

  if (did_respond()) {
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    return RespondLater();
  }

  return RespondNow(Error("not support downloads.open."));
}

void DownloadsOpenFunction::OpenCallback(
    const base::WeakPtr<DownloadsOpenFunction>& function,
    std::optional<std::string> error) {
  DCHECK(function);
  LOG(INFO) << "DownloadsOpenFunction::OpenCallback error: "
            << (error.has_value() ? error.value() : "not exist");

  if (error.has_value()) {
    std::string errorMessage = error.value();
    function->Respond(function->Error(errorMessage));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_downloads_open_) {
    LOG(INFO) << "DownloadsOpenFunction Release";
    function->Release();
  }
}

/* downloads.removeFile */
DownloadsRemoveFileFunction::DownloadsRemoveFileFunction() {}

DownloadsRemoveFileFunction::~DownloadsRemoveFileFunction() {}

ExtensionFunction::ResponseAction DownloadsRemoveFileFunction::Run() {
  std::optional<downloads::RemoveFile::Params> params =
      downloads::RemoveFile::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  int downloadId = params->download_id;
  LOG(INFO) << "DownloadsRemoveFileFunction::Run downloadId: " << downloadId;

  call_downloads_remove_file_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance().RemoveFile(
          downloadId, GetExtensionContextType(browser_context()),
          include_incognito_information(),
          base::BindRepeating(&DownloadsRemoveFileFunction::RemoveFileCallback,
                              weak_ptr_factory_.GetWeakPtr()));

  call_downloads_remove_file_ = false;

  if (did_respond()) {
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    return RespondLater();
  }

  return RespondNow(Error("not support downloads.removeFile."));
}

void DownloadsRemoveFileFunction::RemoveFileCallback(
    const base::WeakPtr<DownloadsRemoveFileFunction>& function,
    std::optional<std::string> error) {
  DCHECK(function);
  LOG(INFO) << "DownloadsRemoveFileFunction::RemoveFileCallback error: "
            << (error.has_value() ? error.value() : "not exist");

  if (error.has_value()) {
    std::string errorMessage = error.value();
    function->Respond(function->Error(errorMessage));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_downloads_remove_file_) {
    LOG(INFO) << "DownloadsRemoveFileFunction Release";
    function->Release();
  }
}

/* downloads.pause */
DownloadsPauseFunction::DownloadsPauseFunction() {}

DownloadsPauseFunction::~DownloadsPauseFunction() {}

ExtensionFunction::ResponseAction DownloadsPauseFunction::Run() {
  std::optional<downloads::Pause::Params> params =
      downloads::Pause::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  int downloadId = params->download_id;
  LOG(INFO) << "DownloadsPauseFunction::Run downloadId: " << downloadId;

  call_downloads_pause_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance().Pause(
          downloadId, GetExtensionContextType(browser_context()),
          include_incognito_information(),
          base::BindRepeating(&DownloadsPauseFunction::PauseCallback,
                              weak_ptr_factory_.GetWeakPtr()));

  call_downloads_pause_ = false;

  if (did_respond()) {
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    return RespondLater();
  }

  return RespondNow(Error("not support downloads.pause."));
}

void DownloadsPauseFunction::PauseCallback(
    const base::WeakPtr<DownloadsPauseFunction>& function,
    std::optional<std::string> error) {
  DCHECK(function);
  LOG(INFO) << "DownloadsPauseFunction::PauseCallback error: "
            << (error.has_value() ? error.value() : "not exist");

  if (error.has_value()) {
    std::string errorMessage = error.value();
    function->Respond(function->Error(errorMessage));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_downloads_pause_) {
    LOG(INFO) << "DownloadsPauseFunction Release";
    function->Release();
  }
}

/* downloads.resume */
DownloadsResumeFunction::DownloadsResumeFunction() {}

DownloadsResumeFunction::~DownloadsResumeFunction() {}

ExtensionFunction::ResponseAction DownloadsResumeFunction::Run() {
  std::optional<downloads::Resume::Params> params =
      downloads::Resume::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  int downloadId = params->download_id;
  LOG(INFO) << "DownloadsResumeFunction::Run downloadId: " << downloadId;

  call_downloads_resume_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance().Resume(
          downloadId, GetExtensionContextType(browser_context()),
          include_incognito_information(),
          base::BindRepeating(&DownloadsResumeFunction::ResumeCallback,
                              weak_ptr_factory_.GetWeakPtr()));

  call_downloads_resume_ = false;

  if (did_respond()) {
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    return RespondLater();
  }

  return RespondNow(Error("not support downloads.resume."));
}

void DownloadsResumeFunction::ResumeCallback(
    const base::WeakPtr<DownloadsResumeFunction>& function,
    std::optional<std::string> error) {
  DCHECK(function);
  LOG(INFO) << "DownloadsResumeFunction::ResumeCallback error: "
            << (error.has_value() ? error.value() : "not exist");

  if (error.has_value()) {
    std::string errorMessage = error.value();
    function->Respond(function->Error(errorMessage));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_downloads_resume_) {
    LOG(INFO) << "DownloadsResumeFunction Release";
    function->Release();
  }
}

/* downloads.cancel */
DownloadsCancelFunction::DownloadsCancelFunction() {}

DownloadsCancelFunction::~DownloadsCancelFunction() {}

ExtensionFunction::ResponseAction DownloadsCancelFunction::Run() {
  std::optional<downloads::Cancel::Params> params =
      downloads::Cancel::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  int downloadId = params->download_id;
  LOG(INFO) << "DownloadsCancelFunction::Run downloadId: " << downloadId;

  call_downloads_cancel_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance().Cancel(
          downloadId, GetExtensionContextType(browser_context()),
          include_incognito_information(),
          base::BindRepeating(&DownloadsCancelFunction::CancelCallback,
                              weak_ptr_factory_.GetWeakPtr()));

  call_downloads_cancel_ = false;

  if (did_respond()) {
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    return RespondLater();
  }

  return RespondNow(Error("not support downloads.cancel."));
}

void DownloadsCancelFunction::CancelCallback(
    const base::WeakPtr<DownloadsCancelFunction>& function,
    std::optional<std::string> error) {
  DCHECK(function);
  LOG(INFO) << "DownloadsCancelFunction::CancelCallback error: "
            << (error.has_value() ? error.value() : "not exist");

  if (error.has_value()) {
    std::string errorMessage = error.value();
    function->Respond(function->Error(errorMessage));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_downloads_cancel_) {
    LOG(INFO) << "DownloadsCancelFunction Release";
    function->Release();
  }
}

/* downloads.acceptDanger */
DownloadsAcceptDangerFunction::DownloadsAcceptDangerFunction() {}

DownloadsAcceptDangerFunction::~DownloadsAcceptDangerFunction() {}

ExtensionFunction::ResponseAction DownloadsAcceptDangerFunction::Run() {
  std::optional<downloads::AcceptDanger::Params> params =
      downloads::AcceptDanger::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  int downloadId = params->download_id;
  LOG(INFO) << "DownloadsAcceptDangerFunction::Run downloadId: " << downloadId;

  call_downloads_accept_danger_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance().AcceptDanger(
          downloadId, GetExtensionContextType(browser_context()),
          include_incognito_information(),
          base::BindRepeating(
              &DownloadsAcceptDangerFunction::AcceptDangerCallback,
              weak_ptr_factory_.GetWeakPtr()));

  call_downloads_accept_danger_ = false;

  if (did_respond()) {
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    return RespondLater();
  }

  return RespondNow(Error("not support downloads.acceptDanger."));
}

void DownloadsAcceptDangerFunction::AcceptDangerCallback(
    const base::WeakPtr<DownloadsAcceptDangerFunction>& function,
    std::optional<std::string> error) {
  DCHECK(function);
  LOG(INFO) << "DownloadsAcceptDangerFunction::AcceptDangerCallback error: "
            << (error.has_value() ? error.value() : "not exist");

  if (error.has_value()) {
    std::string errorMessage = error.value();
    function->Respond(function->Error(errorMessage));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_downloads_accept_danger_) {
    LOG(INFO) << "DownloadsAcceptDangerFunction Release";
    function->Release();
  }
}

/* downloads.setUiOptions */
DownloadsSetUiOptionsFunction::DownloadsSetUiOptionsFunction() = default;

DownloadsSetUiOptionsFunction::~DownloadsSetUiOptionsFunction() = default;

ExtensionFunction::ResponseAction DownloadsSetUiOptionsFunction::Run() {
  std::optional<downloads::SetUiOptions::Params> params =
      downloads::SetUiOptions::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  if (!extension()->permissions_data()->HasAPIPermission(
          APIPermissionID::kDownloadsUi)) {
    return RespondNow(Error(download_extension_errors::kUiPermission));
  }

  ExDownloadsUiOptions options;
  options.enabled = params->options.enabled;
  options.extensionId = extension_id();
  options.contextType = GetExtensionContextType(browser_context());
  options.includeIncognitoInfo = include_incognito_information();

  call_downloads_set_ui_options_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance().SetUiOptions(
          options, base::BindRepeating(
                       &DownloadsSetUiOptionsFunction::SetUiOptionsCallback,
                       weak_ptr_factory_.GetWeakPtr()));

  call_downloads_set_ui_options_ = false;

  if (did_respond()) {
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    return RespondLater();
  }

  return RespondNow(Error("not support downloads.setUiOptions."));
}

void DownloadsSetUiOptionsFunction::SetUiOptionsCallback(
    const base::WeakPtr<DownloadsSetUiOptionsFunction>& function,
    std::optional<std::string> error) {
  DCHECK(function);
  LOG(INFO) << "DownloadsSetUiOptionsFunction::SetUiOptionsCallback error: "
            << error.value_or("not exist.");

  if (error) {
    function->Respond(function->Error(*error));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_downloads_set_ui_options_) {
    LOG(INFO) << "DownloadsSetUiOptionsFunction Release";
    function->Release();
  }
}

/* downloads.setShelfEnabled */
DownloadsSetShelfEnabledFunction::DownloadsSetShelfEnabledFunction() {}

DownloadsSetShelfEnabledFunction::~DownloadsSetShelfEnabledFunction() {}

ExtensionFunction::ResponseAction DownloadsSetShelfEnabledFunction::Run() {
  std::optional<downloads::SetShelfEnabled::Params> params =
      downloads::SetShelfEnabled::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }
  // TODO(devlin): Solve this with the feature system.
  if (!extension()->permissions_data()->HasAPIPermission(
          APIPermissionID::kDownloadsShelf)) {
    return RespondNow(Error(download_extension_errors::kShelfPermission));
  }

  ExDownloadsUiOptions options;
  options.enabled = params->enabled;
  options.extensionId = extension_id();
  options.contextType = GetExtensionContextType(browser_context());
  options.includeIncognitoInfo = include_incognito_information();

  call_downloads_set_shelf_enabled_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance().SetUiOptions(
          options,
          base::BindRepeating(
              &DownloadsSetShelfEnabledFunction::SetShelfEnabledCallback,
              weak_ptr_factory_.GetWeakPtr()));

  call_downloads_set_shelf_enabled_ = false;

  if (did_respond()) {
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    return RespondLater();
  }

  return RespondNow(Error("not support downloads.setShelfEnabled."));
}

void DownloadsSetShelfEnabledFunction::SetShelfEnabledCallback(
    const base::WeakPtr<DownloadsSetShelfEnabledFunction>& function,
    std::optional<std::string> error) {
  DCHECK(function);
  LOG(INFO) << "DownloadsSetShelfEnabledFunction::SetUiOptionsCallback error: "
            << error.value_or("not exist.");

  if (error) {
    function->Respond(function->Error(*error));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_downloads_set_shelf_enabled_) {
    LOG(INFO) << "DownloadsSetShelfEnabledFunction Release";
    function->Release();
  }
}

/* downloads.show */
DownloadsShowFunction::DownloadsShowFunction() {}

DownloadsShowFunction::~DownloadsShowFunction() {}

ExtensionFunction::ResponseAction DownloadsShowFunction::Run() {
  std::optional<downloads::Show::Params> params =
      downloads::Show::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  int downloadId = params->download_id;
  LOG(INFO) << "DownloadsShowFunction::Run downloadId: " << downloadId;

  call_downloads_show_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance().Show(
          downloadId, GetExtensionContextType(browser_context()),
          include_incognito_information(),
          base::BindRepeating(&DownloadsShowFunction::ShowCallback,
                              weak_ptr_factory_.GetWeakPtr()));

  call_downloads_show_ = false;

  if (success) {
    AddRef();
    return RespondLater();
  }
  return RespondNow(NoArguments());
}

void DownloadsShowFunction::ShowCallback(
    const base::WeakPtr<DownloadsShowFunction>& function,
    std::optional<std::string> error) {
  DCHECK(function);
  LOG(INFO) << "DownloadsShowFunction::ShowCallback error: "
            << (error.has_value() ? error.value() : "not exist");

  if (error.has_value()) {
    std::string errorMessage = error.value();
    function->Respond(function->Error(errorMessage));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_downloads_show_) {
    LOG(INFO) << "DownloadsShowFunction Release";
    function->Release();
  }
}

/* downloads.showDefaultFolder */
DownloadsShowDefaultFolderFunction::DownloadsShowDefaultFolderFunction() {}

DownloadsShowDefaultFolderFunction::~DownloadsShowDefaultFolderFunction() {}

ExtensionFunction::ResponseAction DownloadsShowDefaultFolderFunction::Run() {
  OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance()
      .ShowDefaultFolder();

  return RespondNow(NoArguments());
}

/* downloads.search */
DownloadsSearchFunction::DownloadsSearchFunction() {}

DownloadsSearchFunction::~DownloadsSearchFunction() {}

ExtensionFunction::ResponseAction DownloadsSearchFunction::Run() {
  std::optional<downloads::Search::Params> params =
      downloads::Search::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  ExDownloadsQueryInfo queryInfo;
  GetSearchDownloadQueryInfoParams(params, queryInfo);
  queryInfo.contextType = GetExtensionContextType(browser_context());
  queryInfo.includeIncognitoInfo = include_incognito_information();

  call_downloads_search_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance().Search(
          queryInfo,
          base::BindRepeating(&DownloadsSearchFunction::SearchCallback,
                              weak_ptr_factory_.GetWeakPtr()));

  call_downloads_search_ = false;

  if (did_respond()) {
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    return RespondLater();
  }

  return RespondNow(Error("not support downloads.search."));
}

void DownloadsSearchFunction::SearchCallback(
    const base::WeakPtr<DownloadsSearchFunction>& function,
    std::optional<std::string> error,
    const uint32_t count,
    std::vector<ExDownloadsItem>& download_items) {
  LOG(INFO) << "DownloadsSearchFunction SearchCallback";
  DCHECK(function);
  if (error.has_value()) {
    LOG(INFO) << "DownloadsSearchFunction SearchCallback error: "
              << error.value();
    std::string errorMessage = error.value();
    function->Respond(function->Error(errorMessage));
  } else {
    LOG(INFO) << "DownloadsSearchFunction SearchCallback has "
              << download_items.size() << " items.";
    base::Value::List json_results;
    for (uint32_t i = 0; i < download_items.size(); i++) {
      base::Value::Dict json_item = DownloadItemsToJSON(download_items[i]);
      json_results.Append(std::move(json_item));
    }
    function->Respond(function->WithArguments(std::move(json_results)));
  }

  if (!function->call_downloads_search_) {
    LOG(INFO) << "DownloadsSearchFunction Release";
    function->Release();
  }
}

/* downloads.download */
void DownloadsDownloadFunction::OnStarted(
    const base::FilePath& creator_suggested_filename,
    downloads::FilenameConflictAction creator_conflict_action,
    download::DownloadItem* item,
    download::DownloadInterruptReason interrupt_reason) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  if (!item) {
    DCHECK_NE(download::DOWNLOAD_INTERRUPT_REASON_NONE, interrupt_reason);
    Respond(Error(download::DownloadInterruptReasonToString(interrupt_reason)));
    return;
  }

  DCHECK_EQ(download::DOWNLOAD_INTERRUPT_REASON_NONE, interrupt_reason);
  call_downloads_download_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance().GetDownloadId(
          item->GetGuid(),
          base::BindRepeating(&DownloadsDownloadFunction::GetDownloadIdCallback,
                              weak_ptr_factory_.GetWeakPtr(), item,
                              creator_suggested_filename,
                              creator_conflict_action));
  LOG(INFO) << "DownloadsDownloadFunction::OnStarted success: " << success;
  call_downloads_download_ = false;

  if (did_respond()) {
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << " DownloadsDownloadFunction::OnStarted will respondlater";
    return RespondLater();
  }
}

void DownloadsDownloadFunction::GetDownloadIdCallback(
    const base::WeakPtr<DownloadsDownloadFunction>& function,
    download::DownloadItem* item,
    const base::FilePath& creator_suggested_filename,
    downloads::FilenameConflictAction creator_conflict_action,
    std::optional<std::string> error,
    int downloadId) {
  LOG(INFO) << "DownloadsDownloadFunction::GetDownloadIdCallback "
               "downloadId: "
            << (int)downloadId;
  if (error.has_value()) {
    LOG(INFO) << "DownloadsDownloadFunction GetDownloadIdCallback error: "
              << error.value();
    std::string errorMessage = error.value();
    function->Respond(function->Error(errorMessage));
  } else {
    function->Respond(function->WithArguments(downloadId));

    ExDownloadsItem itemStruct = GetExDownloadsItemByNwebItem(
        item, downloadId, function->browser_context());
    itemStruct.byExtensionId = function->extension()->id();
    itemStruct.byExtensionName = function->extension()->name();

    ExDownloadsItemData* itemClass = ExDownloadsItemData::Get(itemStruct.id);
    if (!itemClass) {
      LOG(INFO) << "DownloadsDownloadFunction::GetDownloadIdCallback !item_class";
      itemClass = new ExDownloadsItemData(itemStruct);
    }

    if (!creator_suggested_filename.empty() ||
        (creator_conflict_action !=
         downloads::FilenameConflictAction::kUniquify)) {
      ExtensionDownloadsEventRouterDataEx* data =
          ExtensionDownloadsEventRouterDataEx::Get(itemClass);
      if (!data) {
        LOG(INFO) << "DownloadsDownloadFunction GetDownloadIdCallback !data";
        data = new ExtensionDownloadsEventRouterDataEx(itemClass,
                                                       base::Value::Dict());
      }
      data->CreatorSuggestedFilename(creator_suggested_filename,
                                     creator_conflict_action);
    }
    new DownloadedByExtensionEx(itemClass, function->extension()->id(),
                                function->extension()->name());
  }

  if (!function->call_downloads_download_) {
    LOG(INFO) << "DownloadsDownloadFunction Release";
    function->Release();
  }
}

/* downloads.getFileIcon */
DownloadsGetFileIconFunction::DownloadsGetFileIconFunction() {}

DownloadsGetFileIconFunction::~DownloadsGetFileIconFunction() {}

ExtensionFunction::ResponseAction DownloadsGetFileIconFunction::Run() {
  LOG(INFO) << "DownloadsGetFileIconFunction::Run start";
  std::optional<downloads::GetFileIcon::Params> params =
      downloads::GetFileIcon::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }
  ExDownloadsGetFileIconOptions iconOption;
  iconOption.downloadId = params->download_id;
  if (params->options.has_value()) {
    iconOption.size = params->options->size;
  }
  LOG(INFO) << "DownloadsGetFileIconFunction run downloadId: "
            << iconOption.downloadId;
  if (iconOption.size) {
    LOG(INFO) << "DownloadsGetFileIconFunction run size: " << *iconOption.size;
  } else {
    LOG(INFO) << "DownloadsGetFileIconFunction run size: "
              << "null";
  }

  if (iconOption.size &&
      (*(iconOption.size) != 16 && *(iconOption.size) != 32)) {
    return RespondNow(Error("Invalid `size`. Must be either `16` or `32`."));
  }

  iconOption.contextType = GetExtensionContextType(browser_context());
  iconOption.includeIncognitoInfo = include_incognito_information();

  call_downloads_getfileicon_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance().GetFileIcon(
          iconOption, base::BindRepeating(
                          &DownloadsGetFileIconFunction::GetFileIconCallback,
                          weak_ptr_factory_.GetWeakPtr()));

  call_downloads_getfileicon_ = false;

  if (did_respond()) {
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    return RespondLater();
  }

  return RespondNow(Error("not support downloads.getFileIcon."));
}

void DownloadsGetFileIconFunction::GetFileIconCallback(
    const base::WeakPtr<DownloadsGetFileIconFunction>& function,
    std::optional<std::string> error,
    std::string iconUrl) {
  LOG(INFO) << "DownloadsGetFileIconFunction GetFileIconCallback length: "
            << iconUrl.size();
  if (error) {
    LOG(INFO) << "DownloadsGetFileIconFunction GetFileIconCallback error: "
              << error.value_or("not exist.");
    function->Respond(function->Error(*error));
  } else {
    if (iconUrl.size() == 0) {
      function->Respond(function->Error("Icon not found"));
    }
    function->Respond(function->WithArguments(iconUrl));
  }

  if (!function->call_downloads_getfileicon_) {
    function->Release();
  }
}

}  // namespace extensions
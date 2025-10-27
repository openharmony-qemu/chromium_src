/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "libcef/browser/printing/ohos_print_manager.h"

#include <fcntl.h>

#include <codecvt>
#include <locale>
#include <utility>

#include "base/command_line.h"
#include "base/file_descriptor_posix.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/memory/ref_counted_memory.h"
#include "base/numerics/safe_conversions.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "chrome/browser/printing/print_view_manager_common.h"
#include "chrome/common/chrome_switches.h"
#include "components/pdf/browser/pdf_frame_util.h"
#include "components/printing/browser/print_manager_utils.h"
#include "components/printing/common/print.mojom.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_frame_host.h"
#include "libcef/browser/browser_host_base.h"
#include "printing/print_job_constants.h"
#include "printing/units.h"

namespace printing {

namespace {

constexpr int PRINT_JOB_CREATE_FILE_COMPLETED_SUCCESS = 0;
constexpr int PRINT_JOB_CREATE_FILE_COMPLETED_FAILED = 1;

constexpr int PRINT_JOB_SPOOLER_CLOSED_FOR_CANCELED = 5;

constexpr int LIMITED_PRINT_RANGE = 5;
constexpr int LIMITED_PRINT_DURATION = 10000;
const std::string PROTOCOL_PATH = "://";

uint32_t SaveDataToFd(int fd,
                      uint32_t page_count,
                      scoped_refptr<base::RefCountedSharedMemoryMapping> data) {
  bool result = fd > base::kInvalidFd &&
                base::IsValueInRangeForNumericType<int>(data->size());
  if (result) {
    result = base::WriteFileDescriptor(fd, *data);
  }
  return result ? page_count : 0;
}

int MilsToDots(int val, int dpi) {
  return static_cast<int>(printing::ConvertUnitFloat(val, 1000, dpi));
}

OHOS::NWeb::PrintAttributesAdapter TransformPrintAttrs(
    std::shared_ptr<OHOS::NWeb::NWebPrintAttributesAdapter> newAttrs)
{
  OHOS::NWeb::PrintAttributesAdapter attrs;
  attrs.copyNumber = newAttrs->GetUInt32(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_COPY_NUMBER);
  attrs.pageRange.startPage = newAttrs->GetUInt32(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_PAGE_RANGE_START);
  attrs.pageRange.endPage = newAttrs->GetUInt32(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_PAGE_RANGE_END);
  attrs.pageRange.pages = newAttrs->GetUint32Vector(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_PAGE_RANGE_ARRAY);
  attrs.isSequential = newAttrs->GetBool(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_PAGE_IS_SEQUENTIAL);
  attrs.pageSize.width = newAttrs->GetUInt32(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_PAGE_SIZE_WIDTH);
  attrs.pageSize.height = newAttrs->GetUInt32(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_PAGE_SIZE_HEIGHT);
  attrs.isLandscape = newAttrs->GetBool(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_IS_LANDSCAPE);
  attrs.colorMode = newAttrs->GetUInt32(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_COLOR_MODE);
  attrs.duplexMode = newAttrs->GetUInt32(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_DUPLEX_MODE);
  attrs.margin.top = newAttrs->GetUInt32(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_MARGIN_TOP);
  attrs.margin.bottom = newAttrs->GetUInt32(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_MARGIN_BOTTOM);
  attrs.margin.left = newAttrs->GetUInt32(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_MARGIN_LEFT);
  attrs.margin.right = newAttrs->GetUInt32(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_MARGIN_RIGHT);
  attrs.hasOption = newAttrs->GetBool(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_HAS_OPTION);
  attrs.option = newAttrs->GetString(
      OHOS::NWeb::NWEB_PRINT_ATTR_ID_OPTION);
  return attrs;
}

int32_t ConvertUint32ToInt32(uint32_t value)
{
  if (base::IsValueInRangeForNumericType<int32_t>(value)) {
    return static_cast<int32_t>(value);
  }
  return -1;
}

}  // namespace

class OhosPrintManager;

class PrintDocumentAdapterImpl
    : public OHOS::NWeb::PrintDocumentAdapterAdapter {
 public:
  PrintDocumentAdapterImpl(content::GlobalRenderFrameHostId rfhId)
      : rfhId_(rfhId) {}
  ~PrintDocumentAdapterImpl() = default;

  void OnStartLayoutWrite(
      const std::string& jobId,
      const OHOS::NWeb::PrintAttributesAdapter& oldAttrs,
      const OHOS::NWeb::PrintAttributesAdapter& newAttrs,
      uint32_t fd,
      std::shared_ptr<OHOS::NWeb::PrintWriteResultCallbackAdapter> callback)
      override {
    LOG(INFO) << "OhosPrintManager onStartLayoutWrite.";
    PrintAttrs printAttrs;
    printAttrs.jobId = jobId;
    printAttrs.attrs = newAttrs;
    printAttrs.fd = ConvertUint32ToInt32(fd);
    printAttrs.callback = callback;

    auto main_task_runner = content::GetUIThreadTaskRunner({});
    if (!main_task_runner) {
      LOG(ERROR) << "failed to get main_task_runner";
      return;
    }

    main_task_runner->PostTask(
        FROM_HERE, base::BindOnce(&OnStartLayoutWriteOnUIThread,
                                  rfhId_, printAttrs));
  }

  void OnJobStateChanged(const std::string& jobId, uint32_t state) override {
    LOG(INFO) << "OhosPrintManager onJobStateChanged.";
    state_ = state;

    auto main_task_runner = content::GetUIThreadTaskRunner({});
    if (!main_task_runner) {
      LOG(ERROR) << "failed to get main_task_runner";
      return;
    }

    main_task_runner->PostTask(
        FROM_HERE, base::BindOnce(&OnJobStateChangedOnUIThread, rfhId_,
                                  jobId, state, isCalledOnJobStateChanged));
    if (!isCalledOnJobStateChanged) {
      isCalledOnJobStateChanged = true;
    }
  }

 private:
  static void OnStartLayoutWriteOnUIThread(
      content::GlobalRenderFrameHostId rfhId, PrintAttrs printAttrs) {
    LOG(INFO) << "OhosPrintManager OnStartLayoutWriteOnUIThread.";

    auto* ohosPrintManager = OhosPrintManager::GetOhosPrintManagerToUse(rfhId);
    if (ohosPrintManager) {
      ohosPrintManager->SetPrintAttrs(printAttrs);
      ohosPrintManager->PrintPageImpl(false);
    } else {
      LOG(ERROR) << "failed to get OhosPrintManager";
    }
  }

  static void OnJobStateChangedOnUIThread(
      content::GlobalRenderFrameHostId rfhId, const std::string& jobId,
      uint32_t state, bool isCalled) {
    LOG(INFO) << "OhosPrintManager OnJobStateChangedOnUIThread.";

    auto* ohosPrintManager = OhosPrintManager::GetOhosPrintManagerToUse(rfhId);
    if (ohosPrintManager) {
      ohosPrintManager->SetPrintStatus(false, state);
      if (!isCalled) {
        ohosPrintManager->RunPrintRequestedCallbackImpl(jobId);
      }
      if (state == PRINT_JOB_SPOOLER_CLOSED_FOR_CANCELED) {
        ohosPrintManager->ClearPrintAttrs(jobId);
      }
    } else {
      LOG(ERROR) << "failed to get OhosPrintManager";
    }
  }
  uint32_t state_ = 0;
  bool isCalledOnJobStateChanged = false;
  content::GlobalRenderFrameHostId rfhId_;
};

class ApplicationPrintDocumentAdapterImpl
    : public OHOS::NWeb::PrintDocumentAdapterAdapter {
 public:
  ApplicationPrintDocumentAdapterImpl(content::GlobalRenderFrameHostId rfhId)
      : rfhId_(rfhId) {}
  ~ApplicationPrintDocumentAdapterImpl() = default;

  void OnStartLayoutWrite(
      const std::string& jobId,
      const OHOS::NWeb::PrintAttributesAdapter& oldAttrs,
      const OHOS::NWeb::PrintAttributesAdapter& newAttrs,
      uint32_t fd,
      std::shared_ptr<OHOS::NWeb::PrintWriteResultCallbackAdapter> callback)
      override {
    LOG(INFO) << "Application OhosPrintManager onStartLayoutWrite.";
    PrintAttrs printAttrs;
    printAttrs.jobId = jobId;
    printAttrs.attrs = newAttrs;
    printAttrs.fd = ConvertUint32ToInt32(fd);
    printAttrs.callback = callback;

    auto main_task_runner = content::GetUIThreadTaskRunner({});
    if (!main_task_runner) {
      LOG(ERROR) << "failed to get main_task_runner";
      return;
    }

    main_task_runner->PostTask(
        FROM_HERE, base::BindOnce(&OnStartLayoutWriteOnUIThread, rfhId_,
                                  printAttrs, isCalledBeforeEvent));
    if (!isCalledBeforeEvent) {
      isCalledBeforeEvent = true;
    }
  }

  void OnJobStateChanged(const std::string& jobId, uint32_t state) override {
    LOG(INFO) << "Application OhosPrintManager onJobStateChanged.";
    state_ = state;

    auto main_task_runner = content::GetUIThreadTaskRunner({});
    if (!main_task_runner) {
      LOG(ERROR) << "failed to get main_task_runner";
      return;
    }

    main_task_runner->PostTask(
        FROM_HERE, base::BindOnce(&OnJobStateChangedOnUIThread, rfhId_,
                                  jobId, state, isCalledOnJobStateChanged));
    if (!isCalledOnJobStateChanged) {
      isCalledOnJobStateChanged = true;
    }
  }

  static void OnStartLayoutWriteOnUIThread(
      content::GlobalRenderFrameHostId rfhId, PrintAttrs printAttrs,
      bool isCalled) {
    LOG(INFO) << "Application OhosPrintManager OnStartLayoutWriteOnUIThread.";

    auto* ohosPrintManager = OhosPrintManager::GetOhosPrintManagerToUse(rfhId);
    if (ohosPrintManager) {
      if (!isCalled) {
        ohosPrintManager->DidDispatchPrintEventImpl(true);
      }
      ohosPrintManager->SetPrintAttrs(printAttrs);
      ohosPrintManager->PrintPageImpl(true);
    } else {
      LOG(ERROR) << "failed to get OhosPrintManager";
    }
  }

  static void OnJobStateChangedOnUIThread(
      content::GlobalRenderFrameHostId rfhId, const std::string& jobId,
      uint32_t state, bool isCalled) {
    LOG(INFO) << "OhosPrintManager OnJobStateChangedOnUIThread.";

    auto* ohosPrintManager = OhosPrintManager::GetOhosPrintManagerToUse(rfhId);
    if (ohosPrintManager) {
      ohosPrintManager->SetPrintStatus(false, state);
      if (!isCalled) {
        ohosPrintManager->DidDispatchPrintEventImpl(false);
      }
      if (state == PRINT_JOB_SPOOLER_CLOSED_FOR_CANCELED) {
        ohosPrintManager->ClearPrintAttrs(jobId);
      }
    } else {
      LOG(ERROR) << "failed to get OhosPrintManager";
    }
  }

private:
  uint32_t state_ = 0;
  bool isCalledBeforeEvent = false;
  bool isCalledOnJobStateChanged = false;
  content::GlobalRenderFrameHostId rfhId_;
};

void PrintWriteResultCallbackAdapterV2::WriteResultCallback(std::string jobId,
    uint32_t code)
{
  if (cb_) {
    cb_->WriteResultCallback(jobId, code);
  }
}

class ApplicationPrintDocumentAdapterImplV2
    : public OHOS::NWeb::NWebPrintDocumentAdapterAdapter {
 public:
  explicit ApplicationPrintDocumentAdapterImplV2(
      content::GlobalRenderFrameHostId rfhId) : rfhId_(rfhId) {}
  ~ApplicationPrintDocumentAdapterImplV2() = default;

  void OnStartLayoutWrite(
      const std::string& jobId,
      std::shared_ptr<OHOS::NWeb::NWebPrintAttributesAdapter> oldAttrs,
      std::shared_ptr<OHOS::NWeb::NWebPrintAttributesAdapter> newAttrs,
      uint32_t fd,
      std::shared_ptr<OHOS::NWeb::NWebPrintWriteResultCallbackAdapter> callback)
      override {
    LOG(INFO) << "Application OhosPrintManager OnStartLayoutWrite V2.";
    if (!newAttrs ||
        !newAttrs->GetBool(OHOS::NWeb::NWEB_PRINT_ATTR_ID_IS_VALID)) {
      LOG(ERROR) << "invalid print attr";
      return;
    }

    PrintAttrs printAttrs;
    printAttrs.jobId = jobId;
    printAttrs.attrs = TransformPrintAttrs(newAttrs);
    printAttrs.fd = ConvertUint32ToInt32(fd);
    printAttrs.callback =
      std::make_shared<PrintWriteResultCallbackAdapterV2>(callback);

    auto main_task_runner = content::GetUIThreadTaskRunner({});
    if (!main_task_runner) {
      LOG(ERROR) << "failed to get main_task_runner";
      return;
    }

    main_task_runner->PostTask(
      FROM_HERE, base::BindOnce(
      &ApplicationPrintDocumentAdapterImpl::OnStartLayoutWriteOnUIThread,
      rfhId_, printAttrs, isCalledBeforeEvent));
    if (!isCalledBeforeEvent) {
      isCalledBeforeEvent = true;
    }
  }

  void OnJobStateChanged(const std::string& jobId, uint32_t state) override {
    LOG(INFO) << "Application OhosPrintManager OnJobStateChanged V2.";
    state_ = state;

    auto main_task_runner = content::GetUIThreadTaskRunner({});
    if (!main_task_runner) {
      LOG(ERROR) << "failed to get main_task_runner";
      return;
    }

    main_task_runner->PostTask(
        FROM_HERE, base::BindOnce(
        &ApplicationPrintDocumentAdapterImpl::OnJobStateChangedOnUIThread,
        rfhId_, jobId, state, isCalledOnJobStateChanged));
    if (!isCalledOnJobStateChanged) {
      isCalledOnJobStateChanged = true;
    }
  }

 private:
  uint32_t state_ = 0;
  bool isCalledBeforeEvent = false;
  bool isCalledOnJobStateChanged = false;
  content::GlobalRenderFrameHostId rfhId_;
};

OhosPrintManager::OhosPrintManager(content::WebContents* contents)
    : PrintManager(contents),
      content::WebContentsUserData<OhosPrintManager>(*contents),
      task_runner_(base::ThreadPool::CreateTaskRunner(
          {base::MayBlock(), base::TaskPriority::USER_BLOCKING,
           base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN})),
      weak_ptr_web_contents_(contents->GetWeakPtr()) {}

OhosPrintManager::~OhosPrintManager() = default;

std::unordered_map<std::string, PrintAttrs> OhosPrintManager::printAttrsMap_{};
std::string OhosPrintManager::print_job_id_ = "";
std::unordered_map<uint32_t, void*> OhosPrintManager::printTokenMap_{};
// static
void OhosPrintManager::BindPrintManagerHost(
    mojo::PendingAssociatedReceiver<printing::mojom::PrintManagerHost> receiver,
    content::RenderFrameHost* rfh) {
  LOG(INFO) << "OhosPrintManager::BindPrintManagerHost";
  auto* web_contents = content::WebContents::FromRenderFrameHost(rfh);
  if (!web_contents) {
    LOG(ERROR) << "web_contents is nullptr.";
    return;
  }

  auto* print_manager = OhosPrintManager::FromWebContents(web_contents);
  if (!print_manager) {
    LOG(ERROR) << "print_manager is nullptr.";
    return;
  }

  print_manager->SetRfhId(rfh->GetGlobalId());
  print_manager->BindReceiver(std::move(receiver), rfh);
}

// static
content::RenderFrameHost* OhosPrintManager::GetRenderFrameHostToUse(
    content::WebContents* contents) {
  // Pick the plugin frame if `contents` is a PDF viewer guest.
  content::RenderFrameHost* full_page_plugin = GetFullPagePlugin(contents);
  content::RenderFrameHost* pdf_rfh = pdf_frame_util::FindPdfChildFrame(
      full_page_plugin ? full_page_plugin : contents->GetPrimaryMainFrame());
  if (pdf_rfh) {
    return pdf_rfh;
  }
  return printing::GetFrameToPrint(contents);
}

// static
OhosPrintManager* OhosPrintManager::GetOhosPrintManagerToUse(
    content::GlobalRenderFrameHostId rfhId) {
  auto* rfh = content::RenderFrameHost::FromID(rfhId);
  if (!rfh) {
    LOG(ERROR) << "failed to get rfh from id";
    return nullptr;
  }

  auto* web_contents = content::WebContents::FromRenderFrameHost(rfh);
  if (!web_contents) {
    LOG(ERROR) << "failed to get web_contents from rfh";
    return nullptr;
  }

  auto* print_manager = OhosPrintManager::FromWebContents(web_contents);
  if (!print_manager) {
    LOG(ERROR) << "failed to get print_manager from web_contents";
    return nullptr;
  }
  return print_manager;
}

void OhosPrintManager::PdfWritingDone(int page_count) {
  pdf_writing_done_callback().Run(page_count);
  fd_ = -1;
}

bool OhosPrintManager::PrintNow() {
  if (is_print_now_) {
    LOG(ERROR) << "printing in progress.";
    return false;
  }
  LOG(INFO) << "OhosPrintManager::PrintNow";
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  std::string printJobName = GetHtmlTitle();
  std::shared_ptr<OHOS::NWeb::PrintDocumentAdapterAdapter>
      printDocumentAdapterImpl(new PrintDocumentAdapterImpl(GetRfhId()));
  OHOS::NWeb::PrintAttributesAdapter printAttributesAdapter;

  if (!token_ && printTokenMap_.find(base::Process::Current().Pid()) !=
                     printTokenMap_.end()) {
    token_ = printTokenMap_[base::Process::Current().Pid()];
  }
  int32_t ret = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                    .GetPrintManagerInstance()
                    .Print(printJobName, printDocumentAdapterImpl,
                           printAttributesAdapter, token_);
  LOG(INFO) << "OhosPrintManager::PrintNow ret = " << ret;
  if (ret == -1) {
    LOG(ERROR) << "print failed";
    return false;
  }
  return true;
}

void OhosPrintManager::DidDispatchPrintEventImpl(bool isBefore) {
  LOG(INFO) << "OhosPrintManager::DidDispatchPrintEventImpl isBefore = "
            << isBefore;
  auto web_contents = weak_ptr_web_contents_;
  if (!web_contents) {
    LOG(ERROR) << "DidDispatchPrintEventImpl webContents is nullptr.";
    return;
  }
  auto* rfh = web_contents->GetPrimaryMainFrame();
  if (!rfh || !rfh->IsRenderFrameLive()) {
    LOG(ERROR) << "rfh is nullptr.";
    return;
  }
  GetPrintRenderFrame(rfh)->DidDispatchPrintEvent(isBefore);
}

void OhosPrintManager::PrintPageImpl(bool isApplication) {
  LOG(INFO) << "OhosPrintManager::PrintPageImpl isApplication = "
            << isApplication;
  auto web_contents = weak_ptr_web_contents_;
  if (!web_contents) {
    LOG(ERROR) << "PrintPageImpl webContents is nullptr.";
    return;
  }
  auto* rfh = web_contents->GetPrimaryMainFrame();
  if (!rfh || !rfh->IsRenderFrameLive()) {
    LOG(ERROR) << "rfh is nullptr.";
    RunCallback(print_job_id_, PRINT_JOB_CREATE_FILE_COMPLETED_FAILED);
    return;
  }

  if (!pdf_rfh_id_) {
    pdf_rfh_id_ = rfh_id_;
  }

  if (is_pdf_print_) {
    auto pdf_rfh = content::RenderFrameHost::FromID(pdf_rfh_id_);
    if (pdf_rfh) {
      GetPrintRenderFrame(pdf_rfh)->ApplicationPrintRequestedPages();
    } else {
      LOG(ERROR) << "failed to get rfh from id for pdf print";
      RunCallback(print_job_id_, PRINT_JOB_CREATE_FILE_COMPLETED_FAILED);
    }
    return;
  }

  if (isApplication) {
    auto* app_rfh = GetRenderFrameHostToUse(web_contents.get());
    if (app_rfh) {
      GetPrintRenderFrame(app_rfh)->ApplicationPrintRequestedPages();
    }
  } else {
    GetPrintRenderFrame(rfh)->PrintRequestedPages();
  }
}

void OhosPrintManager::DidShowPrintDialog() {
  LOG(INFO) << "OhosPrintManager::DidShowPrintDialog";
}

void OhosPrintManager::GetDefaultPrintSettings(
    GetDefaultPrintSettingsCallback callback) {
  // Please process in the UI thread.
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  auto params = printing::mojom::PrintParams::New();

  printing::PageRanges page_ranges;
  PdfWritingDoneCallback pdf_writing_done_callback =
      base::BindRepeating([](int page_count) {
        LOG(INFO) << "OhosPrintManager::pdf_writing_done_callback page_count = "
                  << page_count;
      });
  UpdateParam(CreatePdfSettings(page_ranges), fd_, pdf_writing_done_callback);

  if (!settings_) {
    LOG(ERROR) << "settings_ is invalid.";
    return;
  }
  printing::RenderParamsFromPrintSettings(*settings_, params.get());
  params->document_cookie = cookie();
  std::move(callback).Run(std::move(params));
}

void OhosPrintManager::UpdateParam(
    std::unique_ptr<printing::PrintSettings> settings,
    int file_descriptor,
    PrintManager::PdfWritingDoneCallback callback) {
  DCHECK(settings);
  DCHECK(callback);
  settings_ = std::move(settings);
  fd_ = file_descriptor;

  set_pdf_writing_done_callback(std::move(callback));
  // Set a valid dummy cookie value.
  set_cookie(1);
}

void OhosPrintManager::ScriptedPrint(
    printing::mojom::ScriptedPrintParamsPtr scripted_params,
    ScriptedPrintCallback callback) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  auto params = printing::mojom::PrintPagesParams::New();
  params->params = printing::mojom::PrintParams::New();

  if (scripted_params->is_scripted &&
      GetCurrentTargetFrame()->IsNestedWithinFencedFrame()) {
    LOG(ERROR) << "Script Print is not allowed.";
    std::move(callback).Run(std::move(params));
    return;
  }

  if (!settings_) {
    LOG(ERROR) << "settings_ is invalid.";
    return;
  }
  printing::RenderParamsFromPrintSettings(*settings_, params->params.get());
  params->params->document_cookie = scripted_params->cookie;
  params->pages = settings_->ranges();
  std::move(callback).Run(std::move(params));
}

void OhosPrintManager::DidPrintDocument(
    printing::mojom::DidPrintDocumentParamsPtr params,
    DidPrintDocumentCallback callback) {
  LOG(INFO) << "OhosPrintManager::DidPrintDocument";
  if (params->document_cookie != cookie()) {
    std::move(callback).Run(false);
    return;
  }

  const printing::mojom::DidPrintContentParams& content = *params->content;
  if (!content.metafile_data_region.IsValid()) {
    NOTREACHED() << "invalid memory handle";
    if (weak_ptr_web_contents_) {
      weak_ptr_web_contents_->Stop();
    }
    PdfWritingDone(0);
    std::move(callback).Run(false);
    return;
  }

  auto data = base::RefCountedSharedMemoryMapping::CreateFromWholeRegion(
      content.metafile_data_region);
  if (!data) {
    NOTREACHED() << "couldn't map";
    if (weak_ptr_web_contents_) {
      weak_ptr_web_contents_->Stop();
    }
    PdfWritingDone(0);
    std::move(callback).Run(false);
    return;
  }

  if (number_pages() > printing::kMaxPageCount) {
    if (weak_ptr_web_contents_) {
      weak_ptr_web_contents_->Stop();
    }
    PdfWritingDone(0);
    std::move(callback).Run(false);
    return;
  }

  DCHECK(pdf_writing_done_callback());
  task_runner_->PostTaskAndReplyWithResult(
      FROM_HERE, base::BindOnce(&SaveDataToFd, fd_, number_pages(), data),
      base::BindOnce(&OhosPrintManager::OnDidPrintDocumentWritingDone,
                     pdf_writing_done_callback(), std::move(callback)));
}

// static
void OhosPrintManager::RunCallback(const std::string& jobId, int32_t result) {
  if (printAttrsMap_.find(jobId) == printAttrsMap_.end()) {
    return;
  }

  if (printAttrsMap_[jobId].callback) {
    LOG(INFO) << "OhosPrintManager calls WriteResultCallback with " << result;
    printAttrsMap_[jobId].callback->WriteResultCallback(jobId, result);
    printAttrsMap_[jobId].callback = nullptr;
  } else {
    LOG(INFO) << "OhosPrintManager WriteResultCallback is empty";
  }
}

// static
void OhosPrintManager::OnDidPrintDocumentWritingDone(
    const PdfWritingDoneCallback& callback,
    DidPrintDocumentCallback did_print_document_cb,
    uint32_t page_count) {
  LOG(INFO) << "OhosPrintManager::OnDidPrintDocumentWritingDone";
  DCHECK_LE(page_count, printing::kMaxPageCount);
  if (callback) {
    callback.Run(base::checked_cast<int>(page_count));
  }
  std::move(did_print_document_cb).Run(true);
  RunCallback(print_job_id_, PRINT_JOB_CREATE_FILE_COMPLETED_SUCCESS);
}

std::unique_ptr<printing::PrintSettings> OhosPrintManager::CreatePdfSettings(
    const printing::PageRanges& page_ranges) {
  OHOS::NWeb::PrintAttributesAdapter newAttrs;
  if (printAttrsMap_.find(print_job_id_) != printAttrsMap_.end()) {
    newAttrs = printAttrsMap_[print_job_id_].attrs;
  }
  auto settings = std::make_unique<printing::PrintSettings>();
  int dpi = dpi_;
  gfx::Size physical_size_device_units;
  int width_in_dots = MilsToDots(
      newAttrs.pageSize.width ? newAttrs.pageSize.width : width_, dpi);
  int height_in_dots = MilsToDots(
      newAttrs.pageSize.height ? newAttrs.pageSize.height : height_, dpi);
  physical_size_device_units.SetSize(width_in_dots, height_in_dots);

  gfx::Rect printable_area_device_units;

  printable_area_device_units.SetRect(0, 0, width_in_dots, height_in_dots);

  if (!page_ranges.empty()) {
    settings->set_ranges(page_ranges);
  }

  settings->set_dpi(dpi);
  settings->SetPrinterPrintableArea(physical_size_device_units,
                                    printable_area_device_units, true);

  printing::PageMargins margins;
  margins.left = newAttrs.margin.left;
  margins.right = newAttrs.margin.right;
  margins.top = newAttrs.margin.top;
  margins.bottom = newAttrs.margin.bottom;
  settings->SetCustomMargins(margins);
  settings->set_should_print_backgrounds(should_print_background_);
  settings->SetOrientation(newAttrs.isLandscape);
  return settings;
}

void OhosPrintManager::SetPrintAttrs(const PrintAttrs printAttrs) {
  // clear old callback if any
  RunCallback(printAttrs.jobId, PRINT_JOB_CREATE_FILE_COMPLETED_FAILED);
  printAttrsMap_[printAttrs.jobId] = printAttrs;
  if (base::IsValueInRangeForNumericType<int>(printAttrs.fd)) {
    fd_ = static_cast<int>(printAttrs.fd);
  } else {
    fd_ = -1;
  }
  print_job_id_ = printAttrs.jobId;
}

void OhosPrintManager::ClearPrintAttrs(const std::string& jobId) {
  // clear callback before erase
  RunCallback(jobId, PRINT_JOB_CREATE_FILE_COMPLETED_FAILED);
  printAttrsMap_.erase(jobId);
  fd_ = -1;
  print_job_id_ = "";
}

void OhosPrintManager::PrintRequested(PrintRequestedCallback callback) {
  is_pdf_print_ = false;
  if (!PrintNow()) {
    LOG(ERROR) << "Pulling up the printing application failed.";
    std::move(callback).Run();
    return;
  }
  cancel_ = false;
  printRequestedCallback_ = std::move(callback);
}

void OhosPrintManager::CheckCancel(CheckCancelCallback callback) {
  std::move(callback).Run(cancel_);
}

void OhosPrintManager::RunPrintRequestedCallbackImpl(const std::string& jobId) {
  LOG(INFO) << "OhosPrintManager::RunPrintRequestedCallbackImpl.";
  cancel_ = true;
  if (is_pdf_print_) {
    DidDispatchPrintEventImpl(false);
    is_pdf_print_ = false;
    return;
  }
  if (printRequestedCallback_) {
    std::move(printRequestedCallback_).Run();
  }
}

void OhosPrintManager::PrintPdfRequested() {
  is_pdf_print_ = true;
  if (printTokenMap_.find(base::Process::Current().Pid()) !=
      printTokenMap_.end()) {
    token_ = printTokenMap_[base::Process::Current().Pid()];
  }
  cancel_ = false;
  if (!PrintNow()) {
    is_pdf_print_ = false;
    LOG(ERROR) << "Pulling up the printing application failed.";
  }
}

std::string OhosPrintManager::GetHtmlTitle() {
  std::u16string u16str = u"";
  std::string printJobName = "";
  if (weak_ptr_web_contents_) {
    u16str = weak_ptr_web_contents_->GetTitle();
  }
  printJobName = base::UTF16ToUTF8(u16str);
  printJobName = RemoveProtocol(printJobName);
  std::replace(printJobName.begin(), printJobName.end(), '/', '_');
  std::replace(printJobName.begin(), printJobName.end(), '?', '_');
  LOG(INFO) << "OhosPrintManager::GetHtmlTitle printJobName is = "
            << printJobName;
  return printJobName;
}

std::string OhosPrintManager::RemoveProtocol(const std::string& url) {
  LOG(INFO) << "OhosPrintManager::RemoveProtocol";
  std::string result = url;
  std::size_t pos = result.find(PROTOCOL_PATH);
  if (pos != std::string::npos) {
    result = result.substr(pos + PROTOCOL_PATH.size());
  }
  return result;
}

void OhosPrintManager::SetToken(void* token) {
  printTokenMap_[base::Process::Current().Pid()] = token;
  token_ = token;
}

void OhosPrintManager::SetPrintStatus(bool is_print_now, uint32_t state) {
  if (!is_print_disable_) {
    is_print_now_ = is_print_now;
  }

  if (state == PRINT_JOB_SPOOLER_CLOSED_FOR_CANCELED) {
    auto curCancePrintTime = std::chrono::high_resolution_clock::now();
    cancelPrintTimeQueue_.push(curCancePrintTime);
  }

  int qSize = cancelPrintTimeQueue_.size();
  for (int i = 0; i < qSize; i++) {
    auto nowTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> diff =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            nowTime - cancelPrintTimeQueue_.front());
    if (diff.count() >= LIMITED_PRINT_DURATION) {
      cancelPrintTimeQueue_.pop();
    } else {
      break;
    }
  }

  if (cancelPrintTimeQueue_.size() >= LIMITED_PRINT_RANGE) {
    LOG(WARNING)
        << "Printing is frequent, printing on the current page is prohibited.";
    is_print_now_ = true;
    is_print_disable_ = true;
  }
}

void OhosPrintManager::CreateWebPrintDocumentAdapter(
    const CefString& jobName,
    void** webPrintDocumentAdapter) {
  if (is_print_now_) {
    LOG(ERROR) << "Application printing in progress.";
    return;
  }
  cancel_ = false;
  is_print_now_ = true;
  *webPrintDocumentAdapter =
      static_cast<void*>(new ApplicationPrintDocumentAdapterImpl(GetRfhId()));
}

void OhosPrintManager::CreateWebPrintDocumentAdapterV2(
    const CefString& jobName,
    void** adapter) {
  if (is_print_now_) {
    LOG(ERROR) << "Application printing in progress.";
    return;
  }
  cancel_ = false;
  is_print_now_ = true;
  *adapter =
    static_cast<void*>(new ApplicationPrintDocumentAdapterImplV2(GetRfhId()));
}

void OhosPrintManager::SetPrintBackground(bool enable) {
  LOG(INFO) << "OhosPrintManager::SetPrintBackground  = " << enable;
  should_print_background_ = enable;
}

bool OhosPrintManager::GetPrintBackground() {
  LOG(INFO) << "OhosPrintManager::GetPrintBackground = "
            << should_print_background_;
  return should_print_background_;
}

void OhosPrintManager::CheckForCancel(int32_t preview_ui_id,
                                      int32_t request_id,
                                      CheckForCancelCallback callback) {}

void OhosPrintManager::RequestPrintPreview(
    mojom::RequestPrintPreviewParamsPtr params) {}

void OhosPrintManager::SetAccessibilityTree(
    int32_t cookie,
    const ui::AXTreeUpdate& accessibility_tree) {}

void OhosPrintManager::SetupScriptedPrintPreview(
    SetupScriptedPrintPreviewCallback callback) {}

void OhosPrintManager::ShowScriptedPrintPreview(bool source_is_modifiable) {}

void OhosPrintManager::UpdatePrintSettings(
    base::Value::Dict job_settings,
    UpdatePrintSettingsCallback callback) {}

void OhosPrintManager::SetRfhId(content::GlobalRenderFrameHostId rfhId) {
  rfh_id_ = rfhId;
}

content::GlobalRenderFrameHostId OhosPrintManager::GetRfhId() {
  return rfh_id_;
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(OhosPrintManager);

}  // namespace printing

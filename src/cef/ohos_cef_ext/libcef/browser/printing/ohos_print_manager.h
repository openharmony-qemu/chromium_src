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

#ifndef OHOS_PRINT_MANAGER_H_
#define OHOS_PRINT_MANAGER_H_

#include <memory>
#include <vector>

#include "base/memory/raw_ptr.h"
#include "components/printing/browser/print_manager.h"
#include "components/printing/common/print.mojom-forward.h"
#include "content/public/browser/web_contents_user_data.h"
#include "include/cef_base.h"
#include "printing/print_settings.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "ohos_nweb/include/nweb_print_manager_adapter.h"

namespace base {
class TaskRunner;
}

namespace printing {

struct PrintAttrs {
  std::string jobId;
  OHOS::NWeb::PrintAttributesAdapter attrs;
  int32_t fd;
  std::shared_ptr<OHOS::NWeb::PrintWriteResultCallbackAdapter> callback;
};

class PrintWriteResultCallbackAdapterV2 :
    public OHOS::NWeb::PrintWriteResultCallbackAdapter {
public:
    explicit PrintWriteResultCallbackAdapterV2(
        std::shared_ptr<OHOS::NWeb::NWebPrintWriteResultCallbackAdapter> cb)
        : cb_(cb) {}

    void WriteResultCallback(std::string jobId, uint32_t code) override;
private:
    std::shared_ptr<OHOS::NWeb::NWebPrintWriteResultCallbackAdapter> cb_;
};

class OhosPrintManager : public printing::PrintManager,
                         public content::WebContentsUserData<OhosPrintManager> {
 public:
  OhosPrintManager(const OhosPrintManager&) = delete;
  OhosPrintManager& operator=(const OhosPrintManager&) = delete;

  ~OhosPrintManager() override;

  static void BindPrintManagerHost(
      mojo::PendingAssociatedReceiver<printing::mojom::PrintManagerHost>
          receiver,
      content::RenderFrameHost* rfh);
  static content::RenderFrameHost* GetRenderFrameHostToUse(
      content::WebContents* contents);
  static OhosPrintManager* GetOhosPrintManagerToUse(
      content::GlobalRenderFrameHostId rfhId);
  // printing::PrintManager:
  void PdfWritingDone(int page_count) override;

  void DidShowPrintDialog() override;

  // Updates the parameters for printing.
  void UpdateParam(std::unique_ptr<printing::PrintSettings> settings,
                   int file_descriptor,
                   PdfWritingDoneCallback callback);
  bool PrintNow();
  void PrintPageImpl(bool isApplication);
  void DidDispatchPrintEventImpl(bool isBefore);
  void SetPrintAttrs(const PrintAttrs printAttrs);
  void ClearPrintAttrs(const std::string& jobId);
  void RunPrintRequestedCallbackImpl(const std::string& jobId);
  void SetToken(void* token);
  void SetPrintStatus(bool is_print_now, uint32_t state);
  void CreateWebPrintDocumentAdapter(const CefString& jobName,
                                     void** webPrintDocumentAdapter);
  void CreateWebPrintDocumentAdapterV2(const CefString& jobName,
                                       void** adapter);
  void SetPrintBackground(bool enable);
  bool GetPrintBackground();
  void CheckForCancel(int32_t preview_ui_id,
                      int32_t request_id,
                      CheckForCancelCallback callback) override;
  void RequestPrintPreview(mojom::RequestPrintPreviewParamsPtr params) override;
  void SetAccessibilityTree(
      int32_t cookie,
      const ui::AXTreeUpdate& accessibility_tree) override;
  void SetupScriptedPrintPreview(
      SetupScriptedPrintPreviewCallback callback) override;
  void ShowScriptedPrintPreview(bool source_is_modifiable) override;
  void UpdatePrintSettings(base::Value::Dict job_settings,
                           UpdatePrintSettingsCallback callback) override;
  void SetRfhId(content::GlobalRenderFrameHostId rfhId);
  content::GlobalRenderFrameHostId GetRfhId();

 private:
  friend class content::WebContentsUserData<OhosPrintManager>;

  explicit OhosPrintManager(content::WebContents* contents);

  // mojom::PrintManagerHost:
  void DidPrintDocument(printing::mojom::DidPrintDocumentParamsPtr params,
                        DidPrintDocumentCallback callback) override;
  void GetDefaultPrintSettings(
      GetDefaultPrintSettingsCallback callback) override;
  void ScriptedPrint(printing::mojom::ScriptedPrintParamsPtr params,
                     ScriptedPrintCallback callback) override;
  void PrintRequested(PrintRequestedCallback callback) override;
  void CheckCancel(CheckCancelCallback callback) override;
  void PrintPdfRequested() override;

  static void RunCallback(const std::string& jobId, int32_t result);
  static void OnDidPrintDocumentWritingDone(
      const PdfWritingDoneCallback& callback,
      DidPrintDocumentCallback did_print_document_cb,
      uint32_t page_count);

  std::unique_ptr<printing::PrintSettings> CreatePdfSettings(
      const printing::PageRanges& page_ranges);

  void OnScriptedPrint();
  std::string GetHtmlTitle();
  std::string RemoveProtocol(const std::string& url);

  scoped_refptr<base::TaskRunner> task_runner_;
  std::unique_ptr<printing::PrintSettings> settings_;

  int fd_ = -1;
  uint32_t width_ = 8270;
  uint32_t height_ = 11690;
  int dpi_ = 300;  // DPI (Dots Per Inch)
  std::queue<std::chrono::high_resolution_clock::time_point>
      cancelPrintTimeQueue_;
  raw_ptr<void> token_ = nullptr;
  bool cancel_ = false;
  bool is_pdf_print_ = false;
  bool should_print_background_ = true;
  bool is_print_now_ = false;
  bool is_print_disable_ = false;
  content::GlobalRenderFrameHostId rfh_id_;
  content::GlobalRenderFrameHostId pdf_rfh_id_;
  static std::unordered_map<std::string, PrintAttrs> printAttrsMap_;
  static std::string print_job_id_;
  static std::unordered_map<uint32_t, void*> printTokenMap_;
  PrintRequestedCallback printRequestedCallback_;

  base::WeakPtr<content::WebContents> weak_ptr_web_contents_;
  WEB_CONTENTS_USER_DATA_KEY_DECL();
};

}  // namespace printing

#endif  // OHOS_PRINT_MANAGER_H_

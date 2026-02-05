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

#if BUILDFLAG(ARKWEB_PRINT)

namespace {
class SubWindowCallPrint {
  public:
    SubWindowCallPrint() {
      if (!base::ohos::IsPcDevice()) {
        return;
      }
      auto pid = base::GetCurrentRealPid();
      OHOS::NWeb::ResSchedClientAdapter::ReportSubwindowCall(
          OHOS::NWeb::ResSchedStatusAdapter::WEB_SUBWIN_CALL_START,
          static_cast<uint32_t>(pid), static_cast<uint32_t>(pid));
    }
    ~SubWindowCallPrint() {
      if (!base::ohos::IsPcDevice()) {
        return;
      }
      auto pid = base::GetCurrentRealPid();
      OHOS::NWeb::ResSchedClientAdapter::ReportSubwindowCall(
          OHOS::NWeb::ResSchedStatusAdapter::WEB_SUBWIN_CALL_STOP,
          static_cast<uint32_t>(pid), static_cast<uint32_t>(pid));
    }
};
}

void ClosuresForMojoResponse::SetPrintRequestedPreviewQuitClosure(
    base::OnceClosure quit_print_preview)
{
  DCHECK(!print_requested_preview_quit_closure_);
  print_requested_preview_quit_closure_ = std::move(quit_print_preview);
}

void ClosuresForMojoResponse::RunPrintRequestedPreviewQuitClosure()
{
  if (!print_requested_preview_quit_closure_) {
    return;
  }

  std::move(print_requested_preview_quit_closure_).Run();
}

void PrintRenderFrameHelper::DidDispatchPrintEvent(bool isBefore)
{
  blink::WebLocalFrame* web_frame = render_frame()->GetWebFrame();
  if (!web_frame) {
    return;
  }
  if (isBefore) {
    web_frame->DispatchBeforePrintEvent(nullptr);
  } else {
    web_frame->DispatchAfterPrintEvent();
  }
}

void PrintRenderFrameHelper::ApplicationPrintRequestedPages()
{
  SubWindowCallPrint subWindowCallPrint{};
  ScopedIPC scoped_ipc(weak_ptr_factory_.GetWeakPtr());
  if (ipc_nesting_level_ > kAllowedIpcDepthForPrint) {
    return;
  }
  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();
  // Don't print if the RenderFrame is gone.
  if (render_frame_gone_) {
    return;
  }

  // If we are printing a frame with an internal PDF plugin element, find the
  // plugin node and print that instead.
  auto plugin = delegate_->GetPdfElement(frame);

  Print(frame, plugin, PrintRequestType::kRegular);
}

void PrintRenderFrameHelper::PrintRequestedPages() {
  SubWindowCallPrint subWindowCallPrint{};
  PrintRequestedPagesInternal(/*already_notified_frame=*/false);
}

bool PrintRenderFrameHelper::CheckCancel()
{
  bool cancel = false;
  GetPrintManagerHost()->CheckCancel(&cancel);
  return cancel;
}

blink::WebLocalFrame* PrintRenderFrameHelper::static_web_frame_ = nullptr;
#endif  // BUILDFLAG(ARKWEB_PRINT)

bool PrintRenderFrameHelper::PrintQuitLoop(blink::WebLocalFrame* web_frame)
{
#if BUILDFLAG(ARKWEB_PRINT)
  if (delegate_->IsScriptedPrintEnabled()) {
    LOG(INFO) << "OhosPrintManager delegate isScriptedPrintEnabled";
    web_frame->DispatchBeforePrintEvent(nullptr);

    static_web_frame_ = web_frame;

    base::RunLoop loop{base::RunLoop::Type::kNestableTasksAllowed};
    closures_for_mojo_responses_->SetPrintRequestedPreviewQuitClosure(
        loop.QuitClosure());
    GetPrintManagerHost()->PrintRequested(base::BindOnce(
        &ClosuresForMojoResponse::RunPrintRequestedPreviewQuitClosure,
        closures_for_mojo_responses_));
    loop.Run();
    LOG(INFO) << "OhosPrintManager loop quit";
    web_frame->DispatchAfterPrintEvent();
    return true;
  }
#endif  // BUILDFLAG(ARKWEB_PRINT)
  return false;
}

void PrintRenderFrameHelper::ArkWebPrintNode(const blink::WebNode& node)
{
  blink::WebNode duplicate_node(node);

  blink::WebLocalFrame* frame = duplicate_node.GetDocument().GetFrame();
  if (!frame) {
    return;
  }

  frame->DispatchBeforePrintEvent(nullptr);
  GetPrintManagerHost()->PrintPdfRequested();
}

#if BUILDFLAG(ARKWEB_PRINT)
  blink::WebLocalFrame* PrintRenderFrameHelper::ArkWebUpdateFrame(
      blink::WebLocalFrame* frame)
  {
    if (!static_web_frame_) {
      LOG(INFO) << "OhosPrintManager static web frame is null";
      return frame;
    }

    return static_web_frame_;
  }
#endif  // BUILDFLAG(ARKWEB_PRINT)
// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "chrome/renderer/chrome_content_renderer_client.h"
#include "arkweb/chromium_ext/chrome/renderer/arkweb_chrome_content_renderer_client_ext.h"

#include <functional>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>

#include "arkweb/build/features/features.h"
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "base/check_op.h"
#include "base/command_line.h"
#include "base/debug/crash_logging.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/metrics/histogram_functions.h"
#include "base/metrics/user_metrics_action.h"
#include "base/no_destructor.h"
#include "base/notreached.h"
#include "base/process/current_process.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/single_thread_task_runner.h"
#include "base/time/time.h"
#include "base/values.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "chrome/common/buildflags.h"
#include "chrome/common/channel_info.h"
#include "chrome/common/chrome_content_client.h"
#include "chrome/common/chrome_features.h"
#include "chrome/common/chrome_isolated_world_ids.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/crash_keys.h"
#include "chrome/common/pepper_permission_util.h"
#include "chrome/common/ppapi_utils.h"
#include "chrome/common/profiler/chrome_thread_profiler_client.h"
#include "chrome/common/profiler/thread_profiler_configuration.h"
#include "chrome/common/profiler/unwind_util.h"
#include "chrome/common/secure_origin_allowlist.h"
#include "chrome/common/url_constants.h"
#include "chrome/common/webui_url_constants.h"
#include "chrome/common/webui_util.h"
#include "chrome/grit/branded_strings.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/grit/renderer_resources.h"
#include "chrome/renderer/benchmarking_extension.h"
#include "chrome/renderer/browser_exposed_renderer_interfaces.h"
#include "chrome/renderer/chrome_content_settings_agent_delegate.h"
#include "chrome/renderer/chrome_render_frame_observer.h"
#include "chrome/renderer/chrome_render_thread_observer.h"
#include "chrome/renderer/controlled_frame/controlled_frame_extensions_renderer_api_provider.h"
#include "chrome/renderer/google_accounts_private_api_extension.h"
#include "chrome/renderer/loadtimes_extension_bindings.h"
#include "chrome/renderer/media/flash_embed_rewrite.h"
#include "chrome/renderer/media/webrtc_logging_agent_impl.h"
#include "chrome/renderer/net/net_error_helper.h"
#include "chrome/renderer/net_benchmarking_extension.h"
#include "chrome/renderer/plugins/non_loadable_plugin_placeholder.h"
#include "chrome/renderer/plugins/pdf_plugin_placeholder.h"
#include "chrome/renderer/supervised_user/supervised_user_error_page_controller_delegate_impl.h"
#include "chrome/renderer/trusted_vault_encryption_keys_extension.h"
#include "chrome/renderer/url_loader_throttle_provider_impl.h"
#include "chrome/renderer/v8_unwinder.h"
#include "chrome/renderer/web_link_preview_triggerer_impl.h"
#include "chrome/renderer/websocket_handshake_throttle_provider_impl.h"
#include "chrome/renderer/worker_content_settings_client.h"
#include "chrome/services/speech/buildflags/buildflags.h"
#include "components/autofill/content/renderer/autofill_agent.h"
#include "components/autofill/content/renderer/password_autofill_agent.h"
#include "components/autofill/content/renderer/password_generation_agent.h"
#include "components/autofill/core/common/autofill_features.h"
#include "components/commerce/content/renderer/commerce_web_extractor.h"
#include "components/content_capture/common/content_capture_features.h"
#include "components/content_capture/renderer/content_capture_sender.h"
#include "components/content_settings/core/common/content_settings_pattern.h"
#include "components/continuous_search/renderer/search_result_extractor_impl.h"
#include "components/country_codes/country_codes.h"
#include "components/dom_distiller/content/renderer/distillability_agent.h"
#include "components/dom_distiller/content/renderer/distiller_js_render_frame_observer.h"
#include "components/dom_distiller/core/dom_distiller_features.h"
#include "components/dom_distiller/core/dom_distiller_switches.h"
#include "components/dom_distiller/core/url_constants.h"
#include "components/error_page/common/error.h"
#include "components/error_page/common/localized_error.h"
#include "components/feed/feed_feature_list.h"
#include "components/fingerprinting_protection_filter/common/fingerprinting_protection_filter_features.h"
#include "components/fingerprinting_protection_filter/renderer/renderer_agent.h"
#include "components/fingerprinting_protection_filter/renderer/unverified_ruleset_dealer.h"
#include "components/grit/components_scaled_resources.h"
#include "components/guest_view/buildflags/buildflags.h"
#include "components/heap_profiling/in_process/heap_profiler_controller.h"
#include "components/history_clusters/core/config.h"
#include "components/metrics/call_stacks/call_stack_profile_builder.h"
#include "components/network_hints/renderer/web_prescient_networking_impl.h"
#include "components/no_state_prefetch/renderer/no_state_prefetch_client.h"
#include "components/no_state_prefetch/renderer/no_state_prefetch_helper.h"
#include "components/no_state_prefetch/renderer/no_state_prefetch_render_frame_observer.h"
#include "components/no_state_prefetch/renderer/no_state_prefetch_utils.h"
#include "components/optimization_guide/core/optimization_guide_features.h"
#include "components/page_content_annotations/core/page_content_annotations_features.h"
#include "components/page_load_metrics/renderer/metrics_render_frame_observer.h"
#include "components/paint_preview/buildflags/buildflags.h"
#include "components/password_manager/core/common/password_manager_features.h"
#include "components/pdf/common/constants.h"
#include "components/pdf/common/pdf_util.h"
#include "components/permissions/features.h"
#include "components/safe_browsing/buildflags.h"
#include "components/safe_browsing/content/renderer/threat_dom_details.h"
#include "components/sampling_profiler/process_type.h"
#include "components/sampling_profiler/thread_profiler.h"
#include "components/security_interstitials/content/renderer/security_interstitial_page_controller_delegate_impl.h"
#include "components/spellcheck/spellcheck_buildflags.h"
#include "components/subresource_filter/content/renderer/subresource_filter_agent.h"
#include "components/subresource_filter/content/renderer/unverified_ruleset_dealer.h"
#include "components/subresource_filter/content/renderer/user_subresource_filter_agent.h"
#include "components/subresource_filter/content/renderer/user_unverified_ruleset_dealer.h"
#include "components/subresource_filter/core/common/common_features.h"
#include "components/variations/net/variations_http_headers.h"
#include "components/variations/variations_switches.h"
#include "components/version_info/version_info.h"
#include "components/visitedlink/renderer/visitedlink_reader.h"
#include "components/web_cache/renderer/web_cache_impl.h"
#include "components/webapps/renderer/web_page_metadata_agent.h"
#include "content/public/common/content_constants.h"
#include "content/public/common/content_features.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/page_visibility_state.h"
#include "content/public/common/url_constants.h"
#include "content/public/common/url_utils.h"
#include "content/public/common/webplugininfo.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/renderer/render_frame_visitor.h"
#include "extensions/buildflags/buildflags.h"
#include "extensions/renderer/extensions_renderer_api_provider.h"
#include "ipc/ipc_sync_channel.h"
#include "media/base/media_switches.h"
#include "media/media_buildflags.h"
#include "mojo/public/cpp/bindings/generic_pending_receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "net/base/net_errors.h"
#include "pdf/buildflags.h"
#include "ppapi/buildflags/buildflags.h"
#include "printing/buildflags/buildflags.h"
#include "services/network/public/cpp/is_potentially_trustworthy.h"
#include "services/service_manager/public/cpp/interface_provider.h"
#include "services/tracing/public/cpp/stack_sampling/tracing_sampler_profiler.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_provider.h"
#include "third_party/blink/public/common/features.h"
#include "third_party/blink/public/common/features_generated.h"
#include "third_party/blink/public/common/tokens/tokens.h"
#include "third_party/blink/public/mojom/fetch/fetch_api_request.mojom-shared.h"
#include "third_party/blink/public/mojom/page/page_visibility_state.mojom.h"
#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/public/platform/scheduler/web_renderer_process_type.h"
#include "third_party/blink/public/platform/url_conversion.h"
#include "third_party/blink/public/platform/web_cache.h"
#include "third_party/blink/public/platform/web_content_security_policy_struct.h"
#include "third_party/blink/public/platform/web_runtime_features.h"
#include "third_party/blink/public/platform/web_security_origin.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/platform/web_url_error.h"
#include "third_party/blink/public/platform/web_url_request.h"
#include "third_party/blink/public/platform/web_url_response.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_element.h"
#include "third_party/blink/public/web/web_heap.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_origin_trials.h"
#include "third_party/blink/public/web/web_plugin.h"
#include "third_party/blink/public/web/web_plugin_container.h"
#include "third_party/blink/public/web/web_plugin_params.h"
#include "third_party/blink/public/web/web_script_controller.h"
#include "third_party/blink/public/web/web_security_policy.h"
#include "third_party/blink/public/web/web_view.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/webui/jstemplate_builder.h"
#include "url/origin.h"
#include "v8/include/v8-isolate.h"

#if BUILDFLAG(IS_ANDROID)
#include "chrome/renderer/sandbox_status_extension_android.h"
#include "chrome/renderer/wallet/boarding_pass_extractor.h"
#include "components/facilitated_payments/content/renderer/facilitated_payments_agent.h"
#include "components/facilitated_payments/core/features/features.h"
#include "components/feed/content/renderer/rss_link_reader.h"
#include "components/feed/feed_feature_list.h"
#else
#include "chrome/renderer/searchbox/searchbox.h"
#include "chrome/renderer/searchbox/searchbox_extension.h"
#include "components/search/ntp_features.h"  // nogncheck
#endif

#if BUILDFLAG(ENABLE_SPEECH_SERVICE)
#include "chrome/renderer/media/chrome_speech_recognition_client.h"
#endif  // BUILDFLAG(ENABLE_SPEECH_SERVICE)

#if BUILDFLAG(IS_WIN)
#include "chrome/renderer/render_frame_font_family_accessor.h"
#endif

#if BUILDFLAG(ENABLE_NACL)
#include "components/nacl/common/nacl_constants.h"
#include "components/nacl/renderer/nacl_helper.h"
#endif

#if BUILDFLAG(ENABLE_EXTENSIONS_CORE)
#include "chrome/common/initialize_extensions_client.h"
#include "chrome/renderer/extensions/chrome_extensions_renderer_client.h"
#include "extensions/common/constants.h"
#include "extensions/common/context_data.h"
#include "extensions/common/extension_urls.h"
#include "extensions/common/manifest_handlers/csp_info.h"
#include "extensions/common/manifest_handlers/web_accessible_resources_info.h"
#include "extensions/common/switches.h"
#include "extensions/renderer/api/core_extensions_renderer_api_provider.h"
#include "extensions/renderer/dispatcher.h"
#include "extensions/renderer/renderer_extension_registry.h"
#include "third_party/blink/public/mojom/css/preferred_color_scheme.mojom.h"
#include "third_party/blink/public/web/web_settings.h"
#endif  // BUIDFLAG(ENABLE_EXTENSIONS_CORE)

#if BUILDFLAG(ENABLE_EXTENSIONS)
#include "chrome/renderer/extensions/api/chrome_extensions_renderer_api_provider.h"
#endif  // BUILDFLAG(ENABLE_EXTENSIONS)

#if BUILDFLAG(ENABLE_GUEST_VIEW)
#include "extensions/renderer/guest_view/mime_handler_view/mime_handler_view_container_manager.h"
#endif  // BUILDFLAG(ENABLE_GUEST_VIEW)

#if BUILDFLAG(ENABLE_PDF)
#include "chrome/renderer/pdf/chrome_pdf_internal_plugin_delegate.h"
#include "components/pdf/renderer/internal_plugin_renderer_helpers.h"
#endif  // BUILDFLAG(ENABLE_PDF)

#if BUILDFLAG(ENABLE_PLUGINS)
#include "chrome/renderer/plugins/chrome_plugin_placeholder.h"
#endif  // BUILDFLAG(ENABLE_PLUGINS)

#if BUILDFLAG(ENABLE_PPAPI)
#include "chrome/renderer/pepper/pepper_helper.h"
#include "ppapi/shared_impl/ppapi_switches.h"  // nogncheck crbug.com/1125897
#endif

#if BUILDFLAG(ENABLE_PRINTING)
#include "chrome/renderer/printing/chrome_print_render_frame_helper_delegate.h"
#include "components/printing/renderer/print_render_frame_helper.h"  // nogncheck
#include "printing/metafile_agent.h"  // nogncheck
#endif

#if BUILDFLAG(ARKWEB_PRINT)
#include "cef/ohos_cef_ext/libcef/renderer/extensions/ohos_print_render_frame_helper_delegate.h"
#endif  // BUIDFLAG(ARKWEB_PRINT)

#if BUILDFLAG(ENABLE_PAINT_PREVIEW)
#include "components/paint_preview/renderer/paint_preview_recorder_impl.h"  // nogncheck
#endif

#if BUILDFLAG(ENABLE_SPELLCHECK)
#include "components/spellcheck/renderer/spellcheck.h"
#include "components/spellcheck/renderer/spellcheck_provider.h"

#if BUILDFLAG(HAS_SPELLCHECK_PANEL)
#include "components/spellcheck/renderer/spellcheck_panel.h"
#endif  // BUILDFLAG(HAS_SPELLCHECK_PANEL)
#endif  // BUILDFLAG(ENABLE_SPELLCHECK)

#if BUILDFLAG(ENABLE_LIBRARY_CDMS) || BUILDFLAG(IS_WIN) || \
    BUILDFLAG(IS_ANDROID) || BUILDFLAG(ARKWEB_ENABLE_CDM)
#include "chrome/renderer/media/chrome_key_systems.h"
#endif

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
#include "components/js_injection/renderer/js_communication.h"
#endif

ArkWebChromeContentRendererClientExt::ArkWebChromeContentRendererClientExt()
    : ChromeContentRendererClient() {}

ArkWebChromeContentRendererClientExt::~ArkWebChromeContentRendererClientExt() =
    default;

#if BUILDFLAG(ARKWEB_NETWORK_BASE) && BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
void ArkWebChromeContentRendererClientExt::RenderFrameCreatedContentSettings(
    content_settings::ContentSettingsAgentImpl* content_settings,
    ChromeContentRendererClient* chrome_content_renderer_client) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebEx) &&
      chrome_content_renderer_client->chrome_observer_.get() &&
      content_settings) {
    content_settings->AsArkWebContentSettingsAgentImplExt()
        ->SetContentSettingRules(
            chrome_content_renderer_client->chrome_observer_
                ->content_setting_rules());
  }
}
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
void ArkWebChromeContentRendererClientExt::
    RenderFrameCreateSubresourceFilterAgent(
        content::RenderFrame* render_frame,
        raw_ptr<ChromeContentRendererClient> obj) {
  if (obj->subresource_filter_user_ruleset_dealer_) {
    auto* user_subresource_filter_agent =
        new subresource_filter::UserSubresourceFilterAgent(
            render_frame, obj->subresource_filter_user_ruleset_dealer_.get());
    user_subresource_filter_agent->Initialize();
  }
}

void ArkWebChromeContentRendererClientExt::
    RenderFrameCreateSubresourceFilterAgentTriggerHide(
        content::RenderFrame* render_frame) {
  auto routing_id = render_frame->GetRoutingID();
  render_frame->GetTaskRunner(blink::TaskType::kDOMManipulation)
      ->PostTask(FROM_HERE,
                 base::BindOnce(&ArkWebChromeContentRendererClientExt::
                                    TriggerElementHidingInFrame,
                                base::Unretained(this), routing_id));
  render_frame->GetTaskRunner(blink::TaskType::kDOMManipulation)
      ->PostTask(FROM_HERE,
                 base::BindOnce(&ArkWebChromeContentRendererClientExt::
                                    TriggerUserElementHidingInFrame,
                                base::Unretained(this), routing_id));
}

bool ArkWebChromeContentRendererClientExt::GetAdBlockEnabledByFrame(
    content::RenderFrame* render_frame) {
  if (!render_frame) {
    return false;
  }
  // check global switch
  if (!render_frame->GetGlobalAdblockEnabled()) {
    return false;
  }
  if (render_frame->IsMainFrame()) {
    blink::WebLocalFrame* main_web_frame = render_frame->GetWebFrame();
    if (main_web_frame) {
      return main_web_frame->GetAdBlockEnabled();
    }
  }

  // Subframe should get adblock switch from MainFrame
  auto* main_render_frame = render_frame->GetMainRenderFrame();
  if (main_render_frame) {
    blink::WebLocalFrame* main_web_frame = main_render_frame->GetWebFrame();
    if (!main_web_frame) {
      return false;
    }
    return main_web_frame->GetAdBlockEnabled();
  }
  return false;
}

void ArkWebChromeContentRendererClientExt::TriggerElementHidingInFrame(
    int routing_id) {
  // |render_frame| might be dead by now.
  auto* render_frame = content::RenderFrame::FromRoutingID(routing_id);
  if (!render_frame) {
    LOG(ERROR) << "[AdBlock] TriggerElementHidingInFrame render_frame null";
#ifdef OHOS_LOGGER_REPORT
    LOG_FEEDBACK(ERROR)
        << "[AdBlock] TriggerElementHidingInFrame render_frame null";
#endif
    return;
  }

  blink::WebLocalFrame* web_frame = render_frame->GetWebFrame();
  if (!web_frame) {
    LOG(ERROR) << "[AdBlock] TriggerElementHidingInFrame web_frame null";
#ifdef OHOS_LOGGER_REPORT
    LOG_FEEDBACK(ERROR)
        << "[AdBlock] TriggerElementHidingInFrame web_frame null";
#endif
    return;
  }

  blink::WebDocumentSubresourceFilter* filter =
      web_frame->GetDocumentLoader()->GetWebSubresourceFilter();
  if (!filter) {
    LOG(DEBUG) << "[AdBlock] TriggerElementHidingInFrame filter null";
    return;
  }

  if (!GetAdBlockEnabledByFrame(render_frame)) {
    LOG(DEBUG) << "[AdBlock] AdBlock disabled";
    return;
  }

  blink::WebDocument document = web_frame->GetDocument();
  if (!document.Url().ProtocolIs("https") &&
      !document.Url().ProtocolIs("http")) {
    LOG(ERROR) << "[AdBlock] TriggerElementHidingInFrame scheme error";
#ifdef OHOS_LOGGER_REPORT
    LOG_FEEDBACK(ERROR) << "[AdBlock] TriggerElementHidingInFrame scheme error";
#endif
    return;
  }
  if (web_frame->GetHasDocumentTypeOption()) {
    LOG(WARNING) << "[AdBlock] Match $document for "
                 << "***";
#ifdef OHOS_LOGGER_REPORT
    LOG_FEEDBACK(WARNING) << "[AdBlock] Match $document for "
                          << url::LogUtils::ConvertUrlWithMask(
                                 document.Url().GetString().Utf8());
#endif
    return;
  }
  if (web_frame->GetHasElemHideTypeOption()) {
    LOG(WARNING) << "[AdBlock] Match selemhide for "
                 << "***";
#ifdef OHOS_LOGGER_REPORT
    LOG_FEEDBACK(WARNING) << "[AdBlock] Match $elemhide for "
                          << url::LogUtils::ConvertUrlWithMask(
                                 document.Url().GetString().Utf8());
#endif
    return;
  }
  bool has_generichide = web_frame->GetHasGenericHideTypeOption();
  if (has_generichide) {
    LOG(WARNING) << "[AdBlock] Match sgenerichide for "
                 << "***";
#ifdef OHOS_LOGGER_REPORT
    LOG_FEEDBACK(WARNING) << "[AdBlock] Match $generichide for "
                          << url::LogUtils::ConvertUrlWithMask(
                                 document.Url().GetString().Utf8());
#endif
  }

  base::TimeTicks start = base::TimeTicks::Now();
  std::unique_ptr<std::string> selectors;
  selectors =
      filter->GetElementHidingSelectors(document.Url(), !has_generichide);

  if (!selectors->empty()) {
    document.InsertStyleSheet(blink::WebString::FromUTF8(*selectors), nullptr,
                              blink::WebCssOrigin::kAuthor,
                              blink::BackForwardCacheAware::kAllow,
                              blink::WebDocument::StyleSheetType::kAdBlock);
    base::TimeDelta duration = base::TimeTicks::Now() - start;
    LOG(WARNING) << "[AdBlock] Element hiding for "
                 << "***"
                 << "assumming " << duration.InMicroseconds() << "microseconds";
#ifdef OHOS_LOGGER_REPORT
    LOG_FEEDBACK(WARNING) << "[AdBlock] Element hiding for "
                          << url::LogUtils::ConvertUrlWithMask(
                                 document.Url().GetString().Utf8())
                          << "assumming " << duration.InMicroseconds()
                          << "microseconds";
#endif
    return;
  }

  selectors.reset();
}

void ArkWebChromeContentRendererClientExt::TriggerUserElementHidingInFrame(
    int routing_id) {
  // |render_frame| might be dead by now.
  auto* render_frame = content::RenderFrame::FromRoutingID(routing_id);
  if (!render_frame) {
    LOG(ERROR) << "[AdBlock] TriggerUserElementHidingInFrame render_frame null";

#ifdef OHOS_LOGGER_REPORT
    LOG_FEEDBACK(ERROR)
        << "[AdBlock] TriggerUserElementHidingInFrame render_frame null";
#endif

    return;
  }

  blink::WebLocalFrame* web_frame = render_frame->GetWebFrame();
  if (!web_frame) {
    return;
  }

  blink::WebDocument document = web_frame->GetDocument();
  if (!document.Url().ProtocolIs("https") &&
      !document.Url().ProtocolIs("http")) {
    return;
  }
  blink::WebDocumentSubresourceFilter* filter =
      web_frame->GetDocumentLoader()->GetWebUserSubresourceFilter();
  if (!filter) {
    return;
  }
  base::TimeTicks start = base::TimeTicks::Now();
  std::unique_ptr<std::string> selectors;
  selectors = filter->GetElementHidingSelectors(document.Url(), true);
  if (!selectors->empty()) {
    document.InsertStyleSheet(blink::WebString::FromUTF8(*selectors), nullptr,
                              blink::WebCssOrigin::kAuthor,
                              blink::BackForwardCacheAware::kAllow,
                              blink::WebDocument::StyleSheetType::kUserAdBlock);
    base::TimeDelta duration = base::TimeTicks::Now() - start;
    LOG(WARNING) << "[User AdBlock] Element hiding for "
                 << "***"
                 << " assumming " << duration.InMicroseconds()
                 << " microseconds";

#ifdef OHOS_LOGGER_REPORT
    LOG_FEEDBACK(WARNING) << "[User AdBlock] Element hiding for "
                          << url::LogUtils::ConvertUrlWithMask(
                                 document.Url().GetString().Utf8())
                          << " assumming " << duration.InMicroseconds()
                          << " microseconds";
#endif

    return;
  }
  selectors.reset();
}
#endif

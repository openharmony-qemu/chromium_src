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

namespace {

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void ApplyOhosMediaPlayerEnabled(const web_pref::WebPreferences& prefs,
                                 WebView* web_view,
                                 WebSettings* settings) {
  bool exist_enabled = settings->GetCustomMediaPlayerEnabled();
  if (exist_enabled == prefs.custom_media_player_enabled) {
    return;
  }
  LOG(INFO) << "Update custom media player enable, exist_enabled: "
            << exist_enabled << ", prefs.custom_media_player_enabled:"
            << prefs.custom_media_player_enabled;
  settings->SetCustomMediaPlayerEnabled(prefs.custom_media_player_enabled);
  const auto* main_frame = web_view->MainFrame();
  if (!main_frame) {
    return;
  }
  auto* local_main_frame = DynamicTo<WebLocalFrameImpl>(main_frame);
  if (!local_main_frame) {
    return;
  }
  Document* document = local_main_frame->GetDocument();
  if (!document) {
    return;
  }
  document->GetStyleEngine().EnsureUAStyleForMediaElement();
}
#endif
}  // namespace

// LCOV_EXCL_START
void ApplyOhosWebPreferences(const web_pref::WebPreferences& prefs,
                             WebView* web_view,
                             WebSettings* settings,
                             WebViewImpl* web_view_impl) {
#if BUILDFLAG(ARKWEB_AI)
  settings->SetImageAnalyzerEnabled(prefs.image_analyzer_enabled);
  settings->SetArkwebAgentEnabled(prefs.arkweb_agent_enabled);
  settings->SetAgentNeedHighlight(prefs.agent_need_highlight);
#endif  // BUILDFLAG(ARKWEB_AI)
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  settings->SetVerticalHideScrollbars(prefs.hide_vertical_scrollbars);
  settings->SetHorizontalHideScrollbars(prefs.hide_horizontal_scrollbars);
  settings->SetScrollable(prefs.scroll_enabled);
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  settings->SetDrawMode(prefs.draw_mode);
#endif

#if BUILDFLAG(ARKWEB_CSS_FONT)
  settings->SetFontWeightScale(prefs.font_weight_scale);
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  settings->SetNativeEmbedModeEnabled(prefs.native_embed_mode_enabled);
  settings->SetIntrinsicSizeEnabled(prefs.intrinsic_size_enabled);
  settings->SetCssDisplayChangeEnabled(prefs.css_display_change_enabled);
  settings->RegisterNativeEmbedRule(
      WebString::FromASCII(base::ToLowerASCII(prefs.embed_tag)),
      WebString::FromASCII(base::ToLowerASCII(prefs.embed_tag_type)));
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  settings->SetContextMenuCustomization(
      prefs.contextmenu_customization_enabled);
#endif

#if BUILDFLAG(ARKWEB_SCROLLBAR)
  settings->SetScrollBarColor(prefs.scrollbar_color);
#endif  // ARKWEB_SCROLLBAR

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  settings->SetBorderRadiusFromWeb(
      prefs.border_radius_top_left, prefs.border_radius_top_right,
      prefs.border_radius_bottom_left, prefs.border_radius_bottom_right);
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  settings->SetEnableAutoFill(prefs.is_autofill_enabled);
#endif  // BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)

#if BUILDFLAG(ARKWEB_MENU)
  settings->SetTouchHandleExistState(prefs.touch_handle_exist);
  settings->SetViewportScaleState(prefs.viewport_scale);
#endif  // BUILDFLAG(ARKWEB_MENU)

#if BUILDFLAG(ARKWEB_COPY_OPTION)
  settings->SetCopyOption(prefs.copy_option);
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)

#if BUILDFLAG(ARKWEB_FOCUS)
  settings->SetGestureFocusMode(prefs.gesture_focus_mode);
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  settings->SetCustomVideoPlayerEnabled(prefs.custom_video_player_enable);
  settings->SetCustomVideoPlayerOverlay(prefs.custom_video_player_overlay);
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  settings->SetVideoAssistantEnabled(prefs.video_assistant_enabled);
  ApplyOhosMediaPlayerEnabled(prefs, web_view, settings);
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_BFCACHE)
  settings->SetMediaResumeFromBFCachePage(prefs.media_resume_from_bfcache_page);
#endif  // BUILDFLAG(ARKWEB_BFCACHE)

#if BUILDFLAG(ARKWEB_MEDIA)
  settings->SetPreferHiddenVolumeControls(!base::ohos::IsPcDevice());
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  settings->SetUsageScenario(prefs.usage_scenario);
#endif

#if BUILDFLAG(ARKWEB_ZOOM)
  settings->SetTextZoomFactor(prefs.text_zoom_factor);
  const auto* main_frame = web_view->MainFrame();
  if (main_frame) {
    auto* local_main_frame = DynamicTo<WebLocalFrameImpl>(main_frame);
    if (local_main_frame && local_main_frame->FrameWidget()) {
      local_main_frame->FrameWidget()->SetTextZoomFactor(
          prefs.text_zoom_factor);
    }
    for (const auto* frame = main_frame->FirstChild(); frame;
         frame = frame->NextSibling()) {
      auto* web_local_frame = DynamicTo<WebLocalFrameImpl>(frame);
      if (web_local_frame && web_local_frame->FrameWidget()) {
        web_local_frame->FrameWidget()->SetTextZoomFactor(
            prefs.text_zoom_factor);
      }
    }
  }
#endif
#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
  web_view_impl->SetDelayDurationForBackgroundTabFreezing(
      prefs.delay_for_background_tab_freezing);
#endif

#if BUILDFLAG(ARKWEB_SCROLLBAR)
  settings->SetScrollBarColor(prefs.scrollbar_color);
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  settings->SetClipboardSitePermissionEnabled(prefs.clipboard_site_permission_enabled);
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
  settings->SetCastEnabled(prefs.cast_enabled);
#endif  // BUILDFLAG(ARKWEB_MEDIA_CAST)
}

void WebView::ApplyWebPreferencesForInclude(
    const web_pref::WebPreferences& prefs,
    WebView* web_view) {
  WebViewImpl* web_view_impl = To<WebViewImpl>(web_view);
  WebSettings* settings = web_view->GetSettings();
#if BUILDFLAG(ARKWEB_FIT_CONTENT)
  settings->SetForceZeroLayoutHeight(prefs.force_zero_layout_height);
  LOG(DEBUG) << "WebViewImpl::UpdateMainFrameLayoutSize,forceZeroLayoutHeight:"
             << prefs.force_zero_layout_height;
#endif

#if BUILDFLAG(IS_ARKWEB)
  settings->SetForceZeroLayoutHeight(prefs.force_zero_layout_height);
  LOG(DEBUG) << "WebViewImpl::UpdateMainFrameLayoutSize,forceZeroLayoutHeight:"
             << prefs.force_zero_layout_height;
  RuntimeEnabledFeatures::SetMediaCaptureEnabled(true);
  RuntimeEnabledFeatures::SetEyeDropperAPIEnabled(
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetSystemPropertiesInstance()
          .GetBoolParameter("web.eyedropper.enabled", false));
  if (!base::ohos::IsPcDevice()) {
    settings->SetAllowCustomScrollbarInMainFrame(false);
    settings->SetAccessibilityFontScaleFactor(prefs.font_scale_factor);
    settings->SetAccessibilityFontWeightAdjustment(
        prefs.font_weight_adjustment);
    settings->SetAccessibilityTextSizeContrastFactor(
        prefs.text_size_contrast_factor);
    settings->SetDeviceScaleAdjustment(prefs.device_scale_adjustment);
    web_view_impl->SetIgnoreViewportTagScaleLimits(prefs.force_enable_zoom);
    settings->SetDefaultVideoPosterURL(
        WebString::FromASCII(prefs.default_video_poster_url.spec()));
    settings->SetSupportDeprecatedTargetDensityDPI(
        prefs.support_deprecated_target_density_dpi);
    settings->SetWideViewportQuirkEnabled(prefs.wide_viewport_quirk);
    settings->SetUseWideViewport(prefs.use_wide_viewport);
    settings->SetViewportMetaMergeContentQuirk(
        prefs.viewport_meta_merge_content_quirk);
    settings->SetViewportMetaNonUserScalableQuirk(
        prefs.viewport_meta_non_user_scalable_quirk);
    settings->SetViewportMetaZeroValuesQuirk(
        prefs.viewport_meta_zero_values_quirk);
    settings->SetClobberUserAgentInitialScaleQuirk(
        prefs.clobber_user_agent_initial_scale_quirk);
    settings->SetIgnoreMainFrameOverflowHiddenQuirk(
        prefs.ignore_main_frame_overflow_hidden_quirk);
    settings->SetReportScreenSizeInPhysicalPixelsQuirk(
        prefs.report_screen_size_in_physical_pixels_quirk);
    settings->SetShouldReuseGlobalForUnownedMainFrame(
        prefs.reuse_global_for_unowned_main_frame);
    settings->SetPreferHiddenVolumeControls(true);
    settings->SetSpellCheckEnabledByDefault(
        prefs.spellcheck_enabled_by_default);

    RuntimeEnabledFeatures::SetVideoFullscreenOrientationLockEnabled(
        prefs.video_fullscreen_orientation_lock_enabled);
    RuntimeEnabledFeatures::SetVideoRotateToFullscreenEnabled(
        prefs.video_rotate_to_fullscreen_enabled);
    settings->SetEmbeddedMediaExperienceEnabled(
        prefs.embedded_media_experience_enabled);
    settings->SetImmersiveModeEnabled(prefs.immersive_mode_enabled);
    settings->SetDoNotUpdateSelectionOnMutatingSelectionRange(
        prefs.do_not_update_selection_on_mutating_selection_range);
    RuntimeEnabledFeatures::SetCSSHexAlphaColorEnabled(
        prefs.css_hex_alpha_color_enabled);
    RuntimeEnabledFeatures::SetScrollTopLeftInteropEnabled(
        prefs.scroll_top_left_interop_enabled);
    RuntimeEnabledFeatures::SetAcceleratedSmallCanvasesEnabled(
        !prefs.disable_accelerated_small_canvases);
#if BUILDFLAG(IS_ARKWEB)
    RuntimeEnabledFeatures::SetInputMultipleFieldsUIEnabled(false);
#endif
    RuntimeEnabledFeatures::SetLongPressLinkSelectTextEnabled(
        prefs.long_press_link_select_text);
  }
#endif  // BUILDFLAG(IS_ARKWEB)
}

#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
gfx::PointF WebViewImpl::GetScrollOffset() {
  DCHECK(MainFrameImpl());
  DCHECK(MainFrameImpl()->GetFrameView());
  LocalFrameView* view = MainFrameImpl()->GetFrameView();
  DCHECK(view->GetScrollableArea());
  ScrollOffset offset = view->GetScrollableArea()->GetScrollOffset();
  return view->GetScrollableArea()->ScrollOffsetToPosition(offset);
}

float WebViewImpl::GetScrollBottom() {
  ScrollableArea* root_viewport =
      MainFrameImpl()->GetFrame()->View()->GetScrollableArea();
  if (!root_viewport) {
    return -1.0;
  }
  return root_viewport->MaximumScrollOffset().y();
}
#endif  // #if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void WebViewImpl::SetScrollOffset(const gfx::PointF point) {
  DCHECK(MainFrameImpl());
  DCHECK(MainFrameImpl()->GetFrameView());
  LocalFrameView* view = MainFrameImpl()->GetFrameView();
  DCHECK(view->GetScrollableArea());
  view->GetScrollableArea()->SetScrollOffset(
      gfx::Vector2dF(point.OffsetFromOrigin()),
      mojom::blink::ScrollType::kProgrammatic);
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

void WebViewImpl::EnterFullscreen(
    LocalFrame& frame,
    const FullscreenOptions* options,
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
    bool overlay_fullscreen,
#endif  // ARKWEB_VIDEO_ASSISTANT
    FullscreenRequestType request_type
#if BUILDFLAG(ARKWEB_FULLSCREEN)
    ,
    const absl::optional<gfx::Size>& video_natural_size
#endif  // BUILDFLAG(ARKWEB_FULLSCREEN)
) {
#if BUILDFLAG(ARKWEB_FULLSCREEN)
  LOG(INFO) << "WebViewImpl EnterFullscreen";
#endif // BUILDFLAG(ARKWEB_FULLSCREEN)
  fullscreen_controller_->EnterFullscreen(frame, options,
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
                                          overlay_fullscreen,
#endif  // ARKWEB_VIDEO_ASSISTANT
                                          request_type
#if BUILDFLAG(ARKWEB_FULLSCREEN)
                                          ,
                                          video_natural_size
#endif  // BUILDFLAG(ARKWEB_FULLSCREEN)
  );
}
// LCOV_EXCL_STOP

void UpdateStyleAndLayoutTreeForInclude(Page* page) {
  if (page) {
    auto* main_frame = DynamicTo<LocalFrame>(page->MainFrame());
    if (main_frame && main_frame->GetDocument()) {
      main_frame->GetDocument()->UpdateStyleAndLayoutTree();
    }
  }
}

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_PINCH_SMOOTH)
void WebViewImpl::SetPinchSmoothMode(bool isEnable) {
  if (!MainFrame() || !GetPage() || !GetPage()->MainFrame() ||
      !GetPage()->MainFrame()->IsLocalFrame() ||
      !GetPage()->DeprecatedLocalMainFrame()->View()) {
    return;
  }

  if (pinch_smooth_mode == isEnable) {
    return;
  }
  pinch_smooth_mode = isEnable;
  if (does_composite_) {
    MainFrameImpl()
        ->FrameWidgetImpl()
        ->AsWebFrameWidgetImplExt()
        ->SetPinchSmoothMode(isEnable);
  }
}
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
void WebViewImpl::OnSetAdBlockEnable(bool site_adblock_enabled) {
  Page* page = GetPage();
  if (!page) {
    return;
  }
  LocalFrame* frame = DynamicTo<LocalFrame>(page->MainFrame());
  if (!frame) {
    return;
  }
  LocalFrame& root_frame = frame->LocalFrameRoot();
  root_frame.SetAdBlockEnableForSite(site_adblock_enabled);
}

bool WebViewImpl::GetAdBlockEnableForSite() {
  Page* page = GetPage();
  if (!page) {
    return false;
  }
  LocalFrame* frame = DynamicTo<LocalFrame>(page->MainFrame());
  if (!frame) {
    return false;
  }
  LocalFrame& root_frame = frame->LocalFrameRoot();
  return root_frame.GetAdBlockEnableForSite();
}
#endif

#if BUILDFLAG(ARKWEB_FLING)
void WebViewImpl::UpdateFlingVelocityLimit(const gfx::Vector2dF& velocity) {
  if (!GetPage() || !(GetPage()->MainFrame())) {
    LOG(DEBUG) << "[flingTracker] May not use a FlingTracker object "
                    "associated with a Page that is not fully active.";
    return;
  }

  if (velocity == limit_fling_velocity_) {
    LOG(DEBUG) << "Store value hasn't change, no need to limit Velocity";
    return;
  }

  limit_fling_velocity_ = velocity;

  if (GetPage()->MainFrame()->IsLocalFrame()) {
    DCHECK(local_main_frame_host_remote_);
    if (local_main_frame_host_remote_) {
      local_main_frame_host_remote_->UpdateFlingVelocityLimit(velocity);
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
void WebViewImpl::SetDelayDurationForBackgroundTabFreezing(
    int64_t millisecond) {
  if (millisecond < 0) {
    return;
  }

  if (GetPage() && Scheduler()) {
    Scheduler()->SetDelayDurationForBackgroundTabFreezing(millisecond);
  }
}
#endif
// LCOV_EXCL_STOP
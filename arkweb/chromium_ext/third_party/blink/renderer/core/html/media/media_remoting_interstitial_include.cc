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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_MEDIA_MEDIA_REMOTING_INTERSTITIAL_H_
#error "must be in include form THIRD_PARTY_BLINK_RENDERER_CORE_HTML_MEDIA_MEDIA_REMOTING_INTERSTITIAL_H_"
#endif

namespace blink {

void MediaRemotingInterstitial::AddMediaCastBackGround() {
  LOG(INFO) << "AddMediaCastBackGround, enter";

  auto* button_container = MakeGarbageCollected<HTMLDivElement>(GetDocument());
  button_container->setAttribute(html_names::kClassAttr, 
                                AtomicString("internal-media-remoting-container"));

  // Create Left Button
  auto* left_button = MakeGarbageCollected<HTMLDivElement>(GetDocument());
  left_button->setAttribute(html_names::kClassAttr, 
                          AtomicString("internal-media-remoting-button"));
  left_button->setInnerText(String::FromUTF8("\u7ed3\u675f\u6295\u5c4f")); // "结束投屏"

  // Bind a click event to the left button
  left_button->addEventListener(
      event_type_names::kClick,
      MakeGarbageCollected<RemotingButtonEventListener>(weak_factory_.GetWeakPtr(), RemotingButtonType::kStopCasting),
      false
  );

  // Create Right Button
  auto* right_button = MakeGarbageCollected<HTMLDivElement>(GetDocument());
  right_button->setAttribute(html_names::kClassAttr, 
                            AtomicString("internal-media-remoting-button"));
  right_button->setInnerText(String::FromUTF8("\u5207\u6362\u8bbe\u5907")); // "Switch Device"

  // Bind a click event to the right button
  right_button->addEventListener(
      event_type_names::kClick,
      MakeGarbageCollected<RemotingButtonEventListener>(weak_factory_.GetWeakPtr(), RemotingButtonType::kSwitchDevice),
      false
  );

  // Add to container
  button_container->AppendChild(left_button);
  button_container->AppendChild(right_button);

  // Add the container to the parent element
  AppendChild(button_container);

  ParseMediaCastControl();
}

void MediaRemotingInterstitial::ParseMediaCastControl() {
  Document& document = GetDocument();
  
  // Creating a Control Container
  auto* controls_container = MakeGarbageCollected<HTMLDivElement>(document);
  controls_container->setAttribute(
      html_names::kClassAttr, AtomicString("video-controls-container"));
  
  // Left button group: Play/Pause
  auto* left_group = MakeGarbageCollected<HTMLDivElement>(document);
  left_group->setAttribute(
      html_names::kClassAttr, AtomicString("video-controls-left"));
  
  // Create Play/Pause Button
  play_pause_button_ = MakeGarbageCollected<HTMLDivElement>(document);
  play_pause_button_->setAttribute(
      html_names::kClassAttr, AtomicString("play-pause-button"));
  
  // Binding a Click Event
  // Bind a click event to the left button
  play_pause_button_->addEventListener(
      event_type_names::kClick,
      MakeGarbageCollected<RemotingButtonEventListener>(weak_factory_.GetWeakPtr(), RemotingButtonType::kPlayPause),
      false
  );
  
  // Add to left group
  left_group->AppendChild(play_pause_button_);
  
  // Middle Group: Progress Bar
  auto* progress_group = MakeGarbageCollected<HTMLDivElement>(document);
  progress_group->setAttribute(
      html_names::kClassAttr, 
      AtomicString("progress-container"));
  
  // Current time display
  current_time_display_ = MakeGarbageCollected<HTMLSpanElement>(document);
  current_time_display_->setAttribute(
      html_names::kClassAttr, 
      AtomicString("time-display"));
  current_time_display_->setInnerText("0:00");
  
  // Progress Bar Container
  progress_bar_ = MakeGarbageCollected<HTMLDivElement>(document);
  progress_bar_->setAttribute(
      html_names::kClassAttr, 
      AtomicString("progress-bar"));
  
  // Actual Progress
  progress_fill_ = MakeGarbageCollected<HTMLDivElement>(document);
  progress_fill_->setAttribute(
      html_names::kClassAttr, 
      AtomicString("progress-fill"));
  
  // Progress bar slider
  auto* progress_thumb = MakeGarbageCollected<HTMLDivElement>(document);
  progress_thumb->setAttribute(
      html_names::kClassAttr, 
      AtomicString("progress-thumb"));
  
  // Assembly Progress Bar
  progress_bar_->AppendChild(progress_fill_);
  progress_bar_->AppendChild(progress_thumb);

  // Using a dedicated ProgressBarEventListener
  auto* progress_event_listener = 
      MakeGarbageCollected<ProgressBarEventListener>(
          weak_factory_.GetWeakPtr(),
          progress_bar_);
  
  // Binding multiple event types
  progress_bar_->addEventListener(
      event_type_names::kClick,
      progress_event_listener,
      false);
  
  progress_bar_->addEventListener(
      event_type_names::kMousedown,
      progress_event_listener,
      false);
  
  progress_bar_->addEventListener(
      event_type_names::kMousemove,
      progress_event_listener,
      false);
  
  progress_bar_->addEventListener(
      event_type_names::kMouseup,
      progress_event_listener,
      false);
  
  progress_bar_->addEventListener(
      event_type_names::kMouseleave,
      progress_event_listener,
      false);

  // Binding a Touch Event
  progress_bar_->addEventListener(
      event_type_names::kTouchstart,
      progress_event_listener,
      false);
  
  progress_bar_->addEventListener(
      event_type_names::kTouchmove,
      progress_event_listener,
      false);
  
  progress_bar_->addEventListener(
      event_type_names::kTouchend,
      progress_event_listener,
      false);
  
  progress_bar_->addEventListener(
      event_type_names::kTouchcancel,
      progress_event_listener,
      false);
  
  // Total Duration Display
  duration_display_ = MakeGarbageCollected<HTMLSpanElement>(document);
  duration_display_->setAttribute(
      html_names::kClassAttr, 
      AtomicString("time-display"));
  duration_display_->setInnerText(FormatTime(duration_));
  
  // Assemble progress bar group
  progress_group->AppendChild(current_time_display_);
  progress_group->AppendChild(progress_bar_);
  progress_group->AppendChild(duration_display_);
  
  // Right button group: Full screen
  auto* right_group = MakeGarbageCollected<HTMLDivElement>(document);
  right_group->setAttribute(
      html_names::kClassAttr, 
      AtomicString("video-controls-right"));
  
  // Create a full-screen button
  fullscreen_button_ = MakeGarbageCollected<HTMLDivElement>(document);
  fullscreen_button_->setAttribute(
      html_names::kClassAttr, 
      AtomicString("fullscreen-button enter-fullscreen"));
  
  // Binding a Click Event
  fullscreen_button_->addEventListener(
      event_type_names::kClick,
      MakeGarbageCollected<RemotingButtonEventListener>(weak_factory_.GetWeakPtr(), RemotingButtonType::kFullscreenToggle),
      false
  );
  
  // Add to right group
  right_group->AppendChild(fullscreen_button_);
  
  // Assemble all controls
  controls_container->AppendChild(left_group);
  controls_container->AppendChild(progress_group);
  controls_container->AppendChild(right_group);
  
  // Add to current element
  AppendChild(controls_container);

  // Initial Update UI
  UpdateProgressUI();
}

void MediaRemotingInterstitial::OnPlayPauseClicked() {
  // Switching the Playback Status
  is_playing_ = !is_playing_;
  LOG(INFO) << "MediaRemotingInterstitial::OnPlayPauseClicked, is_playing_: " << is_playing_;
  if (!play_pause_button_) return;
  // This is the UI refresh area. Remote Play and Pause need to call back here to refresh the UI.
  UpdatePlayButtonUI();    
  UpdateProgressUI();
}

void MediaRemotingInterstitial::UpdatePlayButtonUI() {
  if (!play_pause_button_) return;
  // This is the UI refresh area. Remote Play and Pause need to call back here to refresh the UI.
  if (is_playing_) {
    LOG(INFO) << "MediaRemotingInterstitial::OnPlayPauseClicked, playing";
    play_pause_button_->setAttribute(
        html_names::kClassAttr,
        AtomicString("play-pause-button playing"));
  } else {
    LOG(INFO) << "MediaRemotingInterstitial::OnPlayPauseClicked, pause";
    play_pause_button_->setAttribute(
        html_names::kClassAttr,
        AtomicString("play-pause-button"));
  }
}

// Handle full-screen button click
void MediaRemotingInterstitial::OnFullscreenClicked() {
  LOG(INFO) << "MediaRemotingInterstitial::OnFullscreenClicked, "
            << "is_fullscreen_=" << is_fullscreen_;
  
  if (!is_fullscreen_) {
    EnterFullscreen();
  } else {
    ExitFullscreen();
  }
}

void MediaRemotingInterstitial::EnterFullscreen() {
  LOG(INFO) << "MediaRemotingInterstitial::EnterFullscreen, enter";
  Fullscreen::RequestFullscreen(GetVideoElement().GetMediaRemotingInterstitial());

    // Switch full-screen mode flag
  is_fullscreen_ = true;

  // Toggle the icon of the full screen button: changes to the icon of exiting the full screen.
  if (fullscreen_button_) {
    fullscreen_button_->setAttribute(
        html_names::kClassAttr,
        AtomicString("fullscreen-button exit-fullscreen"));
    
    LOG(INFO) << "MediaRemotingInterstitial::EnterFullscreen, "
              << "switched to exit-fullscreen icon";
  }
}

// Exit Full Screen
void MediaRemotingInterstitial::ExitFullscreen() {
  Fullscreen::ExitFullscreen(GetDocument());
  
  // Update status flag
  is_fullscreen_ = false;
  
  // Switch full-screen button icon: revert to the full-screen entry icon
  if (fullscreen_button_) {
    fullscreen_button_->setAttribute(
        html_names::kClassAttr,
        AtomicString("fullscreen-button enter-fullscreen"));
    
    LOG(INFO) << "MediaRemotingInterstitial::ExitFullscreen, "
              << "switched to enter-fullscreen icon";
  }
  
  LOG(INFO) << "MediaRemotingInterstitial::ExitFullscreen, exited";
}

void MediaRemotingInterstitial::UpdateProgressUI() {
  // Update Time Display
  if (current_time_display_) {
    current_time_display_->setInnerText(FormatTime(current_time_));
  }
  
  // Update progress bar width
  if (progress_fill_ && duration_ > 0) {
    double percentage = (current_time_ / duration_) * kLargestPercentage;
    percentage = std::max(0.0, std::min(kLargestPercentage, percentage));
    
    // Setting the Progress Bar Width
    String style_value = String::Format("width: %.2f%%", percentage);
    progress_fill_->setAttribute(
        html_names::kStyleAttr,
        AtomicString(style_value));
    
    LOG(INFO) << "Update progress bar width: " << percentage << "%";
  }
}

String MediaRemotingInterstitial::FormatTime(double seconds) {
  return MediaControlsSharedHelpers::FormatTimeHM(seconds);
}

// End screen mirroring button clicked
void MediaRemotingInterstitial::OnStopCastingClicked() {
  LOG(INFO) << "Stop casting button clicked";
  // Execute the logic for ending screen sharing
  GetVideoElement().HandleStopMediaCast();
}

// Switch device button click
void MediaRemotingInterstitial::OnSwitchDeviceClicked() {
  LOG(INFO) << "Switch device button clicked";
  // Execute the logic of the switching device
  GetVideoElement().OnMediaCastEnter();
}

void MediaRemotingInterstitial::UpdateUiPlayState(bool is_playing) {
  LOG(INFO) << "MediaRemotingInterstitial::UpdateUiPlayState " << is_playing;
  is_playing_ = !is_playing;
  OnPlayPauseClicked();
}

void MediaRemotingInterstitial::UpdateUiPlayPosition(int64_t position) {
  LOG(INFO) << "MediaRemotingInterstitial::UpdateUiPlayPosition " << position;
  current_time_ = static_cast<double>(position) / kMillisecond;
  LOG(INFO) << "MediaRemotingInterstitial::current_time_ " << current_time_;
  UpdateProgressUI();
}

void MediaRemotingInterstitial::UpdateRemotePlayState(bool is_playing) {
  GetVideoElement().UpdateRemotePlayState(is_playing);
}

void MediaRemotingInterstitial::UpdateRemotePlayPosition(int64_t position) {
  GetVideoElement().UpdateRemotePlayPosition(position);
}

void MediaRemotingInterstitial::OnProgressDragStart() {
  LOG(INFO) << "Start dragging the progress bar";
  
  // Mark Drag Status
  is_progress_dragging_ = true;
  drag_start_time_ = current_time_;
  
  // Save playback state before dragging
  was_playing_before_drag_ = is_playing_;
  
  // If playing, pause playback.
  if (is_playing_) {
    is_playing_ = false;
    
    // Update play button UI
    UpdatePlayButtonUI();
    
    // Playback needs to be resumed after the marker drag ends.
    should_resume_after_drag_ = true;
    
    LOG(INFO) << "Drag to start, pause playback.";
  } else {
    // If it was originally paused, dragging it won't resume playback.
    should_resume_after_drag_ = false;
    LOG(INFO) << "Originally paused, does not resume playback after dragging.";
  }
  
  // Disable progress bar transition animation
  if (progress_fill_) {
    String current_style = progress_fill_->getAttribute(html_names::kStyleAttr);
    String new_style = current_style + "; transition: none !important;";
    progress_fill_->setAttribute(
        html_names::kStyleAttr,
        AtomicString(new_style));
  }
}

void MediaRemotingInterstitial::OnProgressDragging(double percentage) {
  if (!is_progress_dragging_) {
    LOG(WARNING) << "Non-drag state, ignore drag updates";
    return;
  }
  
  // Verify input
  percentage = std::max(0.0, std::min(kLargestPercentage, percentage));
  
  // Calculate the preview time.
  double preview_time = (percentage / kLargestPercentage) * duration_;
  
  LOG(INFO) << "Dragging: " << percentage << "%, Preview Time: " << preview_time << "s";

  if (!current_time_display_) return;
  
  // Update Time Text
  current_time_display_->setInnerText(FormatTime(preview_time));
  
  // Update progress bar preview
  if (progress_fill_) {
    String preview_style = String::Format(
        "width: %.2f%%; background: #FFFFFF; transition: none !important;", 
        percentage);
    
    progress_fill_->setAttribute(
        html_names::kStyleAttr,
        AtomicString(preview_style));
  }
}

void MediaRemotingInterstitial::OnProgressDragEnd(double percentage) {
  LOG(INFO) << "End of drag progress bar: " << percentage << "%";
  
  if (!is_progress_dragging_) {
    LOG(WARNING) << "Non-drag state, ignore drag end";
    return;
  }
  
  // End the drag state.
  is_progress_dragging_ = false;
  
  // Restore progress bar style
  if (progress_fill_) {
    // Remove Drag Style
    String current_style = progress_fill_->getAttribute(html_names::kStyleAttr);
    String new_style = current_style.Replace("transition: none !important;", "");
    progress_fill_->setAttribute(
        html_names::kStyleAttr,
        AtomicString(new_style));
  }
  
  // Execute jump
  OnProgressBarClicked(percentage);

    // If it was playing before dragging, resume playback.
  if (should_resume_after_drag_ && was_playing_before_drag_) {
    // Resuming the Playback Status
    is_playing_ = true;

    // Update play button UI - Display pause icon
    UpdatePlayButtonUI();
    LOG(INFO) << "Playback resumed";
  } else {
    LOG(INFO) << "Do not resume playback";
    // Ensure the play button is displayed correctly
    UpdatePlayButtonUI();
  }
  // Reset Status
  should_resume_after_drag_ = false;
  was_playing_before_drag_ = false;
}

void MediaRemotingInterstitial::OnProgressBarClicked(double percentage) {
  LOG(INFO) << "Processing progress bar click: " << percentage << "%";
  
  // Verify input
  percentage = std::max(0.0, std::min(kLargestPercentage, percentage));
  
  if (duration_ <= 0) {
    LOG(WARNING) << "Unable to jump: Video duration is 0";
    return;
  }
  
  // Calculate the target time.
  double old_time = current_time_;
  double target_time = (percentage / kLargestPercentage) * duration_;
  
  // Update Time
  current_time_ = target_time;
  
  LOG(INFO) << "Progress Jump, old_time: " << old_time << ", current_time: " << current_time_;
  
  // Update UI Now
  UpdateProgressUI();

  // Clicking on the progress bar requires notification to the remote end.
  UpdateRemotePlayPosition(static_cast<int64_t>(current_time_ * kMillisecond));
}

void MediaRemotingInterstitial::OnProgressDragCancel() {
  LOG(INFO) << "Progress bar drag cancel";
  if (is_progress_dragging_) {
    // Restore playback state before dragging
    if (was_playing_before_drag_) {
      LOG(INFO) << "Drag to cancel, restore the original playback state.";
      
      is_playing_ = true;
      UpdatePlayButtonUI();
    }
    
    is_progress_dragging_ = false;
  
    // Restore progress bar style
    if (progress_fill_) {
      // Remove Drag Style
      String current_style = progress_fill_->getAttribute(html_names::kStyleAttr);
      String new_style = current_style.Replace("transition: none !important;", "");
      progress_fill_->setAttribute(
          html_names::kStyleAttr,
          AtomicString(new_style));
    }

    // Reset Status
    should_resume_after_drag_ = false;
    was_playing_before_drag_ = false;
  }
}

} // namespace blink
/*	 
  * Copyright (c) 2026 Huawei Device Co., Ltd.	 
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

#include "third_party/blink/renderer/core/html/media/media_remoting_interstitial.h"

#include <memory>
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/html/media/html_video_element.h"
#include "third_party/blink/renderer/core/testing/page_test_base.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/testing/task_environment.h"
#include "third_party/blink/renderer/core/dom/element_traversal.h"
#include "third_party/blink/renderer/core/dom/shadow_root.h"

namespace blink {

class MediaRemotingInterstitialTest : public PageTestBase {
 protected:
  void SetUp() override {
    PageTestBase::SetUp();
    SetHtmlInnerHTML("<body><video id='testVideo'></video></body>");
    video_element_ = To<HTMLVideoElement>(GetDocument().getElementById(AtomicString("testVideo")));
    ShadowRoot& shadow_root = video_element_->EnsureUserAgentShadowRoot();
    interstitial_ = MakeGarbageCollected<MediaRemotingInterstitial>(*video_element_);
    shadow_root.appendChild(interstitial_);
    GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);
  }

  void TearDown() override {
    video_element_ = nullptr;
    interstitial_ = nullptr;
    PageTestBase::TearDown();
  }

  // Helper method to find child elements by their shadow pseudo ID
  Element* FindChildByShadowPseudoId(const AtomicString& pseudo_id) {
    if (ShadowRoot* shadow_root = interstitial_->GetShadowRoot()) {
      for (Element* child = ElementTraversal::FirstChild(*shadow_root);
           child; child = ElementTraversal::NextSibling(*child)) {
        if (child->ShadowPseudoId() == pseudo_id) {
          return child;
        }
      }
    }
    for (Element* child = ElementTraversal::FirstChild(*interstitial_);
         child; child = ElementTraversal::NextSibling(*child)) {
      if (child->ShadowPseudoId() == pseudo_id) {
        return child;
      }
    }
    
    return nullptr;
  }

  HTMLVideoElement* video_element_;
  Persistent<MediaRemotingInterstitial> interstitial_;
};

TEST_F(MediaRemotingInterstitialTest, ConstructorInitializesElements) {
  ASSERT_NE(interstitial_, nullptr);
  EXPECT_EQ(interstitial_->tagName(), AtomicString("DIV"));
  EXPECT_TRUE(interstitial_->hasChildren());
  bool found_background = false;
  for (Element* child = ElementTraversal::FirstChild(*interstitial_);
       child; child = ElementTraversal::NextSibling(*child)) {
    if (child->tagName() == AtomicString("IMG")) {
      found_background = true;
      break;
    }
  }
  EXPECT_TRUE(found_background);
}

TEST_F(MediaRemotingInterstitialTest, ShowWithEmptyDeviceName) {
  EXPECT_FALSE(interstitial_->IsVisible());
  interstitial_->Show(WebString());
  EXPECT_TRUE(interstitial_->IsVisible());
}

TEST_F(MediaRemotingInterstitialTest, ShowWithDeviceName) {
  const WebString device_name = WebString::FromUTF8("Living Room TV");
  interstitial_->Show(device_name);
  EXPECT_TRUE(interstitial_->IsVisible());
}

TEST_F(MediaRemotingInterstitialTest, ShowWhenAlreadyVisible) {
  interstitial_->Show(WebString::FromUTF8("Device"));
  EXPECT_TRUE(interstitial_->IsVisible());
  interstitial_->Show(WebString::FromUTF8("Different Device"));
  EXPECT_TRUE(interstitial_->IsVisible());
}

TEST_F(MediaRemotingInterstitialTest, HideWithNoTextError) {
  interstitial_->Show(WebString::FromUTF8("Device"));
  EXPECT_TRUE(interstitial_->IsVisible());
  interstitial_->Hide(0);
  EXPECT_FALSE(interstitial_->IsVisible());
}

TEST_F(MediaRemotingInterstitialTest, HideWithErrorCode) {
  interstitial_->Show(WebString::FromUTF8("Device"));
  EXPECT_TRUE(interstitial_->IsVisible());
  const int error_code = 1;
  interstitial_->Hide(error_code);
  EXPECT_FALSE(interstitial_->IsVisible());
}

TEST_F(MediaRemotingInterstitialTest, HideWhenNotVisible) {
  EXPECT_FALSE(interstitial_->IsVisible());
  interstitial_->Hide(0);
  EXPECT_FALSE(interstitial_->IsVisible());
}

TEST_F(MediaRemotingInterstitialTest, OnPosterImageChanged) {
  video_element_->setAttribute(html_names::kPosterAttr,
                               AtomicString("initial_poster.jpg"));
  ShadowRoot& shadow_root = video_element_->EnsureUserAgentShadowRoot();
  auto* interstitial = MakeGarbageCollected<MediaRemotingInterstitial>(*video_element_);
  shadow_root.appendChild(interstitial);
  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);
  Element* bg_image = FindChildByShadowPseudoId(
        AtomicString("-internal-media-interstitial-background-image"));
  if (!bg_image) {
    for (Element* child = ElementTraversal::FirstChild(*interstitial);
       child; child = ElementTraversal::NextSibling(*child)) {
        if (child->ShadowPseudoId() == AtomicString("IMG")) {
            bg_image = child;
            break;
        }
    }
  }
  ASSERT_NE(bg_image, nullptr);
  AtomicString initial_src = bg_image->getAttribute(html_names::kSrcAttr);
  video_element_->setAttribute(html_names::kPosterAttr,
                               AtomicString("new_poster.jpg"));
  interstitial->OnPosterImageChanged();
  AtomicString updated_src = bg_image->getAttribute(html_names::kSrcAttr);
  EXPECT_EQ(updated_src, AtomicString("new_poster.jpg"));
}

TEST_F(MediaRemotingInterstitialTest, GetVideoElement) {
  EXPECT_EQ(&interstitial_->GetVideoElement(), video_element_);
}

TEST_F(MediaRemotingInterstitialTest, DocumentMovement) {
  GetDocument().body()->appendChild(interstitial_);
  interstitial_->Show(WebString::FromUTF8("Test Device"));
  EXPECT_TRUE(interstitial_->IsVisible());
  interstitial_->Hide(0);
  EXPECT_FALSE(interstitial_->IsVisible());
}

TEST_F(MediaRemotingInterstitialTest, VisibilityStates) {
  EXPECT_FALSE(interstitial_->IsVisible());
  interstitial_->Show(WebString::FromUTF8("Device"));
  EXPECT_TRUE(interstitial_->IsVisible());
  interstitial_->Hide(1);
  EXPECT_FALSE(interstitial_->IsVisible());
}

}  // namespace blink
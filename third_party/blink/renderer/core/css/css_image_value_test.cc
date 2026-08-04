// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/core/css/css_image_value.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/web/web_script_source.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/loader/resource/image_resource_content.h"
#include "third_party/blink/renderer/core/style/computed_style.h"
#include "third_party/blink/renderer/core/testing/sim/sim_request.h"
#include "third_party/blink/renderer/core/testing/sim/sim_test.h"
#include "third_party/blink/renderer/platform/testing/unit_test_helpers.h"

namespace blink {

class CSSImageValueTest : public SimTest {};

TEST_F(CSSImageValueTest, BlockPotentiallyDanglingMarkup) {
  SimRequest main_resource("https://example.com", "text/html");

  LoadURL("https://example.com");

  main_resource.Complete(R"HTML(
    <!doctype html>
    <table id="t1" background="ht
    tps://example.com/y<ay?foo"><td>XXX</td></table>
    <table id="t2" background="ht
    tps://example.com/y<ay?bar#boo"><td>XXX</td></table>
  )HTML");

  test::RunPendingTasks();
  Compositor().BeginFrame();

  auto* t1 = GetDocument().getElementById(AtomicString("t1"));
  ImageResourceContent* content1 =
      t1->ComputedStyleRef().BackgroundLayers().GetImage()->CachedImage();
  ASSERT_TRUE(content1);
  EXPECT_TRUE(content1->ErrorOccurred());

  auto* t2 = GetDocument().getElementById(AtomicString("t2"));
  ImageResourceContent* content2 =
      t2->ComputedStyleRef().BackgroundLayers().GetImage()->CachedImage();
  ASSERT_TRUE(content2);
  EXPECT_TRUE(content2->ErrorOccurred());
}

// Ensure that CSSUrlData::MakeComputed() does not launder the dangling markup
// flag by overwriting relative_url_ with the canonicalized absolute URL.
// The attack vector: style resolution creates a computed value via
// MakeComputed(), then commitStyles() serializes it (via CssText(), which
// uses relative_url_) and re-parses it on the element's inline style. If
// relative_url_ was overwritten with the canonical form, the re-parsed URL
// lacks newlines and the dangling markup flag is lost.
TEST_F(CSSImageValueTest, MakeComputedDoesNotLaunderDanglingMarkup) {
  SimRequest main_resource("https://example.com/index.html", "text/html");

  LoadURL("https://example.com/index.html");

  main_resource.Complete(R"HTML(
    <!doctype html>
    <style>
      @keyframes anim {
        from { background-image: url('/exfil?\a <secret'); }
        to   { background-image: url('/exfil?\a <secret'); }
      }
      #target {
        animation: anim 1s paused;
        width: 100px;
        height: 100px;
      }
    </style>
    <div id="target"></div>
  )HTML");

  test::RunPendingTasks();
  GetDocument().UpdateStyleAndLayoutTree();
  Compositor().BeginFrame();

  // commitStyles() serializes the computed animation style (which goes
  // through MakeComputed()) and sets it on the inline style. If the fix
  // is missing, the re-parsed URL loses the dangling markup flag.
  MainFrame().ExecuteScript(WebScriptSource(R"JS(
    const target = document.getElementById('target');
    target.getAnimations()[0].commitStyles();
  )JS"));

  GetDocument().UpdateStyleAndLayoutTree();
  Compositor().BeginFrame();

  auto* target = GetDocument().getElementById(AtomicString("target"));
  ASSERT_TRUE(target);
  const StyleImage* image =
      target->ComputedStyleRef().BackgroundLayers().GetImage();
  if (image) {
    EXPECT_TRUE(image->ErrorOccurred());
  }
}

}  // namespace blink

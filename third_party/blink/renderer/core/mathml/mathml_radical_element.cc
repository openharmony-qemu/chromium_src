// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/core/mathml/mathml_radical_element.h"

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/core/layout/mathml/layout_mathml_block_with_anonymous_mrow.h"
#if BUILDFLAG(ARKWEB_ADVANCED_SECURITY_MODE)
#include "arkweb/chromium_ext/third_party/blink/renderer/core/css/css_utils.h"
#endif

namespace blink {

MathMLRadicalElement::MathMLRadicalElement(const QualifiedName& tagName,
                                           Document& document)
    : MathMLRowElement(tagName, document) {}

bool MathMLRadicalElement::HasIndex() const {
  return HasTagName(mathml_names::kMrootTag);
}

LayoutObject* MathMLRadicalElement::CreateLayoutObject(
    const ComputedStyle& style) {
#if BUILDFLAG(ARKWEB_ADVANCED_SECURITY_MODE)
  if (Cssutils::IsMathFormulaDisabledMode()) {
    return MathMLElement::CreateLayoutObject(style);
  }
#endif
  if (!style.IsDisplayMathType()) {
    return MathMLElement::CreateLayoutObject(style);
  }
  if (HasTagName(mathml_names::kMsqrtTag))
    return MakeGarbageCollected<LayoutMathMLBlockWithAnonymousMrow>(this);
  return MakeGarbageCollected<LayoutMathMLBlock>(this);
}

}  // namespace blink

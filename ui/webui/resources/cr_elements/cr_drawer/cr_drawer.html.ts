// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import {html} from '//resources/lit/v3_0/lit.rollup.js';

import type {CrDrawerElement} from './cr_drawer.js';

export function getHtml(this: CrDrawerElement) {
  return html`
<dialog id="dialog" @cancel="${this.onDialogCancel_}"
    @click="${this.onDialogClick_}" @close="${this.onDialogClose_}">
  <div id="container" @click="${this.onContainerClick_}">
    <div class="drawer-header">
      <slot name="header-icon">
      </slot>
      <div id="heading" tabindex="-1">${this.heading}</div>
    </div>
    <slot name="body"></slot>
  </div>
</dialog>`;
}

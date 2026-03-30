// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import {html} from '//resources/lit/v3_0/lit.rollup.js';

import type {ToolbarElement} from './toolbar.js';

export function getHtml(this: ToolbarElement) {
  // clang-format off
  return html`<!--_html_template_start_-->
<cr-toolbar id="toolbar" page-name="$i18n{toolbarTitle}"
    search-prompt="$i18n{search}" clear-label="$i18n{clearSearch}" autofocus
    menu-label="$i18n{mainMenu}" ?narrow="${this.narrow}"
    @narrow-changed="${this.onNarrowChanged_}" narrow-threshold="1000"
    ?show-menu=false>
  <div class="more-actions">
    <span id="devModeLabel">$i18n{toolbarDevMode}</span>
    <cr-tooltip-icon ?hidden="${!this.shouldDisableDevMode_()}"
        tooltip-text="${this.getTooltipText_()}" icon-class="${this.getIcon_()}"
        icon-aria-label="${this.getTooltipText_()}">
    </cr-tooltip-icon>
    <cr-toggle id="devMode" @change="${this.onDevModeToggleChange_}"
        ?disabled="${this.shouldDisableDevMode_()}" ?checked="${this.inDevMode}"
        aria-labelledby="devModeLabel">
    </cr-toggle>
  </div>
</cr-toolbar>
${this.showPackDialog_ ? html`
  <extensions-pack-dialog .delegate="${this.delegate}"
      @close="${this.onPackDialogClose_}">
  </extensions-pack-dialog>` : ''}
<div id="devDrawer" ?expanded="${this.expanded_}">
  <div id="buttonStrip">
    <cr-button ?hidden="${!this.canLoadUnpacked_()}" id="loadUnpacked"
        @click="${this.onLoadUnpackedClick_}">
      $i18n{toolbarLoadUnpacked}
    </cr-button>
    <cr-button id="packExtensions" hidden=true @click="${this.onPackClick_}">
      $i18n{toolbarPack}
    </cr-button>
    <cr-button id="updateNow" hidden=true @click="${this.onUpdateNowClick_}"
        title="$i18n{toolbarUpdateNowTooltip}">
      $i18n{toolbarUpdateNow}
    </cr-button>
  </div>
</div>
<cr-dialog id="devModeConfirmationDialog" @cancel="${this.onDevModeDialogCancel_}">
  <div slot="body">$i18n{toolbarDevModeConfirmation}</div>
  <div slot="button-container">
    <cr-button class="cancel-button" @click="${this.onDevModeDialogCancel_}">
      $i18n{toolbarDevModeCancel}
    </cr-button>
    <cr-button class="action-button" @click="${this.onDevModeDialogConfirm_}">
      $i18n{toolbarDevModeConfirm}
    </cr-button>
  </div>
</cr-dialog>
<!--_html_template_end_-->`;
  // clang-format on
}

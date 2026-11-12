// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import {html} from '//resources/lit/v3_0/lit.rollup.js';

import type {ExtensionsBlacklistReviewPanelElement} from './blacklist_review_panel.js';

export function getHtml(this: ExtensionsBlacklistReviewPanelElement) {
  // clang-format off
  return html`<!--_html_template_start_-->
<div class="panel-background"
    ?hidden="${!this.shouldShowBlacklistReviewPanel_()}">
  <cr-expand-button no-hover id="expandButton"
      ?expanded="${this.blacklistReviewListExpanded_}"
      @expanded-changed="${this.onBlacklistReviewListExpandedChanged_}"
      ?hidden="${!this.shouldShowBlacklistedExtensions_}">
    <div class="panel-header" id="blacklistPanelContainer">
      <cr-icon aria-hidden="true" icon="extensions-icons:my_extensions"
          class="panel-header-icon">
      </cr-icon>
      <div class="panel-header-text">
        <h3 id="headingText">${this.headerString_}</h3>
        <div class="cr-secondary-text" id="secondaryText">
          ${this.subtitleString_}
        </div>
      </div>
      <cr-button class="action-button bulk-action-button"
          aria-label="$i18n{safetyCheckRemoveAll}" id="removeAllButton"
          @click="${this.onRemoveAllClick_}"
          ?hidden="${!this.shouldShowRemoveAllButton_()}">
        $i18n{safetyCheckRemoveAll}
      </cr-button>
    </div>
  </cr-expand-button>

  <cr-collapse class="panel-extensions"
      ?opened="${this.blacklistReviewListExpanded_}"
      ?hidden="${!this.shouldShowBlacklistedExtensions_}">
    ${this.extensions.map((item, index) => html`
      <div class="panel-extension-row cr-row">
        <img class="panel-extension-icon" src="${item.iconUrl}"
            role="presentation">
        <div class="panel-extension-info text-elide">
          <div class="extension-representation">${item.name}</div>
          <div class="cr-secondary-text">
            ${this.getBlacklistReason_()}
          </div>
        </div>
        <cr-icon-button iron-icon="cr:delete" data-index="${index}" actionable
            @click="${this.onRemoveExtensionClick_}"
            aria-label="${this.getRemoveButtonA11yLabel_(item.name)}"
            ?hidden="${!this.isExtensionRemovable_(item)}">
        </cr-icon-button>
      </div>`)}
  </cr-collapse>

  <div class="header-with-icon completion-container"
      ?hidden="${!this.shouldShowCompletionInfo_}">
    <cr-icon role="img" icon="cr:check"></cr-icon>
    <span class="header-group-wrapper">${this.completionMessage_}</span>
  </div>
</div>
<!--_html_template_end_-->`;
  // clang-format on
}

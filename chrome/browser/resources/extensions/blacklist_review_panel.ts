// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'chrome://resources/cr_elements/cr_button/cr_button.js';
import 'chrome://resources/cr_elements/cr_collapse/cr_collapse.js';
import 'chrome://resources/cr_elements/cr_expand_button/cr_expand_button.js';
import 'chrome://resources/cr_elements/cr_icon_button/cr_icon_button.js';
import 'chrome://resources/cr_elements/cr_icon/cr_icon.js';
import 'chrome://resources/cr_elements/icons.html.js';

import type {CrButtonElement} from 'chrome://resources/cr_elements/cr_button/cr_button.js';
import type {CrExpandButtonElement} from 'chrome://resources/cr_elements/cr_expand_button/cr_expand_button.js';
import {I18nMixinLit} from 'chrome://resources/cr_elements/i18n_mixin_lit.js';
import {assert} from 'chrome://resources/js/assert.js';
import {loadTimeData} from 'chrome://resources/js/load_time_data.js';
import {PluralStringProxyImpl} from 'chrome://resources/js/plural_string_proxy.js';
import type {PropertyValues} from 'chrome://resources/lit/v3_0/lit.rollup.js';
import {CrLitElement} from 'chrome://resources/lit/v3_0/lit.rollup.js';

import type {ItemDelegate} from './item.js';
import {getCss} from './blacklist_review_panel.css.js';
import {getHtml} from './blacklist_review_panel.html.js';

export interface ExtensionsBlacklistReviewPanelElement {
  $: {
    blacklistPanelContainer: HTMLDivElement,
    expandButton: CrExpandButtonElement,
    headingText: HTMLElement,
    secondaryText: HTMLElement,
    removeAllButton: CrButtonElement,
  };
}

const ExtensionsBlacklistReviewPanelElementBase = I18nMixinLit(CrLitElement);

export class ExtensionsBlacklistReviewPanelElement extends
    ExtensionsBlacklistReviewPanelElementBase {
  static get is() {
    return 'extensions-blacklist-review-panel';
  }

  static override get styles() {
    return getCss();
  }

  override render() {
    return getHtml.bind(this)();
  }

  static override get properties() {
    return {
      delegate: {type: Object},

      /**
       * List of extensions disabled by the blocklist flow. If this list is
       * empty, all the extensions were reviewed and the completion info should
       * be visible.
       */
      extensions: {type: Array},

      /**
       * IDs of extensions that can be removed by the user.
       */
      removableExtensionIds_: {type: Array},

      /**
       * The string for the primary header label.
       */
      headerString_: {type: String},

      /**
       * The string for secondary text under the header string.
       */
      subtitleString_: {type: String},

      /**
       * The text of the completion state.
       */
      completionMessage_: {type: String},

      /**
       * Indicates whether to show the blocklisted extensions.
       */
      shouldShowBlacklistedExtensions_: {type: Boolean},

      /**
       * Indicates whether to show completion info after the user has finished
       * the review process.
       */
      shouldShowCompletionInfo_: {type: Boolean},

      /**
       * Indicates if the list of blocklisted extensions is expanded.
       */
      blacklistReviewListExpanded_: {type: Boolean},
    };
  }

  accessor delegate: ItemDelegate|undefined;
  accessor extensions: chrome.developerPrivate.ExtensionInfo[] = [];
  protected accessor removableExtensionIds_: string[] = [];
  protected accessor headerString_: string = '';
  protected accessor subtitleString_: string = '';
  protected accessor blacklistReviewListExpanded_: boolean = true;
  protected accessor completionMessage_: string = '';
  protected accessor shouldShowCompletionInfo_: boolean = false;
  protected accessor shouldShowBlacklistedExtensions_: boolean = false;

  /**
   * Tracks if the last action that led to the number of extensions under
   * review going to 0 was taken in this panel.
   */
  private numberOfExtensionsChangedByLastPanelAction_: number = 0;
  private completionMetricLogged_: boolean = false;

  override willUpdate(changedProperties: PropertyValues<this>) {
    super.willUpdate(changedProperties);

    if (changedProperties.has('extensions')) {
      this.removableExtensionIds_ = this.computeRemovableExtensionIds_();
      this.shouldShowCompletionInfo_ = this.computeShouldShowCompletionInfo_();
      this.shouldShowBlacklistedExtensions_ =
          this.computeShouldShowBlacklistedExtensions_();
      this.onExtensionsChanged_();
    }
  }

  private async onExtensionsChanged_() {
    this.headerString_ =
        await PluralStringProxyImpl.getInstance().getPluralString(
            'blacklistReviewTitle', this.extensions.length);
    this.subtitleString_ =
        await PluralStringProxyImpl.getInstance().getPluralString(
            'blacklistReviewDescription', this.extensions.length);
    this.completionMessage_ =
        await PluralStringProxyImpl.getInstance().getPluralString(
            'blacklistReviewAllDoneForNow',
            this.numberOfExtensionsChangedByLastPanelAction_);
  }

  private computeShouldShowCompletionInfo_(): boolean {
    if (this.extensions?.length === 0 &&
        this.numberOfExtensionsChangedByLastPanelAction_ !== 0) {
      if (!this.completionMetricLogged_) {
        this.completionMetricLogged_ = true;
      }
      return true;
    }
    return false;
  }

  private computeShouldShowBlacklistedExtensions_(): boolean {
    if (this.extensions?.length !== 0) {
      this.completionMetricLogged_ = false;
      if (this.shouldShowCompletionInfo_) {
        this.numberOfExtensionsChangedByLastPanelAction_ = 0;
      }
      return true;
    }
    return false;
  }

  private canRemoveExtension_(extension: chrome.developerPrivate.ExtensionInfo):
      boolean {
    return !extension.mustRemainInstalled && extension.userMayModify;
  }

  private computeRemovableExtensionIds_(): string[] {
    return this.extensions.filter(extension => this.canRemoveExtension_(extension))
        .map(extension => extension.id);
  }

  protected shouldShowBlacklistReviewPanel_(): boolean {
    return loadTimeData.getBoolean('blacklistReviewShowReviewPanel') &&
        (this.shouldShowBlacklistedExtensions_ || this.shouldShowCompletionInfo_);
  }

  protected shouldShowRemoveAllButton_(): boolean {
    return this.extensions.length > 1 &&
        this.removableExtensionIds_.length > 0;
  }

  protected onBlacklistReviewListExpandedChanged_(
      e: CustomEvent<{value: boolean}>) {
    this.blacklistReviewListExpanded_ = e.detail.value;
  }

  protected getRemoveButtonA11yLabel_(extensionName: string): string {
    return loadTimeData.substituteString(
        this.i18n('safetyCheckRemoveButtonA11yLabel'), extensionName);
  }

  protected getBlacklistReason_(): string {
    return this.i18n('blacklistReviewRowReason');
  }

  protected isExtensionRemovable_(
      extension: chrome.developerPrivate.ExtensionInfo): boolean {
    return this.canRemoveExtension_(extension);
  }

  protected async onRemoveExtensionClick_(e: Event): Promise<void> {
    const index = Number((e.target as HTMLElement).dataset['index']);
    const item = this.extensions[index]!;
    if (!this.canRemoveExtension_(item)) {
      return;
    }

    if (this.extensions?.length === 1) {
      this.numberOfExtensionsChangedByLastPanelAction_ = 1;
    }

    try {
      assert(this.delegate);
      await this.delegate.uninstallItem(item.id);
    } catch (_) {
      // The error was almost certainly the user canceling the dialog.
      this.numberOfExtensionsChangedByLastPanelAction_ = 0;
    }
  }

  protected async onRemoveAllClick_(event: Event): Promise<void> {
    event.stopPropagation();
    if (this.removableExtensionIds_.length === 0) {
      return;
    }

    if (this.removableExtensionIds_.length === this.extensions.length) {
      this.numberOfExtensionsChangedByLastPanelAction_ =
          this.removableExtensionIds_.length;
    } else {
      this.numberOfExtensionsChangedByLastPanelAction_ = 0;
    }
    try {
      assert(this.delegate);
      await this.delegate.deleteItems(this.removableExtensionIds_);
    } catch (_) {
      // The error was almost certainly the user canceling the dialog.
      this.numberOfExtensionsChangedByLastPanelAction_ = 0;
    }
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'extensions-blacklist-review-panel': ExtensionsBlacklistReviewPanelElement;
  }
}

customElements.define(
    ExtensionsBlacklistReviewPanelElement.is,
    ExtensionsBlacklistReviewPanelElement);

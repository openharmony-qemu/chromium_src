// Copyright 2015 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'chrome://resources/cr_elements/cr_icon_button/cr_icon_button.js';
import 'chrome://resources/cr_elements/icons.html.js';

import type {CrIconButtonElement} from 'chrome://resources/cr_elements/cr_icon_button/cr_icon_button.js';
import {CrLitElement} from 'chrome://resources/lit/v3_0/lit.rollup.js';

import type {Bookmark} from '../bookmark_type.js';

import {getCss} from './viewer_bookmark.css.js';
import {getHtml} from './viewer_bookmark.html.js';

/** Amount that each level of bookmarks is indented by (px). */
const BOOKMARK_INDENT: number = 20;

export enum ChangePageOrigin {
  BOOKMARK = 'bookmark',
  THUMBNAIL = 'thumbnail',
  PAGE_SELECTOR = 'pageSelector',
}

export interface ChangePageAndXyDetail {
  page: number;
  x: number;
  y: number;
  origin: ChangePageOrigin;
}

export interface ChangePageDetail {
  page: number;
  origin: ChangePageOrigin;
}

export interface ChangeZoomDetail {
  zoom: number;
}

export interface NavigateDetail {
  newtab: boolean;
  uri: string;
}

declare global {
  interface HTMLElementEventMap {
    'change-page-and-xy': CustomEvent<ChangePageAndXyDetail>;
    'change-page': CustomEvent<ChangePageDetail>;
    'change-zoom': CustomEvent<ChangeZoomDetail>;
    'navigate': CustomEvent<NavigateDetail>;
    // arkweb_pdf: extends for bookmark clicking.
    'click-bookmark':CustomEvent<string>;
  }
}

export interface ViewerBookmarkElement {
  $: {
    item: HTMLElement,
    expand: CrIconButtonElement,
  };
}

export class ViewerBookmarkElement extends CrLitElement {
  static get is() {
    return 'viewer-bookmark';
  }

  static override get styles() {
    return getCss();
  }

  override render() {
    return getHtml.bind(this)();
  }

  static override get properties() {
    return {
      bookmark: {type: Object},

      depth: {type: Number},

      childrenShown_: {
        type: Boolean,
        reflect: true,
      },
    };
  }

  // arkweb_pdf: Remove dest info. Move them to native side accroding to id.
  accessor bookmark: Bookmark = {title: '', id:'', children: []};
  accessor depth: number = 0;
  protected accessor childrenShown_: boolean = false;

  override firstUpdated() {
    this.$.item.addEventListener('keydown', e => {
      if (e.key === 'Enter') {
        this.onEnter_(e);
      } else if (e.key === ' ') {
        this.onSpace_(e);
      }
    });
  }

  protected getItemStartPaddingStyle_(): string {
    return `padding-inline-start: ${this.depth * BOOKMARK_INDENT}px`;
  }

  protected getChildDepth_(): number {
    return this.depth + 1;
  }

  protected getExpandHidden_(): boolean {
    return this.bookmark.children.length <= 0;
  }

  protected onClick_() {
    if (!this.bookmark.id) {
      console.error('Bookmark ID is missing');
      return;
    }
    // arkweb_pdf: Now bookmark has no any information about the destination.
    this.fire('click-bookmark', this.bookmark.id);
  }

  private onEnter_(e: KeyboardEvent) {
    // Don't allow events which have propagated up from the expand button to
    // trigger a click.
    if (e.target !== this.$.expand) {
      this.onClick_();
    }
  }

  private onSpace_(e: KeyboardEvent) {
    // cr-icon-button stops propagation of space events, so there's no need
    // to check the event source here.
    this.onClick_();
    // Prevent default space scroll behavior.
    e.preventDefault();
  }

  protected toggleChildren_(e: Event) {
    this.childrenShown_ = !this.childrenShown_;
    e.stopPropagation();  // Prevent the above onClick_ handler from firing.
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'viewer-bookmark': ViewerBookmarkElement;
  }
}

customElements.define(ViewerBookmarkElement.is, ViewerBookmarkElement);

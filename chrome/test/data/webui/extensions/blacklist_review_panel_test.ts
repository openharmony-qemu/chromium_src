// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/** @fileoverview Suite of tests for extensions-blacklist-review-panel. */
import 'chrome://extensions/extensions.js';

import type {ExtensionsBlacklistReviewPanelElement} from 'chrome://extensions/extensions.js';
import {PluralStringProxyImpl} from 'chrome://extensions/extensions.js';
import {loadTimeData} from 'chrome://resources/js/load_time_data.js';
import {assertDeepEquals, assertEquals, assertFalse, assertTrue} from 'chrome://webui-test/chai_assert.js';
import {TestPluralStringProxy} from 'chrome://webui-test/test_plural_string_proxy.js';
import {isVisible, microtasksFinished} from 'chrome://webui-test/test_util.js';

import {createExtensionInfo, MockItemDelegate} from './test_util.js';

suite('ExtensionsBlacklistReviewPanel', function() {
  let element: ExtensionsBlacklistReviewPanelElement;
  let pluralString: TestPluralStringProxy;

  setup(async function() {
    pluralString = new TestPluralStringProxy();
    PluralStringProxyImpl.setInstance(pluralString);
    loadTimeData.overrideValues({
      blacklistReviewShowReviewPanel: true,
    });
    document.body.innerHTML = window.trustedTypes!.emptyHTML;
    element = document.createElement('extensions-blacklist-review-panel');
    element.extensions = [
      createExtensionInfo({
        name: 'Alpha',
        id: 'a'.repeat(32),
        disableReasons: {
          ...createExtensionInfo().disableReasons,
          suspiciousInstall: true,
        },
      }),
    ];
    document.body.appendChild(element);
    await microtasksFinished();
  });

  test('ReviewPanelTextExists', function() {
    const reviewPanelContainer = element.$.blacklistPanelContainer;
    assertTrue(!!reviewPanelContainer);
    assertTrue(isVisible(reviewPanelContainer));

    const expandButton = element.$.expandButton;
    assertTrue(!!expandButton);
    assertTrue(isVisible(expandButton));

    const headingContainer = element.$.headingText;
    assertTrue(!!headingContainer);

    const headingArgs = pluralString.getArgs('getPluralString')[0];
    assertEquals('blacklistReviewTitle', headingArgs.messageName);
    assertEquals(1, headingArgs.itemCount);

    const descriptionArgs = pluralString.getArgs('getPluralString')[1];
    assertEquals('blacklistReviewDescription', descriptionArgs.messageName);
    assertEquals(1, descriptionArgs.itemCount);
  });

  test('NonRemovableItemHasNoDeleteButton', async function() {
    element.extensions = [
      createExtensionInfo({
        name: 'Policy Item',
        id: 'p'.repeat(32),
        mustRemainInstalled: true,
        disableReasons: {
          ...createExtensionInfo().disableReasons,
          suspiciousInstall: true,
        },
      }),
    ];
    await microtasksFinished();

    const removeButton = element.shadowRoot.querySelector<HTMLElement>(
        'cr-icon-button[iron-icon="cr:delete"]');
    assertTrue(!!removeButton);
    assertFalse(isVisible(removeButton));
  });

  test('RemoveSingleExtension', async function() {
    let removedId = '';
    class MockUninstallItemDelegate extends MockItemDelegate {
      override uninstallItem(id: string): Promise<void> {
        removedId = id;
        element.extensions =
            element.extensions.filter(extension => extension.id !== id);
        return Promise.resolve();
      }
    }

    element.delegate = new MockUninstallItemDelegate();
    element.shadowRoot.querySelector<HTMLElement>(
                           'cr-icon-button[iron-icon="cr:delete"]')!.click();
    await microtasksFinished();

    assertEquals('a'.repeat(32), removedId);
    const completionTextContainer =
        element.shadowRoot.querySelector<HTMLElement>('.completion-container');
    assertTrue(!!completionTextContainer);
    assertTrue(isVisible(completionTextContainer));
  });

  test('RemoveAllOnlyTargetsRemovableExtensions', async function() {
    const removableA = createExtensionInfo({
      name: 'Removable A',
      id: 'a'.repeat(32),
      disableReasons: {
        ...createExtensionInfo().disableReasons,
        suspiciousInstall: true,
      },
    });
    const nonRemovable = createExtensionInfo({
      name: 'Non Removable',
      id: 'b'.repeat(32),
      userMayModify: false,
      disableReasons: {
        ...createExtensionInfo().disableReasons,
        suspiciousInstall: true,
      },
    });
    const removableC = createExtensionInfo({
      name: 'Removable C',
      id: 'c'.repeat(32),
      disableReasons: {
        ...createExtensionInfo().disableReasons,
        suspiciousInstall: true,
      },
    });
    element.extensions = [removableA, nonRemovable, removableC];

    let removedIds: string[] = [];
    class MockDeleteItemsDelegate extends MockItemDelegate {
      override deleteItems(ids: string[]) {
        removedIds = ids;
        element.extensions =
            element.extensions.filter(extension => !ids.includes(extension.id));
        return Promise.resolve();
      }
    }

    element.delegate = new MockDeleteItemsDelegate();
    await microtasksFinished();

    element.shadowRoot.querySelector<HTMLElement>('#removeAllButton')!.click();
    await microtasksFinished();

    assertDeepEquals(['a'.repeat(32), 'c'.repeat(32)], removedIds);
    assertEquals(1, element.extensions.length);
    assertEquals('b'.repeat(32), element.extensions[0]?.id);

    const completionTextContainer =
        element.shadowRoot.querySelector<HTMLElement>('.completion-container');
    assertTrue(!!completionTextContainer);
    assertFalse(isVisible(completionTextContainer));
  });

  test('RemoveAllCanShowCompletionState', async function() {
    const removableA = createExtensionInfo({
      name: 'Removable A',
      id: 'a'.repeat(32),
      disableReasons: {
        ...createExtensionInfo().disableReasons,
        suspiciousInstall: true,
      },
    });
    const removableB = createExtensionInfo({
      name: 'Removable B',
      id: 'b'.repeat(32),
      disableReasons: {
        ...createExtensionInfo().disableReasons,
        suspiciousInstall: true,
      },
    });
    element.extensions = [removableA, removableB];

    class MockDeleteItemsDelegate extends MockItemDelegate {
      override deleteItems(ids: string[]) {
        element.extensions =
            element.extensions.filter(extension => !ids.includes(extension.id));
        return Promise.resolve();
      }
    }

    element.delegate = new MockDeleteItemsDelegate();
    await microtasksFinished();

    element.shadowRoot.querySelector<HTMLElement>('#removeAllButton')!.click();
    await microtasksFinished();

    const completionTextContainer =
        element.shadowRoot.querySelector<HTMLElement>('.completion-container');
    assertTrue(!!completionTextContainer);
    assertTrue(isVisible(completionTextContainer));
  });

  test('RemoveAllShownWhenListHasAtLeastTwoRows', async function() {
    element.extensions = [
      createExtensionInfo({
        name: 'Removable',
        id: 'r'.repeat(32),
        disableReasons: {
          ...createExtensionInfo().disableReasons,
          suspiciousInstall: true,
        },
      }),
      createExtensionInfo({
        name: 'Non Removable',
        id: 'n'.repeat(32),
        userMayModify: false,
        disableReasons: {
          ...createExtensionInfo().disableReasons,
          suspiciousInstall: true,
        },
      }),
    ];
    await microtasksFinished();

    const removeAllButton =
        element.shadowRoot.querySelector<HTMLElement>('#removeAllButton');
    assertTrue(!!removeAllButton);
    assertTrue(isVisible(removeAllButton));
  });
});

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

class ArkWebEntityReplacer {
  static DATA_DETECTOR_ATTR_PREFIX = 'ohos-arkweb-data-detectors-attr-';
  static DATA_DETECTOR_TYPE = 'ohos-arkweb-data-detectors-type';
  static DATA_DETECTOR_NATIVE_KEYS = ['ohosarkwebtype', 'ohos-arkweb-type'];

  constructor() {
    this.pendingRequests = new Map();
    this.listenerMap = new WeakMap();
    this.listenerMapForTouchTest = new WeakMap();
    this.lastClickedEntity = null;
    this.entityAllAttrs = new Set();
    this.init();
  }

  init() {
    this.lastClickedEntity = null;
    this.pendingRequests.clear();
    console.log('JS::WebDataDetector init');
    this.queueProcessing(document.body);
    this.addSmartClickListener(document.body, this.handleLinkClick);
    this.addSmartTouchTestListener(document.body, this.handleLinkTouchTest);
  }

  getScreenElementEdgesExt(element) {
    const rect = element.getBoundingClientRect(); // Gets the element's position information in the layout viewport
    let win = element.ownerDocument.defaultView;
    let left = rect.left;
    let top = rect.top;
    let width = rect.width;
    let height = rect.height;
    while (true) {
      let scale = 1;
      let offsetLeft = 0;
      let offsetTop = 0;
      if (win.visualViewport) {
        scale = win.visualViewport.scale;
        offsetLeft = win.visualViewport.offsetLeft; // visual viewport horizontal offset
        offsetTop = win.visualViewport.offsetTop; // visual viewport vertical offset
      }
      left = (left - offsetLeft) * scale;
      top = (top - offsetTop) * scale;
      width *= scale;
      height *= scale;
      if (win === window.top) {
        break;
      }
      const frameElement = win.frameElement;
      if (frameElement) {
        const frameRect = frameElement.getBoundingClientRect();
        left += frameRect.left;
        top += frameRect.top;
      } else {
        break;
      }
      win = win.parent;
    }

    const dpr = window?.devicePixelRatio || 1;
    return {
      left: left * dpr,
      top: top * dpr,
      right: (left + width) * dpr,
      bottom: (top + height) * dpr,
    };
  }

  getScreenElementEdges(element) {
    const rect = element.getBoundingClientRect();
    const win = element.ownerDocument.defaultView;
    let scale = 1;
    let offsetLeft = 0;
    let offsetTop = 0;
    if (win.visualViewport) {
        scale = win.visualViewport.scale;
        offsetLeft = win.visualViewport.offsetLeft;
        offsetTop = win.visualViewport.offsetTop;
    }
    const visualViewportLeft = rect.left - offsetLeft;
    const visualViewportTop = rect.top - offsetTop;
    const visualViewportRight = rect.right - offsetLeft;
    const visualViewportBottom = rect.bottom - offsetTop;

    const dpr = window?.devicePixelRatio || 1;

    const physicalLeft = visualViewportLeft * dpr * scale;
    const physicalTop = visualViewportTop * dpr * scale;
    const physicalRight = visualViewportRight * dpr * scale;
    const physicalBottom = visualViewportBottom * dpr * scale;

    return {
      left: physicalLeft,
      top: physicalTop,
      right: physicalRight,
      bottom: physicalBottom,
    };
  }

  replayLastClickEntity() {
    if (!this.lastClickedEntity) {
      return;
    }
    const link = this.lastClickedEntity.deref();
    if (!link) {
      return;
    }
    let msg = {
      content: link.textContent,
      outerHTML: link.outerHTML,
      entityType: link.getAttribute(ArkWebEntityReplacer.DATA_DETECTOR_TYPE),
      rect: this.getScreenElementEdgesExt(link),
    };
    this.sendMsgToNative(msg);
  }

  handleLinkClick = (event) => {
    const link = event.target;
    if (link.tagName !== 'A' || !link.hasAttribute(ArkWebEntityReplacer.DATA_DETECTOR_TYPE)) {
      return;
    }
    event.preventDefault();

    let msg = {
      content: link.textContent,
      outerHTML: link.outerHTML,
      entityType: link.getAttribute(ArkWebEntityReplacer.DATA_DETECTOR_TYPE),
      rect: this.getScreenElementEdgesExt(link),
    };
    this.lastClickedEntity = new WeakRef(link);
    this.sendMsgToNative(msg);
  };

  handleLinkTouchTest = (event) => {
    if (event.touches.length !== 1) {
      return;
    }
    const link = event.target;
    if (link.tagName !== 'A' || !link.hasAttribute(ArkWebEntityReplacer.DATA_DETECTOR_TYPE)) {
      return;
    }
    
    let msg = {
      content: link.textContent,
      entityType: link.getAttribute(ArkWebEntityReplacer.DATA_DETECTOR_TYPE),
      touchTest: true,
      attrs: this.getAttrsJson(link),
    };

    this.sendMsgToNative(msg);
  };

  sendMsgToNative(msg) {
    try {
      if (window.arkWebAceEntityReplacerProxy) {
        window.arkWebAceEntityReplacerProxy.clickEntity(JSON.stringify(msg));
      } else {
        throw new Error('JS::WebDataDetector Native bridge not available');
      }
    } catch (error) {
      console.error('JS::WebDataDetector error:', error);
    }
  }

  getAttrsJson(elem) {
    let attrsJson = {};
    try {
      const prefix = ArkWebEntityReplacer.DATA_DETECTOR_ATTR_PREFIX;
      this.entityAllAttrs.forEach((key) => {
        if (elem.hasAttribute(key) && key.startsWith(prefix)) {
          const newKey = key.slice(prefix.length).replace(/-([a-z])/g, (_, group1) => group1.toUpperCase());
          attrsJson[newKey] = elem.getAttribute(key);
        }
      });
    } catch (error) {
      console.error('JS::WebDataDetector error:', error);
      return {};
    }
    
    return attrsJson;
  }

  addSmartClickListener(element, callback) {
    if (this.listenerMap.has(element)) {
      return;
    }

    function handler(event) {
      callback(event);
    }

    this.listenerMap.set(element, handler);
    element.addEventListener('click', handler);
  }

  addSmartTouchTestListener(element, callback) {
    if (this.listenerMapForTouchTest.has(element)) {
      return;
    }
  
    function handler(event) {
      callback(event);
    }

    this.listenerMapForTouchTest.set(element, handler);
    element.addEventListener('touchstart', handler);
  }

  queueProcessing(root) {
    requestIdleCallback(() => {
      this.processNode(root);
    }, { timeout: 1000 });
  }

  getTreeWalker(root) {
    return document.createTreeWalker(
      root,
      NodeFilter.SHOW_TEXT,
      {
        acceptNode: node => {
          if (node.parentNode.style.display === 'none' ||
            node.parentNode.style.visibility === 'hidden') {
            return NodeFilter.FILTER_REJECT;
          }

          const text = node.nodeValue.trim();
          if (!text) {
            return NodeFilter.FILTER_REJECT;
          }

          const parentTag = node.parentNode.tagName;
          const rejectTags = ['SCRIPT', 'STYLE', 'NOSCRIPT', 'TEMPLATE', 'SVG', 'CODE', 'PRE', 'INPUT', 'TEXTAREA', 'A'];
          if (rejectTags.includes(parentTag)) {
            return NodeFilter.FILTER_REJECT;
          }

          if (/^[\s\W_]+$/.test(text) && !/[\p{L}\p{N}]/u.test(text)) {
            return NodeFilter.FILTER_REJECT;
          }

          if (text.length < 2 || text.length > 1000) {
            return NodeFilter.FILTER_REJECT;
          }

          if (/^\s*<!DOCTYPE/i.test(text)) {
            return NodeFilter.FILTER_REJECT;
          }

          if (node?.parentNode && node.parentNode.classList?.contains('ohos-arkweb-wrapper')) {
            return NodeFilter.FILTER_REJECT;
          }

          let currentNode = node.parentNode;
          while (currentNode && currentNode !== currentNode.ownerDocument.documentElement) {
            if (currentNode.isContentEditable || currentNode.getAttribute('contenteditable') === 'true' || rejectTags.includes(currentNode.tagName)) {
              return NodeFilter.FILTER_REJECT;
            }
            currentNode = currentNode.parentNode;
          }

          return NodeFilter.FILTER_ACCEPT;
        }
      }
    );
  }

  collectTextNodes(root) {
    const nodes = [];
    const walker = this.getTreeWalker(root);
    if (walker) {
      while (walker.nextNode()) {
        const node = walker.currentNode;
        nodes.push(node);
      }
    }

    const iframes = root.querySelectorAll('iframe');
    for (const iframe of iframes) {
      try {
        if (iframe.contentDocument &&
          iframe.contentDocument.readyState === 'complete' &&
          this.isSameOrigin(iframe)) {
          nodes.push(...this.collectTextNodes(iframe.contentDocument.body));
        }
      } catch (e) {
        console.warn('JS::WebDataDetector Iframe access denied:', e);
      }
    }

    return nodes;
  }

  isSameOrigin(iframe) {
    try {
      return iframe.contentWindow.location.host === window.location.host;
    } catch (e) {
      return false;
    }
  }

  async processNode(root) {
    let nodes = this.collectTextNodes(root);
    let currentChunk = [];
    let currentSize = 0;

    console.log('JS::WebDataDetector processNode');
    for (const node of nodes) {
      if (!node.isConnected) {
        continue;
      }

      const textLength = node.nodeValue ? node.nodeValue.length : 0;
      if (currentSize + textLength > 300 && currentChunk.length > 0) {
        await this.processSingleTask(currentChunk);
        currentChunk = [node];
        currentSize = textLength;
      } else {
        currentChunk.push(node);
        currentSize += textLength;
      }
    };

    if (currentChunk.length > 0) {
      await this.processSingleTask(currentChunk);
    }
  }

  async processSingleTask(nodes) {
    console.log('JS::WebDataDetector processSingleTask');
    try {
      let requestId = await this.sendToNative(nodes);
      console.log('JS::WebDataDetector receive result requestId', requestId);
    } catch (error) {
      console.error(error);
    }
  }

  async sendToNative(nodes, timeout = 1000) {
    const requestId = crypto.randomUUID();
    const abortController = new AbortController();

    // create a new promise
    const promise = new Promise((resolve, reject) => {
      this.pendingRequests.set(requestId, { nodes, resolve, reject });

      // if the promise is rejected or aborted, remove it from the pending requests
      abortController.signal.addEventListener('abort', () => {
        this.pendingRequests.delete(requestId);
        reject(new Error('JS::WebDataDetector Native bridge request aborted'));
      });
    });

    // set timeout
    const timeoutId = setTimeout(() => {
      abortController.abort();
    }, timeout);

    // send request to native
    const payload = {
      requestId,
      nodes: nodes.map(node => ({
        path: this.getNodePath(node),
        text: node.nodeValue,
      }))
    };

    try {
      if (window.arkWebAceEntityReplacerProxy) {
        window.arkWebAceEntityReplacerProxy.processRequest(JSON.stringify(payload));
      } else {
        throw new Error('JS::WebDataDetector Native bridge not available');
      }
    } catch (error) {
      clearTimeout(timeoutId);
      this.pendingRequests.delete(requestId);
      throw error;
    }

    promise.finally(() => {
      clearTimeout(timeoutId);
    });

    return promise;
  }

  handleNativeResult(resultJson) {
    let context;
    try {
      const result = JSON.parse(resultJson);
      if (!result) {
        console.warn('JS::WebDataDetector result is empty');
        return;
      }
      context = this.pendingRequests.get(result.requestId);

      if (!context) {
        console.warn('JS::WebDataDetector unknown requestId:', result.requestId);
        return;
      }

      context.resolve(result.requestId);
      this.applyReplacements(result, context.nodes);
      this.pendingRequests.delete(result.requestId);
    } catch (error) {
      console.error('JS::WebDataDetector handleNativeResult error:', error);
      context?.reject(error);
    }
  }

  replaceARGBToRGBA(text) {
    // match #AARRGGBB
    const argbRegex = /#([0-9a-f]{2})([0-9a-f]{2})([0-9a-f]{2})([0-9a-f]{2})\b/gi;

    // replace to #RRGGBBAA
    return text.replace(argbRegex, (match, alpha, red, green, blue) => {
      return `#${red}${green}${blue}${alpha}`.toUpperCase();
    });
  }

  applyReplacements(result, originalNodes) {
    if (!result.matches) {
      return;
    }
    if (result.style) {
      if (result.style.color) {
        result.style.color = this.replaceARGBToRGBA(result.style.color);
      }
      if (result.style.textDecoration) {
        result.style.textDecoration = this.replaceARGBToRGBA(result.style.textDecoration);
      }
    }

    result.matches.forEach(match => {
      const originalNode = originalNodes[match.nodeIndex];
      if (!originalNode.isConnected) {
        return;
      }

      const fragment = document.createDocumentFragment();
      let lastPos = 0;

      match.entities.forEach(entity => {
        // add text before entity
        if (entity.start > lastPos) {
          fragment.appendChild(document.createTextNode(
            originalNode.nodeValue.slice(lastPos, entity.start)
          ));
        }

        // create entity link
        const link = document.createElement('a');
        link.href = entity.hrefType + entity.clean;
        link.className = 'ohos-arkweb-link';
        link.textContent = originalNode.nodeValue.slice(entity.start, entity.end);
        if (result.style) {
          link.style.color = result.style.color;
          link.style.textDecoration = result.style.textDecoration;
        }
        if (entity.attrs) {
          Object.entries(entity.attrs).forEach(([key, value]) => {
            if (ArkWebEntityReplacer.DATA_DETECTOR_NATIVE_KEYS.includes(key.toLowerCase())) {
              link.setAttribute(ArkWebEntityReplacer.DATA_DETECTOR_TYPE, value);
            } else {
              const newKey = ArkWebEntityReplacer.DATA_DETECTOR_ATTR_PREFIX + key.replace(/[A-Z]/g, (letter) => {
                return `-${letter.toLowerCase()}`;
              });
              link.setAttribute(newKey, value);
              this.entityAllAttrs.add(newKey);
            }
          });
        }
        fragment.appendChild(link);

        lastPos = entity.end;
      });

      if (lastPos === 0) {
        return;
      }
      // add remaining text
      if (lastPos < originalNode.nodeValue.length) {
        fragment.appendChild(document.createTextNode(
          originalNode.nodeValue.slice(lastPos)
        ));
      }

      // replace original node with fragment
      const wrapper = document.createElement('span');
      wrapper.className = 'ohos-arkweb-wrapper';
      wrapper.appendChild(fragment);
      originalNode.parentNode.replaceChild(wrapper, originalNode);
      // add click listener to wrapper ownerDocument.body
      this.addSmartClickListener(wrapper.ownerDocument.body, this.handleLinkClick);
      this.addSmartTouchTestListener(wrapper.ownerDocument.body, this.handleLinkTouchTest);
    });
    console.log('JS::WebDataDetector end replace for requestId', result.requestId);
  }

  getNodePath(node) {
    const path = [];
    while (node.parentNode) {
      const index = Array.prototype.indexOf.call(node.parentNode.childNodes, node);
      path.unshift(index);
      node = node.parentNode;
    }
    return path.join('/');
  }

}

function runArkWebDataDetectorInit() {
  if (window.arkWebEntityReplacer) {
    window.arkWebEntityReplacer.init();
  } else {
    window.arkWebEntityReplacer = new ArkWebEntityReplacer();
  }
}

function runArkWebDataDetectorUpdate() {
  if (window.arkWebEntityReplacer) {
    window.arkWebEntityReplacer.queueProcessing(document.body);
  } else {
    console.error(
      'JS::WebDataDetector runArkWebDataDetectorUpdate: Not initialized');
  }
}

if (document.readyState === 'loading') {
  document.addEventListener('DOMContentLoaded', runArkWebDataDetectorInit);
} else {
  runArkWebDataDetectorInit();
}

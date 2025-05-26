/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
const AndroidPhoneUserAgent99: string = "Mozilla/5.0 (Linux; Android 12; NOH-AN00 Build/HUAWEINOH-AN00; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/99.0.4844.84 Mobile Safari/537.36"
const TAG = 'KeyLog '
import web_webview from '@ohos.web.webview'
import common from '@ohos.app.ability.common';
import fileio from '@ohos.fileio';

@Component
export struct WebTestTool {
  // 当前测试的url
  @State url: string = ''
  // 当前测试的url下标
  @State index: number = 0
  // 需要测试的url列表
  private urls: Array<string> = []

  // 页面切换间隔时间
  private interval: number = 20000
  // 是否展示Web
  @State show: boolean = false
  // 是否展示url列表
  @State showList: boolean = true
  // 是否展示测试结果
  @State showResult: boolean = false
  // 当前测试结果
  @State pageLogList: Array<any> = []

  controller: web_webview.WebviewController = new web_webview.WebviewController()

  scroller: Scroller = new Scroller();
  //  controller: WebController = new WebController();
  private context = getContext(this) as common.UIAbilityContext;


  aboutToAppear() {
    // 允许chrome://inspect调试
    web_webview.WebviewController.setWebDebuggingAccess(true)
  }


  init() {
    this.index = 0
    this.pageLogList = []
    this.url = ''
  }

  switchToNextPage(callback) {
    this.index++
    if(this.index >= this.urls.length) {
      this.showTestResult()
      return
    } else {
      this.url = this.urls[this.index]
      console.log(TAG + ': url:' +  this.url + 'index: ' + this.index)
      this.show = true
    }
    callback && callback(this.index)
  }

  // 手动点击测试
  next(callback) {
    this.showList = false
    this.showResult = false
    if(this.show) {
      this.show = false
      setTimeout(this.switchToNextPage.bind(this, callback), 1000)
    } else {
      this.show = true
      if(this.index < this.urls.length) {
        this.url = this.urls[this.index]
      } else {
        this.index = this.urls.length - 1
      }
      callback && callback(this.index)
    }
  }

  showTestResult() {
    this.showList = false
    this.show = false
    this.showResult = true
  }

  showUrlList() {
    this.showList = true
    this.show = false
    this.showResult = false
  }

  showWeb() {
    this.showList = false
    this.show = true
    this.showResult = false

  }

  testSinglePage():void {
    this.showWeb()
    this.index = 0
    this.url = this.urls[this.index]
  }

  // 自动全量测试
  autoTestStart() {
    new Promise((resolve, reject) => {
      setTimeout(() => {
        this.next(resolve)
      }, this.interval)
    }).then((index) => {
      if(this.index < this.urls.length) {
        this.autoTestStart()
      }
    })
  }

  start() {
    if(this.index >= this.urls.length) {
      this.index = 0
    }
    this.next(() => {})
    this.autoTestStart()
  }

  calculateAverage(key: string):string {
    let total = 0
    this.pageLogList.forEach(pageLog => {
      total += Number(pageLog[key])
    })
    return (total / this.pageLogList.length).toFixed(0).toString()
  }

  getFSPTime(domLoad: number, imgLoad: number): string {
    return Math.max(domLoad, imgLoad).toFixed(0).toString()
  }

  downLoadResult():void {
    const path = this.context.filesDir + '/result.json'
    let fd = fileio.openSync(path, 0o100 | 0o2, 0o666);
    let num = fileio.writeSync(fd, JSON.stringify(this.pageLogList));
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
      if(this.showList) {
        Flex({ justifyContent: FlexAlign.SpaceAround }) {
          Button('开始测试').onClick(this.start.bind(this))
          Button('测试结果').onClick(this.showTestResult.bind(this))
        }
        Scroll(this.scroller) {
          Column() {
            ForEach(this.urls, url => {
              Text(url.toString()).width('90%').fontSize(20);
            })
          }
        }.scrollable(ScrollDirection.Vertical).width('100%').height('90%').margin({ top: '2%' });
      }
      if(this.show) {
        Text(this.url).fontSize(16)
        Web({ src: this.url, controller: this.controller })
          .onPageBegin((event) => {
            // LOAD
            this.controller.runJavaScript(
              "window.addEventListener('load', () => console.log('keyLog Load:' + performance.now()));\n" +
              "console.log(`listener load add`)");

            // DCL
            this.controller.runJavaScript(
              "window.addEventListener('DOMContentLoaded', () => console.log('keyLog DCL:' + performance.now()));\n" +
              "console.log(`listener dcl add`)");

            // FCP
            this.controller.runJavaScript(
              "new PerformanceObserver(entryList => {\n" +
              "    for (const entry of entryList.getEntriesByName('first-contentful-paint')) {\n" +
              "        console.log('keyLog FCP:', entry.startTime);\n" +
              "    }\n" +
              "    })\n" +
              "    // Some browsers throw when 'type' is passed:\n" +
              "    .observe({ type: \"paint\", buffered: true });" +
              "console.log(`listener fcp add`)");

            // LCP
            this.controller.runJavaScript(
              "new PerformanceObserver(entryList => {\n" +
              "    for (const entry of entryList.getEntries()) {\n" +
              "        console.log('keyLog LCP:', entry.startTime);\n" +
              "    }\n" +
              "    })\n" +
              "    // Some browsers throw when 'type' is passed:\n" +
              "    .observe({ type: \"largest-contentful-paint\", buffered: true });" +
              "console.log(`listener lcp add`)");

            // 图片监听
            this.controller.runJavaScript(
              "window.addEventListener('DOMContentLoaded', () => {\n" +
              "    const imgs = document.getElementsByTagName('img')\n" +
              "    Array.from(imgs).forEach(element => {\n" +
              "        const { top, left } = element.getBoundingClientRect()\n" +
              "        if (top < window.innerHeight && left < window.innerWidth) {\n" +
              "            element.addEventListener('load', () => {\n" +
              "                console.log('keyLog IMG: ' + performance.now());\n" +
              "            })\n" +
              "        }\n" +
              "    })\n" +
              "});\n" +
              "console.log(`listener img add`)");

            // 屏幕范围内dom监听
            this.controller.runJavaScript(
              "const registerListener = () => {\n" +
              "    if (!window || !window.document || !window.document.documentElement) {\n" +
              "        setTimeout(registerListener, 10)\n" +
              "        return\n" +
              "    }\n" +
              "\n" +
              "    const recordTime = (time) => {\n" +
              "        const span = document.getElementById('hw_ignore_span')\n" +
              "        if(span) {\n" +
              "            span.innerText = time.toFixed(0)\n" +
              "        } else {\n" +
              "            const span = document.createElement('span')\n" +
              "            span.id = 'hw_ignore_span'\n" +
              "            span.style.position = 'fixed'\n" +
              "            span.style.top = '200px'\n" +
              "            span.style.right = '0px'\n" +
              "            span.style.fontSize = '30px'\n" +
              "            span.style.color = 'red'\n" +
              "            span.style.zIndex = '9999999'\n" +
              "            span.innerText = time.toFixed(0)\n" +
              "            document.documentElement.appendChild(span)\n" +
              "        }\n" +
              "    }\n" +
              "\n" +
              "    const domObserver = new window.MutationObserver((records) => {\n" +
              "        for (let item of records) {\n" +
              "            if (item.addedNodes.length > 0) {\n" +
              "                let time = performance.now()\n" +
              "                for (let i = 0; i < item.addedNodes.length; i++) {\n" +
              "                    const element = item.addedNodes[i];\n" +
              "                    if (element.id === 'hw_ignore_span') {\n" +
              "                        continue\n" +
              "                    }\n" +
              "                    if (element && element.getBoundingClientRect) {\n" +
              "                        const { top, left, width, height } = element.getBoundingClientRect()\n" +
              "                        const elementSize = height * width\n" +
              "                        const totalSize = window.innerHeight * window.innerWidth\n" +
              "                        if (top < window.innerHeight && left < window.innerWidth && elementSize / totalSize >= 0.005) {\n" +
              "                            if(element.tagName === 'IMG') {\n" +
              "                                element.addEventListener('load', () => {\n" +
              "                                    console.log('keyLog IMG: ' + performance.now());\n" +
              "                                    recordTime(performance.now())\n" +
              "                                })\n" +
              "                            }\n" +
              "                            const imgs = element.getElementsByTagName('img')\n" +
              "                            Array.from(imgs).forEach(img => {\n" +
              "                                const { top, left } = img.getBoundingClientRect()\n" +
              "                                if(top < window.innerHeight && left < window.innerWidth) {\n" +
              "                                    img.addEventListener('load', () => {\n" +
              "                                        console.log('keyLog IMG: ' + performance.now());\n" +
              "                                        recordTime(performance.now())\n" +
              "                                    })\n" +
              "                                }\n" +
              "                            })\n" +
              "                            console.log('keyLog time: ' + time);\n" +
              "                            recordTime(time)\n" +
              "                        }\n" +
              "                    }\n" +
              "                }\n" +
              "            };\n" +
              "        };\n" +
              "    });\n" +
              "    domObserver.observe(document.documentElement, {\n" +
              "        childList: true,\n" +
              "        subtree: true,\n" +
              "        attributes: true\n" +
              "    });\n" +
              "};\n" +
              "registerListener()");
          })
          .fileAccess(true)
          .domStorageAccess(true)
          .geolocationAccess(true)
          .userAgent(AndroidPhoneUserAgent99)
          .width("100%")
          .height("100%")
          .zIndex(10)
          .onConsole((event) => {
            const savedInfo = this.pageLogList.find(item => item.url === this.url)
            let pageLogInfo
            if(savedInfo) {
              pageLogInfo = savedInfo
            } else {
              pageLogInfo = { url: this.url, lcp: 0, load: 0, dcl: 0, fcp: 0, time: 0, img: 0 }
              this.pageLogList.push(pageLogInfo)
            }
            const logMsg = event.message.getMessage()
            console.log(logMsg)
            if(logMsg && logMsg.includes('keyLog')) {
              const [key, value] = logMsg.split(':')
              const num = Number(value).toFixed(0)
              switch (key) {
                case 'keyLog FCP':
                  pageLogInfo.fcp = num;
                  break;
                case 'keyLog LCP':
                  pageLogInfo.lcp = num;
                  break;
                case 'keyLog DCL':
                  pageLogInfo.dcl = num
                  break;
                case 'keyLog Load':
                  pageLogInfo.load = num
                  break;
                case 'keyLog time':
                  pageLogInfo.time = num
                  break;
                case 'keyLog IMG':
                  pageLogInfo.img = num
                  break;
                default: break;
              }
            }
            console.log('keyLog total: ' + JSON.stringify(this.pageLogList))
            return false;
          })
      }
      if(this.showResult) {
        Flex({ justifyContent: FlexAlign.SpaceAround }) {
          Button('测试列表').onClick(this.showUrlList.bind(this))
          Button('开始测试').onClick(this.start.bind(this))
        }
        Scroll(this.scroller) {
          Column() {
            Button('下载结果').onClick(this.downLoadResult.bind(this))
            Text('平均值：').fontSize(16)
            Row() {
              Text('fcp: ').fontSize(14)
              Text(this.calculateAverage('fcp')).fontSize(14).margin({ right: '45px' });

              Text('lcp: ').fontSize(14)
              Text(this.calculateAverage('lcp')).fontSize(14).margin({ right: '45px' });

              //              Text('dcl: ').fontSize(14)
              //              Text(this.calculateAverage('dcl')).fontSize(14).margin({ right: '45px' })

              Text('load: ').fontSize(14)
              Text(this.calculateAverage('load')).fontSize(14).margin({ right: '45px' })
            }.margin({ bottom: '2%'})

            ForEach(this.pageLogList, pageLog => {
              Row() {
                Text(pageLog.url).fontSize(16)
              }.margin({ top: '2%' })
              Row() {
                Text('FCP: ').fontSize(14)
                Text(pageLog.fcp.toString()).fontSize(14).margin({ right: '25px' });

                Text('LCP: ').fontSize(14)
                Text(pageLog.lcp.toString()).fontSize(14).margin({ right: '25px' });

                //                Text('dcl: ').fontSize(14)
                //                Text(pageLog.dcl.toString()).fontSize(14).margin({ right: '30px' })

                Text('Load: ').fontSize(14)
                Text(pageLog.load.toString()).fontSize(14).margin({ right: '25px' })

                //                Text('dom: ').fontSize(14)
                //                Text(pageLog.time.toString()).fontSize(14).margin({ right: '25px' })
                //
                //                Text('img: ').fontSize(14)
                //                Text(pageLog.img.toString()).fontSize(14).margin({ right: '25px' })

                Text('FSP: ').fontSize(14)
                Text(this.getFSPTime(pageLog.time, pageLog.img)).fontSize(14).margin({ right: '25px' })
              }.border({ width: 1 }).width('95%')
            })
          }.margin({ top: '2%' })
        }.scrollable(ScrollDirection.Vertical).width('100%').height('90%').margin({ top: '2%' });
      }
    }
  }
}

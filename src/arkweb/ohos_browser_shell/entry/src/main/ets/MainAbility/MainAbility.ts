import Ability from '@ohos.application.Ability'
import { BrowserServiceInitializer } from 'libbrowser_service.so'

export default class MainAbility extends Ability {
    onCreate(want, launchParam) {
        console.log("[BrowserShell] MainAbility onCreate")
        globalThis.abilityWant = want;

        // BrowserService 是否初始化成功。
        globalThis.browserServiceInitialized = false
        console.log("[BrowserShell] MainAbility BrowserService version: " + BrowserServiceInitializer.getVersionName())
    }

    onDestroy() {
        console.log("[BrowserShell] MainAbility onDestroy")
    }

    onWindowStageCreate(windowStage) {
        // Main window is created, set main page for this ability
        console.log("[BrowserShell] MainAbility onWindowStageCreate")

        windowStage.setUIContent(this.context, "pages/frontPage", null)
    }

    onWindowStageDestroy() {
        // Main window is destroyed, release UI related resources
        console.log("[BrowserShell] MainAbility onWindowStageDestroy")
    }

    onForeground() {
        // Ability has brought to foreground
        console.log("[BrowserShell] MainAbility onForeground")
    }

    onBackground() {
        // Ability has back to background
        console.log("[BrowserShell] MainAbility onBackground")
    }
};

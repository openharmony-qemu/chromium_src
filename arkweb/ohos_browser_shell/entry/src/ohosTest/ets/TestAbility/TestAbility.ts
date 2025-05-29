import Ability from '@ohos.application.Ability'

import {Core, ExpectExtend, OhReport} from 'deccjsunit/index'
import testsuite from '../test/List.test'

export default class TestAbility extends Ability {
    onCreate(want, launchParam) {
        console.log("TestAbility onCreate")
        console.info("start run testcase!!!!")
        console.info("++")
        const core = Core.getInstance()
        console.info("const core = Core.getInstance()")
        const expectExtend = new ExpectExtend({
            'id': 'extend'
        })
        console.info("const expectExtend = new ExpectExtend({")
        core.addService('expect', expectExtend)
        console.info("core.addService('expect', expectExtend)")

        const ohReport = new OhReport({
            'delegator': 'helloworld'
        })
        console.info("const ohReport = new OhReport({")

        core.addService('report', ohReport)
        console.info("core.addService('report', ohReport)")
        core.init()
        console.info("core.init()")

        core.subscribeEvent('spec', ohReport)
        core.subscribeEvent('suite', ohReport)
        core.subscribeEvent('task', ohReport)
        console.info("core.subscribeEvent")
        const configService = core.getDefaultService("config")
        console.info("const configService = core.getDefaultService")

        configService.setConfig({'value':'helloworld','class':'ActsAbilityTest#String_assertContain_success','level':'1'})
        console.info("configService.setConfig(...)")
        testsuite()
        console.info("testsuite()")
        core.execute()
        console.info("core.execute()")
    }

    onDestroy() {
        console.log("TestAbility onDestroy")
    }

    onWindowStageCreate(windowStage) {
        console.log("TestAbility onWindowStageCreate")
        windowStage.setUIContent(this.context, "pages/index", null)
        globalThis.abilityContext = this.context;
    }

    onWindowStageDestroy() {
        console.log("TestAbility onWindowStageDestroy")
    }

    onForeground() {
        console.log("TestAbility onForeground")
    }

    onBackground() {
        console.log("TestAbility onBackground")
    }
};
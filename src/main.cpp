#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(GeodeBridgeMenu, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto menu = this->getChildByID("bottom-menu");
        if (!menu) {
            log::warn("Geode Bridge: bottom menu was not found.");
            return true;
        }

        auto sprite = CCSprite::createWithSpriteFrameName("GJ_infoBtn_001.png");
        if (!sprite) {
            sprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        }
        if (!sprite) {
            log::warn("Geode Bridge: could not create the menu icon.");
            return true;
        }

        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(GeodeBridgeMenu::onBridgeButton)
        );
        if (!button) {
            log::warn("Geode Bridge: could not create the menu button.");
            return true;
        }

        button->setID("geode-bridge-open-button"_spr);
        menu->addChild(button);
        menu->updateLayout();

        log::info("Geode Bridge loaded.");
        return true;
    }

    void onBridgeButton(CCObject*) {
        FLAlertLayer::create(
            "Geode Bridge",
            "Bridge prototype loaded.\n\n"
            "The first milestone is read-only level diagnostics. "
            "Nothing is uploaded automatically, and this prototype does not change your levels.",
            "OK"
        )->show();
    }
};

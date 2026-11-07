#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "decor_ai.hpp"

using namespace geode::prelude;

namespace {
    // First UI milestone: a branded entry point for the future Voidlight tools.
    void showVoidlightMenu() {
        FLAlertLayer::create(
            "VOIDLIGHT",
            "<cg>GigaChat Mod Voidlight</c>\n"
            voidlight::DECOR_AI_GUIDE,
            "CLOSE"
        )->show();
    }
}

class $modify(VoidlightMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto menu = this->getChildByID("bottom-menu");
        if (!menu) {
            log::warn("Voidlight: bottom-menu node was not found.");
            return true;
        }

        auto sprite = CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png");
        if (!sprite) {
            log::warn("Voidlight: could not create the menu button sprite.");
            return true;
        }

        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(VoidlightMenuLayer::onVoidlightButton)
        );

        if (!button) {
            log::warn("Voidlight: could not create the menu button.");
            return true;
        }

        button->setID("voidlight-open"_spr);
        menu->addChild(button);
        menu->updateLayout();

        return true;
    }

    void onVoidlightButton(CCObject*) {
        showVoidlightMenu();
    }
};

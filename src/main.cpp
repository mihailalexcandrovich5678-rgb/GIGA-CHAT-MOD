#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

namespace {
    // First UI milestone: a branded entry point for the future Voidlight tools.
    void showVoidlightMenu() {
        FLAlertLayer::create(
            "VOIDLIGHT",
            "<cg>GigaChat Mod Voidlight</c>\n"
            "<cy>DECOR AI</c> — decoration assistant\n\n"
            "<co>Planned tools</c>\n"
            "• Style ideas: Glow, Neon, Modern, Cyberpunk, Fantasy\n"
            "• Block and object suggestions\n"
            "• Color and opacity guidance\n"
            "• Step-by-step decoration plans\n\n"
            "<cr>Prototype UI</c>: the AI generator is not connected yet.",
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

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "decor_ai.hpp"

using namespace geode::prelude;

namespace {
    // First UI milestone: a branded entry point for the future Voidlight tools.
    void showVoidlightMenu() {
        FLAlertLayer::create(
            "VOIDLIGHT",
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

        // Use a clear text label instead of a standard GD icon so the
        // Voidlight entry is easy to distinguish from built-in menu buttons.
        auto label = CCLabelBMFont::create("VOIDLIGHT", "bigFont.fnt");
        if (!label) {
            log::warn("Voidlight: could not create the menu button label.");
            return true;
        }
        label->setScale(0.45f);

        auto button = CCMenuItemSpriteExtra::create(
            label,
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

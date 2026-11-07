#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "decor_ai.hpp"

using namespace geode::prelude;

namespace {
    void showVoidlightPanel() {
        FLAlertLayer::create(
            "VOIDLIGHT",
            "DECOR AI\\n\\n"
            "Your Geometry Dash decoration assistant.\\n\\n"
            "Available guide sections: Glow Lab, Color Lab, Style Book, "
            "Level Planner, Music Matcher and experimental Auto Decor.\\n\\n"
            "Choose a guide from the next update of the panel.",
            "CLOSE"
        )->show();
    }
}

class $modify(VoidlightEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Use the same in-game options gear artwork so the Voidlight control
        // has a familiar Geometry Dash shape and a matching visual size.
        auto sprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        if (!sprite) {
            log::warn("Voidlight: options button sprite was not found.");
            return true;
        }

        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(VoidlightEditorUI::onVoidlight)
        );
        if (!button) {
            log::warn("Voidlight: failed to create editor button.");
            return true;
        }
        button->setID("gigachat-voidlight-editor-button");

        auto menu = CCMenu::create();
        if (!menu) {
            log::warn("Voidlight: failed to create editor button menu.");
            return true;
        }
        menu->setPosition(CCPointZero);
        menu->addChild(button);
        button->setPosition({winSize.width - 205.f, winSize.height - 42.f});
        this->addChild(menu, 1000, "gigachat-voidlight-editor-menu");

        log::info("Voidlight: editor button added.");
        return true;
    }

    void onVoidlight(CCObject*) {
        showVoidlightPanel();
    }
};

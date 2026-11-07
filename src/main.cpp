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

        auto label = CCLabelBMFont::create("VOIDLIGHT", "bigFont.fnt");
        if (!label) {
            log::warn("Voidlight: failed to create editor button label.");
            return true;
        }
        label->setScale(0.42f);

        auto button = CCMenuItemSpriteExtra::create(
            label,
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
        button->setPosition({winSize.width - 170.f, winSize.height - 42.f});
        this->addChild(menu, 1000, "gigachat-voidlight-editor-menu");

        log::info("Voidlight: editor button added.");
        return true;
    }

    void onVoidlight(CCObject*) {
        showVoidlightPanel();
    }
};

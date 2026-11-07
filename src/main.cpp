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

        // Custom Voidlight emblem: violet circular core, neon rings and a
        // centered five-point star. It deliberately does not reuse GD gears.
        auto emblem = CCNode::create();
        if (!emblem) {
            log::warn("Voidlight: failed to create emblem node.");
            return true;
        }
        emblem->setContentSize({64.f, 64.f});
        emblem->setAnchorPoint({0.5f, 0.5f});

        auto glow = CCDrawNode::create();
        if (!glow) return true;
        glow->drawDot({32.f, 32.f}, 31.f, {0.62f, 0.12f, 1.f, 0.24f});
        glow->drawDot({32.f, 32.f}, 27.f, {0.75f, 0.25f, 1.f, 0.8f});
        glow->drawDot({32.f, 32.f}, 23.f, {0.20f, 0.04f, 0.42f, 1.f});
        glow->drawDot({32.f, 32.f}, 19.f, {0.34f, 0.08f, 0.62f, 1.f});
        emblem->addChild(glow, 0);

        auto star = CCDrawNode::create();
        if (!star) return true;
        CCPoint starPoints[] = {
            {32.f, 51.f}, {36.5f, 37.f}, {51.f, 37.f}, {39.5f, 28.f},
            {44.f, 14.f}, {32.f, 22.5f}, {20.f, 14.f}, {24.5f, 28.f},
            {13.f, 37.f}, {27.5f, 37.f}
        };
        star->drawPolygon(
            starPoints, 10,
            {0.92f, 0.82f, 1.f, 1.f},
            1.5f,
            {0.72f, 0.35f, 1.f, 1.f}
        );
        emblem->addChild(star, 1);

        auto button = CCMenuItemSpriteExtra::create(
            emblem,
            this,
            menu_selector(VoidlightEditorUI::onVoidlight)
        );
        if (!button) {
            log::warn("Voidlight: failed to create editor button.");
            return true;
        }
        button->setID("gigachat-voidlight-editor-button");

        auto menu = CCMenu::create();
        if (!menu) return true;
        menu->setPosition(CCPointZero);
        menu->addChild(button);
        button->setPosition({winSize.width - 205.f, winSize.height - 42.f});
        this->addChild(menu, 1000, "gigachat-voidlight-editor-menu");

        log::info("Voidlight: custom purple neon star button added.");
        return true;
    }

    void onVoidlight(CCObject*) {
        showVoidlightPanel();
    }
};

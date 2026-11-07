#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "decor_ai.hpp"

using namespace geode::prelude;

namespace {
    void showVoidlightPanel() {
        FLAlertLayer::create(
            "VOIDLIGHT — DECOR AI",
            "Hello from My Custom Mod 27\n\n"
            "DECOR AI — OFFLINE ASSISTANT\n"
            "A practical Geometry Dash decoration guide. Live GPT is not connected yet.\n\n"
            "GLOW LAB\n"
            "Build solid blocks first, add a bright edge, then place soft glow behind the shape. "
            "Lower opacity until gameplay stays readable.\n\n"
            "COLOR LAB\n"
            "PURPLE VOID: deep purple + lavender + pale violet. "
            "ICE: dark blue + cyan + white. "
            "EMBER: dark brown + orange + warm yellow. "
            "Use one main color, one support color and one highlight.\n\n"
            "STYLE BOOK\n"
            "GLOW — bright edges and soft light. NEON — dark base and clean bright geometry. "
            "MODERN — panels and even spacing. CYBERPUNK — layered panels and neon accents. "
            "FANTASY — arches and crystals. ATMOSPHERIC — silhouettes and subtle particles. "
            "MINIMAL — one clear structure. HARD DECOR — large forms, details, micro-details.\n\n"
            "OBJECT HELPER\n"
            "Use blocks for the main silhouette, slopes for angled forms, outlines for frames, "
            "glow objects for light and small details only where they improve the shape. "
            "Keep hazards and the player path easy to see.\n\n"
            "LEVEL PLANNER\n"
            "INTRO: introduce the world. MAIN: establish the theme. DROP: strongest visual moment. "
            "VARIATION: change scenery or gameplay. FINALE: resolve the idea. "
            "Mark beats and transitions before decorating.\n\n"
            "MUSIC MATCHER\n"
            "Dreamy: ambient and flowing visuals. Dark: deep bass and silhouettes. "
            "Energetic: clear beats and sharp transitions. Epic: build toward a climax. "
            "Cyberpunk: synth/electro and neon accents. Check track length and availability.\n\n"
            "SCREENSHOT ANALYZER\n"
            "Use a screenshot as a reference: check contrast, clutter, empty space, "
            "gameplay readability and consistency. This build does not automatically inspect images.\n\n"
            "PERFORMANCE HELPER\n"
            "Avoid unnecessary layers and effects, reuse a consistent palette, "
            "and test dense sections for visibility and performance.\n\n"
            "MY PROJECTS\n"
            "Keep a note of each level's concept, palette, song, section plan and unfinished tasks.\n\n"
            "AUTO DECOR — EXPERIMENTAL\n"
            "Prototype plan: main block silhouette, thin frame, soft background glow, "
            "a few corner details and clear gameplay space. It does not place objects automatically yet.",
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
            emblem, this, menu_selector(VoidlightEditorUI::onVoidlight)
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
        this->addChild(menu, 1000);

        log::info("Voidlight: purple neon star button initialized.");
        return true;
    }

    void onVoidlight(CCObject*) {
        showVoidlightPanel();
    }
};

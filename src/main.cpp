#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "decor_ai.hpp"

using namespace geode::prelude;

namespace {
    void showTool(const char* title, const char* body) {
        FLAlertLayer::create(title, body, "CLOSE")->show();
    }

    void showDecorAI() {
        showTool("DECOR AI", voidlight::DECOR_AI_GUIDE);
    }

    void showStyleBook() {
        showTool("VOIDLIGHT — STYLE BOOK", voidlight::STYLE_BOOK);
    }

    void showGlowLab() {
        showTool("GLOW LAB", voidlight::GLOW_GUIDE);
    }

    void showColorLab() {
        showTool("COLOR LAB", voidlight::COLOR_GUIDE);
    }

    void showLevelPlanner() {
        showTool("LEVEL PLANNER", voidlight::LEVEL_PLANNER_GUIDE);
    }

    void showMusicMatcher() {
        showTool("MUSIC MATCHER", voidlight::MUSIC_MATCHER_GUIDE);
    }

    void showAutoDecor() {
        showTool("AUTO DECOR — EXPERIMENTAL", voidlight::AUTO_DECOR_GUIDE);
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

        struct ToolButton {
            const char* label;
            SEL_MenuHandler callback;
        };

        ToolButton tools[] = {
            {"VOIDLIGHT", menu_selector(VoidlightMenuLayer::onDecorAI)},
            {"STYLES", menu_selector(VoidlightMenuLayer::onStyleBook)},
            {"GLOW", menu_selector(VoidlightMenuLayer::onGlowLab)},
            {"COLORS", menu_selector(VoidlightMenuLayer::onColorLab)},
            {"AUTO", menu_selector(VoidlightMenuLayer::onAutoDecor)},
            {"PLAN", menu_selector(VoidlightMenuLayer::onLevelPlanner)},
            {"MUSIC", menu_selector(VoidlightMenuLayer::onMusicMatcher)}
        };

        for (auto const& tool : tools) {
            auto label = CCLabelBMFont::create(tool.label, "bigFont.fnt");
            if (!label) {
                log::warn("Voidlight: failed to create a tool label.");
                continue;
            }
            label->setScale(0.32f);

            auto button = CCMenuItemSpriteExtra::create(
                label,
                this,
                tool.callback
            );
            if (!button) {
                log::warn("Voidlight: failed to create a tool button.");
                continue;
            }

            button->setID(("voidlight-" + std::string(tool.label)) .c_str());
            menu->addChild(button);
        }

        menu->updateLayout();
        return true;
    }

    void onDecorAI(CCObject*) { showDecorAI(); }
    void onStyleBook(CCObject*) { showStyleBook(); }
    void onGlowLab(CCObject*) { showGlowLab(); }
    void onColorLab(CCObject*) { showColorLab(); }
    void onAutoDecor(CCObject*) { showAutoDecor(); }
    void onLevelPlanner(CCObject*) { showLevelPlanner(); }
    void onMusicMatcher(CCObject*) { showMusicMatcher(); }
};

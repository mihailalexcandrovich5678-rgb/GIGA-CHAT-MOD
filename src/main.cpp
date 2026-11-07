#include <Geode/Geode.hpp>
#include <cmath>
#include <cstring>
#include <Geode/modify/EditorUI.hpp>
#include "decor_ai.hpp"

using namespace geode::prelude;

namespace {
    void drawRoundedRect(CCDrawNode* draw, float x, float y, float w, float h, float r,
                         ccColor4F fill, ccColor4F border) {
        CCPoint pts[36];
        int n = 0;
        const float centers[4][2] = {
            {x + w - r, y + r}, {x + w - r, y + h - r},
            {x + r, y + h - r}, {x + r, y + r}
        };
        const float starts[4] = {-90.f, 0.f, 90.f, 180.f};
        for (int c = 0; c < 4; ++c) {
            for (int i = 0; i <= 8; ++i) {
                float a = (starts[c] + i * 90.f / 8.f) * 0.0174532925f;
                pts[n++] = CCPointMake(centers[c][0] + r * std::cos(a), centers[c][1] + r * std::sin(a));
            }
        }
        draw->drawPolygon(pts, n, fill, 2.f, border);
    }

    const char* toolText(const char* id) {
        if (!strcmp(id, "glow")) return voidlight::GLOW_GUIDE;
        if (!strcmp(id, "color")) return voidlight::COLOR_GUIDE;
        if (!strcmp(id, "styles")) return voidlight::STYLE_BOOK;
        if (!strcmp(id, "objects")) return voidlight::OBJECT_HELPER_GUIDE;
        if (!strcmp(id, "planner")) return voidlight::LEVEL_PLANNER_GUIDE;
        if (!strcmp(id, "music")) return voidlight::MUSIC_MATCHER_GUIDE;
        return voidlight::DECOR_AI_GUIDE;
    }
}

class $modify(VoidlightEditorUI, EditorUI) {
    struct Fields {
        CCNode* panel = nullptr;
    };

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        auto win = CCDirector::sharedDirector()->getWinSize();
        auto emblem = CCNode::create();
        emblem->setContentSize({48.f, 48.f});
        auto art = CCDrawNode::create();
        art->drawDot({24.f, 24.f}, 23.f, {0.24f, 0.08f, 0.43f, 1.f});
        art->drawDot({24.f, 24.f}, 20.f, {0.43f, 0.22f, 0.68f, 1.f});
        CCPoint starPts[] = {{24,40},{28,28},{40,24},{28,20},{24,8},{20,20},{8,24},{20,28}};
        art->drawPolygon(starPts, 8, {0.96f,0.91f,1.f,1.f}, 1.2f, {0.72f,0.58f,0.91f,1.f});
        emblem->addChild(art);
        auto button = CCMenuItemSpriteExtra::create(emblem, this, menu_selector(VoidlightEditorUI::onVoidlight));
        if (!button) return true;
        button->setID("gigachat-voidlight-editor-button");
        auto menu = CCMenu::create();
        menu->setPosition(CCPointZero);
        menu->addChild(button);
        button->setPosition({win.width - 205.f, win.height - 42.f});
        this->addChild(menu, 1000);
        return true;
    }

    void closeVoidlight(CCObject*) {
        if (m_fields->panel) {
            m_fields->panel->removeFromParentAndCleanup(true);
            m_fields->panel = nullptr;
        }
    }

    void showTool(CCObject* sender) {
        auto item = typeinfo_cast<CCMenuItem*>(sender);
        if (!item) return;
        auto id = item->getTag();
        const char* key = id == 1 ? "glow" : id == 2 ? "color" : id == 3 ? "styles" :
                          id == 4 ? "objects" : id == 5 ? "planner" : "music";
        FLAlertLayer::create("VOIDLIGHT", toolText(key), "OK")->show();
    }

    void onVoidlight(CCObject*) {
        if (m_fields->panel) {
            closeVoidlight(nullptr);
            return;
        }
        auto win = CCDirector::sharedDirector()->getWinSize();
        const float w = 330.f, h = 252.f;
        auto panel = CCNode::create();
        panel->setContentSize({w, h});
        panel->setAnchorPoint({0.f, 0.f});
        auto bg = CCDrawNode::create();
        drawRoundedRect(bg, 0, 0, w, h, 18.f,
                       {0.055f, 0.025f, 0.12f, 0.97f},
                       {0.68f, 0.43f, 0.94f, 1.f});
        drawRoundedRect(bg, 5, 5, w - 10, h - 10, 14.f,
                       {0.10f, 0.055f, 0.19f, 0.35f},
                       {0.35f, 0.20f, 0.52f, 0.8f});
        panel->addChild(bg);

        auto title = CCLabelBMFont::create("VOIDLIGHT", "bigFont.fnt");
        title->setScale(0.48f);
        title->setAnchorPoint({0.f, 0.5f});
        title->setPosition({22.f, h - 27.f});
        panel->addChild(title, 2);
        auto subtitle = CCLabelBMFont::create("DECOR AI  -  EDITOR ASSISTANT", "goldFont.fnt");
        subtitle->setScale(0.28f);
        subtitle->setAnchorPoint({0.f, 0.5f});
        subtitle->setPosition({22.f, h - 49.f});
        panel->addChild(subtitle, 2);

        auto closeLabel = CCLabelBMFont::create("X", "bigFont.fnt");
        closeLabel->setScale(0.42f);
        auto close = CCMenuItemSpriteExtra::create(closeLabel, this, menu_selector(VoidlightEditorUI::closeVoidlight));
        close->setPosition({w - 22.f, h - 25.f});
        auto actions = CCMenu::create();
        actions->setPosition(CCPointZero);
        actions->addChild(close);

        const char* names[] = {"Glow Lab", "Color Lab", "Style Book", "Object Helper", "Level Planner", "Music Matcher"};
        for (int i = 0; i < 6; ++i) {
            float x = (i % 2 == 0) ? 91.f : 244.f;
            float y = h - 91.f - (i / 2) * 48.f;
            auto shape = CCNode::create();
            shape->setContentSize({143.f, 38.f});
            auto buttonBg = CCDrawNode::create();
            drawRoundedRect(buttonBg, 0, 0, 143.f, 38.f, 10.f,
                            {0.25f, 0.16f, 0.39f, 1.f},
                            {0.48f, 0.34f, 0.67f, 1.f});
            shape->addChild(buttonBg);
            auto label = CCLabelBMFont::create(names[i], "bigFont.fnt");
            label->setScale(0.31f);
            label->setPosition({71.5f, 19.f});
            shape->addChild(label);
            auto item = CCMenuItemSpriteExtra::create(shape, this, menu_selector(VoidlightEditorUI::showTool));
            item->setTag(i + 1);
            item->setPosition({x, y});
            actions->addChild(item);
        }
        panel->addChild(actions, 3);

        auto hint = CCLabelBMFont::create("Choose a tool...", "chatFont.fnt");
        hint->setScale(0.48f);
        hint->setAnchorPoint({0.f, 0.5f});
        hint->setPosition({22.f, 18.f});
        panel->addChild(hint, 2);

        panel->setPosition({win.width - w - 16.f, win.height - h - 58.f});
        this->addChild(panel, 1100);
        m_fields->panel = panel;
    }
};

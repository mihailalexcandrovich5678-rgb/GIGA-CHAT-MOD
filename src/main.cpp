#include <Geode/Geode.hpp>
#include <cmath>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

namespace {
    void drawRoundedRect(CCDrawNode* draw, float x, float y, float w, float h, float r,
                         ccColor4F fill, ccColor4F border) {
        CCPoint pts[36]; int n = 0;
        const float centers[4][2] = {{x+w-r,y+r},{x+w-r,y+h-r},{x+r,y+h-r},{x+r,y+r}};
        const float starts[4] = {-90.f,0.f,90.f,180.f};
        for (int c=0;c<4;++c) for(int i=0;i<=8;++i) {
            float a=(starts[c]+i*90.f/8.f)*0.0174532925f;
            pts[n++]=CCPointMake(centers[c][0]+r*std::cos(a),centers[c][1]+r*std::sin(a));
        }
        draw->drawPolygon(pts,n,fill,2.f,border);
    }
}

class $modify(VoidlightEditorUI, EditorUI) {
    struct Fields {
        CCNode* panel = nullptr;
        CCLabelBMFont* selectedStyle = nullptr;
        CCLabelBMFont* previewText = nullptr;
        std::string style = "PURPLE GLOW";
    };

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;
        auto win=CCDirector::sharedDirector()->getWinSize();
        auto emblem=CCNode::create(); emblem->setContentSize({48.f,48.f});
        auto art=CCDrawNode::create();
        art->drawDot({24.f,24.f},23.f,{0.20f,0.06f,0.38f,1.f});
        art->drawDot({24.f,24.f},20.f,{0.43f,0.22f,0.68f,1.f});
        CCPoint star[]={{24,40},{28,28},{40,24},{28,20},{24,8},{20,20},{8,24},{20,28}};
        art->drawPolygon(star,8,{0.96f,0.91f,1.f,1.f},1.2f,{0.72f,0.58f,0.91f,1.f});
        emblem->addChild(art);
        auto button=CCMenuItemSpriteExtra::create(emblem,this,menu_selector(VoidlightEditorUI::onAutoDecor));
        if(!button) return true;
        button->setID("gigachat-voidlight-auto-decor-button");
        auto menu=CCMenu::create(); menu->setPosition(CCPointZero); menu->addChild(button);
        button->setPosition({win.width-86.f,win.height-42.f});
        this->addChild(menu,1000);
        return true;
    }

    void closePanel(CCObject*) {
        if(m_fields->panel) {
            m_fields->panel->removeFromParentAndCleanup(true);
            m_fields->panel=nullptr;
            m_fields->selectedStyle=nullptr;
            m_fields->previewText=nullptr;
        }
    }

    void buildDecor() {
        if (!m_editorLayer) return;

        // Build a compact, non-destructive motif just beyond the current level end.
        // createObject(..., false) keeps the additions in the editor undo history.
        static const char* purple[] = {
            "...###...", "..#...#..", ".#.....#.", "#.......#",
            "#...#...#", "#.......#", ".#.....#.", "..#...#..", "...###..."
        };
        static const char* minimal[] = {
            "....#....", "...###...", "..#.#.#..", ".#..#..#.",
            "....#....", "....#....", "....#...."
        };
        static const char* modern[] = {
            "########.", "#......#.", "#.####.#.", "#.#..#.#.",
            "#.####.#.", "#......#.", "########."
        };
        static const char* cyber[] = {
            "#.......#", ".#.....#..", "..#...#...", "...###....",
            "...###....", "..#...#...", ".#.....#..", "#.......#"
        };

        const char** pattern = purple;
        int rows = 9;
        ccColor3B mainColor(150, 95, 235);
        ccColor3B glowColor(195, 135, 255);
        if (m_fields->style == "MINIMAL") {
            pattern = minimal; rows = 7;
            mainColor = ccColor3B(185, 185, 205);
            glowColor = ccColor3B(210, 210, 225);
        } else if (m_fields->style == "MODERN") {
            pattern = modern; rows = 7;
            mainColor = ccColor3B(90, 165, 220);
            glowColor = ccColor3B(135, 205, 255);
        } else if (m_fields->style == "CYBER") {
            pattern = cyber; rows = 8;
            mainColor = ccColor3B(55, 220, 205);
            glowColor = ccColor3B(100, 255, 235);
        }

        float startX = m_editorLayer->getLastObjectX() + 90.f;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; pattern[row][col] != '\0'; ++col) {
                if (pattern[row][col] != '#') continue;
                auto object = m_editorLayer->createObject(
                    1, CCPointMake(startX + col * 30.f, 90.f + (rows - row - 1) * 30.f), false
                );
                if (!object) continue;
                object->setObjectColor(mainColor);
                object->setGlowColor(glowColor);
            }
        }
    }

    void setStyle(std::string const& style, CCObject*) {
        m_fields->style = style;
        buildDecor();
        closePanel(nullptr);
    }

    void onPurple(CCObject* sender) { setStyle("PURPLE GLOW",sender); }
    void onMinimal(CCObject* sender) { setStyle("MINIMAL",sender); }
    void onModern(CCObject* sender) { setStyle("MODERN",sender); }
    void onCyber(CCObject* sender) { setStyle("CYBER",sender); }

    void onPreview(CCObject*) {
        auto message = std::string("Style: <cy>") + m_fields->style +
            "</c>\n\nThis is the first interface build. It previews a decoration direction only; it does not change or delete anything in your level. The next step is the real object-placement generator with confirmation and undo.";
        FLAlertLayer::create("AUTO DECOR PREVIEW", message.c_str(), "OK")->show();
    }

    void onAutoDecor(CCObject*) {
        if(m_fields->panel) { closePanel(nullptr); return; }
        auto win=CCDirector::sharedDirector()->getWinSize();
        const float w=360.f,h=270.f,scale=0.78f;
        auto panel=CCNode::create(); panel->setContentSize({w,h}); panel->setAnchorPoint({0,0});
        auto bg=CCDrawNode::create();
        drawRoundedRect(bg,0,0,w,h,18.f,{0.055f,0.025f,0.12f,0.98f},{0.68f,0.43f,0.94f,1.f});
        drawRoundedRect(bg,5,5,w-10,h-10,14.f,{0.10f,0.055f,0.19f,0.25f},{0.35f,0.20f,0.52f,0.8f});
        panel->addChild(bg);

        auto title=CCLabelBMFont::create("AUTO DECOR","bigFont.fnt");
        title->setScale(0.52f); title->setAnchorPoint({0,0.5f}); title->setPosition({18,h-24}); panel->addChild(title);
        auto closeLabel=CCLabelBMFont::create("X","bigFont.fnt"); closeLabel->setScale(0.42f);
        auto close=CCMenuItemSpriteExtra::create(closeLabel,this,menu_selector(VoidlightEditorUI::closePanel));
        close->setPosition({w-20,h-23});
        auto previewLabel=CCLabelBMFont::create("PREVIEW","bigFont.fnt"); previewLabel->setScale(0.38f);
        auto preview=CCMenuItemSpriteExtra::create(previewLabel,this,menu_selector(VoidlightEditorUI::onPreview));
        preview->setPosition({w-48,25});
        auto purpleLabel=CCLabelBMFont::create("PURPLE","bigFont.fnt"); purpleLabel->setScale(0.34f);
        auto purple=CCMenuItemSpriteExtra::create(purpleLabel,this,menu_selector(VoidlightEditorUI::onPurple));
        purple->setPosition({55,h-76});
        auto minimalLabel=CCLabelBMFont::create("MINIMAL","bigFont.fnt"); minimalLabel->setScale(0.34f);
        auto minimal=CCMenuItemSpriteExtra::create(minimalLabel,this,menu_selector(VoidlightEditorUI::onMinimal));
        minimal->setPosition({145,h-76});
        auto modernLabel=CCLabelBMFont::create("MODERN","bigFont.fnt"); modernLabel->setScale(0.34f);
        auto modern=CCMenuItemSpriteExtra::create(modernLabel,this,menu_selector(VoidlightEditorUI::onModern));
        modern->setPosition({235,h-76});
        auto cyberLabel=CCLabelBMFont::create("CYBER","bigFont.fnt"); cyberLabel->setScale(0.34f);
        auto cyber=CCMenuItemSpriteExtra::create(cyberLabel,this,menu_selector(VoidlightEditorUI::onCyber));
        cyber->setPosition({315,h-76});
        auto actions=CCMenu::create(); actions->setPosition(CCPointZero);
        actions->addChild(close); actions->addChild(preview);
        actions->addChild(purple); actions->addChild(minimal); actions->addChild(modern); actions->addChild(cyber);
        panel->addChild(actions,4);

        auto style=CCLabelBMFont::create("STYLE: PURPLE GLOW","bigFont.fnt");
        style->setScale(0.42f); style->setAnchorPoint({0,0.5f}); style->setPosition({18,h-112});
        panel->addChild(style,3); m_fields->selectedStyle=style;
        auto description=CCLabelBMFont::create("Dark blocks, lavender frames and soft violet light.","chatFont.fnt");
        description->setAnchorPoint({0,1}); description->setPosition({18,h-137});
        description->setWidth(320.f); description->setScale(0.58f);
        panel->addChild(description,3); m_fields->previewText=description;
        auto note=CCLabelBMFont::create("SELECT A STYLE TO BUILD DECOR","chatFont.fnt");
        note->setScale(0.40f); note->setAnchorPoint({0,0.5f}); note->setPosition({18,25});
        panel->addChild(note,3);
        panel->setScale(scale); panel->setPosition({win.width-w*scale-18.f,win.height-h*scale-58.f});
        this->addChild(panel,1100); m_fields->panel=panel;
    }
};

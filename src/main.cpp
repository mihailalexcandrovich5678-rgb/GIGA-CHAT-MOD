#include <Geode/Geode.hpp>
#include <cmath>
#include <algorithm>
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

        // Auto Decor v2: build a wide, layered geometric structure.
        // It adds new objects after the current level content and never removes existing ones.
        const float x = m_editorLayer->getLastObjectX() + 120.f;
        const float y = 105.f;
        const float w = 560.f, h = 210.f;
        float step = 24.f, scale = 0.72f;
        ccColor3B main(150, 95, 235), glow(195, 135, 255);
        ccColor3B bright(225, 195, 255), shadow(95, 55, 155);

        if (m_fields->style == "MINIMAL") {
            main = ccColor3B(185,185,205); glow = ccColor3B(215,215,230);
            bright = ccColor3B(245,245,255); shadow = ccColor3B(115,115,145);
            step = 28.f; scale = 0.62f;
        } else if (m_fields->style == "MODERN") {
            main = ccColor3B(75,145,210); glow = ccColor3B(125,195,250);
            bright = ccColor3B(205,235,255); shadow = ccColor3B(45,85,135);
            step = 25.f; scale = 0.70f;
        } else if (m_fields->style == "CYBER") {
            main = ccColor3B(40,205,190); glow = ccColor3B(90,255,235);
            bright = ccColor3B(195,255,245); shadow = ccColor3B(25,105,115);
            step = 23.f; scale = 0.68f;
        }

        auto put = [&](float px, float py, float s, ccColor3B c, float angle = 0.f) {
            auto obj = m_editorLayer->createObject(1, CCPointMake(px, py), false);
            if (!obj) return;
            obj->setScale(s);
            obj->setRotation(angle);
            obj->setObjectColor(c);
            obj->setGlowColor(glow);
        };
        auto line = [&](float x1, float y1, float x2, float y2, float spacing,
                        float s, ccColor3B c) {
            float dx=x2-x1, dy=y2-y1;
            float len=std::sqrt(dx*dx+dy*dy);
            int n=std::max(1, static_cast<int>(len/spacing));
            float angle=std::atan2(dy,dx)*57.2957795f;
            for(int i=0;i<=n;++i) {
                float t=static_cast<float>(i)/static_cast<float>(n);
                put(x1+dx*t,y1+dy*t,s,c,angle);
            }
        };
        auto frame = [&](float px,float py,float fw,float fh,float spacing,
                         float s,ccColor3B c) {
            line(px,py,px+fw,py,spacing,s,c);
            line(px,py+fh,px+fw,py+fh,spacing,s,c);
            line(px,py,px,py+fh,spacing,s,c);
            line(px+fw,py,px+fw,py+fh,spacing,s,c);
        };
        auto diamond = [&](float cx,float cy,float r,float s,ccColor3B c) {
            line(cx,cy+r,cx+r,cy,step,s,c);
            line(cx+r,cy,cx,cy-r,step,s,c);
            line(cx,cy-r,cx-r,cy,step,s,c);
            line(cx-r,cy,cx,cy+r,step,s,c);
        };

        // Large outer frame and a second inset frame create depth.
        frame(x,y,w,h,step,scale,main);
        frame(x+18.f,y+18.f,w-36.f,h-36.f,step*1.4f,scale*0.52f,shadow);

        // Double side pillars, caps and inner supports.
        for(int side=0;side<2;++side) {
            float px=x+(side==0?42.f:w-42.f);
            line(px,y+28.f,px,y+h-28.f,step,scale*1.05f,bright);
            line(px-14.f,y+28.f,px-14.f,y+h-28.f,step,scale*0.45f,shadow);
            line(px+14.f,y+28.f,px+14.f,y+h-28.f,step,scale*0.45f,shadow);
            line(px-28.f,y+24.f,px+28.f,y+24.f,step,scale*0.8f,bright);
            line(px-28.f,y+h-24.f,px+28.f,y+h-24.f,step,scale*0.8f,bright);
        }

        // Three framed panels and a central diamond emblem.
        frame(x+105.f,y+42.f,105.f,126.f,step,scale*0.58f,shadow);
        frame(x+350.f,y+42.f,105.f,126.f,step,scale*0.58f,shadow);
        frame(x+228.f,y+30.f,104.f,150.f,step,scale*0.78f,bright);
        const float cx=x+w*0.5f, cy=y+h*0.5f;
        diamond(cx,cy,58.f,scale*0.9f,main);
        diamond(cx,cy,34.f,scale*0.58f,bright);
        line(cx-20.f,cy,cx+20.f,cy,step*0.7f,scale,bright);
        line(cx,cy-20.f,cx,cy+20.f,step*0.7f,scale,bright);

        // Each style gets its own finishing details.
        if(m_fields->style=="MINIMAL") {
            line(x+80.f,y+12.f,x+w-80.f,y+12.f,step*2.f,scale*0.4f,bright);
            line(x+80.f,y+h-12.f,x+w-80.f,y+h-12.f,step*2.f,scale*0.4f,bright);
        } else if(m_fields->style=="MODERN") {
            for(int i=0;i<5;++i) {
                float px=x+145.f+i*68.f;
                line(px,y+18.f,px,y+34.f,step,scale*0.42f,bright);
                line(px,y+h-34.f,px,y+h-18.f,step,scale*0.42f,bright);
            }
        } else if(m_fields->style=="CYBER") {
            for(int i=0;i<6;++i) {
                float px=x+90.f+i*76.f;
                line(px,y+42.f,px+18.f,y+60.f,step,scale*0.7f,bright);
                line(px+18.f,y+60.f,px+18.f,y+78.f,step,scale*0.5f,shadow);
                put(px+18.f,y+78.f,scale*0.9f,bright,45.f);
            }
        } else {
            diamond(x+70.f,cy,22.f,scale*0.7f,bright);
            diamond(x+w-70.f,cy,22.f,scale*0.7f,bright);
            line(x+95.f,cy,x+205.f,cy,step*1.2f,scale*0.36f,shadow);
            line(x+w-205.f,cy,x+w-95.f,cy,step*1.2f,scale*0.36f,shadow);
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

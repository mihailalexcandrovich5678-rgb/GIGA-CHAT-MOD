#include <Geode/Geode.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/web.hpp>
#include <cmath>
#include <Geode/modify/EditorUI.hpp>
#include "decor_ai.hpp"

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
        geode::TextInput* input = nullptr;
        CCLabelBMFont* conversation = nullptr;
        async::TaskHolder<web::WebResponse> request;
        bool waiting = false;
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
        auto button=CCMenuItemSpriteExtra::create(emblem,this,menu_selector(VoidlightEditorUI::onVoidlight));
        if(!button) return true;
        button->setID("gigachat-voidlight-editor-button");
        auto menu=CCMenu::create(); menu->setPosition(CCPointZero); menu->addChild(button);
        // Keep the round launcher beside the upper-right editor controls.
        button->setPosition({win.width-86.f,win.height-42.f});
        this->addChild(menu,1000);
        return true;
    }

    void closeVoidlight(CCObject*) {
        if(m_fields->panel){m_fields->panel->removeFromParentAndCleanup(true);m_fields->panel=nullptr;m_fields->input=nullptr;m_fields->conversation=nullptr;}
    }

    void appendConversation(std::string const& text) {
        if(!m_fields->conversation) return;
        m_fields->conversation->setString(text.c_str());
    }

    void sendMessage(CCObject*) {
        if(m_fields->waiting) { appendConversation("VOIDLIGHT\nPlease wait for the current answer..."); return; }
        if(!m_fields->input) return;
        auto message=m_fields->input->getString();
        if(message.empty()) { appendConversation("VOIDLIGHT\nWrite a question first."); return; }
        if(message.size()>4000) { appendConversation("VOIDLIGHT\nMessage is too long (max 4000 characters)."); return; }
        auto endpoint=Mod::get()->getSettingValue<std::string>("server-url");
        if(endpoint.empty()) {
            appendConversation("VOIDLIGHT\nServer URL is not configured yet. Deploy the Worker, then paste its URL in Geode mod settings.");
            return;
        }
        while(!endpoint.empty() && endpoint.back()=='/') endpoint.pop_back();
        auto req=web::WebRequest();
        req.header("Content-Type","application/json");
        req.timeout(std::chrono::seconds(45));
        matjson::Value body=matjson::Value::object();
        body["message"]=message;
        req.bodyJSON(body);
        m_fields->waiting=true;
        appendConversation(("YOU\n"+message+"\n\nVOIDLIGHT\nThinking...").c_str());
        m_fields->input->setString("");
        m_fields->request.spawn(
            req.post(endpoint+"/chat"),
            [this](web::WebResponse response) {
                m_fields->waiting=false;
                if(!m_fields->panel) return;
                if(!response.ok()) {
                    appendConversation("VOIDLIGHT\nCould not reach the AI server. Check the Worker URL, deployment and connection.");
                    return;
                }
                auto parsed=response.json();
                if(!parsed) { appendConversation("VOIDLIGHT\nThe server returned invalid data."); return; }
                auto answer=parsed.unwrap()["answer"].asString();
                if(!answer) { appendConversation("VOIDLIGHT\nThe server did not return an answer."); return; }
                appendConversation(("YOU\n"+message+"\n\nVOIDLIGHT\n"+answer.unwrap()).c_str());
            }
        );
    }

    void onVoidlight(CCObject*) {
        if(m_fields->panel){closeVoidlight(nullptr);return;}
        auto win=CCDirector::sharedDirector()->getWinSize();
        const float w=360.f,h=270.f,scale=0.78f;
        auto panel=CCNode::create(); panel->setContentSize({w,h}); panel->setAnchorPoint({0,0});
        auto bg=CCDrawNode::create();
        drawRoundedRect(bg,0,0,w,h,18.f,{0.055f,0.025f,0.12f,0.98f},{0.68f,0.43f,0.94f,1.f});
        drawRoundedRect(bg,5,5,w-10,h-10,14.f,{0.10f,0.055f,0.19f,0.25f},{0.35f,0.20f,0.52f,0.8f});
        panel->addChild(bg);
        auto title=CCLabelBMFont::create("VOIDLIGHT AI","bigFont.fnt");
        title->setScale(0.52f); title->setAnchorPoint({0,0.5f}); title->setPosition({18,h-24}); panel->addChild(title);
        auto closeLabel=CCLabelBMFont::create("X","bigFont.fnt"); closeLabel->setScale(0.42f);
        auto close=CCMenuItemSpriteExtra::create(closeLabel,this,menu_selector(VoidlightEditorUI::closeVoidlight));
        close->setPosition({w-20,h-23});
        auto sendLabel=CCLabelBMFont::create("SEND","bigFont.fnt"); sendLabel->setScale(0.38f);
        auto send=CCMenuItemSpriteExtra::create(sendLabel,this,menu_selector(VoidlightEditorUI::sendMessage));
        send->setPosition({w-48,27});
        auto actions=CCMenu::create(); actions->setPosition(CCPointZero); actions->addChild(close); actions->addChild(send); panel->addChild(actions,4);
        auto history=CCLabelBMFont::create("Ask me about Geometry Dash decoration, glow, colors or level planning.","chatFont.fnt");
        history->setAnchorPoint({0,1}); history->setPosition({18,h-55}); history->setWidth(315.f); history->setScale(0.58f);
        panel->addChild(history,3); m_fields->conversation=history;
        auto input=geode::TextInput::create(255.f,"Write your question...","bigFont.fnt");
        input->setMaxCharCount(4000); input->setCommonFilter(geode::CommonFilter::Any);
        input->setPosition({145.f,27.f}); panel->addChild(input,5); m_fields->input=input;
        auto hint=CCLabelBMFont::create("AI connection requires a deployed server URL.","chatFont.fnt");
        hint->setScale(0.42f); hint->setAnchorPoint({0,0.5f}); hint->setPosition({18,10}); panel->addChild(hint,3);
        panel->setScale(scale); panel->setPosition({win.width-w*scale-18.f,win.height-h*scale-58.f});
        this->addChild(panel,1100); m_fields->panel=panel;
    }
};

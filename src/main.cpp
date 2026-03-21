#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

static bool g_invisibleMode = false;

static void applyOpacityToChildren(CCNode* node, GLubyte opacity) {
    if (!node) return;

    for (auto child : CCArrayExt<CCNode*>(node->getChildren())) {
        if (typeinfo_cast<PlayerObject*>(child)) continue;

        if (auto rgba = dynamic_cast<CCRGBAProtocol*>(child)) {
            rgba->setOpacity(opacity);
        }
        applyOpacityToChildren(child, opacity);
    }
}

class $modify(InvisibleObjectsMod, PlayLayer) {
    struct Fields {
        CCLabelBMFont* buttonLabel = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto label = CCLabelBMFont::create("EYE:OFF", "bigFont.fnt");
        label->setScale(0.35f);
        m_fields->buttonLabel = label;

        auto bg = CCScale9Sprite::create("square02_small.png");
        bg->setContentSize({80.f, 24.f});
        bg->setOpacity(180);

        auto item = CCMenuItemSpriteExtra::create(
            bg, nullptr,
            this,
            menu_selector(InvisibleObjectsMod::onToggleButton)
        );

        label->setPosition({40.f, 12.f});
        bg->addChild(label);

        auto menu = CCMenu::createWithItem(item);
        menu->setPosition({50.f, winSize.height - 25.f});
        this->addChild(menu, 9999);

        return true;
    }

    // NEW: Force every frame so GD can't reset it
    void update(float dt) override {
        PlayLayer::update(dt);

        if (g_invisibleMode && m_objectLayer) {
            applyInvisible(true);
        }
    }

    void applyInvisible(bool invisible) {
        if (!m_objectLayer) return;

        GLubyte op = invisible ? 0 : 255;
        applyOpacityToChildren(m_objectLayer, op);

        // Players always fully visible
        if (m_player1) {
            m_player1->setOpacity(255);
            applyOpacityToChildren(m_player1, 255);
        }
        if (m_player2) {
            m_player2->setOpacity(255);
            applyOpacityToChildren(m_player2, 255);
        }
    }

    void onToggleButton(CCObject*) {
        g_invisibleMode = !g_invisibleMode;
        applyInvisible(g_invisibleMode);

        if (m_fields->buttonLabel) {
            m_fields->buttonLabel->setString(g_invisibleMode ? "EYE:ON" : "EYE:OFF");
        }

        if (g_invisibleMode) {
            Notification::create("Invisible Mode ON - Good luck!", NotificationIcon::Warning)->show();
        } else {
            Notification::create("Invisible Mode OFF - Objects visible", NotificationIcon::Success)->show();
        }
    }

    void addObject(GameObject* obj) override {
        PlayLayer::addObject(obj);
        if (g_invisibleMode && obj) {
            if (!typeinfo_cast<PlayerObject*>(obj)) {
                if (auto rgba = dynamic_cast<CCRGBAProtocol*>(obj)) rgba->setOpacity(0);
                applyOpacityToChildren(obj, 0);
            }
        }
    }

    void resetLevel() override {
        PlayLayer::resetLevel();
        if (g_invisibleMode) {
            applyInvisible(true);
        }
    }

    void onQuit() override {
        g_invisibleMode = false;
        PlayLayer::onQuit();
    }
};

$on_mod(Loaded) {
    log::info("Invisible Objects Mod loaded (now with per-frame force!)");
}

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

static bool g_invisibleMode = false;

class $modify(InvisibleObjectsMod, PlayLayer) {
    struct Fields {
        CCLabelBMFont* buttonLabel = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto label = CCLabelBMFont::create("EYE:OFF", "bigFont.fnt");
        label->setScale(0.35f);
        m_fields->buttonLabel = label;

        auto bg = CCScale9Sprite::create("square02_small.png");
        bg->setContentSize({ 80.f, 24.f });
        bg->setOpacity(180);

        auto item = CCMenuItemSpriteExtra::create(
            bg, this,
            menu_selector(InvisibleObjectsMod::onToggleButton)
        );

        label->setPosition({ 40.f, 12.f });
        bg->addChild(label);

        auto menu = CCMenu::create();
        menu->addChild(item);
        menu->setPosition({ 50.f, winSize.height - 25.f });
        this->addChild(menu, 9999);

        return true;
    }

    // ---------------------------------------------------------------
    //  Loop through objectLayer children
    //  Hide everything EXCEPT the player — player stays in place!
    // ---------------------------------------------------------------
    void applyToObjectLayer(bool invisible) {
        if (!m_objectLayer) return;

        for (auto child : CCArrayExt<CCNode*>(m_objectLayer->getChildren())) {
            // Skip PlayerObject — always keep player visible
            if (typeinfo_cast<PlayerObject*>(child)) {
                child->setVisible(true);
                continue;
            }
            child->setVisible(!invisible);
        }

        // Always force players visible just in case
        if (m_player1) m_player1->setVisible(true);
        if (m_player2) m_player2->setVisible(true);
    }

    void onToggleButton(CCObject*) {
        g_invisibleMode = !g_invisibleMode;

        applyToObjectLayer(g_invisibleMode);

        if (m_fields->buttonLabel) {
            m_fields->buttonLabel->setString(
                g_invisibleMode ? "EYE:ON " : "EYE:OFF"
            );
        }

        if (g_invisibleMode) {
            Notification::create(
                "Invisible Mode ON - Good luck!",
                NotificationIcon::Warning
            )->show();
        } else {
            Notification::create(
                "Invisible Mode OFF - Objects visible",
                NotificationIcon::Success
            )->show();
        }

        log::info("Invisible mode toggled");
    }

    // Re-hide after new objects stream in
    void addObject(GameObject* obj) {
        PlayLayer::addObject(obj);
        if (g_invisibleMode && obj && !typeinfo_cast<PlayerObject*>(obj)) {
            obj->setVisible(false);
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        if (g_invisibleMode) {
            applyToObjectLayer(true);
        }
    }

    void onQuit() {
        g_invisibleMode = false;
        PlayLayer::onQuit();
    }
};

$on_mod(Loaded) {
    log::info("Level Invisible Mod loaded");
}

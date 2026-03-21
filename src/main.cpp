#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

static bool g_invisibleMode = false;

class $modify(InvisibleObjectsMod, PlayLayer) {
    struct Fields {
        CCLabelBMFont* buttonLabel = nullptr;
        CCNode* playerParent1 = nullptr;
        CCNode* playerParent2 = nullptr;
        int playerZ1 = 0;
        int playerZ2 = 0;
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

    void hideObjects() {
        // Pull player1 out of objectLayer, attach directly to PlayLayer
        if (m_player1) {
            m_fields->playerParent1 = m_player1->getParent();
            m_fields->playerZ1 = m_player1->getZOrder();
            m_player1->retain();
            m_player1->removeFromParentAndCleanup(false);
            this->addChild(m_player1, 9000);
            m_player1->release();
        }
        if (m_player2) {
            m_fields->playerParent2 = m_player2->getParent();
            m_fields->playerZ2 = m_player2->getZOrder();
            m_player2->retain();
            m_player2->removeFromParentAndCleanup(false);
            this->addChild(m_player2, 9001);
            m_player2->release();
        }

        // Only hide objectLayer — ground and background stay visible
        if (m_objectLayer) {
            m_objectLayer->setVisible(false);
        }
    }

    void showObjects() {
        // Show objectLayer again
        if (m_objectLayer) {
            m_objectLayer->setVisible(true);
        }

        // Move players back to their original parent
        if (m_player1 && m_fields->playerParent1) {
            m_player1->retain();
            m_player1->removeFromParentAndCleanup(false);
            m_fields->playerParent1->addChild(m_player1, m_fields->playerZ1);
            m_player1->release();
        }
        if (m_player2 && m_fields->playerParent2) {
            m_player2->retain();
            m_player2->removeFromParentAndCleanup(false);
            m_fields->playerParent2->addChild(m_player2, m_fields->playerZ2);
            m_player2->release();
        }
    }

    void onToggleButton(CCObject*) {
        g_invisibleMode = !g_invisibleMode;

        if (g_invisibleMode) {
            hideObjects();
        } else {
            showObjects();
        }

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

    void resetLevel() {
        if (g_invisibleMode) showObjects();
        PlayLayer::resetLevel();
        if (g_invisibleMode) hideObjects();
    }

    void onQuit() {
        if (g_invisibleMode) {
            showObjects();
            g_invisibleMode = false;
        }
        PlayLayer::onQuit();
    }
};

$on_mod(Loaded) {
    log::info("Level Invisible Mod loaded");
}

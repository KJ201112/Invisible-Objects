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

        // Button label
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
        this->addChild(menu, 999);

        return true;
    }

    void onToggleButton(CCObject*) {
        g_invisibleMode = !g_invisibleMode;

        // Hide/show the entire object layer at once
        // This is the most reliable method — one call hides everything
        if (m_objectLayer) {
            m_objectLayer->setVisible(!g_invisibleMode);
        }

        // Move player ABOVE the object layer so it's always visible
        if (m_player1) {
            m_player1->setZOrder(m_objectLayer ? m_objectLayer->getZOrder() + 10 : 10);
        }
        if (m_player2) {
            m_player2->setZOrder(m_objectLayer ? m_objectLayer->getZOrder() + 10 : 10);
        }

        // Update button text
        if (m_fields->buttonLabel) {
            m_fields->buttonLabel->setString(
                g_invisibleMode ? "EYE:ON " : "EYE:OFF"
            );
        }

        // Notification
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

    void onQuit() {
        g_invisibleMode = false;
        // Restore object layer visibility when leaving
        if (m_objectLayer) {
            m_objectLayer->setVisible(true);
        }
        PlayLayer::onQuit();
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        // Keep state after death
        if (m_objectLayer) {
            m_objectLayer->setVisible(!g_invisibleMode);
        }
        // Keep player visible
        if (m_player1 && m_objectLayer) {
            m_player1->setZOrder(m_objectLayer->getZOrder() + 10);
        }
    }
};

$on_mod(Loaded) {
    log::info("Level Invisible Mod loaded");
}

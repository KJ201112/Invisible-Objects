// =============================================================================
//  Level Invisible Mod — Geode Mod for Geometry Dash (Android)
//  Author  : KJ  |  Version : v1.0.0
//
//  HOW TO USE: Tap the "EYE:OFF" button in the top-left corner.
//
//  HOW IT WORKS:
//  A black overlay covers the entire screen BETWEEN the objects and
//  the player icon. Objects are hidden behind it, but your cube/ship
//  is drawn on top so you can still see yourself.
// =============================================================================

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

static bool g_invisible = false;

void showNotification(bool invisible) {
    if (invisible) {
        Notification::create(
            "Invisible Mode ON - Good luck!",
            NotificationIcon::Warning
        )->show();
    } else {
        Notification::create(
            "Invisible Mode OFF - Objects visible again",
            NotificationIcon::Success
        )->show();
    }
}

class $modify(InvisibleObjectsMod, PlayLayer) {

    struct Fields {
        CCLabelBMFont* buttonLabel = nullptr;
        CCLayerColor* overlay = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // ---------------------------------------------------------------
        //  BLACK OVERLAY
        //  zOrder 10 = above all level objects (blocks/spikes/portals)
        //  zOrder below player so your icon stays visible on top
        //  The player is typically at zOrder 100+ in PlayLayer
        // ---------------------------------------------------------------
        auto overlay = CCLayerColor::create(
            ccc4(0, 0, 0, 255), // solid black
            winSize.width,
            winSize.height
        );
        overlay->setVisible(false); // start hidden
        overlay->setZOrder(10);     // above objects, below player UI
        this->addChild(overlay, 10);
        m_fields->overlay = overlay;

        // ---------------------------------------------------------------
        //  EYE BUTTON — top left corner
        // ---------------------------------------------------------------
        auto label = CCLabelBMFont::create("EYE:OFF", "bigFont.fnt");
        label->setScale(0.35f);
        m_fields->buttonLabel = label;

        auto bg = CCScale9Sprite::create("square02_small.png");
        bg->setContentSize({70, 22});
        bg->setOpacity(200);
        bg->setColor({0, 0, 0});

        auto btn = CCMenuItemSpriteExtra::create(
            bg, this,
            menu_selector(InvisibleObjectsMod::onToggleButton)
        );

        label->setPosition({35, 11});
        bg->addChild(label);

        auto menu = CCMenu::create();
        menu->addChild(btn);
        menu->setPosition({40, winSize.height - 20});
        this->addChild(menu, 999); // always on top
        return true;
    }

    void onToggleButton(CCObject*) {
        g_invisible = !g_invisible;

        // Show or hide the black overlay
        if (m_fields->overlay) {
            m_fields->overlay->setVisible(g_invisible);
        }

        // Update button text
        if (m_fields->buttonLabel) {
            m_fields->buttonLabel->setString(
                g_invisible ? "EYE:ON " : "EYE:OFF"
            );
        }

        showNotification(g_invisible);
        log::info("Invisible toggled");
    }

    void onQuit() {
        g_invisible = false;
        if (m_fields->overlay) {
            m_fields->overlay->setVisible(false);
        }
        PlayLayer::onQuit();
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        // Keep overlay state after death
        if (m_fields->overlay) {
            m_fields->overlay->setVisible(g_invisible);
        }
    }
};

$on_mod(Loaded) {
    log::info("Level Invisible Mod loaded! Tap EYE button in a level.");
}

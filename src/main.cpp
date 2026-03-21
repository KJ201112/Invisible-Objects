// =============================================================================
//  Level Invisible Mod — Geode Mod for Geometry Dash (Android)
//  Author  : KJ  |  Version : v1.0.0
//
//  HOW TO USE: Tap the "EYE:OFF" button in the top-left corner
//  while playing any level to toggle all objects invisible/visible.
//
//  FIX: We hide m_objectLayer (the entire object container) every frame
//  in update() so GD can't override our visibility change.
// =============================================================================

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

static bool g_invisible = false;

// =============================================================================
//  Show notification banner
// =============================================================================
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
    log::info("Invisible mode toggled");
}

// =============================================================================
//  PlayLayer Hook
// =============================================================================
class $modify(InvisibleObjectsMod, PlayLayer) {

    struct Fields {
        CCLabelBMFont* buttonLabel = nullptr;
    };

    // -------------------------------------------------------------------------
    //  init — adds the toggle button when the level loads
    // -------------------------------------------------------------------------
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Button label
        auto label = CCLabelBMFont::create("EYE:OFF", "bigFont.fnt");
        label->setScale(0.35f);
        m_fields->buttonLabel = label;

        // Dark background
        auto bg = CCScale9Sprite::create("square02_small.png");
        bg->setContentSize({70, 22});
        bg->setOpacity(180);
        bg->setColor({0, 0, 0});

        // Button
        auto btn = CCMenuItemSpriteExtra::create(
            bg, this,
            menu_selector(InvisibleObjectsMod::onToggleButton)
        );

        label->setPosition({35, 11});
        bg->addChild(label);

        auto menu = CCMenu::create();
        menu->addChild(btn);
        menu->setPosition({40, winSize.height - 20});

        // zOrder 999 keeps button above everything
        this->addChild(menu, 999);

        return true;
    }

    // -------------------------------------------------------------------------
    //  update — called every frame
    //  We hide/show the object layer here so GD can't override it
    // -------------------------------------------------------------------------
    void update(float dt) {
        PlayLayer::update(dt);

        // m_objectLayer contains ALL visual objects in the level
        // Hiding it every frame ensures nothing overrides our toggle
        if (m_objectLayer) {
            m_objectLayer->setVisible(!g_invisible);
        }
    }

    // -------------------------------------------------------------------------
    //  onToggleButton — tap to toggle
    // -------------------------------------------------------------------------
    void onToggleButton(CCObject*) {
        g_invisible = !g_invisible;

        // Update button text
        if (m_fields->buttonLabel) {
            m_fields->buttonLabel->setString(
                g_invisible ? "EYE:ON " : "EYE:OFF"
            );
        }

        showNotification(g_invisible);
    }

    // -------------------------------------------------------------------------
    //  onQuit — reset when leaving
    // -------------------------------------------------------------------------
    void onQuit() {
        g_invisible = false;
        // Make sure objects are visible again before quitting
        if (m_objectLayer) {
            m_objectLayer->setVisible(true);
        }
        PlayLayer::onQuit();
    }
};

$on_mod(Loaded) {
    log::info("Level Invisible Mod loaded! Tap EYE button in a level.");
}

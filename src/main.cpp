// =============================================================================
//  Level Invisible Mod — Geode Mod for Geometry Dash (Android)
//  Author  : KJ
//  Version : v1.0.0
//
//  HOW TO USE
//  ----------
//  A small "👁 ON/OFF" button appears in the top-left corner while
//  playing any level. Tap it to toggle all objects invisible/visible.
// =============================================================================

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Global toggle state
static bool g_invisible = false;

// =============================================================================
//  Apply invisibility to all objects in the level
// =============================================================================
void applyInvisibility(PlayLayer* pl, bool invisible) {
    if (!pl || !pl->m_objects) return;

    GLubyte opacity = invisible ? 0 : 255;
    CCArray* objects = pl->m_objects;

    for (int i = 0; i < objects->count(); i++) {
        auto* obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        if (obj) {
            obj->setOpacity(opacity);
        }
    }
}

// =============================================================================
//  Show a notification banner on screen
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
//  We add an on-screen button when the level starts (init)
//  and update its label when tapped.
// =============================================================================
class $modify(InvisibleObjectsMod, PlayLayer) {

    // Store pointer to the button label so we can update its text
    struct Fields {
        CCLabelBMFont* buttonLabel = nullptr;
    };

    // -------------------------------------------------------------------------
    //  init — called when the level loads
    //  We create a small button and place it in the top-left corner
    // -------------------------------------------------------------------------
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        // Run original init first — MUST return false if it fails
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        // Get screen size so we can position the button correctly
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // --- Create the button label (the text shown on the button) ---
        auto label = CCLabelBMFont::create("EYE:OFF", "bigFont.fnt");
        label->setScale(0.35f);
        m_fields->buttonLabel = label;

        // --- Create a dark background box behind the label ---
        auto bg = CCScale9Sprite::create("square02_small.png");
        bg->setContentSize({70, 22});
        bg->setOpacity(160);
        bg->setColor({0, 0, 0});

        // --- Wrap label + bg into a menu item ---
        // CCMenuItemSpriteExtra is Geode's button class
        auto btn = CCMenuItemSpriteExtra::create(
            bg,
            this,
            menu_selector(InvisibleObjectsMod::onToggleButton)
        );

        // Put the label on top of the background
        label->setPosition({35, 11});
        bg->addChild(label);

        // --- Create a menu to hold the button ---
        auto menu = CCMenu::create();
        menu->addChild(btn);

        // Position button in top-left corner
        menu->setPosition({40, winSize.height - 20});

        // zOrder 100 keeps it above game objects
        this->addChild(menu, 100);

        return true;
    }

    // -------------------------------------------------------------------------
    //  onToggleButton — called when the button is tapped
    // -------------------------------------------------------------------------
    void onToggleButton(CCObject*) {
        // Flip toggle
        g_invisible = !g_invisible;

        // Apply to all objects
        applyInvisibility(this, g_invisible);

        // Update button label text
        if (m_fields->buttonLabel) {
            m_fields->buttonLabel->setString(
                g_invisible ? "EYE:ON" : "EYE:OFF"
            );
        }

        // Show notification
        showNotification(g_invisible);
    }

    // -------------------------------------------------------------------------
    //  addObject — hide new objects immediately if invisible mode is on
    // -------------------------------------------------------------------------
    void addObject(GameObject* obj) {
        PlayLayer::addObject(obj);
        if (g_invisible && obj) {
            obj->setOpacity(0);
        }
    }

    // -------------------------------------------------------------------------
    //  resetLevel — re-apply invisibility after death/restart
    // -------------------------------------------------------------------------
    void resetLevel() {
        PlayLayer::resetLevel();
        if (g_invisible) {
            applyInvisibility(this, true);
        }
    }

    // -------------------------------------------------------------------------
    //  onQuit — reset toggle when leaving the level
    // -------------------------------------------------------------------------
    void onQuit() {
        g_invisible = false;
        PlayLayer::onQuit();
    }
};

$on_mod(Loaded) {
    log::info("Level Invisible Mod loaded! Tap the EYE button in a level.");
}

// =============================================================================
//  Level Invisible Mod — Geode Mod for Geometry Dash (Android)
//  Author  : KJ
//  Version : v1.0.0
//
//  HOW TO USE ON ANDROID
//  ----------------------
//  Tap with 3 fingers at the same time while playing any level
//  to toggle all objects invisible/visible.
//
//  HOW IT WORKS
//  ------------
//  We hook into PlayLayer's touch handler to detect 3-finger taps.
//  When 3 fingers are detected, we toggle opacity on all objects.
//  setOpacity(0) = invisible but hitbox still works!
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
// =============================================================================
class $modify(InvisibleObjectsMod, PlayLayer) {

    // -------------------------------------------------------------------------
    //  ccTouchesEnded fires when the player lifts fingers off the screen.
    //  We check if 3 or more fingers were used to toggle invisibility.
    // -------------------------------------------------------------------------
    void ccTouchesEnded(CCSet* touches, CCEvent* event) {
        // Call original so normal jumping still works
        PlayLayer::ccTouchesEnded(touches, event);

        // 3 or more fingers = toggle invisibility
        if (touches && touches->count() >= 3) {
            g_invisible = !g_invisible;
            applyInvisibility(this, g_invisible);
            showNotification(g_invisible);
        }
    }

    // -------------------------------------------------------------------------
    //  Hide new objects immediately if invisible mode is on
    // -------------------------------------------------------------------------
    void addObject(GameObject* obj) {
        PlayLayer::addObject(obj);
        if (g_invisible && obj) {
            obj->setOpacity(0);
        }
    }

    // -------------------------------------------------------------------------
    //  Re-apply invisibility after death/restart
    // -------------------------------------------------------------------------
    void resetLevel() {
        PlayLayer::resetLevel();
        if (g_invisible) {
            applyInvisibility(this, true);
        }
    }

    // -------------------------------------------------------------------------
    //  Reset toggle when leaving the level
    // -------------------------------------------------------------------------
    void onQuit() {
        g_invisible = false;
        PlayLayer::onQuit();
    }
};

$on_mod(Loaded) {
    log::info("Level Invisible Mod loaded! Tap 3 fingers in a level to toggle.");
}

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

static bool g_invisibleMode = false;

static void applyVisibilityToNode(cocos2d::CCNode* node, bool invisible) {
    if (!node) return;

    if (auto player = typeinfo_cast<PlayerObject*>(node)) {
        player->setVisible(true);
    }
    else if (auto obj = typeinfo_cast<GameObject*>(node)) {
        obj->setVisible(!invisible);
    }

    for (auto child : CCArrayExt<cocos2d::CCNode*>(node->getChildren())) {
        applyVisibilityToNode(child, invisible);
    }
}

class $modify(InvisibleObjectsMod, PlayLayer) {
    struct Fields {
        CCLabelBMFont* buttonLabel = nullptr;
        CCMenu* menu = nullptr;
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
        this->addChild(menu, 999);
        m_fields->menu = menu;

        return true;
    }

    // ---------------------------------------------------------------
    //  update — runs every frame
    //  Re-applies invisibility constantly so newly loaded objects
    //  ahead of the player are always hidden immediately
    // ---------------------------------------------------------------
    void update(float dt) {
        PlayLayer::update(dt);

        if (g_invisibleMode) {
            applyVisibilityToNode(this, true);
        }
    }

    void addObject(GameObject* object) {
        PlayLayer::addObject(object);
        if (g_invisibleMode && object) {
            object->setVisible(false);
        }
    }

    void onToggleButton(CCObject*) {
        g_invisibleMode = !g_invisibleMode;
        applyVisibilityToNode(this, g_invisibleMode);

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

    void onQuit() {
        g_invisibleMode = false;
        PlayLayer::onQuit();
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        if (g_invisibleMode) {
            applyVisibilityToNode(this, true);
        }
    }
};

$on_mod(Loaded) {
    log::info("Level Invisible Mod loaded");
}

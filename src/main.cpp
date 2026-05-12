#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
    struct Fields {
        // Click timestamps
        std::chrono::steady_clock::time_point m_lastExit, m_lastRestart, m_lastFull, m_lastPractice, m_lastNormal;
        bool m_isNotifying = false;
        Notification* m_currentNotif = nullptr;
    };

    bool checkClick(std::chrono::steady_clock::time_point& lastTime) {
        auto mod = Mod::get();

        // Enable mod settings
        if (!mod->getSettingValue<bool>("enable-mod")) return true;

        // Only on plat levels settings
        bool isPlat = false;
        if (auto pl = PlayLayer::get()) if (pl->m_level) isPlat = pl->m_level->isPlatformer();
        if (mod->getSettingValue<bool>("plat-only") && !isPlat) return true;

        // Double click
        auto now = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();
        lastTime = now;

        if (diff < 500) { 
            if (m_fields->m_currentNotif) { m_fields->m_currentNotif->hide(); m_fields->m_currentNotif = nullptr; }
            return true; 
        } else {
            // Click notification
            if (mod->getSettingValue<bool>("show-notification") && !m_fields->m_isNotifying) {
                m_fields->m_isNotifying = true;
                m_fields->m_currentNotif = Notification::create("Double click to confirm", NotificationIcon::Info, 0.5f);
                m_fields->m_currentNotif->show();
                this->scheduleOnce(schedule_selector(MyPauseLayer::resetNotif), 1.0f);
            }
            return false;
        }
    }

    // Back button
    void keyBackClicked() override {
        this->onQuit(nullptr);
    }

    // Buttons
    void onQuit(CCObject* s) { if (checkClick(m_fields->m_lastExit)) PauseLayer::onQuit(s); }
    void onRestart(CCObject* s) { if (checkClick(m_fields->m_lastRestart)) PauseLayer::onRestart(s); }
    void onRestartFull(CCObject* s) { if (checkClick(m_fields->m_lastFull)) PauseLayer::onRestartFull(s); }
    void onPracticeMode(CCObject* s) { if (checkClick(m_fields->m_lastPractice)) PauseLayer::onPracticeMode(s); }
    void onNormalMode(CCObject* s) { if (checkClick(m_fields->m_lastNormal)) PauseLayer::onNormalMode(s); }

    void resetNotif(float dt) { m_fields->m_isNotifying = false; m_fields->m_currentNotif = nullptr; }
};

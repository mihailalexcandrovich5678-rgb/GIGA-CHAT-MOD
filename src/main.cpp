#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

namespace {
    // Set this only after a real HTTPS bridge service has been deployed.
    // Keeping it empty prevents the mod from contacting an unknown server.
    constexpr std::string_view kBridgeHealthEndpoint = "";

    async::TaskHolder<web::WebResponse> g_healthRequest;

    void startBridge() {
        if (kBridgeHealthEndpoint.empty()) {
            log::info(
                "Geode Bridge loaded in background. "
                "No server configured; network connection is inactive."
            );
            return;
        }

        if (!kBridgeHealthEndpoint.starts_with("https://")) {
            log::error("Geode Bridge refused to connect: HTTPS is required.");
            return;
        }

        log::info("Geode Bridge: starting secure HTTPS handshake.");

        g_healthRequest.spawn(
            web::WebRequest().get(std::string(kBridgeHealthEndpoint)),
            [](web::WebResponse response) {
                if (response.code() >= 200 && response.code() < 300) {
                    log::info(
                        "Geode Bridge: HTTPS server responded with status {}.",
                        response.code()
                    );
                } else {
                    log::warn(
                        "Geode Bridge: server returned HTTP status {}.",
                        response.code()
                    );
                }
            }
        );
    }
}

class $modify(GeodeBridgeMenu, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        // The bridge starts automatically when the game reaches its main menu.
        // No buttons, popups, level edits, or gameplay UI are created here.
        static bool started = false;
        if (!started) {
            started = true;
            startBridge();
        }

        return true;
    }
};

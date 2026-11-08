#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/utils/async.hpp>
#include <arc/time/Sleep.hpp>
#include <asp/time/SystemTime.hpp>
#include <algorithm>

using namespace geode::prelude;

namespace {
    constexpr std::string_view kDeviceTokenKey = "bridge-device-token";
    constexpr std::string_view kDeviceIdKey = "bridge-device-id";

    async::TaskHolder<> g_bridgeTask;

    std::string baseUrl() {
        auto url = Mod::get()->getSettingValue<std::string>("server-url");
        while (!url.empty() && url.back() == '/') url.pop_back();
        return url;
    }

    bool validBaseUrl(std::string const& url) {
        return url.starts_with("https://") && url.find_first_of(" \r\n") == std::string::npos;
    }

    std::string deviceToken() {
        return Mod::get()->getSavedValue<std::string>(kDeviceTokenKey, "");
    }

    std::string deviceId() {
        return Mod::get()->getSavedValue<std::string>(kDeviceIdKey, "");
    }

    matjson::Value makeAck(std::string status, matjson::Value result) {
        auto body = matjson::Value::object();
        body["status"] = std::move(status);
        body["result"] = std::move(result);
        return body;
    }

    arc::Future<> sendAck(
        std::string const& base,
        std::string const& token,
        std::string const& commandId,
        std::string status,
        matjson::Value result
    ) {
        auto body = makeAck(std::move(status), std::move(result));
        auto response = co_await web::WebRequest()
            .header("Authorization", "Bearer " + token)
            .bodyJSON(body)
            .followRedirects(false)
            .post(base + "/v1/device/commands/" + commandId + "/ack");
        if (response.code() < 200 || response.code() >= 300) {
            log::warn("Geode Bridge: command acknowledgement failed (HTTP {}).", response.code());
        }
    }

    arc::Future<> claimDevice(std::string const& base, std::string code) {
        auto body = matjson::Value::object();
        body["code"] = std::move(code);

        auto response = co_await web::WebRequest()
            .bodyJSON(body)
            .followRedirects(false)
            .post(base + "/v1/pair/claim");

        if (response.code() < 200 || response.code() >= 300) {
            log::error("Geode Bridge: pairing failed (HTTP {}). Check the one-time code.", response.code());
            co_return;
        }

        auto parsed = response.json();
        if (!parsed || !parsed.unwrap().isObject()) {
            log::error("Geode Bridge: pairing response was not valid JSON.");
            co_return;
        }

        auto json = std::move(parsed).unwrap();
        auto token = json["device_token"].asString().unwrapOr("");
        auto id = json["device_id"].asString().unwrapOr("");
        if (token.empty() || id.empty()) {
            log::error("Geode Bridge: pairing response did not contain device credentials.");
            co_return;
        }

        // Geode stores this in the mod's app-private save data. It is not placed in the repository.
        Mod::get()->setSavedValue(std::string(kDeviceTokenKey), token);
        Mod::get()->setSavedValue(std::string(kDeviceIdKey), id);
        Mod::get()->setSettingValue<std::string>("pairing-code", "");
        log::info("Geode Bridge: device paired successfully. Device ID: {}", id);
    }

    arc::Future<> runBridge() {
        auto base = baseUrl();
        if (!validBaseUrl(base)) {
            log::info("Geode Bridge: set an HTTPS server URL in the mod settings to enable the connection.");
            co_return;
        }

        auto token = deviceToken();
        if (token.empty()) {
            auto code = Mod::get()->getSettingValue<std::string>("pairing-code");
            if (code.size() == 6 && std::ranges::all_of(code, [](unsigned char c) { return c >= '0' && c <= '9'; })) {
                co_await claimDevice(base, code);
                token = deviceToken();
            }
        }

        if (token.empty()) {
            log::info("Geode Bridge: waiting for device pairing. Enter a short-lived pairing code in mod settings.");
            co_return;
        }

        log::info("Geode Bridge: secure HTTPS command polling started.");

        while (true) {
            auto response = co_await web::WebRequest()
                .header("Authorization", "Bearer " + token)
                .followRedirects(false)
                .get(base + "/v1/device/commands");

            if (response.code() == 401) {
                log::error("Geode Bridge: device token was rejected. Revoke and pair this device again.");
                co_return;
            }

            if (response.code() >= 200 && response.code() < 300) {
                auto parsed = response.json();
                if (parsed && parsed.unwrap().isObject()) {
                    auto json = std::move(parsed).unwrap();
                    auto commands = json["commands"].asArray();
                    if (commands) {
                        for (auto const& command : commands.unwrap()) {
                            auto id = command["id"].asString().unwrapOr("");
                            auto kind = command["kind"].asString().unwrapOr("");
                            if (id.empty()) continue;

                            if (kind == "bridge.ping") {
                                auto result = matjson::Value::object();
                                result["pong"] = true;
                                result["protocol"] = 1;
                                co_await sendAck(base, token, id, "completed", std::move(result));
                            }
                            else if (kind == "editor.get_status") {
                                auto result = matjson::Value::object();
                                result["game"] = "running";
                                result["editor"] = "unknown";
                                result["note"] = "Editor state inspection is not implemented yet; no level data was accessed.";
                                co_await sendAck(base, token, id, "completed", std::move(result));
                            }
                            else {
                                auto result = matjson::Value::object();
                                result["error"] = "unsupported_command";
                                co_await sendAck(base, token, id, "failed", std::move(result));
                            }
                        }
                    }
                }
            }
            else {
                log::warn("Geode Bridge: command poll returned HTTP {}.", response.code());
            }

            co_await arc::sleepUntil(asp::Instant::now() + asp::Duration::fromSecs(2.5));
        }
    }

    void startBridge() {
        if (g_bridgeTask.isPending()) return;
        g_bridgeTask.spawn(runBridge(), [] {});
    }
}

class $modify(GeodeBridgeMenu, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        static bool started = false;
        if (!started) {
            started = true;
            startBridge();
        }

        return true;
    }
};

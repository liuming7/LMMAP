#include "conf.h"
#include "pch.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#include <third-party/Nlohmann/json.hpp>

#define CONFIG_PATH "./plugins/LMMap/lmmap_config.json"

Config config;

void to_json(nlohmann::json& node, const Config& cfg) {
    node["webServer"]["port"] = cfg.webServerPort;
    node["apiServer"]["port"] = cfg.apiServerPort;
    node["customMarkers"]["enabled"] = cfg.enableCustomMarkers;
    node["updateMap"]["enabled"] = cfg.enableUpdateMap;
    node["updateMap"]["skip"] = cfg.updateMapSkip;
    node["mapRender"]["zoomIn"] = cfg.zoomIn;
    node["mapRender"]["zoomOut"] = cfg.zoomOut;
}

void from_json(const nlohmann::json& node, Config& cfg) {
    if (node.contains("webServer") && node["webServer"].contains("port")) {
        cfg.webServerPort = node["webServer"]["port"].get<uint16_t>();
    }
    if (node.contains("apiServer") && node["apiServer"].contains("port")) {
        cfg.apiServerPort = node["apiServer"]["port"].get<uint16_t>();
    }
    if (node.contains("customMarkers") && node["customMarkers"].contains("enable")) {
        cfg.apiServerPort = node["customMarkers"]["enable"].get<bool>();
    }
    if (node.contains("updateMap")) {
        if (node["updateMap"].contains("enable")) {
            cfg.enableWebChat = node["updateMap"]["enable"].get<bool>();
        }
        if (node["updateMap"].contains("skip")) {
            cfg.webChatOutboundPrefix = node["updateMap"]["skip"].get<uint32_t>();
        }
    }
    if (node.contains("mapRender")) {
        if (node["mapRender"].contains("zoomIn")) {
            cfg.zoomIn = node["mapRender"]["zoomIn"].get<int16_t>();
        }
        if (node["mapRender"].contains("zoomOut")) {
            cfg.zoomOut = node["mapRender"]["zoomOut"].get<int16_t>();
        }
    }
}

void Config::load() {
    try {
        if (!std::filesystem::exists(CONFIG_PATH)) {
            this->save();
            return;
        }
        std::ifstream in(CONFIG_PATH);
        nlohmann::json conf;
        in>>conf;
        from_json(conf, config);
        in.close();
        nlohmann::json node = nlohmann::json::object();
        to_json(node, config);
		if (node.dump() != conf.dump()) {
            Logger("LMMAP").warn("Your config is outdated! Auto-completing...");
            this->save();
        }
    }
    catch (const std::exception& e) {
        Logger("LMMAP").error("Failed to load config: {}", e.what());
        Logger("LMMAP").error("Using the default config");
    }
}

void Config::save() {
    try {
        std::ofstream out(CONFIG_PATH);
        nlohmann::json node = nlohmann::json::object();
        to_json(node, config);
        out<<node.dump(4);
        out.close();
    }
    catch (const std::exception) {
        Logger("LMMAP").error("Failed to save(create) config!");
    }
}

#include "pch.h"

#include <Nlohmann/json.hpp>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/ScheduleAPI.h>

#include <mutex>
#include <shared_mutex>

#define MARKERBOOK_PATH "./plugins/LMMap/unmined-web/customMarkers.json"

std::shared_mutex markersReadMutex;

nlohmann::json playerMarkers = nlohmann::json::array();
nlohmann::json customMarkers = nlohmann::json::array();
std::string getPlayerMarkers() {
	std::string markersDump;
	std::shared_lock lock{ markersReadMutex };
	markersDump = playerMarkers.dump(4);
	lock.unlock();
	return playerMarkers.dump(4);
}

std::string getCustomMarkers() {
	std::string markersDump;
	std::shared_lock lock{ markersReadMutex };
	markersDump = customMarkers.dump(4);
	lock.unlock();
	return customMarkers.dump(4);
}

void updatePlayerMarkers() {
	std::unique_lock lock{ markersReadMutex };
	playerMarkers.clear();
	for (Player* player : Level::getAllPlayers()) {
		nlohmann::json playerMarker = {};
		Vec3 playerPos = player->getPos();
		int dimensionId = player->getDimensionId();
		if(dimensionId!=0){
			continue;
		}
		playerMarker["x"] = playerPos.x;
		playerMarker["z"] = playerPos.z;
		playerMarker["image"] = "steve.png";
		playerMarker["imageAnchor"] = nlohmann::json::array();
		playerMarker["imageAnchor"][0] = 0.5;
		playerMarker["imageAnchor"][1] = 1;
		playerMarker["imageScale"] = 1;
		playerMarker["text"] = player->getName();
		playerMarker["textColor"] = "yellow";
		playerMarker["offsetX"] = 0;
		playerMarker["offsetY"] = 20;
		playerMarker["font"] = "bold 20px Calibri,sans serif";
		playerMarkers.push_back(playerMarker);
	}
	lock.unlock();
}

void addCustomMarker(std::string text, int x,int y, int z){
	std::unique_lock lock{ markersReadMutex };
	nlohmann::json customMarker = {};
	customMarker["x"] = x;
	customMarker["z"] = z;
	customMarker["image"] = "custom.pin.png";
	customMarker["imageAnchor"] = nlohmann::json::array();
	customMarker["imageAnchor"][0] = 0.5;
	customMarker["imageAnchor"][1] = 1;
	customMarker["imageScale"] = 0.2;
	customMarker["text"] = text;
	customMarker["textColor"] = "white";
	customMarker["offsetX"] = 0;
	customMarker["offsetY"] = 20;
	customMarker["font"] = "bold 20px Calibri,sans serif";
	customMarkers.push_back(customMarker);
	lock.unlock();
	saveCustomMarkers();
}

std::string getAllCustomMarkers(){
	return "";
}

void deleteCustomMarker(std::string text){
	std::unique_lock lock{ markersReadMutex };
	int i = 0;
	for (nlohmann::json customMarker : customMarkers) {
		if(customMarker["text"].get<std::string>()==text){
			customMarkers.erase(i);
			break;
		}
		i++;
	}
	lock.unlock();
	saveCustomMarkers();
}

void loadCustomMarkers(){
	try {
        if (!std::filesystem::exists(MARKERBOOK_PATH)) {
            saveCustomMarkers();
            return;
        }
        std::ifstream in(MARKERBOOK_PATH);
        in>>customMarkers;
        in.close();
    }
    catch (const std::exception& e) {
        Logger("LMMAP").error("Failed to load custom markers: {}", e.what());
    }
}

void saveCustomMarkers() {
    try {
        std::ofstream out(MARKERBOOK_PATH);
        out<<customMarkers.dump(4);
        out.close();
    }
    catch (const std::exception) {
        Logger("LMMAP").error("Failed to save(create) custom markers.");
    }
}


void markersInit() {
	loadCustomMarkers();
	Schedule::repeat(updatePlayerMarkers, 20);
	//Schedule::repeat(saveCustomMarkers, 36000);
}
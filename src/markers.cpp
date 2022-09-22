#include "pch.h"

#include <third-party/Nlohmann/json.hpp>
#include <MC/Level.hpp>
#include <MC/Player.hpp>
#include <ScheduleAPI.h>

#include <mutex>
#include <shared_mutex>

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
		playerMarker["textColor"] = "red";
		playerMarker["offsetX"] = 0;
		playerMarker["offsetY"] = 20;
		playerMarker["font"] = "bold 20px Calibri,sans serif";
		playerMarkers.push_back(playerMarker);
	}
	lock.unlock();
}

void addCustomMarker(std::string text, Vec3& position){
	std::unique_lock lock{ markersReadMutex };
	nlohmann::json customMarker = {};
	customMarker["x"] = position.x;
	customMarker["z"] = position.z;
	customMarker["image"] = "custom.pin.png";
	customMarker["imageAnchor"] = nlohmann::json::array();
	customMarker["imageAnchor"][0] = 0.5;
	customMarker["imageAnchor"][1] = 1;
	customMarker["imageScale"] = 1;
	customMarker["text"] = text;
	customMarker["textColor"] = "red";
	customMarker["offsetX"] = 0;
	customMarker["offsetY"] = 20;
	customMarker["font"] = "bold 20px Calibri,sans serif";
	customMarkers.push_back(customMarker);
	lock.unlock();
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
}

void markersInit() {
	Schedule::repeat(updatePlayerMarkers, 20);
}
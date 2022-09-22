#pragma once
#include "pch.h"

class Config {
	
public:

	// Please write default values below
	uint16_t webServerPort = 5000;
	uint16_t apiServerPort = 5001;
	bool enableCustomMarkers = true;
	bool enableUpdateMap = true;
	uint32_t updateMapSkip = 12000;//10min
	int16_t zoomIn = -3;
	int16_t zoomOut = 3;
	
	void load();
	void save();
	
};
extern Config config;

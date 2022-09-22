#include <EventAPI.h>
#include <LoggerAPI.h>
#include <ScheduleAPI.h>

#include <MC/Player.hpp>
#include <MC/Types.hpp>
#include <MC/Objective.hpp>
#include <MC/Scoreboard.hpp>

#include <string>
#include "version.h"

#include "pch.h"
#include <thread>
#include "conf.h"

// We recommend using the global logger.
extern Logger logger;

/**
 * @brief The entrypoint of the plugin. DO NOT remove or rename this function.
 *        
 */
void PluginInit()
{
    Logger logger(PLUGIN_NAME);
    logger.info("           ___          ___          ___          ___          ___          ___");
    logger.info("          /\\__\\        /\\__\\        /\\  \\        /\\  \\        /\\  \\        /\\__\\");
    logger.info("&6=========&r/:/  /&6=======&r/::L_L_&6=======&r\\:\\  \\&6======&r/::\\  \\&6======&r/::\\  \\&6======&r/:/__/_&6=============");
    logger.info(" &6\\\\&r     /:/__/       /:/L:\\__\\      /::\\__\\    /::\\:\\__\\    /:/\\:\\__\\    /::\\/\\__\\           &6\\\\");
    logger.info("  &6\\\\&r    \\:\\  \\       \\/_/:/  /     /:/\\/__/    \\:\\:\\/__/    \\:\\ \\/__/    \\/\\::/  /       ___  &6\\\\");
    logger.info("   &6\\\\&r    \\:\\__\\        /:/  /      \\/__/        \\:\\/__/      \\:\\__\\        /:/  /       /\\__\\  &6\\\\");
    logger.info("    &6\\\\&r    \\/__/        \\/__/         &6________&r    \\/__/        \\/__/        \\/__/        \\/__/   &6\\\\");
    logger.info("     &6\\\\                             /liuming7\\                                                   \\\\");
    logger.info("      &6==============================================================================================");
    logger.info("LMMap is made by liuming7, and Powered by LiteLoaderBDS");

    /*Event::ServerStartedEvent::subscribe([](const Event::ServerStartedEvent& event) {
        auto* objective = Scoreboard::newObjective("disablesidebar", "sidebar switch");
        //getScore(const std::string& objname, const std::string& id);
        return true;
    });*/

    Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& event) {
        auto player = event.mPlayer;
        
        player->sendTitlePacket(
        "Welcome to 203!",
        TitleType::SetTitle,
        /* FadeInDuration =  */ 1,
        /* RemainDuration =  */ 2,
        /* FadeOutDuration =  */ 1
        );

        //logger.info("Player {} has joined the server.", player->getName());
        return true;
    });

    Event::PlayerChatEvent::subscribe([](const Event::PlayerChatEvent& event) {
        if(config.enableCustomMarkers){
            std::string message_content = event.mMessage;
        }
		return true;
	});

    std::ios::sync_with_stdio(false);
    config.load();
    markersInit();
    std::thread apiServerThread(startApiServer);
    apiServerThread.detach();
    preStartUnmined();
    stopNginx();
    startNginx();
}

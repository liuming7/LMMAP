#include <ModUtils/ModUtils.h>

#include <llapi/EventAPI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/ScheduleAPI.h>
#include <llapi/DynamicCommandAPI.h>

#include <llapi/mc/Player.hpp>
#include <llapi/mc/Types.hpp>
#include <llapi/mc/Objective.hpp>
#include <llapi/mc/Scoreboard.hpp>

#include <cstring>
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

    Event::ServerStartedEvent::subscribe([](const Event::ServerStartedEvent& event) {
        using ParamType = DynamicCommand::ParameterType;
        using Param = DynamicCommand::ParameterData;
        DynamicCommand::setup(
        "lmmap", // The command
        "mark a marker on the map", // The description
        {
            {"enum_1", {"add", "del"}},
            //{"enum_2", {"cord", "here"}},
        }, // The enumeration
        {
            Param("op", ParamType::Enum, false, "enum_1"),
            Param("name", ParamType::String, false),
            //Param("location type", ParamType::Enum, false, "enum_2"),
            Param("x", ParamType::Int, true),
            Param("y", ParamType::Int, true),
            Param("z", ParamType::Int, true),
        }, // The parameters
        {
            // overloads{ (type == Enum ? enumOptions : name) ...}
            {"enum_1", "name", "x", "y", "z"}, // example_command <add|remove> [testInt]
        }, // The overloads
        [](
            DynamicCommand const& command,
            CommandOrigin const& origin,
            CommandOutput& output,
            std::unordered_map<std::string, DynamicCommand::Result>& results
        ) {}, // The callback function
        CommandPermissionLevel::Any // The permission level
        );
        return true;
    });

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

/*
    Event::PlayerChatEvent::subscribe([](const Event::PlayerChatEvent& event) {
        if(config.enableCustomMarkers){
            std::string text;
            std::string command;
            int x=0,y=0,z=0;
            std::string message_content = event.mMessage;
            if(message_content.find_first_of('|')==0){
                command = message_content.substr(message_content.find_first_of('|')+1,message_content.find_first_of(' ')-message_content.find_first_of('|')-1);
                text = message_content.substr(message_content.find_first_of(' ')+1,message_content.find_last_of(' ')-message_content.find_first_of(' ')-1);
                if(command=="del"){
                    deleteCustomMarker(text);
                    return true;
                }else if(command=="add"){
                    x = stoi(message_content.substr(message_content.find_last_of(' ')+1,message_content.find_first_of(',')-message_content.find_last_of(' ')-1));
                    y = stoi(message_content.substr(message_content.find_first_of(',')+1,message_content.find_last_of(',')-message_content.find_first_of(',')-1));
                    z = stoi(message_content.substr(message_content.find_last_of(',')+1,message_content.length()-message_content.find_last_of(',')-1));
                    addCustomMarker(text,x,y,z);
                    return true;
                }
            }
        }
		return true;
	});
*/

    Event::PlayerCmdEvent::subscribe([](const Event::PlayerCmdEvent& event){
        if(config.enableCustomMarkers){
            std::vector<std::string> command_set = ModUtils::split(event.mCommand, " ");
            if(command_set[1]=="del"){
                deleteCustomMarker(command_set[2]);
                event.mPlayer->sendTextPacket(command_set[2] + " has been deleted");
            }else if(command_set[1]=="add"){
                if(command_set.size() == 3){
                    addCustomMarker(command_set[2], event.mPlayer->getBlockPos().x, event.mPlayer->getBlockPos().y, event.mPlayer->getBlockPos().z);
                }else if(command_set.size() == 6){
                    addCustomMarker(command_set[2], stoi(command_set[3]), stoi(command_set[4]), stoi(command_set[5]));
                }
                event.mPlayer->sendTextPacket(command_set[2] + " has been added");
            }
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

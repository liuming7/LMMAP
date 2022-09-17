/**
 * @file plugin.cpp
 * @brief The main file of the plugin
 */

#include <LoggerAPI.h>

#include "version.h"

// We recommend using the global logger.
extern Logger logger;

/**
 * @brief The entrypoint of the plugin. DO NOT remove or rename this function.
 *        
 */
void PluginInit()
{
    // Your code here
    Logger logger(PLUGIN_NAME);
    logger.info("lmmap is on");
    logger.info("           ___          ___          ___          ___          ___          ___");
    logger.info("          /\\__\\        /\\__\\        /\\  \\        /\\  \\        /\\  \\        /\\__\\");
    logger.info("=========/:/  /=======/::L_L_=======\\:\\  \\======/::\\  \\======/::\\  \\======/:/__/_=============");
    logger.info(" \\\\     /:/__/       /:/L:\\__\\      /::\\__\\    /::\\:\\__\\    /:/\\:\\__\\    /::\\/\\__\\           \\\\");
    logger.info("  \\\\    \\:\\  \\       \\/_/:/  /     /:/\\/__/    \\:\\:\\/__/    \\:\\ \\/__/    \\/\\::/  /       ___  \\\\");
    logger.info("   \\\\    \\:\\__\\        /:/  /      \\/__/        \\:\\/__/      \\:\\__\\        /:/  /       /\\__\\  \\\\");
    logger.info("    \\\\    \\/__/        \\/__/                     \\/__/        \\/__/        \\/__/        \\/__/   \\\\");
    logger.info("     \\\\                                                                                          \\\\");
    logger.info("      ==============================================================================================");
}

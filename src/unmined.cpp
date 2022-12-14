// start and monitor uNmINeD
#include "pch.h"
#include "shell.h"
#include "conf.h"
#include <llapi/LoggerAPI.h>
#include <llapi/ScheduleAPI.h>
#include <llapi/mc/Level.hpp>
#include <llapi/EventAPI.h>
#include <llapi/LLAPI.h>
#include <algorithm>

std::string& trim(std::string& s)
{
	if (s.empty())
	{
		return s;
	}
	//s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

std::string getLevelName() {
	std::ifstream properties("server.properties");
	string line;
	// search world name from server.properties
	while (std::getline(properties, line)) {
		if (line.find("level-name") != string::npos) {
			string levelName = line.substr(line.find("=") + 1);
			return trim(levelName);
		}
	}
	return "";
}

Shell shell;
bool errorMode = false;
bool needResumeMap = false;
int startUnmined() {
	Logger logger("LMMAP");
	logger.info << "启动地图渲染进程……" << logger.endl;
	string levelName = getLevelName();
	if (levelName == "") {
		logger.warn << "无法从server.properties中读取level-name项。使用默认值 level。" << logger.endl;
		levelName = "level";
	}
	bool status = true;
	status = shell.RunProcess(TextEncoding::toUnicode(".\\plugins\\LMMAP\\unmined\\unmined-cli.exe web render --world=\"./worlds/" + levelName + "\" --output=\"./plugins/LMMAP/unmined-web/\" --imageformat=webp -c --zoomin=" + std::to_string(config.zoomIn) + " --zoomout=" + std::to_string(config.zoomOut)));
	Schedule::repeat([]() {
		string line;
		shell.GetOutput(1, line);
		//Logger logger("LMMAP");
		//logger.info(line.data());
		if (line.find("Elapsed time total") != string::npos) {
			Logger logger("LMMAP");
			logger.info << "地图生成完毕！" << logger.endl;
			if (needResumeMap) {
				Level::runcmdEx("save resume");
			}
		}
		// 在此处判断unmined无法接受输入参数时直接退出的行为，否则LMMAP将卡在挂起阶段
		else if (line.find("exception") != string::npos) {
			Logger logger("LMMAP");
			if (needResumeMap) {
				logger.error << "地图生成失败：" << logger.endl;
				errorMode = true;
			}
			else {
				logger.error << "地图生成失败" << logger.endl;
				logger.warn << "由于您安装了不兼容的插件，为了避免不必要的反馈，我们将不会输出错误日志。" << logger.endl;
			}
		}
		// To-Do: 在此处读入unmined的进度并在控制台显示
		else {

		}
		if (errorMode) {
			std::cout << line;
		}
		}, 40);
	return status;
}


ScheduleTask checkIfDataSaved;
ScheduleTask updateMap;
void preStartUnmined() {
	Event::ServerStartedEvent::subscribe([](const Event::ServerStartedEvent& ev) {
		Logger logger("LMMAP");
		ll::Plugin* backupHelperPlugin = ll::getPlugin("BackupHelper");
		// 查找是否有BackupHelper
		if (backupHelperPlugin != nullptr) {
			std::string backupHelperPluginVersion = backupHelperPlugin->version.toString();
			backupHelperPluginVersion.erase(std::remove(backupHelperPluginVersion.begin(), backupHelperPluginVersion.end(), '.'), backupHelperPluginVersion.end());
			if (std::stoi(backupHelperPluginVersion) < 207) {
				logger.warn << "检测到不兼容的插件：BackupHelper，将尝试不挂起地图生成图像，请勿反馈由此造成的任何问题。" << logger.endl;
				logger.info << "在新版本BackupHelper（版本号大于等于2.0.7）中，我们已经解决了冲突问题，请更新您的BackupHelper" << logger.endl;
				startUnmined();
				return true;
			}
		}
		logger.info << "正在挂起地图，将在挂起后启动渲染" << logger.endl;
		Level::runcmdEx("save hold");
		needResumeMap = true;
		return true;
		});
	checkIfDataSaved = Schedule::repeat([&]() {
		// 检查地图是否已被挂起
		if (Level::runcmdEx("save query").second.find("Data saved") != string::npos) {
			checkIfDataSaved.cancel();
			startUnmined();
		}
		}, 40);
	if(config.enableUpdateMap){
		updateMap = Schedule::repeat([](){
			if(Level::getAllPlayers().size()>0){
				startUnmined();
			}
		}, config.updateMapSkip>=12000 ? config.updateMapSkip : 12000);//10min
	}
}

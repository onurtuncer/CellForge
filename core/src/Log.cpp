// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include "CellForge/PreCompiledHeader.h"
#include "CellForge/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <filesystem>

#define CF_HAS_CONSOLE !CF_DIST

namespace CellForge {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	std::map<std::string, Log::TagDetails> Log::s_DefaultTagDetails = {
		{ "Animation",         TagDetails{  true, Level::Warn  } },
		{ "Core",              TagDetails{  true, Level::Trace } },
		{ "Memory",            TagDetails{  true, Level::Error } },
		{ "Project",           TagDetails{  true, Level::Warn  } },
		{ "Renderer",          TagDetails{  true, Level::Info  } },
		{ "Workcell",          TagDetails{  true, Level::Info  } },
		{ "Timer",             TagDetails{ false, Level::Trace } }
	};

	void Log::Init()
	{
		// Create "logs" directory if doesn't exist
		std::string logsDirectory = "logs";
		if (!std::filesystem::exists(logsDirectory))
			std::filesystem::create_directories(logsDirectory);

		std::vector<spdlog::sink_ptr> cellForgeSinks =
		{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/cellForge.log", true),
#if CF_HAS_CONSOLE
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
#endif
		};

		std::vector<spdlog::sink_ptr> appSinks =
		{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/APP.log", true),
#if CF_HAS_CONSOLE
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
#endif
		};

		cellForgeSinks[0]->set_pattern("[%T] [%l] %n: %v");
		appSinks[0]->set_pattern("[%T] [%l] %n: %v");

#if CF_HAS_CONSOLE
		cellForgeSinks[1]->set_pattern("%^[%T] %n: %v%$");
		appSinks[1]->set_pattern("%^[%T] %n: %v%$");
#endif

		s_CoreLogger = std::make_shared<spdlog::logger>("cellForge", cellForgeSinks.begin(), cellForgeSinks.end());
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = std::make_shared<spdlog::logger>("APP", appSinks.begin(), appSinks.end());
		s_ClientLogger->set_level(spdlog::level::trace);

		SetDefaultTagSettings();
	}

	void Log::Shutdown()
	{
		s_ClientLogger.reset();
		s_CoreLogger.reset();
		spdlog::drop_all();
	}

	void Log::SetDefaultTagSettings()
	{
		s_EnabledTags = s_DefaultTagDetails;
	}

}
// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include "CellForge/Base.h"

#include <spdlog/spdlog.h>

#include <format>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>


namespace CellForge {

	class CF_API Log
	{
	public:
		enum class Type : uint8_t
		{
			Core = 0, Client = 1
		};
		enum class Level : uint8_t
		{
			Trace = 0, Info, Warn, Error, Fatal
		};
		struct TagDetails
		{
			bool Enabled = true;
			Level LevelFilter = Level::Trace;
		};

	public:
		static void Init();
		static void Shutdown();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

		static bool HasTag(const std::string& tag) { return s_EnabledTags.find(tag) != s_EnabledTags.end(); }
		static std::map<std::string, TagDetails>& EnabledTags() { return s_EnabledTags; }
		static void SetDefaultTagSettings();

		template<typename... Args>
		static void PrintMessage(Log::Type type, Log::Level level, std::format_string<Args...> format, Args&&... args);

		template<typename... Args>
		static void PrintMessageTag(Log::Type type, Log::Level level, std::string_view tag, std::format_string<Args...> format, Args&&... args);

		static void PrintMessageTag(Log::Type type, Log::Level level, std::string_view tag, std::string_view message);

		template<typename... Args>
		static void PrintAssertMessage(Log::Type type, std::string_view prefix, std::format_string<Args...> message, Args&&... args);

		static void PrintAssertMessage(Log::Type type, std::string_view prefix);

	public:
		// Enum utils
		static const char* LevelToString(Level level)
		{
			switch (level)
			{
				case Level::Trace: return "Trace";
				case Level::Info:  return "Info";
				case Level::Warn:  return "Warn";
				case Level::Error: return "Error";
				case Level::Fatal: return "Fatal";
			}
			return "";
		}
		static Level LevelFromString(std::string_view string)
		{
			if (string == "Trace") return Level::Trace;
			if (string == "Info")  return Level::Info;
			if (string == "Warn")  return Level::Warn;
			if (string == "Error") return Level::Error;
			if (string == "Fatal") return Level::Fatal;

			return Level::Trace;
		}

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

		inline static std::map<std::string, TagDetails> s_EnabledTags;
		static std::map<std::string, TagDetails> s_DefaultTagDetails;
	};

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tagged logs (prefer these!)                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Core logging
#define CF_CORE_TRACE_TAG(tag, ...) ::CellForge::Log::PrintMessageTag(::CellForge::Log::Type::Core, ::CellForge::Log::Level::Trace, tag, __VA_ARGS__)
#define CF_CORE_INFO_TAG(tag, ...)  ::CellForge::Log::PrintMessageTag(::CellForge::Log::Type::Core, ::CellForge::Log::Level::Info, tag, __VA_ARGS__)
#define CF_CORE_WARN_TAG(tag, ...)  ::CellForge::Log::PrintMessageTag(::CellForge::Log::Type::Core, ::CellForge::Log::Level::Warn, tag, __VA_ARGS__)
#define CF_CORE_ERROR_TAG(tag, ...) ::CellForge::Log::PrintMessageTag(::CellForge::Log::Type::Core, ::CellForge::Log::Level::Error, tag, __VA_ARGS__)
#define CF_CORE_FATAL_TAG(tag, ...) ::CellForge::Log::PrintMessageTag(::CellForge::Log::Type::Core, ::CellForge::Log::Level::Fatal, tag, __VA_ARGS__)

// Client logging
#define CF_TRACE_TAG(tag, ...) ::CellForge::Log::PrintMessageTag(::CellForge::Log::Type::Client, ::CellForge::Log::Level::Trace, tag, __VA_ARGS__)
#define CF_INFO_TAG(tag, ...)  ::CellForge::Log::PrintMessageTag(::CellForge::Log::Type::Client, ::CellForge::Log::Level::Info, tag, __VA_ARGS__)
#define CF_WARN_TAG(tag, ...)  ::CellForge::Log::PrintMessageTag(::CellForge::Log::Type::Client, ::CellForge::Log::Level::Warn, tag, __VA_ARGS__)
#define CF_ERROR_TAG(tag, ...) ::CellForge::Log::PrintMessageTag(::CellForge::Log::Type::Client, ::CellForge::Log::Level::Error, tag, __VA_ARGS__)
#define CF_FATAL_TAG(tag, ...) ::CellForge::Log::PrintMessageTag(::CellForge::Log::Type::Client, ::CellForge::Log::Level::Fatal, tag, __VA_ARGS__)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Core Logging
#define CF_CORE_TRACE(...)  ::CellForge::Log::PrintMessage(::CellForge::Log::Type::Core, ::CellForge::Log::Level::Trace, __VA_ARGS__)
#define CF_CORE_INFO(...)   ::CellForge::Log::PrintMessage(::CellForge::Log::Type::Core, ::CellForge::Log::Level::Info, __VA_ARGS__)
#define CF_CORE_WARN(...)   ::CellForge::Log::PrintMessage(::CellForge::Log::Type::Core, ::CellForge::Log::Level::Warn, __VA_ARGS__)
#define CF_CORE_ERROR(...)  ::CellForge::Log::PrintMessage(::CellForge::Log::Type::Core, ::CellForge::Log::Level::Error, __VA_ARGS__)
#define CF_CORE_FATAL(...)  ::CellForge::Log::PrintMessage(::CellForge::Log::Type::Core, ::CellForge::Log::Level::Fatal, __VA_ARGS__)

// Client Logging
#define CF_TRACE(...)   ::CellForge::Log::PrintMessage(::CellForge::Log::Type::Client, ::CellForge::Log::Level::Trace, __VA_ARGS__)
#define CF_INFO(...)    ::CellForge::Log::PrintMessage(::CellForge::Log::Type::Client, ::CellForge::Log::Level::Info, __VA_ARGS__)
#define CF_WARN(...)    ::CellForge::Log::PrintMessage(::CellForge::Log::Type::Client, ::CellForge::Log::Level::Warn, __VA_ARGS__)
#define CF_ERROR(...)   ::CellForge::Log::PrintMessage(::CellForge::Log::Type::Client, ::CellForge::Log::Level::Error, __VA_ARGS__)
#define CF_FATAL(...)   ::CellForge::Log::PrintMessage(::CellForge::Log::Type::Client, ::CellForge::Log::Level::Fatal, __VA_ARGS__)


namespace CellForge {

	template<typename... Args>
	void Log::PrintMessage(Log::Type type, Log::Level level, std::format_string<Args...> format, Args&&... args)
	{
		auto detail = s_EnabledTags[""];
		if (detail.Enabled && detail.LevelFilter <= level)
		{
			// Pre-format with std::format so spdlog receives a plain string —
			// avoids the fmt::format_string / std::format_string type mismatch.
			std::string msg = std::format(format, std::forward<Args>(args)...);
			auto logger = (type == Type::Core) ? GetCoreLogger() : GetClientLogger();
			switch (level)
			{
				case Level::Trace:   logger->trace("{}", msg);    break;
				case Level::Info:    logger->info("{}", msg);     break;
				case Level::Warn:    logger->warn("{}", msg);     break;
				case Level::Error:   logger->error("{}", msg);    break;
				case Level::Fatal:   logger->critical("{}", msg); break;
			}
		}
	}


	template<typename... Args>
	void Log::PrintMessageTag(Log::Type type, Log::Level level, std::string_view tag, const std::format_string<Args...> format, Args&&... args)
	{
		auto detail = s_EnabledTags[std::string(tag)];
		if (detail.Enabled && detail.LevelFilter <= level)
		{
			auto logger = (type == Type::Core) ? GetCoreLogger() : GetClientLogger();
			std::string formatted = std::format(format, std::forward<Args>(args)...);
			switch (level)
			{
				case Level::Trace:
					logger->trace("[{0}] {1}", tag, formatted);
					break;
				case Level::Info:
					logger->info("[{0}] {1}", tag, formatted);
					break;
				case Level::Warn:
					logger->warn("[{0}] {1}", tag, formatted);
					break;
				case Level::Error:
					logger->error("[{0}] {1}", tag, formatted);
					break;
				case Level::Fatal:
					logger->critical("[{0}] {1}", tag, formatted);
					break;
			}
		}
	}


	inline void Log::PrintMessageTag(Log::Type type, Log::Level level, std::string_view tag, std::string_view message)
	{
		auto detail = s_EnabledTags[std::string(tag)];
		if (detail.Enabled && detail.LevelFilter <= level)
		{
			auto logger = (type == Type::Core) ? GetCoreLogger() : GetClientLogger();
			switch (level)
			{
				case Level::Trace:
					logger->trace("[{0}] {1}", tag, message);
					break;
				case Level::Info:
					logger->info("[{0}] {1}", tag, message);
					break;
				case Level::Warn:
					logger->warn("[{0}] {1}", tag, message);
					break;
				case Level::Error:
					logger->error("[{0}] {1}", tag, message);
					break;
				case Level::Fatal:
					logger->critical("[{0}] {1}", tag, message);
					break;
			}
		}
	}


	template<typename... Args>
	void Log::PrintAssertMessage(Log::Type type, std::string_view prefix, std::format_string<Args...> message, Args&&... args)
	{
		auto logger = (type == Type::Core) ? GetCoreLogger() : GetClientLogger();
		auto formatted = std::format(message, std::forward<Args>(args)...);
		logger->error("{0}: {1}", prefix, formatted);
	}


	inline void Log::PrintAssertMessage(Log::Type type, std::string_view prefix)
	{
		auto logger = (type == Type::Core) ? GetCoreLogger() : GetClientLogger();
		logger->error("{0}", prefix);
	}
}
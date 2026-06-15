// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <map>
#include <string>
#include <string_view>
#include <filesystem>

namespace CellForge {

	class ApplicationSettings
	{
	public:
		ApplicationSettings(const std::filesystem::path& filepath);

		void Serialize();
		bool Deserialize();

		bool HasKey(std::string_view key) const;

		std::string Get(std::string_view name, const std::string& defaultValue = "") const;
		float GetFloat(std::string_view name, float defaultValue = 0.0f) const;
		int GetInt(std::string_view name, int defaultValue = 0) const;

		void Set(std::string_view name, std::string_view value);
		void SetFloat(std::string_view name, float value);
		void SetInt(std::string_view name, int value);
	private:
		std::filesystem::path m_FilePath;
		std::map<std::string, std::string> m_Settings;
	};


}
// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include "CellForge/ecs/UUID.h"

#include <cstdint>
#include <random>

namespace CellForge {

	static std::random_device s_RandomDevice;
	static std::mt19937_64 eng(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	static std::mt19937 eng32(s_RandomDevice());
	static std::uniform_int_distribution<uint32_t> s_UniformDistribution32;

	UUID::UUID()
		: m_UUID(s_UniformDistribution(eng))
	{
	}

	UUID::UUID(uint64_t uuid)
		: m_UUID(uuid)
	{
	}

	UUID::UUID(const UUID& other)
		: m_UUID(other.m_UUID)
	{
	}


	UUID32::UUID32()
		: m_UUID(s_UniformDistribution32(eng32))
	{
	}

	UUID32::UUID32(uint32_t uuid)
		: m_UUID(uuid)
	{
	}

	UUID32::UUID32(const UUID32& other)
		: m_UUID(other.m_UUID)
	{
	}

}
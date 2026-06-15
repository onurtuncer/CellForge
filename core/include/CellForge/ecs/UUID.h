// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string_view>

namespace CellForge {

	// "UUID" (universally unique identifier) or GUID is (usually) a 128-bit integer
	// used to "uniquely" identify information. Even though we use the term GUID and
	// UUID, at the moment we're simply using a randomly generated 64-bit integer, as
	// the possibility of a clash is low enough for now.
	// This may change in the future.
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID& other);

		operator uint64_t () { return m_UUID; }
		operator const uint64_t () const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};

	class UUID32
	{
	public:
		UUID32();
		UUID32(uint32_t uuid);
		UUID32(const UUID32& other);

		operator uint32_t () { return m_UUID; }
		operator const uint32_t() const { return m_UUID; }
	private:
		uint32_t m_UUID;
	};

}

namespace std {

	template <>
	struct hash<CellForge::UUID>
	{
		std::size_t operator()(const CellForge::UUID& uuid) const
		{
			// uuid is already a randomly generated number, and is suitable as a hash key as-is.
			// this may change in future, in which case return hash<uint64_t>{}(uuid); might be more appropriate
			return uuid;
		}
	};

	template <>
	struct hash<CellForge::UUID32>
	{
		std::size_t operator()(const CellForge::UUID32& uuid) const
		{
			return hash<uint32_t>()((uint32_t)uuid);
		}
	};
}
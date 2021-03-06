//
//  SBJEV3VariableSizedEntity.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/18/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3TupleIter.h"

#include <type_traits>
#include <cassert>

namespace SBJ
{
namespace EV3
{

#pragma mark - VariableSizedEntity

struct VariableSizedEntity
{
	size_t pack(uint8_t* buffer) const
	{
		assert(false);
	}
};

template <typename Entity>
constexpr inline bool isVariableSizedEntity()
{
	return std::is_base_of<VariableSizedEntity, Entity>::value;
}

template <typename Entity, std::enable_if_t<isVariableSizedEntity<Entity>() == false>* = nullptr>
inline size_t packEntity(const Entity& entity, uint8_t* buffer)
{
	if (buffer) ::memcpy(buffer, &entity, sizeof(entity));
	return sizeof(entity);
}

template <typename Entity, std::enable_if_t<isVariableSizedEntity<Entity>() == true>* = nullptr>
inline size_t packEntity(const Entity& entity, uint8_t* buffer)
{
	return entity.pack(buffer);
}

#pragma mark - Tuple Aggregate

template <typename Tuple>
constexpr bool isVariableSizedTuple()
{
	bool isVariableSized = false;
	tuple_for_each<Tuple>([&isVariableSized](auto N, auto T)
	{
		isVariableSized = isVariableSizedEntity<typename decltype(T)::type>();
		return true;
	});
	return isVariableSized;
}

template <typename Tuple>
inline size_t packTuple(const Tuple& tuple, uint8_t* buffer)
{
	const uint8_t* start = buffer;
	if (isVariableSizedTuple<Tuple>())
	{
		tuple_for_each(tuple, [&tuple, &buffer](auto N, const auto& element)
		{
			size_t entityLen = packEntity(element, buffer);
			buffer += entityLen;
			return true;
		});
	}
	else
	{
		if (buffer != nullptr)
		{
			::memcpy(buffer, &tuple, sizeof(Tuple));
		}
		buffer += sizeof(Tuple);
	}
	return (buffer - start);
}

}
}

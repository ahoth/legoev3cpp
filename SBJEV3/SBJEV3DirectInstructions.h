//
//  SBJEV3DirectInstructions.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Opcodes.h"

#include <tuple>
#include <cassert>
#include <stdio.h>
namespace SBJ
{
namespace EV3
{

// TODO: support runtime sized opcodes. The previous NXT opcodes were all fixed size.
// EV3 opcodes use null termitated strings with variable allocated size. This
// DirectInstructions design assumed fixed allocation sizes for strings.
	
/*
 * DirectInstructions creates a buffer of a mini-program that can be sent to the EV3.
 */
 
template <typename... Opcodes>
class DirectInstructions
{
public:
	
	DirectInstructions(unsigned short counter, bool forceReply, Opcodes... opcodes)
	: _header
	{
		{
			sizeof(COMCMD) - sizeof(CMDSIZE) + sizeof(DIRCMD) + sizeof(AllOpcodes),
			counter,
			static_cast<UBYTE>(forceReply ? DIRECT_COMMAND_REPLY : DIRECT_COMMAND_NO_REPLY)
		},
		{
			0,
			0
		}
	}
	, _opcodes(ExtendedOpcode<Opcodes>(opcodes)...)
	{
		calculateReplies();
		
		_data = new uint8_t[size()];
		::memcpy(_data, &_header, sizeof(Header));
		::memcpy(_data + sizeof(Header) , &_opcodes, sizeof(AllOpcodes));
	}
	
	size_t size() const
	{
		return sizeof(Header) + sizeof(AllOpcodes);
	}
	
	const uint8_t* data() const
	{
		return _data;
	}
	
private:
	
#pragma pack(push, 1)
	
	template <typename Opcode>
	class ExtendedOpcode : Opcode
	{
	public:
		ExtendedOpcode(const Opcode& opcode)
		: Opcode(opcode)
		{
		}
		
		// Tells the EV3 where in the global space to store the resulting values.
		size_t setPositions(UBYTE startPosition)
		{
			size_t replySize = 0;
			for (size_t i = 0; i < Opcode::Result::ResultCount; i++)
			{
				_pos[i] = startPosition + replySize;
				replySize += Opcode::Result::allocatedSize(i);
			}
			return replySize;
		}
		
	private:
		// TODO: This may have to have an LValue as well
		GUValue _pos[Opcode::Result::ResultCount];
	};
	
	using AllOpcodes = std::tuple<ExtendedOpcode<Opcodes>...>;
	
	AllOpcodes _opcodes; // payload
	
	struct Header
	{
		COMCMD _cmd; // bytes { {0, 1}, {2, 3}, {4} }
		DIRCMD _vars; // bytes {5, 6}
	};
	
	Header _header;
	uint8_t* _data;
	
#pragma pack(pop)
	
	inline void calculateReplies()
	{
		UWORD replySize = 0;
		calculateReply(replySize, SizeT<0>());
		setVariables(replySize, 0);
	}
	
	template <size_t N>
	inline void calculateReply(UWORD& replySize, SizeT<N>)
	{
		// Set the global space positions for this reply
		// And increment the global space size
		auto& opcode = std::get<N>(_opcodes);
		replySize += opcode.setPositions(replySize);
		calculateReply(replySize, SizeT<N+1>());
	}
	
	inline void calculateReply(UWORD& replySize, SizeT<std::tuple_size<AllOpcodes>::value>)
	{
	}
	
	// The documentation states that direct command replies are always stored in the global space.
	// The reply buffer is a snapshot of the global space.
	// TODO: determine how LValues are used.
	inline void setVariables(UWORD globalSize, UWORD localSize)
	{
		assert(globalSize <= MAX_COMMAND_GLOBALS);
		assert(localSize <= MAX_COMMAND_LOCALS);
		
		// We want a reply if the opcode has reserved space
		if (localSize > 0 || globalSize > 0)
		{
			_header._cmd.Cmd = DIRECT_COMMAND_REPLY;
		}
		
		_header._vars.Globals = (UBYTE)(0x00FF & globalSize);
		_header._vars.Locals = (UBYTE)((0xFF00 & globalSize) >> 8);
		_header._vars.Locals |= (UBYTE)(localSize << 2);
	}
};


}
}
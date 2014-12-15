//
//  SBJEV3DirectCommand.h
//  LEGO Control
//
//  Created by David Giovannini on 11/21/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3DirectInstructions.h"
#include "SBJEV3DirectReply.h"

namespace SBJ
{
namespace EV3
{

/*
 * DirectCommand merges DirectInstructions with a DirectReply to create an Invocation
 */
 
template <typename... Opcodes>
class DirectCommand
{
public:
	using Results = typename DirectReply<Opcodes...>::Results;
	
	DirectCommand(unsigned short messageId, float timeout, Opcodes... opcodes)
	: _messageId(messageId)
	, _instructions(messageId, timeout > 0.0, opcodes...)
	, _reply(timeout)
	{
	}
	
	Invocation invocation()
	{
		return { _messageId, _instructions.data(), _instructions.size(), _reply.replyResponse() };
	}
	
	ReplyStatus status() const
	{
		return _reply.status();
	}
	
	const Results& wait()
	{
		return _reply.wait();
	}
	
private:
	unsigned short _messageId;
	DirectInstructions<Opcodes...> _instructions;
	DirectReply<Opcodes...> _reply;
};


}
}
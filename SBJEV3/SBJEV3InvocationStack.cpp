//
//  SBJEV3InvocationStack.cpp
//  LEGO Control
//
//  Created by David Giovannini on 11/30/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#include "SBJEV3InvocationStack.h"
#include "SBJEV3Connection.h"
#include "SBJEV3ByteCodes.h"
#include <iostream>

using namespace SBJ::EV3;

// TODO: implement a different logging technique

InvocationStack::InvocationStack()
: _connection(nullptr)
{	
}

InvocationStack::~InvocationStack()
{
}

#pragma mark - public thread-safe

void InvocationStack::connectionChange(Connection* connection)
{
	_connection.reset(connection);
	if (_connection)
	{
		_connection->start([this](const uint8_t* buffer, size_t len)
		{
			connectionReplied(buffer, len);
		});
	}
}

void InvocationStack::invoke(Invocation& invocation)
{
	const uint8_t* buffer = invocation.data();
	size_t size = invocation.size();
	{
		std::unique_lock<std::mutex> lock(_mutex);
		pushInvocation(invocation);
	}
	if (_connection)
	{
		if (_connection->write(buffer, size) == false)
		{
			remove(invocation.ID());
		}
	}
	else
	{
		remove(invocation.ID());
	}
}

void InvocationStack::remove(unsigned short messageId)
{
	std::unique_lock<std::mutex> lock(_mutex);
	removeInvocation(messageId);
}

void InvocationStack::connectionReplied(const uint8_t* buffer, size_t len)
{
	COMRPL* header = (COMRPL*)buffer;
	unsigned short invocationKey = header->MsgCnt;
	{
		std::unique_lock<std::mutex> lock(_mutex);
		replyInvocation(invocationKey, buffer, len);
	}
}

#pragma mark - private

void InvocationStack::pushInvocation(Invocation& invocation)
{
	if (invocation.wantsReply())
	{
		_invocations.insert(std::make_pair(invocation.ID(), std::move(invocation)));
		std::cout << "Sync " << invocation.ID() << std::endl;
	}
	else
	{
		std::cout << "Call " << invocation.ID() << std::endl;
	}
}

void InvocationStack::replyInvocation(unsigned short messageId, const uint8_t* buffer, size_t len)
{
	auto i = _invocations.find(messageId);
	if (i != _invocations.end())
	{
		bool complete = i->second.reply(buffer, len);
		std::cout << "Reply " << messageId << std::endl;
		if (complete)
		{
			_invocations.erase(i);
			std::cout << "Complete " << messageId << std::endl;
		}
	}
	else
	{
		errorInvocation(messageId);
	}
}

void InvocationStack::errorInvocation(unsigned short messageId)
{
	auto i = _invocations.find(messageId);
	if (i != _invocations.end())
	{
		i->second.reply(nullptr, 0);
		_invocations.erase(i);
		std::cout << "Error " << messageId << std::endl;
	}
	else
	{
		std::cout << "Unknown " << messageId << std::endl;
	}
}

void InvocationStack::removeInvocation(unsigned short messageId)
{
	auto i = _invocations.find(messageId);
	if (i != _invocations.end())
	{
		i->second.reply(nullptr, 0);
		_invocations.erase(i);
		std::cout << "Removed " << messageId << std::endl;
	}
}
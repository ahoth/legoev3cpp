//
//  ConnectionPreference.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

namespace SBJ
{
namespace EV3
{

enum class ConnectionTransport : char
{
	none = '\0',
	bluetooth = 'b',
	wifi = 'w',
	usb = 'u',
};

class ConnectionPreference
{
public:

	ConnectionPreference()
#if (TARGET_IPHONE_SIMULATOR)
	: _transport{ConnectionTransport::wifi, ConnectionTransport::bluetooth, ConnectionTransport::usb}
#else
	: _transport{ConnectionTransport::usb, ConnectionTransport::wifi, ConnectionTransport::bluetooth}
#endif
	{
	}
	
	ConnectionPreference(ConnectionTransport transport)
	: _transport{transport, ConnectionTransport::none, ConnectionTransport::none}
	{
	}
	
	void makePriority(ConnectionTransport transport)
	{
		ConnectionTransport t0 = _transport[0];
		if (t0 != transport)
		{
			_transport[0] = transport;
			ConnectionTransport t1 = _transport[1];
			_transport[1] = t0;
			if (t1 != transport)
			{
				_transport[2] = t1;
			}
		}
	}
	
	void addTransport(ConnectionTransport transport)
	{
		for (int i = 0; i < 3; i++)
		{
			if (_transport[i] == ConnectionTransport::none)
			{
				_transport[i] = transport;
				return;
			}
		}
	}
	
	void removeTransport(ConnectionTransport transport)
	{
		for (int i = 0; i < 3; i++)
		{
			if (_transport[i] == transport)
			{
				_transport[i] = ConnectionTransport::none;
				return;
			}
		}
	}
	
	const ConnectionTransport* begin() const
	{
		return _transport;
	}
	
	const ConnectionTransport* end() const
	{
		return _transport + 3;
	}
	
private:
	ConnectionTransport _transport[3];
};

}
}
//
//  SBJEV3ConnectionToken.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3DeviceIdentifier.h"

#include <string>
#include <functional>

namespace SBJ
{
namespace EV3
{

class ConnectionFactory;
class Connection;

using ConnectionChanged = std::function<void(DeviceIdentifier updatedIdentifier, Connection* connection)>;
using PromptBluetoothCompleted = std::function<void(bool canceled)>;

/*
 * ConnectionToken is an RAII connection change registration token
 */
 
class ConnectionToken
{
public:
	ConnectionToken(ConnectionFactory& factory, const DeviceIdentifier& identifier, ConnectionChanged action);
	
	~ConnectionToken();
	
	const DeviceIdentifier& identifier() const
	{
		return _identifier;
	}
	
	bool isConnected() const
	{
		return _connected;
	}
	
	bool makeConnection(const DeviceIdentifier& updatedIdentifier, Connection* connection);
	
	void promptBluetooth(PromptBluetoothCompleted completion = PromptBluetoothCompleted());
	
	void disconnect();
	
private:
	ConnectionFactory& _factory;
	DeviceIdentifier _identifier;
	ConnectionChanged _action;
	bool _connected = false;
};


}
}
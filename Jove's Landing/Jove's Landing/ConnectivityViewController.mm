//
//  FirstViewController.m
//  Jove's Landing
//
//  Created by David Giovannini on 12/14/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#import "ConnectivityViewController.h"
#include "SBJEV3Brick.h"
#include "SBJEV3Log.h"

using namespace SBJ::EV3;

@interface ConnectivityViewController ()
{
	Brick* _brick;
	IBOutlet UISwitch* _connected;
	IBOutlet UITableViewCell* _name;
	IBOutlet UITableViewCell* _serial;
	IBOutlet UIProgressView* _battery;
	IBOutlet UIImageView* _connectType;
}

@end

@implementation ConnectivityViewController

- (void)viewDidLoad 
{
	[super viewDidLoad];
	self.tableView.contentInset = UIEdgeInsetsMake(20, 0, 0, 0);
	
	UITapGestureRecognizer* tap = [[UITapGestureRecognizer alloc] initWithTarget: self action: @selector(renameBrick:)];
	tap.numberOfTapsRequired = 2;
	[_name addGestureRecognizer: tap];
}

- (void) viewWillAppear:(BOOL)animated
{
	[super viewWillAppear:animated];
	[self updateUI];
}

- (void) setBrick: (SBJ::EV3::Brick*) brick
{
	_brick = brick;
	if (self.isViewLoaded)
	{
		[self updateUI];
	}
}

- (IBAction) reconnect:(id)sender
{
	if (_connected.isOn)
	{
		_brick->prompt(^(Brick& brick, PromptAccessoryError error)
		{
			[self updateUI];
		});
	}
	else
	{
		_brick->disconnect();
	}
}

- (IBAction)promptBluetooth:(id)sender
{
	_brick->prompt(ConnectionTransport::bluetooth, ^(Brick& brick, PromptAccessoryError error)
	{
		[self updateUI];
	});
}

- (IBAction)promptWifi:(id)sender
{
	_brick->prompt(ConnectionTransport::wifi, ^(Brick& brick, PromptAccessoryError error)
	{
		[self updateUI];
	});
}

- (IBAction)promptUSB:(id)sender
{

	_brick->prompt(ConnectionTransport::usb, ^(Brick& brick, PromptAccessoryError error)
	{
		[self updateUI];
	});
}

- (void) renameBrick: (UITapGestureRecognizer*) recognizer
{
	if (_brick and _brick->isConnected())
	{
		UIAlertController* alert = [UIAlertController alertControllerWithTitle: @"Name" message: @"Up to 31 Characters" preferredStyle: UIAlertControllerStyleAlert];
		[alert addAction: [UIAlertAction actionWithTitle: @"OK" style: UIAlertActionStyleDefault handler: ^(UIAlertAction *action)
		{
			if ([[alert.textFields[0] text] length])
			{
				_brick->setName([[alert.textFields[0] text] UTF8String]);
				[self updateUI];
			}
		}]];
		[alert addAction: [UIAlertAction actionWithTitle: @"Cancel" style: UIAlertActionStyleCancel handler: nil]];
		[alert addTextFieldWithConfigurationHandler:^(UITextField *textField)
		{
			textField.placeholder = [NSString stringWithUTF8String: _brick->name().c_str()];
		}];
		[self presentViewController: alert animated: YES completion: nil];
	}
}

- (void) updateUI
{
	if (_brick)
	{
		_connected.on = _brick->isConnected();
		// Seriously Apple? Setting the text to empty string makes future setting invalid!
		_name.detailTextLabel.text = [NSString stringWithUTF8String: (_brick->name() + " ").c_str()];
		_serial.detailTextLabel.text = [NSString stringWithUTF8String: _brick->identifier().serial.c_str()];
		switch (_brick->connectionType())
		{
			case ConnectionTransport::usb:
				_connectType.image = [UIImage imageNamed: @"USB"];
				break;
			case ConnectionTransport::bluetooth:
				_connectType.image = [UIImage imageNamed: @"Bluetooth"];
				break;
			case ConnectionTransport::wifi:
				_connectType.image = [UIImage imageNamed: @"WIFI"];
				break;
			case ConnectionTransport::none:
				_connectType.image = [UIImage imageNamed: @"None"];
				break;
		}
		if (_brick->isConnected())
		{
			Brick::Battery battery = _brick->battery();
			_battery.progress = battery.indicator();
		}
		else
		{
			_battery.progress = 0.0;
		}
	}
	else
	{
		_connected.on = false;
		_name.detailTextLabel.text = @"N/A";
		_serial.detailTextLabel.text = @"N/A";
		_connectType.image = [UIImage imageNamed: @"None"];
		_battery.progress = 0.0;
	}
}

@end

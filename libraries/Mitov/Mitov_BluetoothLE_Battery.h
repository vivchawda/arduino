////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BLUETOOTH_LE_BATTERY_h
#define _MITOV_BLUETOOTH_LE_BATTERY_h

#include <Mitov.h>
#include <Mitov_BluetoothLE.h>

namespace Mitov
{
	class ArduinoBluetoothLEBatteryService : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		float	InitialValue	= 0.0;

	public:
		ArduinoBluetoothLE &FOwner;

	protected:
		BLEService						*FService;
		BLEUnsignedCharCharacteristic	*FCharacteristic;

	protected:
		void DoReceive( void *_Data )
		{
			float AValue = *(float *)_Data;
			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			FCharacteristic->setValue( InitialValue * 100 + 0.5 );
		}

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			FService = new BLEService( "180F" ); // Add -0000-1000-8000-00805F9B34FB for standard
			FOwner.FPeripheral.addAttribute( *FService );
			FOwner.FPeripheral.setAdvertisedServiceUuid( FService->uuid());
			FCharacteristic = new BLEUnsignedCharCharacteristic( "2A19", BLERead | BLENotify );
			FOwner.FPeripheral.addAttribute( *FCharacteristic );
			FCharacteristic->setValue( InitialValue * 100 + 0.5 );
		}

	public:
		ArduinoBluetoothLEBatteryService( ArduinoBluetoothLE &AOwner ) :
			FOwner( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoBluetoothLEBatteryService::DoReceive );
		}
	};
//---------------------------------------------------------------------------
}

#endif

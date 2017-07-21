////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BLUETOOTH_LE_HEARTHRATE_h
#define _MITOV_BLUETOOTH_LE_HEARTHRATE_h

#include <Mitov.h>
#include <Mitov_BluetoothLE.h>

namespace Mitov
{
	class ArduinoBluetoothLEHearthRateService : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;

#ifdef _MITOV_BLE_HEARTHRATE_CONTACT_PIN_
		OpenWire::ValueSimpleSinkPin<bool>	ContactInputPin;
#endif

#ifdef _MITOV_BLE_HEARTHRATE_ENERGY_EXPANDED_PIN_
		OpenWire::ValueSimpleSinkPin<float>	EnergyExpandedInputPin;
#endif

#ifdef _MITOV_BLE_HEARTHRATE_RESET_ENERGY_EXPANDED_PIN_
		OpenWire::SourcePin	ResetEnergyExpendedOutputPin;
#endif

	public:
		float	InitialValue	= 0.0;
		bool	TwoBytes = false;
		TArduinoBluetoothSensorLocation	Location = slNone;

	public:
		ArduinoBluetoothLE &FOwner;

	protected:
		BLEService						*FService;
		BLECharacteristic				*FCharacteristic;
		BLEUnsignedCharCharacteristic	*FLocationCharacteristic;
#ifdef _MITOV_BLE_HEARTHRATE_RESET_ENERGY_EXPANDED_PIN_
		BLEUnsignedCharCharacteristic	*FControlPointCharacteristic;
#endif

	protected:
		void DoReceive( void *_Data )
		{
			float AValue = *(float *)_Data;
			if( AValue < 0 )
				AValue = 0;

			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			SendData();
		}

		void SendData()
		{
//			const uint8_t heartRateCharArray[2] = { 0, (uint8_t)( InitialValue + 0.5 ) };
//			FCharacteristic->setValue( heartRateCharArray, 2 );
			uint8_t AFlags = 0;
#ifdef _MITOV_BLE_HEARTHRATE_CONTACT_PIN_
			if( ContactInputPin.Value )
				AFlags |= 0b110; // Contact

			else
				AFlags |= 0b100; // No Contact
#endif // _MITOV_BLE_HEARTHRATE_CONTACT_PIN_

#ifdef _MITOV_BLE_HEARTHRATE_ENERGY_EXPANDED_PIN_
			AFlags |= 0b1000;
#endif // _MITOV_BLE_HEARTHRATE_ENERGY_EXPANDED_PIN_

			if( TwoBytes )
			{
				uint16_t AValue16 =  InitialValue + 0.5;

#ifdef _MITOV_BLE_HEARTHRATE_ENERGY_EXPANDED_PIN_
				uint16_t AEnergy16 =  EnergyExpandedInputPin.Value + 0.5;

				const uint8_t heartRateCharArray[5] = { uint8_t( AFlags | 1 ), uint8_t( AValue16 ), uint8_t( AValue16 >> 8 ), uint8_t( AEnergy16 ), uint8_t( AEnergy16 >> 8 ) };
				FCharacteristic->setValue( heartRateCharArray, 5 );
#else
				const uint8_t heartRateCharArray[3] = { uint8_t( AFlags | 1 ), uint8_t( AValue16 ), uint8_t( AValue16 >> 8 ) };
				FCharacteristic->setValue( heartRateCharArray, 3 );
#endif
			}
			else
			{
#ifdef _MITOV_BLE_HEARTHRATE_ENERGY_EXPANDED_PIN_
				uint16_t AEnergy16 =  EnergyExpandedInputPin.Value + 0.5;

				const uint8_t heartRateCharArray[4] = { AFlags, uint8_t( InitialValue + 0.5 ), uint8_t( AEnergy16 ), uint8_t( AEnergy16 >> 8 ) };
				FCharacteristic->setValue( heartRateCharArray, 4 );
#else
				const uint8_t heartRateCharArray[2] = { AFlags, uint8_t( InitialValue + 0.5 ) };
				FCharacteristic->setValue( heartRateCharArray, 2 );
#endif
			}
		}

	protected:
#ifdef _MITOV_BLE_HEARTHRATE_RESET_ENERGY_EXPANDED_PIN_
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );
			if( FControlPointCharacteristic->written() )
				if( FControlPointCharacteristic->value() == 1 )
					ResetEnergyExpendedOutputPin.Notify( nullptr );
			
		}
#endif // _MITOV_BLE_HEARTHRATE_RESET_ENERGY_EXPANDED_PIN_

		virtual void SystemInit() override
		{
			inherited::SystemInit();
			FService = new BLEService( "180D" ); // Add -0000-1000-8000-00805F9B34FB for standard
			FOwner.FPeripheral.addAttribute( *FService );
			FOwner.FPeripheral.setAdvertisedServiceUuid( FService->uuid());

#ifdef _MITOV_BLE_HEARTHRATE_ENERGY_EXPANDED_PIN_
			if( TwoBytes )
				FCharacteristic = new BLECharacteristic( "2A37", BLERead | BLENotify, 5 );

			else
				FCharacteristic = new BLECharacteristic( "2A37", BLERead | BLENotify, 4 );
#else // _MITOV_BLE_HEARTHRATE_ENERGY_EXPANDED_PIN_
			if( TwoBytes )
				FCharacteristic = new BLECharacteristic( "2A37", BLERead | BLENotify, 3 );

			else
				FCharacteristic = new BLECharacteristic( "2A37", BLERead | BLENotify, 2 );
#endif // _MITOV_BLE_HEARTHRATE_ENERGY_EXPANDED_PIN_

			FOwner.FPeripheral.addAttribute( *FCharacteristic );

			if( Location != slNone )
			{
				FLocationCharacteristic = new BLEUnsignedCharCharacteristic( "2A38", BLERead );
				FLocationCharacteristic->setValue( uint8_t( Location ) - 1 );
				FOwner.FPeripheral.addAttribute( *FLocationCharacteristic );
			}

#ifdef _MITOV_BLE_HEARTHRATE_RESET_ENERGY_EXPANDED_PIN_
			FControlPointCharacteristic = new BLEUnsignedCharCharacteristic( "2A39", BLEWrite );
			FOwner.FPeripheral.addAttribute( *FControlPointCharacteristic );
#endif // _MITOV_BLE_HEARTHRATE_RESET_ENERGY_EXPANDED_PIN_

			SendData();
		}

	public:
		ArduinoBluetoothLEHearthRateService( ArduinoBluetoothLE &AOwner ) :
			FOwner( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoBluetoothLEHearthRateService::DoReceive );
//#ifdef _MITOV_BLE_HEARTHRATE_ENERGY_EXPANDED_PIN_
//			EnergyExpandedInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoBluetoothLEHearthRateService::DoEnergyExpandedReceive );
//#endif
		}
	};
//---------------------------------------------------------------------------
}

#endif

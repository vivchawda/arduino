////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_I2C_SCANNER_h
#define _MITOV_I2C_SCANNER_h

#include <Mitov.h>
#include <Wire.h>

namespace Mitov
{
	class I2CScaner : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	ScanInputPin;

		OpenWire::SourcePin	ScanningOutputPin;
		OpenWire::SourcePin	ScanningAddressOutputPin;
		OpenWire::SourcePin	AddressOutputPin;

	protected:
		TwoWire &FWire;

	protected:
		virtual void DoScanReceive( void *_Data )
		{
			ScanningOutputPin.SendValue( true );
			for( uint8_t address = 1; address < 127; address++ ) 
			{
				ScanningAddressOutputPin.SendValue<uint32_t>( address );
				// The i2c_scanner uses the return value of
				// the Write.endTransmisstion to see if
				// a device did acknowledge to the address.
				FWire.beginTransmission(address);
				uint8_t error = FWire.endTransmission();

				if (error == 0)
				{
					AddressOutputPin.SendValue<uint32_t>( address );
				}
/*
				else if (error==4) 
				{
				}    
*/
			}

			ScanningOutputPin.SendValue( false );
		}

	protected:
		virtual void SystemStart() override
		{
			inherited::SystemStart();
			if( ! ScanningOutputPin.IsConnected() )
				DoScanReceive( nullptr );

		}

	public:
		I2CScaner( TwoWire &AWire ) :
			FWire( AWire )
		{
			ScanInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&I2CScaner::DoScanReceive );
		}

	};
}

#endif

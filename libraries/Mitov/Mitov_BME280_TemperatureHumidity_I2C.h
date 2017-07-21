////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BME280_TEMPERATUREHUMIDITY_I2C_h
#define _MITOV_BME280_TEMPERATUREHUMIDITY_I2C_h

#include <Mitov_BME280_TemperatureHumidity.h>
#include <Wire.h> //I2C Arduino Library

namespace Mitov
{
//---------------------------------------------------------------------------
	class BME280_TemperatureHumidityI2C : public BME280_TemperatureHumidityBasic
	{
		typedef	BME280_TemperatureHumidityBasic	inherited;

	protected:
		TwoWire	&FWire;

	protected:
		uint8_t GetAddress()
		{
			if( Address )
				return 0x76;

			return 0x77;
		}

		virtual uint8_t read8(uint8_t reg) override
		{
			uint8_t AAddress = GetAddress();
			FWire.beginTransmission( AAddress );
			FWire.write( reg );
			FWire.endTransmission();
			FWire.requestFrom( AAddress, (uint8_t)1 );
			return FWire.read();
		}

		virtual uint16_t read16(uint8_t reg) override
		{
			uint8_t AAddress = GetAddress();
			FWire.beginTransmission( AAddress );
			FWire.write( reg );
			FWire.endTransmission();
			FWire.requestFrom( AAddress, (uint8_t)2 );
			uint16_t AValue = FWire.read();
			return ( AValue << 8 ) | FWire.read();
		}

		virtual uint16_t read16_LE(uint8_t reg) override
		{
			uint8_t AAddress = GetAddress();
			FWire.beginTransmission( AAddress );
			FWire.write( reg );
			FWire.endTransmission();
			FWire.requestFrom( AAddress, (uint8_t)2 );
			uint16_t AValue = FWire.read();
			return AValue | ( uint16_t( FWire.read() ) << 8 );
		}

		virtual uint32_t read24(uint8_t reg) override
		{
			uint8_t AAddress = GetAddress();
			FWire.beginTransmission( AAddress );
			FWire.write( reg );
			FWire.endTransmission();
			FWire.requestFrom( AAddress, (uint8_t)3 );
    
			uint32_t AValue = FWire.read();
			AValue <<= 8;
			AValue |= FWire.read();
			AValue <<= 8;
			AValue |= FWire.read();

			return AValue;
		}

		virtual void write8(uint8_t reg, uint8_t value) override
		{
			FWire.beginTransmission( GetAddress() );
			FWire.write((uint8_t)reg);
			FWire.write((uint8_t)value);
			FWire.endTransmission();
		}

	public:
		BME280_TemperatureHumidityI2C( TwoWire &AWire ) :
			FWire( AWire )
		{
		}
	};
//---------------------------------------------------------------------------
}

#endif

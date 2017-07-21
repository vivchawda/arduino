////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BME280_TEMPERATUREHUMIDITY_SPI_h
#define _MITOV_BME280_TEMPERATUREHUMIDITY_SPI_h

#include <Mitov_BME280_TemperatureHumidity.h>
#include <Mitov_Basic_SPI.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif


namespace Mitov
{
//---------------------------------------------------------------------------
	class BME280_TemperatureHumiditySPI : public BME280_TemperatureHumidityBasic
	{
		typedef	BME280_TemperatureHumidityBasic	inherited;

	public:
		OpenWire::SourcePin	ChipSelectOutputPin;

	protected:
		BasicSPI	&FSPI;

	protected:
		virtual uint8_t read8(uint8_t reg) override
		{
			FSPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
			ChipSelectOutputPin.SendValue<bool>( false );

			FSPI.transfer( reg | 0x80 ); // read, bit 7 high
			uint8_t AValue = FSPI.transfer( 0 );

			ChipSelectOutputPin.SendValue<bool>( true );
			FSPI.endTransaction(); 

			return AValue;
		}

		virtual uint16_t read16(uint8_t reg) override
		{
			FSPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
			ChipSelectOutputPin.SendValue<bool>( false );

			FSPI.transfer( reg | 0x80 ); // read, bit 7 high
			uint16_t AValue = FSPI.transfer( 0 );
			AValue = ( AValue << 8 ) | FSPI.transfer( 0 );

			ChipSelectOutputPin.SendValue<bool>( true );
			FSPI.endTransaction(); 

			return AValue;
		}

		virtual uint16_t read16_LE(uint8_t reg) override
		{
			FSPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
			ChipSelectOutputPin.SendValue<bool>( false );

			FSPI.transfer( reg | 0x80 ); // read, bit 7 high
			uint16_t AValue = FSPI.transfer( 0 );
			AValue |= uint16_t( FSPI.transfer( 0 ) ) << 8;

			ChipSelectOutputPin.SendValue<bool>( true );
			FSPI.endTransaction(); 

			return AValue;
		}

		virtual uint32_t read24(uint8_t reg) override
		{
			FSPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
			ChipSelectOutputPin.SendValue<bool>( false );

			FSPI.transfer( reg | 0x80 ); // read, bit 7 high
			uint32_t AValue = FSPI.transfer( 0 );
			AValue <<= 8;
			AValue |= FSPI.transfer( 0 );
			AValue <<= 8;
			AValue |= FSPI.transfer( 0 );

			ChipSelectOutputPin.SendValue<bool>( true );
			FSPI.endTransaction(); 

			return AValue;
		}

		virtual void write8(uint8_t reg, uint8_t value) override
		{
			FSPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
			ChipSelectOutputPin.SendValue<bool>( false );

			FSPI.transfer( reg | 0x80 ); // read, bit 7 high
			FSPI.transfer( value );

			ChipSelectOutputPin.SendValue<bool>( true );
			FSPI.endTransaction(); 
		}

	public:
		BME280_TemperatureHumiditySPI( BasicSPI &ASPI ) :
			FSPI( ASPI )
		{
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

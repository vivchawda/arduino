////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MICROCHIP_MCP4725_h
#define _MITOV_MICROCHIP_MCP4725_h

#include <Mitov.h>
#include <Wire.h> //I2C Arduino Library

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	enum TArduinoMicrochipMCP4725ShitDownMode { sdm1K, sdm100K, sdm500K };
//---------------------------------------------------------------------------
	class Microchip_MCP4725 : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin			InputPin;
		OpenWire::ConnectSinkPin	ClockInputPin;

	public:
		float	InitialValue = 0;
		float	CurrentValue = 0;

		bool	Enabled : 1;
		bool	ShitDown : 1;
		bool	WriteToEEPROM : 1;
		TArduinoMicrochipMCP4725ShitDownMode	ShitDownMode : 2;

	protected:
		uint8_t FAdress : 3;

		TwoWire &FWire;

	protected:
		static const uint8_t	MCP4726_CMD_WRITEDAC		= 0x40;  // Writes data to the DAC
		static const uint8_t	MCP4726_CMD_WRITEDACEEPROM  = 0x60;  // Writes data to the DAC and the EEPROM (persisting the assigned value after reset)

	protected:
		void DoReceive( void *_Data )
		{
			InitialValue = *(float * )_Data;
			if( Enabled )
				if( ! ClockInputPin.IsConnected() )
					DoClockReceive( nullptr );
		}

		void DoClockReceive( void *_Data )
		{
			if( ! Enabled )
				return;

//			Serial.println( "DoClockReceive" );
			CurrentValue = InitialValue;
			UpdateValue();
		}

	public:
		void UpdateValue()
		{
			uint16_t AValue = constrain( CurrentValue, 0.0, 1.0 ) * 0xFFF + 0.5;
#ifdef TWBR
			uint8_t twbrback = TWBR;
			TWBR = ((F_CPU / 400000L) - 16) / 2; // Set I2C frequency to 400kHz
#endif
			FWire.beginTransmission( 0x60 | uint8_t( FAdress ));
//			FWire.beginTransmission( 0x62 );

			uint8_t	ACommand;
  			if( WriteToEEPROM )
				ACommand = MCP4726_CMD_WRITEDACEEPROM;

			else
				ACommand = MCP4726_CMD_WRITEDAC;

			if( ShitDown )
				ACommand |= uint8_t( ShitDownMode ) + 1;

			FWire.write( ACommand );

//			Serial.println( ACommand, HEX );
//			Serial.println( CurrentValue );
//			Serial.println( AValue );

			FWire.write( AValue / 16);                   // Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
			FWire.write(( AValue % 16) << 4);            // Lower data bits          (D3.D2.D1.D0.x.x.x.x)
			FWire.endTransmission();

#ifdef TWBR
			TWBR = twbrback;
#endif		
		}

	protected:
/*
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			if( Enabled )
//				if( ! ClockInputPin.IsConnected() )
//					DoClockReceive( nullptr );

//			inherited::SystemLoopBegin( currentMicros );
		}
*/
		virtual void SystemStart() override
		{
			if( Enabled )
				if( ! ClockInputPin.IsConnected() )
					DoClockReceive( nullptr );

//			inherited::SystemStart();
		}

	public:
		Microchip_MCP4725( uint8_t AAdress, TwoWire &AWire ) :
			FAdress( AAdress & 0x7 ),
			Enabled( true ),
			ShitDown( false ),
			WriteToEEPROM( false ),
			ShitDownMode( sdm500K ),
			FWire( AWire )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Microchip_MCP4725::DoReceive );
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Microchip_MCP4725::DoClockReceive );
		}

	};
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

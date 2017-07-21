////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ROHM_BH1750_H
#define _MITOV_ROHM_BH1750_H

#include <Mitov.h>

namespace Mitov
{
	enum TArduinoLightSensorRohmBH1750ResolutionMode { rmVeryHigh, rmHigh, rmLow };

	class LightSensorBH1750 : public OpenWire::Component
	{

		// No active state
		static const uint8_t BH1750_POWER_DOWN = 0x00;

		// Wating for measurment command
		static const uint8_t BH1750_POWER_ON = 0x01;

		// Reset data register value - not accepted in POWER_DOWN mode
		static const uint8_t BH1750_RESET = 0x07;

		// Start measurement at 1lx resolution. Measurement time is approx 120ms.
		static const uint8_t BH1750_CONTINUOUS_HIGH_RES_MODE  = 0x10;

		// Start measurement at 0.5lx resolution. Measurement time is approx 120ms.
		static const uint8_t BH1750_CONTINUOUS_HIGH_RES_MODE_2  = 0x11;

		// Start measurement at 4lx resolution. Measurement time is approx 16ms.
		static const uint8_t BH1750_CONTINUOUS_LOW_RES_MODE  = 0x13;

		// Start measurement at 1lx resolution. Measurement time is approx 120ms.
		// Device is automatically set to Power Down after measurement.
		static const uint8_t BH1750_ONE_TIME_HIGH_RES_MODE  = 0x20;

		// Start measurement at 0.5lx resolution. Measurement time is approx 120ms.
		// Device is automatically set to Power Down after measurement.
		static const uint8_t BH1750_ONE_TIME_HIGH_RES_MODE_2  = 0x21;

		// Start measurement at 1lx resolution. Measurement time is approx 120ms.
		// Device is automatically set to Power Down after measurement.
		static const uint8_t BH1750_ONE_TIME_LOW_RES_MODE  = 0x23;

	public:
		OpenWire::ConnectSinkPin	ClockInputPin;
		OpenWire::ConnectSinkPin	ResetInputPin;
		OpenWire::SourcePin			OutputPin;

	public:
		float	MeasurementTime = 1.0;

		bool	Enabled : 1;
		bool	ContinuousMode : 1;
		bool	InLumens : 1;
		TArduinoLightSensorRohmBH1750ResolutionMode	Resolution : 2;

	public:
		bool	Address : 1;

	protected:
		TwoWire	&FWire;

	public:
		void UpdateEnabled()
		{
			if( ! Enabled )
			{
				write8( BH1750_POWER_DOWN );
				return;
			}

			write8( BH1750_POWER_ON );
            delay(10);

			uint8_t AMode;
			if( ContinuousMode || !ClockInputPin.IsConnected()  )
			{
				switch ( Resolution )
				{
					case rmVeryHigh : AMode = BH1750_CONTINUOUS_HIGH_RES_MODE_2; break; 
					case rmHigh : AMode = BH1750_CONTINUOUS_HIGH_RES_MODE; break; 
					case rmLow : AMode = BH1750_CONTINUOUS_LOW_RES_MODE; break; 
				}
			}
			else
			{
				switch ( Resolution )
				{
					case rmVeryHigh : AMode = BH1750_ONE_TIME_HIGH_RES_MODE_2; break; 
					case rmHigh : AMode = BH1750_ONE_TIME_HIGH_RES_MODE; break; 
					case rmLow : AMode = BH1750_ONE_TIME_LOW_RES_MODE; break; 
				}
			}

			write8( AMode );
            delay(10);
			UpdateMeasurementTime();
		}

		void UpdateMeasurementTime()
		{
			uint8_t ATime = Func::MapRange<float>( MeasurementTime, 0.45, 3.68, 31, 254 ) + 0.5;

			uint8_t AHighBits = 0b01000000 | ( ATime >> 5 );
			write8( AHighBits );

			uint8_t ALowBits = 0b01100000 | ( ATime & 0b11111 );
			write8( ALowBits );
		}

	protected:
		void write8(uint8_t AData ) 
		{
			FWire.beginTransmission( Address ? 0x5C : 0x23 );
			FWire.write( AData );
			FWire.endTransmission();
		}

	protected:
		void DoClockReceive( void *_Data )
		{
			if( ! Enabled )
				return;

			uint16_t level;

			uint8_t AAddr = Address ? 0x5C : 0x23;

			FWire.beginTransmission( AAddr );
			FWire.requestFrom( AAddr, uint8_t( 2 ));
			level = FWire.read();
			level <<= 8;
			level |= FWire.read();
			FWire.endTransmission();

			float AFloatLevel = level;
			AFloatLevel = InLumens ? AFloatLevel / 1.2 : AFloatLevel / 0xFFFF;
			OutputPin.Notify( &AFloatLevel );
		}

		void DoResetReceive( void *_Data )
		{
			if( Enabled )
				write8( BH1750_RESET );
		}

	protected:
		virtual void SystemStart() override
		{
			UpdateEnabled();
//			if( Enabled )
//				if( ! ClockInputPin.IsConnected() )
//					StartReadingTemp();

//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ! ClockInputPin.IsConnected() )
				DoClockReceive( nullptr );

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		LightSensorBH1750( TwoWire &AWire ) :
			Enabled( true ),
			ContinuousMode( true ),
			InLumens( true ),
			Resolution( rmVeryHigh ),
			Address( false ),
			FWire( AWire )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&LightSensorBH1750::DoClockReceive );
			ResetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&LightSensorBH1750::DoResetReceive );
		}

	};

}

#endif

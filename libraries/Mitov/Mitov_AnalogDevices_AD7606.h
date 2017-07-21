////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ANALOGDEVICES_AD7606_h
#define _MITOV_ANALOGDEVICES_AD7606_h

#include <Mitov.h>

namespace Mitov
{
	enum TArduinoAD7606Oversampling { aosNone, aos2, aos4, aos8, aos16, aos32, aos64 };
//---------------------------------------------------------------------------
	class AD7606Parallel16bit : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	protected:
		static const uint8_t RESET_PIN = 16;
		static const uint8_t CHIP_SELECT_PIN = 17;
		static const uint8_t READ_PIN = 18;
		static const uint8_t START_PIN = 19;
		static const uint8_t BUSY_PIN = 20;

	public:
		OpenWire::SourcePin			OutputPins[ 8 ];
		OpenWire::SourcePin			OversamplingOutputPins[ 3 ];
		OpenWire::ConnectSinkPin	ClockInputPin;
		OpenWire::ConnectSinkPin	ResetInputPin;

	public:
		bool	Enabled : 1;

		TArduinoAD7606Oversampling Oversampling : 3;

	protected:
		uint16_t FPins[ 16 + 5 ];

	protected:
		void DoClockReceive( void *_Data )
		{
			if( ! Enabled )
				return;

//			Serial.println( "READ" );
			digitalWrite( FPins[ START_PIN ], LOW );

			unsigned long ACurrentTime = micros();

			digitalWrite( FPins[ START_PIN ], HIGH );

//			Serial.println( "WAIT" );
			while( micros() - ACurrentTime < 4 ); // Wait 4 uS
//			Serial.println( "WAIT1" );
			while( digitalRead( FPins[ BUSY_PIN ] ) != LOW ); // Wait to finish reading
//			Serial.println( "WAIT2" );

			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

			for( int AChannel = 0; AChannel < 8; ++AChannel )
			{
				digitalWrite( FPins[ READ_PIN ], LOW );

				uint16_t AResult = 0;
				for( int i = 0; i < 16; ++i )
					if( digitalRead( FPins[ i ] ) == HIGH )
						AResult |= ( 1 << i );

				digitalWrite( FPins[ READ_PIN ], HIGH );

//				if( ! AChannel )
//					Serial.println( AResult, HEX );

				float AValue = float( int16_t( AResult )) / 0x7FFF;
				OutputPins[ AChannel ].Notify( &AValue );
			}

			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
		}

		void DoResetReceive( void *_Data )
		{
//			Serial.println( "RESET" );
			digitalWrite( FPins[ RESET_PIN ], HIGH );
			delay( 1 );
			digitalWrite( FPins[ RESET_PIN ], LOW );
		}

	protected:
		virtual void SystemStart() override
		{
			for( int i = 0; i < 15; ++ i )
				pinMode( FPins[ i ], INPUT );

			for( int i = 16; i < 16 + 4; ++ i )
				pinMode( FPins[ i ], OUTPUT );

			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
			digitalWrite( FPins[ READ_PIN ], HIGH );
			digitalWrite( FPins[ START_PIN ], HIGH );
			digitalWrite( FPins[ RESET_PIN ], LOW );

			pinMode( FPins[ BUSY_PIN ], INPUT );
			
			for( int i = 0; i < 3; ++i )
				OversamplingOutputPins[ i ].SendValue<bool>( Oversampling & ( 1 << i ) != 0 );
			
			DoResetReceive( nullptr );
//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ! ClockInputPin.IsConnected() )
				DoClockReceive( nullptr );

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		AD7606Parallel16bit( uint16_t AD0, uint16_t AD1, uint16_t AD2, uint16_t AD3, uint16_t AD4, uint16_t AD5, uint16_t AD6, uint16_t AD7, uint16_t AD8, uint16_t AD9, uint16_t AD10, uint16_t AD11, uint16_t AD12, uint16_t AD13, uint16_t AD14, uint16_t AD15, uint16_t AReset, uint16_t AChipSelect, uint16_t ARead, uint16_t AStart, uint16_t ABusy ) :
			Enabled( true ),
			Oversampling( aosNone )
		{
			FPins[ 0 ] = AD0;
			FPins[ 1 ] = AD1;
			FPins[ 2 ] = AD2;
			FPins[ 3 ] = AD3;
			FPins[ 4 ] = AD4;
			FPins[ 5 ] = AD5;
			FPins[ 6 ] = AD6;
			FPins[ 7 ] = AD7;
			FPins[ 8 ] = AD8;
			FPins[ 9 ] = AD9;
			FPins[ 10 ] = AD10;
			FPins[ 11 ] = AD11;
			FPins[ 12 ] = AD12;
			FPins[ 13 ] = AD13;
			FPins[ 14 ] = AD14;
			FPins[ 15 ] = AD15;
			FPins[ RESET_PIN ] = AReset;
			FPins[ CHIP_SELECT_PIN ] = AChipSelect;
			FPins[ READ_PIN ] = ARead;
			FPins[ START_PIN ] = AStart;
			FPins[ BUSY_PIN ] = ABusy;

			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AD7606Parallel16bit::DoClockReceive );
			ResetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AD7606Parallel16bit::DoResetReceive );
		}
	};
//---------------------------------------------------------------------------
	class AD7606Parallel8bit : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	protected:
		static const uint8_t RESET_PIN = 8;
		static const uint8_t CHIP_SELECT_PIN = 9;
		static const uint8_t READ_PIN = 10;
		static const uint8_t START_PIN = 11;
		static const uint8_t BUSY_PIN = 12;

	public:
		OpenWire::SourcePin			OutputPins[ 8 ];
		OpenWire::SourcePin			OversamplingOutputPins[ 3 ];
		OpenWire::ConnectSinkPin	ClockInputPin;
		OpenWire::ConnectSinkPin	ResetInputPin;

	public:
		bool	Enabled : 1;
		bool	MostSignificantFirst : 1;

		TArduinoAD7606Oversampling Oversampling : 3;

	protected:
		uint16_t FPins[ 8 + 5 ];

	protected:
		void DoClockReceive( void *_Data )
		{
			if( ! Enabled )
				return;

//			Serial.println( "READ" );
			digitalWrite( FPins[ START_PIN ], LOW );

			unsigned long ACurrentTime = micros();

			digitalWrite( FPins[ START_PIN ], HIGH );

//			Serial.println( "WAIT" );
			while( micros() - ACurrentTime < 4 ); // Wait 4 uS
//			Serial.println( "WAIT1" );
			while( digitalRead( FPins[ BUSY_PIN ] ) != LOW ); // Wait to finish reading
//			Serial.println( "WAIT2" );

			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

			for( int AChannel = 0; AChannel < 8; ++AChannel )
			{
				uint16_t AResult = 0;

				digitalWrite( FPins[ READ_PIN ], LOW );

				if( MostSignificantFirst )
				{
					for( int i = 0; i < 8; ++i )
						if( digitalRead( FPins[ i ] ) == HIGH )
							AResult |= ( 1 << ( 8 + i ));
				}

				else
				{
					for( int i = 0; i < 8; ++i )
						if( digitalRead( FPins[ i ] ) == HIGH )
							AResult |= ( 1 << i );
				}

				digitalWrite( FPins[ READ_PIN ], HIGH );

//				delay( 100 );

				digitalWrite( FPins[ READ_PIN ], LOW );

				if( MostSignificantFirst )
				{
					for( int i = 0; i < 8; ++i )
						if( digitalRead( FPins[ i ] ) == HIGH )
							AResult |= ( 1 << i );
				}

				else
				{
					for( int i = 0; i < 8; ++i )
						if( digitalRead( FPins[ i ] ) == HIGH )
							AResult |= ( 1 << ( 8 + i ));
				}

				digitalWrite( FPins[ READ_PIN ], HIGH );

//				if( ! AChannel )
//					Serial.println( AResult, HEX );

				float AValue = float( int16_t( AResult )) / 0x7FFF;
				OutputPins[ AChannel ].Notify( &AValue );
			}

			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
		}

		void DoResetReceive( void *_Data )
		{
//			Serial.println( "RESET" );
			digitalWrite( FPins[ RESET_PIN ], HIGH );
			delay( 1 );
			digitalWrite( FPins[ RESET_PIN ], LOW );
		}

	protected:
		virtual void SystemStart() override
		{
			for( int i = 0; i < 15; ++ i )
				pinMode( FPins[ i ], INPUT );

			for( int i = 16; i < 16 + 4; ++ i )
				pinMode( FPins[ i ], OUTPUT );

			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
			digitalWrite( FPins[ READ_PIN ], HIGH );
			digitalWrite( FPins[ START_PIN ], HIGH );
			digitalWrite( FPins[ RESET_PIN ], LOW );

			pinMode( FPins[ BUSY_PIN ], INPUT );
			
			for( int i = 0; i < 3; ++i )
				OversamplingOutputPins[ i ].SendValue<bool>( Oversampling & ( 1 << i ) != 0 );
			
			DoResetReceive( nullptr );
//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ! ClockInputPin.IsConnected() )
				DoClockReceive( nullptr );

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		AD7606Parallel8bit( uint16_t AD0, uint16_t AD1, uint16_t AD2, uint16_t AD3, uint16_t AD4, uint16_t AD5, uint16_t AD6, uint16_t AD7, uint16_t AReset, uint16_t AChipSelect, uint16_t ARead, uint16_t AStart, uint16_t ABusy ) :
			Enabled( true ),
			MostSignificantFirst( false ),
			Oversampling( aosNone )
		{
			FPins[ 0 ] = AD0;
			FPins[ 1 ] = AD1;
			FPins[ 2 ] = AD2;
			FPins[ 3 ] = AD3;
			FPins[ 4 ] = AD4;
			FPins[ 5 ] = AD5;
			FPins[ 6 ] = AD6;
			FPins[ 7 ] = AD7;
			FPins[ RESET_PIN ] = AReset;
			FPins[ CHIP_SELECT_PIN ] = AChipSelect;
			FPins[ READ_PIN ] = ARead;
			FPins[ START_PIN ] = AStart;
			FPins[ BUSY_PIN ] = ABusy;

			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AD7606Parallel8bit::DoClockReceive );
			ResetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AD7606Parallel8bit::DoResetReceive );
		}
	};
//---------------------------------------------------------------------------
}

#endif

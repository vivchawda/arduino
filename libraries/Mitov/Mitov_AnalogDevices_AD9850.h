////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ANALOGDEVICES_AD9850_h
#define _MITOV_ANALOGDEVICES_AD9850_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class AnalogDevices_AD9850_Serial : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
#ifdef _MITOV_ANALOGDEVICES_AD9850_RESET_PIN_
		OpenWire::SinkPin	ResetInputPin;
#endif

		OpenWire::TypedSourcePin<bool>	WordLoadClockOutputPin;
		OpenWire::TypedSourcePin<bool>	FrequencyUpdateOutputPin;
		OpenWire::TypedSourcePin<bool>	DataOutputPin;
#ifdef _MITOV_ANALOGDEVICES_AD9850_OUTPUT_RESET_PIN_
		OpenWire::TypedSourcePin<bool>	ResetOutputPin;
#endif

	public:
		bool	Enabled = true;
		float	ReferenceClockFrequency = 125000000.0;
		float	Frequency = 200.0;
		float	Phase = 0.0;

	public:
		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			if( Enabled )
				UpdateFrequencyAndPhase( true );

			else
				PowerDown();
		}

		void UpdateFrequency()
		{
			if( ! Enabled )
				return;

			UpdateFrequencyAndPhase( false );
		}

		void UpdatePhase()
		{
			if( ! Enabled )
				return;

			UpdateFrequencyAndPhase( true );
		}

	protected:
#ifdef _MITOV_ANALOGDEVICES_AD9850_RESET_PIN_
		void DoResetReceive( void *_Data )
		{
			Reset();
		}
#endif

	protected:
		void UpdateFrequencyAndPhase( bool AUpdatePhase )
		{
//			unsigned char i,w;
//			long int y;
//			double x;
	  
			//Calculate the frequency of the HEX value
			float x = 4294967295 / ( ReferenceClockFrequency / 1000000 ) ;//Suitable for 125M Crystal 
			float AFrequency = Frequency / 1000000;
			AFrequency *= x;
			uint32_t	y = AFrequency;

//			Serial.println( y );

			for( int i = 0; i < 32; ++i )
			{
				DataOutputPin.SetValue( ( y >> i ) & 0x01, false );
				WordLoadClockOutputPin.SetValue( true, false );
				WordLoadClockOutputPin.SetValue( false, false );
			}

			if( AUpdatePhase )
			{
//				uint8_t w = ( Enabled )
				uint8_t w = ( 0b00011111 * Phase ) + 0.5;
				w <<= 3;
//				uint8_t w = 0;
				for( int i = 0; i < 8; ++i )
				{
					DataOutputPin.SetValue( ( w >> i ) & 0x01, false );
					WordLoadClockOutputPin.SetValue( true, false );
					WordLoadClockOutputPin.SetValue( false, false );
				}
			}

			FrequencyUpdateOutputPin.SetValue( true, false );
			FrequencyUpdateOutputPin.SetValue( false, false );
		}

		void PowerDown()
		{
			DataOutputPin.SetValue( false, false );

			FrequencyUpdateOutputPin.SetValue( true, false );

			WordLoadClockOutputPin.SetValue( true, false );

			FrequencyUpdateOutputPin.SetValue( false, false );

			WordLoadClockOutputPin.SetValue( false, false );

			WordLoadClockOutputPin.SetValue( true, false );
			WordLoadClockOutputPin.SetValue( false, false );

			DataOutputPin.SetValue( true, false );

			for( int i = 0; i < 6; ++ i )
			{
				WordLoadClockOutputPin.SetValue( true, false );
				WordLoadClockOutputPin.SetValue( false, false );
			}

			DataOutputPin.SetValue( false, false );

			FrequencyUpdateOutputPin.SetValue( true, false );
			FrequencyUpdateOutputPin.SetValue( false, false );
//			FrequencyUpdateOutputPin.SetValue( false, false );
		}

		void Reset()
		{
#ifdef _MITOV_ANALOGDEVICES_AD9850_OUTPUT_RESET_PIN_
			ResetOutputPin.SetValue( true, false );
			ResetOutputPin.SetValue( false, false );
#endif
			WordLoadClockOutputPin.SetValue( true, false );
			WordLoadClockOutputPin.SetValue( false, false );

			FrequencyUpdateOutputPin.SetValue( true, false );
			FrequencyUpdateOutputPin.SetValue( false, false );
			if( Enabled )
				UpdateFrequencyAndPhase( true );

			else
				PowerDown();

		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();

#ifdef _MITOV_ANALOGDEVICES_AD9850_OUTPUT_RESET_PIN_
			ResetOutputPin.SetValue( false, false );
#endif
			FrequencyUpdateOutputPin.SetValue( false, false );
			WordLoadClockOutputPin.SetValue( false, false );
			DataOutputPin.SetValue( false, false );

			Reset();
		}

	public:
		AnalogDevices_AD9850_Serial()
		{
#ifdef _MITOV_ANALOGDEVICES_AD9850_RESET_PIN_
			ResetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AnalogDevices_AD9850_Serial::DoResetReceive );
#endif
		}

	};
//---------------------------------------------------------------------------
	class AnalogDevices_AD9850_Parallel : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPins[ 8 ];
#ifdef _MITOV_ANALOGDEVICES_AD9850_RESET_PIN_
		OpenWire::SinkPin	ResetInputPin;
#endif

		OpenWire::TypedSourcePin<bool>	WordLoadClockOutputPin;
		OpenWire::TypedSourcePin<bool>	FrequencyUpdateOutputPin;
#ifdef _MITOV_ANALOGDEVICES_AD9850_OUTPUT_RESET_PIN_
		OpenWire::TypedSourcePin<bool>	ResetOutputPin;
#endif

	public:
		bool	Enabled = true;
		float	ReferenceClockFrequency = 125000000.0;
		float	Frequency = 200.0;
		float	Phase = 0.0;

	protected:
#ifdef _MITOV_ANALOGDEVICES_AD9850_RESET_PIN_
		void DoResetReceive( void *_Data )
		{
			Reset();
		}
#endif

	public:
		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			if( Enabled )
			{
				PowerUp();
				UpdateFrequencyAndPhase( true );
			}

			else
				PowerDown();
		}

		void UpdateFrequency()
		{
			if( ! Enabled )
				return;

			UpdateFrequencyAndPhase( true );
		}

		void UpdatePhase()
		{
			if( ! Enabled )
				return;

			UpdateFrequencyAndPhase( false );
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();

#ifdef _MITOV_ANALOGDEVICES_AD9850_OUTPUT_RESET_PIN_
			ResetOutputPin.SetValue( false, false );
#endif
			FrequencyUpdateOutputPin.SetValue( false, false );
			WordLoadClockOutputPin.SetValue( false, false );
			SendByte( 0 );
//			DataOutputPin.SetValue( false, false );

			Reset();
		}

	protected:
		void SendByte( uint8_t AValue )
		{
			for( int i = 0; i < 8; ++i )
				OutputPins[ i ].SendValue<bool>( (( AValue >> i ) & 1 ) != 0 );

			WordLoadClockOutputPin.SetValue( true, false );
			WordLoadClockOutputPin.SetValue( false, false );
		}

		void Reset()
		{
#ifdef _MITOV_ANALOGDEVICES_AD9850_OUTPUT_RESET_PIN_
			ResetOutputPin.SetValue( true, false );
			ResetOutputPin.SetValue( false, false );
#endif
//			WordLoadClockOutputPin.SetValue( true, false );
//			WordLoadClockOutputPin.SetValue( false, false );

//			FrequencyUpdateOutputPin.SetValue( true, false );
//			FrequencyUpdateOutputPin.SetValue( false, false );
			if( Enabled )
				UpdateFrequencyAndPhase( true );

			else
				PowerDown();

		}

		void UpdateFrequencyAndPhase( bool AUpdateFrequency )
		{
//			unsigned char i,w;
//			long int y;
//			double x;
			uint8_t w = ( 0b00011111 * Phase ) + 0.5;
			w <<= 3;

			SendByte( w );
	  
			if( AUpdateFrequency )
			{
				//Calculate the frequency of the HEX value
				float x = 4294967295 / ( ReferenceClockFrequency / 1000000 ) ;//Suitable for 125M Crystal 
				float AFrequency = Frequency / 1000000;
				AFrequency *= x;
				uint32_t	y = AFrequency;

	//			Serial.println( y );

				for( int i = 0; i < 4; ++i )
					SendByte( y >> 8 * ( 3 - i ));

			}

/*
			for( int i = 0; i < 32; ++i )
			{
				DataOutputPin.SetValue( ( y >> i ) & 0x01, false );
				WordLoadClockOutputPin.SetValue( true, false );
				WordLoadClockOutputPin.SetValue( false, false );
			}
*/
			FrequencyUpdateOutputPin.SetValue( true, false );
			FrequencyUpdateOutputPin.SetValue( false, false );
		}

		void PowerDown()
		{
			SendByte( 0b11111100 );

			FrequencyUpdateOutputPin.SetValue( true, false );
			FrequencyUpdateOutputPin.SetValue( false, false );
		}

		void PowerUp()
		{
			SendByte( 0 );

			FrequencyUpdateOutputPin.SetValue( true, false );
			FrequencyUpdateOutputPin.SetValue( false, false );
		}

	public:
		AnalogDevices_AD9850_Parallel()
		{
#ifdef _MITOV_ANALOGDEVICES_AD9850_RESET_PIN_
			ResetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AnalogDevices_AD9850_Parallel::DoResetReceive );
#endif
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_RAMP_TO_VALUE_h
#define _MITOV_RAMP_TO_VALUE_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class RampToValue : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled = true;
		float	InitialValue = 0.0f;
		float	Slope = 1.0f;

	public:
		void SetInitialValue( float AValue )
		{
			InitialValue = AValue;
			FCurrentValue = InitialValue;
		}

	protected:
		unsigned long	FLastTime = 0;
		float			FCurrentValue = 0.0f;
		float			FTargetValue = 0.0f;

	protected:
		void DoReceive( void *_Data )
		{
			float AValue = *(float *)_Data;
			FTargetValue = AValue;
		}

		virtual void DoClockReceive( void *_Data ) override
		{
			Generate( micros(), true );
		}

		void Generate( unsigned long currentMicros, bool FromClock )
		{
			if( FCurrentValue != FTargetValue )
			{
				if( ! Enabled )
					FCurrentValue = FTargetValue;

				else
				{
					float ARamp = abs( ( currentMicros - FLastTime ) * Slope / 1000000 );
					if( FCurrentValue < FTargetValue )
					{
						FCurrentValue += ARamp;
						if( FCurrentValue > FTargetValue )
							FCurrentValue = FTargetValue;

					}
					else
					{
						FCurrentValue -= ARamp;
						if( FCurrentValue < FTargetValue )
							FCurrentValue = FTargetValue;

					}
				}

				OutputPin.Notify( &FCurrentValue );
			}

			else if( FromClock )
				OutputPin.Notify( &FCurrentValue );

			FLastTime = currentMicros;
//			inherited::SendOutput();
		}

	protected:
		virtual void SystemStart() override
		{
			FCurrentValue = InitialValue;
			FTargetValue = InitialValue;

			OutputPin.Notify( &FCurrentValue );
//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ! ClockInputPin.IsConnected() )
				Generate( currentMicros, false );

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		RampToValue()
		{
			InputPin.SetCallback( MAKE_CALLBACK( RampToValue::DoReceive ));
		}
	};
//---------------------------------------------------------------------------
	class RampToColorValue : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled = true;
		TColor	InitialValue;
		float	Slope = 1.0f;

	public:
		void SetInitialValue( float AValue )
		{
			InitialValue = AValue;
			FCurrentValue = InitialValue;
		}

	protected:
		unsigned long	FLastTimeRed = 0;
		unsigned long	FLastTimeGreen = 0;
		unsigned long	FLastTimeBlue = 0;
		TColor			FSlopeValues;
		TColor			FCurrentValue;
		TColor			FTargetValue;

	public:
		static void ProcessColor( uint8_t &ACurrentColor, uint8_t &ATargetColor, int ARamp )
		{
			int ANewColor = ACurrentColor;
			if( ACurrentColor < ATargetColor )
			{
				ANewColor += ARamp;
				if( ANewColor > ATargetColor )
					ANewColor = ATargetColor;

			}
			else
			{
				ANewColor -= ARamp;
				if( ANewColor < ATargetColor )
					ANewColor = ATargetColor;

			}

			ACurrentColor = ANewColor;
		}

	protected:
		void DoReceive( void *_Data )
		{
			TColor AValue = *(TColor *)_Data;
			FTargetValue = AValue;
			FSlopeValues.Red = abs( int( FTargetValue.Red ) - int( FCurrentValue.Red ) );
			FSlopeValues.Green = abs( int( FTargetValue.Green ) - int( FCurrentValue.Green ) );
			FSlopeValues.Blue = abs( int( FTargetValue.Blue ) - int( FCurrentValue.Blue ) );

			unsigned long currentMicros = micros();
			FLastTimeRed = currentMicros;
			FLastTimeGreen = currentMicros;
			FLastTimeBlue = currentMicros;
		}

		virtual void DoClockReceive( void *_Data ) override
		{
			Generate( micros(), true );
		}

		void Generate( unsigned long currentMicros, bool FromClock )
		{
			if( FCurrentValue != FTargetValue )
			{
				if( ! Enabled )
					FCurrentValue = FTargetValue;

				else
				{
					float ACoef =  Slope / 1000000;
					int ARamp = abs( ACoef * float( currentMicros - FLastTimeRed ) * FSlopeValues.Red ) + 0.5;
					if( ARamp )
					{
						ProcessColor( FCurrentValue.Red, FTargetValue.Red, ARamp );
						FLastTimeRed = currentMicros;
					}

					ARamp = abs( ACoef * float( currentMicros - FLastTimeGreen ) * FSlopeValues.Green ) + 0.5;
					if( ARamp )
					{
						ProcessColor( FCurrentValue.Green, FTargetValue.Green, ARamp );
						FLastTimeGreen = currentMicros;
					}

					ARamp = abs( ACoef * float( currentMicros - FLastTimeBlue ) * FSlopeValues.Blue ) + 0.5;
					if( ARamp )
					{
						ProcessColor( FCurrentValue.Blue, FTargetValue.Blue, ARamp );
						FLastTimeBlue = currentMicros;
					}
				}

				OutputPin.Notify( &FCurrentValue );
			}

			else if( FromClock )
			{
				OutputPin.Notify( &FCurrentValue );
				FLastTimeRed = currentMicros;
				FLastTimeGreen = currentMicros;
				FLastTimeBlue = currentMicros;
			}
//			inherited::SendOutput();
		}

	protected:
		virtual void SystemStart() override
		{
			FCurrentValue = InitialValue;
			FTargetValue = InitialValue;

			OutputPin.Notify( &FCurrentValue );
//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ! ClockInputPin.IsConnected() )
				Generate( currentMicros, false );

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		RampToColorValue()
		{
			InputPin.SetCallback( MAKE_CALLBACK( RampToColorValue::DoReceive ));
		}
	};
//---------------------------------------------------------------------------
	class RampToRGBWColorValue : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled = true;
		TColor	InitialValue;
		float	Slope = 1.0f;

	public:
		void SetInitialValue( float AValue )
		{
			InitialValue = AValue;
			FCurrentValue = InitialValue;
		}

	protected:
		unsigned long	FLastTimeRed = 0;
		unsigned long	FLastTimeGreen = 0;
		unsigned long	FLastTimeBlue = 0;
		unsigned long	FLastTimeWhite = 0;
		TRGBWColor		FSlopeValues;
		TRGBWColor		FCurrentValue;
		TRGBWColor		FTargetValue;

	protected:
		void DoReceive( void *_Data )
		{
			TColor AValue = *(TColor *)_Data;
			FTargetValue = AValue;
			FSlopeValues.Red = abs( int( FTargetValue.Red ) - int( FCurrentValue.Red ) );
			FSlopeValues.Green = abs( int( FTargetValue.Green ) - int( FCurrentValue.Green ) );
			FSlopeValues.Blue = abs( int( FTargetValue.Blue ) - int( FCurrentValue.Blue ) );
			FSlopeValues.White = abs( int( FTargetValue.White ) - int( FCurrentValue.White ) );

			unsigned long currentMicros = micros();
			FLastTimeRed = currentMicros;
			FLastTimeGreen = currentMicros;
			FLastTimeBlue = currentMicros;
			FLastTimeWhite = currentMicros;
		}

		virtual void DoClockReceive( void *_Data ) override
		{
			Generate( micros(), true );
		}

		void Generate( unsigned long currentMicros, bool FromClock )
		{
			if( FCurrentValue != FTargetValue )
			{
				if( ! Enabled )
					FCurrentValue = FTargetValue;

				else
				{
					float ACoef =  Slope / 1000000;
					int ARamp = abs( ACoef * float( currentMicros - FLastTimeRed ) * FSlopeValues.Red ) + 0.5;
					if( ARamp )
					{
						RampToColorValue::ProcessColor( FCurrentValue.Red, FTargetValue.Red, ARamp );
						FLastTimeRed = currentMicros;
					}

					ARamp = abs( ACoef * float( currentMicros - FLastTimeGreen ) * FSlopeValues.Green ) + 0.5;
					if( ARamp )
					{
						RampToColorValue::ProcessColor( FCurrentValue.Green, FTargetValue.Green, ARamp );
						FLastTimeGreen = currentMicros;
					}

					ARamp = abs( ACoef * float( currentMicros - FLastTimeBlue ) * FSlopeValues.Blue ) + 0.5;
					if( ARamp )
					{
						RampToColorValue::ProcessColor( FCurrentValue.Blue, FTargetValue.Blue, ARamp );
						FLastTimeBlue = currentMicros;
					}

					ARamp = abs( ACoef * float( currentMicros - FLastTimeWhite ) * FSlopeValues.White ) + 0.5;
					if( ARamp )
					{
						RampToColorValue::ProcessColor( FCurrentValue.White, FTargetValue.White, ARamp );
						FLastTimeWhite = currentMicros;
					}
				}

				OutputPin.Notify( &FCurrentValue );
			}

			else if( FromClock )
			{
				OutputPin.Notify( &FCurrentValue );
				FLastTimeRed = currentMicros;
				FLastTimeGreen = currentMicros;
				FLastTimeBlue = currentMicros;
				FLastTimeWhite = currentMicros;
			}
//			inherited::SendOutput();
		}

	protected:
		virtual void SystemStart() override
		{
			FCurrentValue = InitialValue;
			FTargetValue = InitialValue;

			OutputPin.Notify( &FCurrentValue );
//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ! ClockInputPin.IsConnected() )
				Generate( currentMicros, false );

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		RampToRGBWColorValue()
		{
			InputPin.SetCallback( MAKE_CALLBACK( RampToRGBWColorValue::DoReceive ));
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

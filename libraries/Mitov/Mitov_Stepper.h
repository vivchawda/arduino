////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_STEPPER_h
#define _MITOV_STEPPER_h

#include <Mitov.h>

namespace Mitov
{

/*
	enum StepperMotorType =
	{
		smtDirectional,
		smtFull2Wire,
		smtFull3Wire,
		smtFull4Wire,
		smtHalf3Wire,
		smtHalf4Wire
	};
*/
	const unsigned char C_StepperBits4[] =
	{
			0b1100,
			0b0110,
			0b0011,
			0b1001
	};

	const unsigned char C_StepperBits8[] =
	{
			0b1000,
			0b1100,
			0b0100,
			0b0110,
			0b0010,
			0b0011,
			0b0001,
			0b1001
	};
//---------------------------------------------------------------------------
	const unsigned char C_Stepper3Bits3[] =
	{
			0b110,
			0b011,
			0b101
	};

	const unsigned char C_Stepper3Bits6[] =
	{
			0b100,
			0b110,
			0b010,
			0b011,
			0b001,
			0b101
	};
//---------------------------------------------------------------------------
	template <int C_NUMOUTPINS> class BasicStepperMotor : public OpenWire::Component
	{
	public:
		OpenWire::SourcePin	OutputPins[ C_NUMOUTPINS ];
		OpenWire::SinkPin	StepInputPin;

	protected:
		unsigned long	FLastTime = 0;
		uint8_t	FStep : 4;

	public:
		bool	Enabled : 1;
		bool	Reversed : 1;
		float	StepsPerSecond = 300.f;

	protected:
		virtual void UpdatePins() = 0;

		void DoStepReceive( void *_Data )
		{
			Step();
		}

		void SetPinsValue( unsigned char AValue )
		{
			for( int i = 0; i < C_NUMOUTPINS; ++i )
			{
				bool APinValue = ( AValue & 1 );
				OutputPins[ i ].Notify( &APinValue );
				AValue >>= 1;
			}
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( !Enabled )
				FLastTime = currentMicros;

			else
			{
				unsigned long APeriod = ( 1000000 / abs( StepsPerSecond )) + 0.5;
				if( ( currentMicros - FLastTime ) >= APeriod )
				{
					FLastTime += APeriod;
					if( StepsPerSecond )
					{
						if(( StepsPerSecond > 0 ) ^ Reversed )
							++FStep;

						else
							--FStep;

						UpdatePins();
					}
				}
			}
		}

		public:
			BasicStepperMotor() : 
				Enabled( true ),
				Reversed( false ),
				FStep( 0 )
			{
				StepInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&BasicStepperMotor::DoStepReceive );
			}

	public:
		void Step()
		{
			if( Reversed ) //^ ( StepsPerSecond > 0 ) )
				--FStep;

			else
				++FStep;

			UpdatePins();
		}
	};
//---------------------------------------------------------------------------
	class StepperMotorDirectional : public BasicStepperMotor<2>
	{
	};
//---------------------------------------------------------------------------
	class StepperMotor2Wire : public BasicStepperMotor<2>
	{
	};
//---------------------------------------------------------------------------
	template <int C_NUMOUTPINS> class BasicStepperMotorHalfStep : public BasicStepperMotor<C_NUMOUTPINS>
	{
	public:
		bool	HalfStep = true;

	};
//---------------------------------------------------------------------------
	class StepperMotor4Wire : public BasicStepperMotorHalfStep<4>
	{
	protected:
		virtual void UpdatePins() override
		{
			unsigned char AValue = HalfStep ? C_StepperBits8[ FStep & 0b0111 ] : C_StepperBits4[ FStep & 0b0011 ];
			SetPinsValue( AValue );
		}

	};
//---------------------------------------------------------------------------
	class StepperMotor3Wire : public BasicStepperMotorHalfStep<3>
	{
	protected:
		virtual void UpdatePins() override
		{
			unsigned char AValue = HalfStep ? C_Stepper3Bits3[ FStep % 3 ] : C_Stepper3Bits6[ FStep % 6 ];
			SetPinsValue( AValue );
		}

	};
//---------------------------------------------------------------------------
}

#endif

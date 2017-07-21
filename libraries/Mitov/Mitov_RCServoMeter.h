////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_RC_SERVO_METER_h
#define _MITOV_RC_SERVO_METER_h

#include <Mitov.h>

namespace Mitov
{
	class RCServoMeter : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;


    public:
        bool Enabled : 1;

	protected:
		bool			FOldValue : 1;
		unsigned long	FStartTime = 0;

	protected:
		void DoReceive( void *_Data )
		{
            if( ! Enabled )
				return;

			bool AValue = *(bool *)_Data;
			if( FOldValue == AValue )
				return;

			unsigned long ANow = micros();
			FOldValue = AValue;
			if( AValue )
			{
				FStartTime = ANow;
				return;
			}

			float APeriod = ANow - FStartTime;
//			float AFloatValue = MitovMin<float>( ( APeriod - 1000 ) / 1000, 1 );
			float AFloatValue = MitovMax<float>( MitovMin<float>( ( float( APeriod ) - 500 ) / 1900, 1.0f ), 0.0f );

			OutputPin.Notify( &AFloatValue );
		}

	public:
		RCServoMeter() :
			Enabled( true ),
			FOldValue( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&RCServoMeter::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class RCServoMeter_Interrupt : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

    public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

	protected:
		bool FSignaled : 1;
		unsigned long	FStartTime = 0;
		unsigned long	FPeriod = 0;

	public:
		void InterruptHandler( bool AValue )
		{
			unsigned long ANow = micros();
			if( AValue )
			{
				FStartTime = ANow;
				return;
			}

			FPeriod = ANow - FStartTime;
			FSignaled = true;
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );
			if( ! Enabled )
				return;

//			Serial.println( "TEST" );
			if( ! FSignaled )
				return;

			noInterrupts(); // Protect the FReceivedPackets from being corrupted
			float APeriod = FPeriod;
			FSignaled = false;
			interrupts();

//			float AFloatValue = MitovMin( ( float( APeriod ) - 2000 ) / 2000, 1 );
			float AFloatValue = MitovMax<float>( MitovMin<float>( ( float( APeriod ) - 500 ) / 1900, 1.0f ), 0.0f );
			OutputPin.Notify( &AFloatValue );
		}
	public:
		RCServoMeter_Interrupt() :
			Enabled( true ),
			FSignaled( false )
		{
		}
/*
		RCServoMeter_Interrupt( int AInterruptPin, void (*AInterruptRoutine)() ) :
			Enabled( true ),
			FSignaled( false ),
			FInterruptPin( AInterruptPin )
		{
			// Add by Adrien van den Bossche <vandenbo@univ-tlse2.fr> for Teensy
			// ARM M4 requires the below. else pin interrupt doesn't work properly.
			// On all other platforms, its innocuous, belt and braces
			pinMode( AInterruptPin, INPUT ); 

			int AInterruptNumber = digitalPinToInterrupt( AInterruptPin );
			attachInterrupt( AInterruptNumber, AInterruptRoutine, CHANGE );
		}
*/
	};

}

#endif

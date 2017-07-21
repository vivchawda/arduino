////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_FREQUENCY_METER_h
#define _MITOV_FREQUENCY_METER_h

#include <Mitov.h>

namespace Mitov
{
	class FrequencyMeter : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

	protected:
		bool			FFirstTime : 1;
		unsigned long	FStartTime = 0;

	protected:
		void DoReceive( void *_Data )
		{
			if( FFirstTime )
			{
				FFirstTime = false;
				FStartTime = micros();
				return;
			}

            if( ! Enabled )
				return;

			unsigned long ANow = micros();
			unsigned long APeriod = ANow - FStartTime;
			FStartTime = ANow;

			if( APeriod == 0 )
				APeriod = 1;

			float AFrequency =  1000000.0f / APeriod;
			OutputPin.Notify( &AFrequency );
		}

	public:
		FrequencyMeter() :
			Enabled( true ),
			FFirstTime( true )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&FrequencyMeter::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class FrequencyMeter_Interrupt : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

    public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	protected:
//		bool			FFirstTime : 1;
		unsigned long	FLastTime = 0;
		uint32_t FCounter = 0;

	public:
		void InterruptHandler( bool )
		{
			++ FCounter;
		}

/*
	protected:
		virtual void DoReceive( void *_Data )
		{
			if( FFirstTime )
			{
				FFirstTime = false;
				FStartTime = micros();
				return;
			}

            if( ! Enabled )
				return;

			unsigned long ANow = micros();
			unsigned long APeriod = ANow - FStartTime;
			FStartTime = ANow;

			if( APeriod == 0 )
				APeriod = 1;

			float AFrequency =  1000000.0f / APeriod;
			OutputPin.Notify( &AFrequency );
		}
*/
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );

			if( ! Enabled )
			{
				noInterrupts(); // Protect the FReceivedPackets from being corrupted
				FCounter = 0;
				interrupts();
				FLastTime = currentMicros;
				return;
			}				

			currentMicros = micros();
			unsigned long ATimeDiff = ( currentMicros - FLastTime );

			noInterrupts(); // Protect the FReceivedPackets from being corrupted
			uint32_t ACount = FCounter;
			if( ( ACount < 10 ) && ( ATimeDiff < 1000000 ) )
			{
				interrupts();
				return;
			}

			FCounter = 0;

			interrupts();


//			Serial.println( ACount );

			float AFrequency = ( float( ACount ) / ATimeDiff ) * 1000000;
			OutputPin.Notify( &AFrequency );
//			Serial.println( AFrequency );

			FLastTime = currentMicros;
		}

	public:
/*
		FrequencyMeter_Interrupt( int AInterruptPin, void (*AInterruptRoutine)() )
//			Enabled( true )
//			FFirstTime( true )
		{
			// Add by Adrien van den Bossche <vandenbo@univ-tlse2.fr> for Teensy
			// ARM M4 requires the below. else pin interrupt doesn't work properly.
			// On all other platforms, its innocuous, belt and braces
			pinMode( AInterruptPin, INPUT ); 

			int AInterruptNumber = digitalPinToInterrupt( AInterruptPin );
			attachInterrupt( AInterruptNumber, AInterruptRoutine, RISING );
		}
*/
	};
}

#endif

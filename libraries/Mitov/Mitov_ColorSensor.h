////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SERVO_h
#define _MITOV_SERVO_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	enum TArduinoColorSensorThaosFrequencyScaling { cfsScaling2, cfsScaling20, cfsScaling100 };
//---------------------------------------------------------------------------
	class ColorSensor : public OpenWire::Component
	{
	public:
		OpenWire::SourcePin	SensorSelectOutputPins[ 4 ];
		OpenWire::SourcePin	OutputPins[ 4 ];

	public:
		struct
		{
		public:
			float Red = 1.0;
			float Green = 1.0;
			float Blue = 1.0;
			float Clear = 1.0;

		public:
			void SetRed( float AValue )
			{
				if( AValue == 0 )
					AValue = 0.000001;

				Red = AValue;
			}

			void SetGreen( float AValue )
			{
				if( AValue == 0 )
					AValue = 0.000001;

				Green = AValue;
			}

			void SetBlue( float AValue )
			{
				if( AValue == 0 )
					AValue = 0.000001;

				Blue = AValue;
			}

			void SetClear( float AValue )
			{
				if( AValue == 0 )
					AValue = 0.000001;

				Clear = AValue;
			}

		} ScaleDividers;

		bool PoweredUp : 1;
		TArduinoColorSensorThaosFrequencyScaling FrequencyScaling : 2;

	protected:
		uint8_t	 FCurrentState = 0;
		uint32_t FCounter = 0;
		unsigned long FLastTime = 0;

	public:
		void UpdateFrequencyScaling()
		{
			if( PoweredUp )
			{
				switch( FrequencyScaling )
				{
					case cfsScaling2:
						SensorSelectOutputPins[ 0 ].SendValue<bool>( false );
						SensorSelectOutputPins[ 1 ].SendValue<bool>( true );
						break;

					case cfsScaling20:
						SensorSelectOutputPins[ 0 ].SendValue<bool>( true );
						SensorSelectOutputPins[ 1 ].SendValue<bool>( false );
						break;

					case cfsScaling100:
						SensorSelectOutputPins[ 0 ].SendValue<bool>( true );
						SensorSelectOutputPins[ 1 ].SendValue<bool>( true );
						break;
				}
			}

			else
			{
				SensorSelectOutputPins[ 0 ].SendValue<bool>( false );
				SensorSelectOutputPins[ 1 ].SendValue<bool>( false );
			}
		}

	public:
		void InterruptHandler( bool )
		{
			++ FCounter;
		}

	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			UpdateFrequencyScaling();
		}

		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			FLastTime = micros();

			SensorSelectOutputPins[ 2 ].SendValue( false );
			SensorSelectOutputPins[ 3 ].SendValue( false );
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );

			currentMicros = micros();
			unsigned long ATimeDiff = ( currentMicros - FLastTime );

			noInterrupts(); // Protect the FReceivedPackets from being corrupted
			uint32_t ACount = FCounter; 
			if( ( ACount < 10 ) && ( ATimeDiff < 1000 ) )
			{
				interrupts();
				return;
			}

//			FCounter = 0;

			interrupts();


//			Serial.println( ACount );

			float AFrequency = ( float( ACount ) / ATimeDiff ) * 1000000;
//			Serial.println( AFrequency );

			FLastTime = currentMicros;

			const uint8_t PinMap[] = { 0, 2, 3, 1 };

			switch( FCurrentState )
			{
				case 0: // Red
					AFrequency /= ScaleDividers.Red;
					break;

				case 1: // Blue
					AFrequency /= ScaleDividers.Blue;
					break;

				case 2: // Clear
					AFrequency /= ScaleDividers.Clear;
					break;

				case 3: // Green
					AFrequency /= ScaleDividers.Green;
					break;
			};

			OutputPins[ PinMap[ FCurrentState ] ].Notify( &AFrequency );
/*
			switch( FCurrentState )
			{
				case 0:
					break;

				case 1:
					break;

				case 2:
					break;

				case 3:
					break;
			}
*/
			for( int i = 0; i < 4; ++i )
			{
				++FCurrentState;
				if( FCurrentState > 3 )
					FCurrentState = 0;

				if( OutputPins[ PinMap[ FCurrentState ] ].IsConnected() )
					break;

			}

			SensorSelectOutputPins[ 2 ].SendValue<bool>( ( FCurrentState & 1 ) != 0 );
			SensorSelectOutputPins[ 3 ].SendValue<bool>( ( FCurrentState & 2 ) != 0 );

//			delay( 10 );

			// Restart the count with the new pin settings
			FLastTime = micros();
			noInterrupts(); // Protect the FReceivedPackets from being corrupted
			FCounter = 0;
			interrupts();
		}

	public:
		ColorSensor() :
			PoweredUp( true ),
			FrequencyScaling( cfsScaling100 )
		{
		}
/*
		ColorSensor( int AInterruptPin, void (*AInterruptRoutine)() )
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

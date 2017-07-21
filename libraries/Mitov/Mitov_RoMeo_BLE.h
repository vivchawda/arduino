////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ROMEO_BLE_h
#define _MITOV_ROMEO_BLE_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class ArduinoRoMeoBLEButtons : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::TypedSourcePin<bool>	OutputPins[ 5 ];	

	protected:
		void ReadButtons( bool AOnlyChanged )
		{
			const int  adc_key_val[5] = { 30, 150, 360, 535, 760 };

			int input = analogRead(7);
			int AButtonIndex = -1;
			for( int i = 0; i < 5; ++ i )
				if (input < adc_key_val[i])
				{
					AButtonIndex = i;
					break;
				}
		
			for( int i = 0; i < 5; ++ i )
				OutputPins[ i ].SetValue( ( i == AButtonIndex ), AOnlyChanged );

		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			inherited::SystemLoopBegin( currentMicros );
			ReadButtons( true );
		}

		virtual void SystemStart() override
		{
			inherited::SystemStart();
			ReadButtons( false );
		}
	};
//---------------------------------------------------------------------------
	class ArduinoRoMeoBLEMotors : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ValueSimpleModifiedSinkPin<float>	InputPins[ 2 ];

	protected:
		void SetSpeed( int ADirectionPin, int ASpeedPin, float AValue )
		{
			AValue = constrain( AValue, 0.0, 1.0 );
//			Serial.print( AValue ); Serial.print( " - " );
			if( AValue > 0.5 )
			{
				digitalWrite( ADirectionPin, HIGH );
				AValue -= 0.5;
			}
			else
			{
				digitalWrite( ADirectionPin, LOW );
				AValue = 0.5 - AValue;
			}

			AValue = AValue * 511 + 0.5;
//			Serial.println( AValue );
			analogWrite( ASpeedPin, AValue );
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			inherited::SystemLoopBegin( currentMicros );
			if( InputPins[ 0 ].Modified )
				SetSpeed( 4, 5, InputPins[ 0 ].Value );

			if( InputPins[ 1 ].Modified )
				SetSpeed( 7, 6, InputPins[ 1 ].Value );
		}

		virtual void SystemStart() override
		{
			inherited::SystemStart();
			SetSpeed( 4, 5, InputPins[ 0 ].Value );
			SetSpeed( 7, 6, InputPins[ 1 ].Value );
		}
	};
//---------------------------------------------------------------------------
}

#endif

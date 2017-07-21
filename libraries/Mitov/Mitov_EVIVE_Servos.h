////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_EVIVE_SERVOS_h
#define _MITOV_EVIVE_SERVOS_h

#include <Mitov.h>

#include <Servo.h>

namespace Mitov
{
	class ArduinoEVIVEServos : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPins[ 2 ];

	protected:
		Servo	FServos[ 2 ];

	protected:
		virtual void SystemStart() override
		{
			FServos[ 0 ].attach( 44 );
			FServos[ 1 ].attach( 45 );

//			inherited::SystemStart();
		}

	protected:
		void DoReceive1( void *_Data )
		{
			float AValue = constrain( *((float *)_Data), 0.0f, 1.0f ) * 180;
			FServos[ 0 ].write( AValue );
		}

		void DoReceive2( void *_Data )
		{
			float AValue = constrain( *((float *)_Data), 0.0f, 1.0f ) * 180;
			FServos[ 1 ].write( AValue );
		}

	public:
		ArduinoEVIVEServos()
		{
			InputPins[ 0 ].SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoEVIVEServos::DoReceive1 );
			InputPins[ 1 ].SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoEVIVEServos::DoReceive2 );
		}
	};
//---------------------------------------------------------------------------
}

#endif

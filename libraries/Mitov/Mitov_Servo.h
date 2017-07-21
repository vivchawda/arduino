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

#include <Servo.h>

namespace Mitov
{
	template<int PIN_NUMBER> class MitovServo : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		bool	Enabled = true;
		float	InitialValue = 0.5;

	public:
		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			if( Enabled )
			{
				FServo.attach( PIN_NUMBER );
				DoReceive( &InitialValue );
			}
			else
				FServo.detach();
		}

	protected:
		Servo	FServo;
		
	protected:
		void DoReceive( void *_Data )
		{
			InitialValue = *((float *)_Data);
			if( ! Enabled )
				return;

			float AValue = constrain( InitialValue, 0.0f, 1.0f ) * 180;
			FServo.write( AValue );
		}

		virtual void SystemStart() override
		{
			if( Enabled )
			{
				FServo.attach( PIN_NUMBER );
				DoReceive( &InitialValue );
			}
//			float AValue = constrain( InitialValue, 0.0f, 1.0f ) * 180;
//			FServo.write( AValue );
//			inherited::SystemStart();
		}

	public:
		MitovServo()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MitovServo::DoReceive );
		}

	};
}

#endif

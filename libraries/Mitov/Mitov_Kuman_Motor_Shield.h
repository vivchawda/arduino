////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_KUMAN_MOTOR_SHIELD_h
#define _MITOV_KUMAN_MOTOR_SHIELD_h

#include <Mitov.h>

namespace Mitov
{
	class KumanRobotMotorShieldChannel : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		bool	Enabled = true;

	protected:
		bool	FIsSecond : 1;

	public:
		void	SetEnabled( bool AValue ) 
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			UpdateOutputs();
		}

	protected:
		float	FCurrentSpeed = 0.5f;

	protected:
		void DoReceive( void *_Data )
		{
			float ASpeed = constrain( *(float *)_Data, 0.0, 1.0 );
			if( FCurrentSpeed == ASpeed )
				return;

			FCurrentSpeed = ASpeed;
			UpdateOutputs();
		}

		void	UpdateOutputs()
		{
			bool AForward;
			bool AReverse;
			float AOutSpeed;
			if( Enabled )
			{
				AOutSpeed = abs( FCurrentSpeed - 0.5 ) * 2;
				bool ADirection = FCurrentSpeed > 0.5;

				AReverse = ADirection;
				AForward = ! ADirection;
			}

			else
			{
				AOutSpeed = 0.5;
				AReverse = false;
				AForward = false;
			}

			if( FIsSecond )
			{
				digitalWrite( 13, AForward );
				digitalWrite( 12, AReverse );
				analogWrite( 6, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}
			else
			{
				digitalWrite( 7, AForward );
				digitalWrite( 8, AReverse );
				analogWrite( 5, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}

		}

	protected:
		virtual void SystemStart()
		{
			inherited::SystemStart();
			UpdateOutputs();
		}

	public:
		KumanRobotMotorShieldChannel( bool AIsSecond ) :
			Enabled( true ),
			FIsSecond( AIsSecond )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&KumanRobotMotorShieldChannel::DoReceive );
			if( AIsSecond )
			{
				pinMode( 13, OUTPUT );
				pinMode( 12, OUTPUT );
				pinMode( 6, OUTPUT );
			}
			else
			{
				pinMode( 7, OUTPUT );
				pinMode( 8, OUTPUT );
				pinMode( 5, OUTPUT );
			}
		}
	};
}

#endif

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_L298N_DUAL_MOTOR_h
#define _MITOV_L298N_DUAL_MOTOR_h

#include <Mitov.h>

namespace Mitov
{
	class L298NDualMotorDriverChannel : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	ForwardOutputPin;
		OpenWire::SourcePin	ReverseOutputPin;
		OpenWire::SourcePin	SpeedOutputPin;

	public:
		bool	Enabled = true;

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
			if( Enabled )
			{
				float AOutSpeed = abs( FCurrentSpeed - 0.5 ) * 2;
				bool ADirection = FCurrentSpeed > 0.5;

				ReverseOutputPin.Notify( &ADirection );
				ADirection = !ADirection;
				ForwardOutputPin.Notify( &ADirection );

				SpeedOutputPin.Notify( &AOutSpeed );
			}

			else
			{
				SpeedOutputPin.SendValue( 0.5 );
				ForwardOutputPin.SendValue( false );
				ReverseOutputPin.SendValue( false );
			}
		}

	protected:
		virtual void SystemStart()
		{
			inherited::SystemStart();
			UpdateOutputs();
		}

	public:
		L298NDualMotorDriverChannel()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&L298NDualMotorDriverChannel::DoReceive );
		}
	};
//---------------------------------------------------------------------------
}

#endif

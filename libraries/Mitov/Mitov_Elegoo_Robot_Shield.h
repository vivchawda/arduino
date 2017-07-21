////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ELEGOO_MOTOR_SHIELD_h
#define _MITOV_ELEGOO_MOTOR_SHIELD_h

#include <Mitov.h>
#include <Mitov_UltrasonicRanger.h>
#include <Servo.h>

namespace Mitov
{
	class ElegooRobotMotorShieldChannel : public OpenWire::Component
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
			if( Enabled && ( FCurrentSpeed != 0.5 ))
			{
				AOutSpeed = abs( FCurrentSpeed - 0.5 ) * 2;
				bool ADirection = FCurrentSpeed > 0.5;

				AReverse = ADirection;
				AForward = ! ADirection;
			}

			else
			{
				AOutSpeed = 0;
				AReverse = false;
				AForward = false;
			}

			if( FIsSecond )
			{
				digitalWrite( 8, AForward );
				digitalWrite( 9, AReverse );
				analogWrite( 11, ( AOutSpeed * PWMRANGE ) + 0.5 );
//				analogWrite( 3, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}
			else
			{
				digitalWrite( 7, AForward );
				digitalWrite( 6, AReverse );
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
		ElegooRobotMotorShieldChannel( bool AIsSecond ) :
			Enabled( true ),
			FIsSecond( AIsSecond )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ElegooRobotMotorShieldChannel::DoReceive );
			if( AIsSecond )
			{
				pinMode( 8, OUTPUT );
				pinMode( 9, OUTPUT );
				pinMode( 11, OUTPUT );
//				pinMode( 3, OUTPUT );
			}
			else
			{
				pinMode( 7, OUTPUT );
				pinMode( 6, OUTPUT );
				pinMode( 5, OUTPUT );
			}
		}
	};
//---------------------------------------------------------------------------
	class ElegooRobotServo : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

#ifdef _MITOV_ELEGOO_ROBOT_V2_SERVO_PIN_
	public:
		OpenWire::SinkPin	InputPin;
#endif

	public:
		float	InitialValue = 0.5;

	protected:
		Servo	FServo;

	protected:
		virtual void SystemStart() override
		{
			FServo.attach( 3 );
			DoReceive( &InitialValue );

//			inherited::SystemStart();
		}

	protected:
		void DoReceive( void *_Data )
		{
			float AValue = constrain( *((float *)_Data), 0.0f, 1.0f ) * 180;
			FServo.write( AValue );
		}

#ifdef _MITOV_ELEGOO_ROBOT_V2_SERVO_PIN_

	public:
		ElegooRobotServo()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ElegooRobotServo::DoReceive );
		}
#endif
	};
//---------------------------------------------------------------------------
	class ElegooRobotUltrasonicRanger : public OpenWire::Component
#ifdef _MITOV_ELEGOO_ROBOT_ULTRASONIC_RANGE_CLOCK_PIN_
		, public Mitov::ClockingSupport
#endif
	{
		typedef OpenWire::Component inherited;

	public:
		enum State { sStartUp, sStartDown, sListeningEdgeUp, sListeningEdgeDown, sEchoDetected, sPause };

	public:
//		OpenWire::SinkPin	EchoInputPin;
//		OpenWire::SourcePin	PingOutputPin;
#ifdef _MITOV_ELEGOO_ROBOT_ULTRASONIC_RANGE_TIMEOUT_PIN_
		OpenWire::SourcePin	TimeoutOutputPin;
#endif
		OpenWire::SourcePin	OutputPin;

	public:
		uint16_t	Timeout = 1000;
		uint16_t	PingTime = 2;
		uint16_t	PauseTime = 100;
		float		TimeoutValue = -1;

		UltrasonicRangerUnits	Units : 2;

		bool	Enabled : 1;

	protected:
		State			FState : 3;
#ifdef _MITOV_ELEGOO_ROBOT_ULTRASONIC_RANGE_CLOCK_PIN_
		bool			FClocked : 1;
#endif

		unsigned long	FStartTime;
		unsigned long	FEchoStartTime;
		unsigned long	FEndTime;

	protected:
		void DoReceive( bool AValue )
		{
//			Serial.println( AValue );
			if( ( FState != sListeningEdgeUp ) && ( FState != sListeningEdgeDown ) )
				return;

			if( FState == sListeningEdgeDown )
			{
				if( AValue )
					return;

				FEndTime = micros();
				FState = sEchoDetected;
			}

			else
			{
			  if( ! AValue )
				  return;

			  FState = sListeningEdgeDown;
			  FEchoStartTime = micros();
			}
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ! Enabled )
				return;

			bool AValue = ( digitalRead( 18 ) == HIGH ); // Echo
			DoReceive( AValue );

			switch ( FState )
			{
				case sPause:
				{
					if( ( currentMicros - FStartTime ) < ((unsigned long)PauseTime ) * 1000 )
						return;

#ifdef _MITOV_ELEGOO_ROBOT_ULTRASONIC_RANGE_CLOCK_PIN_
					if( ClockInputPin.IsConnected() )
						if( ! FClocked )
							return;

					FClocked = false;
#endif
				}

				case sStartUp:
				{
//					Serial.println( "start" );
					digitalWrite( 19, true ); // Ping
//					PingOutputPin.SendValue<bool>( true );
					FStartTime = currentMicros;
					FState = sStartDown;
					break;
				}

				case sStartDown:
				{
					if( ( currentMicros - FStartTime ) < PingTime )
						return;

					digitalWrite( 19, false ); // Ping
//					PingOutputPin.SendValue<bool>( false );
					FStartTime = currentMicros;
					FState = sListeningEdgeUp;
					break;
				}

				case sListeningEdgeUp:
				case sListeningEdgeDown:
				{
					if( ( currentMicros - FStartTime ) < ((unsigned long)Timeout ) * 1000 )
						return;

					OutputPin.Notify( &TimeoutValue );
#ifdef _MITOV_ELEGOO_ROBOT_ULTRASONIC_RANGE_TIMEOUT_PIN_
					TimeoutOutputPin.SendValue<bool>( true );
#endif
					FState = sPause;
					break;
				}

				case sEchoDetected:
				{
					unsigned long APeriod = FEndTime - FEchoStartTime;
					float AValue;
					switch( Units )
					{
						case rdTime:
							AValue = APeriod;
							break;

						case rdCm:
							AValue = ((float)APeriod) / 29 / 2; // / 58.2;
							break;

						case rdInch:
							AValue = ((float)APeriod) / 74 / 2;
							break;
					}

					OutputPin.Notify( &AValue );
#ifdef _MITOV_ELEGOO_ROBOT_ULTRASONIC_RANGE_TIMEOUT_PIN_
					TimeoutOutputPin.SendValue<bool>( false );
#endif
					FState = sPause;
					break;
				}
			}
		}

#ifdef _MITOV_ELEGOO_ROBOT_ULTRASONIC_RANGE_CLOCK_PIN_
		virtual void DoClockReceive( void *_Data ) override
		{
			FClocked = true;
		}
#endif

	public:
		ElegooRobotUltrasonicRanger() :
			FState( sStartUp ),
			Enabled( true ),
			Units( rdCm )
#ifdef _MITOV_ELEGOO_ROBOT_ULTRASONIC_RANGE_CLOCK_PIN_
			,
			FClocked( false )
#endif
		{
			pinMode( 19, OUTPUT ); // Ping
//			EchoInputPin.SetCallback( MAKE_CALLBACK( UltrasonicRanger::DoReceive ));
		}

	};
//---------------------------------------------------------------------------
	class ElegooRobotLineFollowingSensors : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::TypedSourcePin<bool>	OutputPins[ 3 ];

	protected:
		bool	FChangeOnly = false;

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			OutputPins[ 0 ].SetValue( digitalRead( 10 ) == HIGH , FChangeOnly );
			OutputPins[ 1 ].SetValue( digitalRead( 4 ) == HIGH , FChangeOnly );
			OutputPins[ 2 ].SetValue( digitalRead( 2 ) == HIGH , FChangeOnly );

			FChangeOnly = true;
//			inherited::SystemLoopBegin( currentMicros );
		}
	};
//---------------------------------------------------------------------------
}

#endif

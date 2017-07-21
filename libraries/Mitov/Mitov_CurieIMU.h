////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_CURIE_IMU_h
#define _MITOV_CURIE_IMU_h

#include <Mitov.h>
#include <CurieIMU.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class Arduino101SensorInterrupt
	{
	public:
		virtual void HandleInterrupt() = 0;
	};
//---------------------------------------------------------------------------
	class Arduino101BasicXYZSensor : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPins[3];

	};
//---------------------------------------------------------------------------
	class Arduino101Gyroscope : public Arduino101BasicXYZSensor
	{
		typedef Arduino101BasicXYZSensor inherited;

	public:
		uint16_t	Rate = 3200;
		uint16_t	Range = 2000;

	public:
		void SetRate( uint16_t AValue )
		{
			const uint16_t CList[] = { 25, 50, 100, 200, 400, 800, 1600, 3200 };
			AValue = ConstrainToList<uint16_t>( AValue, CList, 8 );
			if( Rate == AValue )
				return;

			Rate = AValue;
			CurieIMU.setGyroRate( Rate );
		}

		void SetRange( uint16_t AValue )
		{
			const uint16_t CList[] = { 125, 250, 500, 1000, 2000 };
			AValue = ConstrainToList<uint16_t>( AValue, CList, 5 );
			if( Range == AValue )
				return;

			Range = AValue;
			CurieIMU.setGyroRange( Range );
		}

	protected:
		inline float convertRawGyro(int gRaw) 
		{
			// since we are using 250 degrees/seconds range
			// -250 maps to a raw value of -32768
			// +250 maps to a raw value of 32767
  
			float g = (gRaw * 250.0) / 32768.0;

			return g;
		}

	protected:
		void ReadSensors()
		{
			int gxRaw, gyRaw, gzRaw;         // raw gyro values
			CurieIMU.readGyro(gxRaw, gyRaw, gzRaw);
			OutputPins[ 0 ].SendValue( convertRawGyro( gxRaw ));
			OutputPins[ 1 ].SendValue( convertRawGyro( gyRaw ));
			OutputPins[ 2 ].SendValue( convertRawGyro( gzRaw ));
		}

		virtual void DoClockReceive( void *_Data ) override
		{
			ReadSensors();
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ! ClockInputPin.IsConnected() )
				ReadSensors();

			inherited::SystemLoopBegin( currentMicros );
		}

		virtual void SystemStart() override
		{
			inherited::SystemStart();
			CurieIMU.setGyroRate( Rate );
			CurieIMU.setGyroRange( Range );
		}

	};
//---------------------------------------------------------------------------
	class Arduino101Accelerometer : public Arduino101BasicXYZSensor
	{
		typedef Arduino101BasicXYZSensor inherited;

	public:
		static Arduino101SensorInterrupt *FTapInterrupt;
		static Arduino101SensorInterrupt *FShockInterrupt;
		static Arduino101SensorInterrupt *FMotionInterrupt;
		static Arduino101SensorInterrupt *FZeroMotionInterrupt;

	public:
		static void eventCallback()
		{
			if( FTapInterrupt )
				FTapInterrupt->HandleInterrupt();

			if( FShockInterrupt )
				FShockInterrupt->HandleInterrupt();

			if( FMotionInterrupt )
				FMotionInterrupt->HandleInterrupt();

			if( FZeroMotionInterrupt )
				FZeroMotionInterrupt->HandleInterrupt();

		}

	public:
		float	Rate = 1600;
		uint8_t	Range = 16;

	public:
		void SetRate( float AValue )
		{
			const float CList[] = { 12.5, 25, 50, 100, 200, 400, 800, 1600 };
			AValue = ConstrainToList<float>( AValue, CList, 8 );
			if( Rate == AValue )
				return;

			Rate = AValue;
			CurieIMU.setAccelerometerRate( Rate );
		}

		void SetRange( uint8_t AValue )
		{
			const uint8_t CList[] = { 2, 4, 8, 16 };
			AValue = ConstrainToList<uint8_t>( AValue, CList, 4 );
			if( Range == AValue )
				return;

			Range = AValue;
			CurieIMU.setAccelerometerRange( Range );
		}

	protected:
		inline float convertRawAcceleration(int aRaw) 
		{
			// since we are using 2G range
			// -2g maps to a raw value of -32768
			// +2g maps to a raw value of 32767
  
			float a = (aRaw * 2.0) / 32768.0;

			return a;
		}

	protected:
		void ReadSensors()
		{
			int axRaw, ayRaw, azRaw;         // raw gyro values
			CurieIMU.readAccelerometer(axRaw, ayRaw, azRaw);
			OutputPins[ 0 ].SendValue( convertRawAcceleration( axRaw ));
			OutputPins[ 1 ].SendValue( convertRawAcceleration( ayRaw ));
			OutputPins[ 2 ].SendValue( convertRawAcceleration( azRaw ));
		}

		virtual void DoClockReceive( void *_Data ) override
		{
			ReadSensors();
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ! ClockInputPin.IsConnected() )
				ReadSensors();

			inherited::SystemLoopBegin( currentMicros );
		}

		virtual void SystemStart() override
		{
			inherited::SystemStart();
			CurieIMU.setAccelerometerRate( Rate );
			CurieIMU.setAccelerometerRange( Range );
		}

	};
//---------------------------------------------------------------------------
	Arduino101SensorInterrupt *Arduino101Accelerometer::FTapInterrupt = nullptr;
	Arduino101SensorInterrupt *Arduino101Accelerometer::FShockInterrupt = nullptr;
	Arduino101SensorInterrupt *Arduino101Accelerometer::FMotionInterrupt = nullptr;
	Arduino101SensorInterrupt *Arduino101Accelerometer::FZeroMotionInterrupt = nullptr;
//---------------------------------------------------------------------------
	class Arduino101Thermometer : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::TypedSourcePin<float>	OutputPin;

	public:
		bool	InFahrenheit = false;

	protected:
		void ReadSensors( bool AChangeOnly )
		{
			float AValue = CurieIMU.readTemperature();
			AValue = ( AValue / 32767.0) + 23;
			if( InFahrenheit )
				AValue = Func::ConvertCtoF( AValue );

			OutputPin.SetValue( AValue, AChangeOnly );
		}

		virtual void DoClockReceive( void *_Data ) override
		{
			ReadSensors( true );
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ! ClockInputPin.IsConnected() )
				ReadSensors( true );

			inherited::SystemLoopBegin( currentMicros );
		}

		virtual void SystemStart() override
		{
			inherited::SystemStart();
			ReadSensors( false );
		}
	};
//---------------------------------------------------------------------------	
	class Arduino101BasicDigitalSensor : public OpenWire::Component, public Arduino101SensorInterrupt
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPins[6];

	};
//---------------------------------------------------------------------------
	class Arduino101CurieDetectTap : public Arduino101BasicDigitalSensor
	{
		typedef Arduino101BasicDigitalSensor inherited;

	public:
		OpenWire::SourcePin	DoubleTapOutputPin;

	public:
		float Threshold = 31.25;
		float DurationQuiet = 20;
		float DurationShock = 50;
		uint16_t DurationDoubleTap = 700;

	protected:
		uint8_t	FDetectedBits = 0;
		bool	FDetectedDoubleTap = false;

	public:
		void SetThreshold( float AValue )
		{
			if( Threshold == AValue )
				return;

			Threshold = AValue;
			CurieIMU.setDetectionThreshold( CURIE_IMU_SHOCK, Threshold );
		}

		void SetDurationQuiet( float AValue )
		{
			if( DurationQuiet == AValue )
				return;

			DurationQuiet = AValue;
			CurieIMU.setDetectionDuration( CURIE_IMU_TAP_QUIET, DurationQuiet );
		}

		void SetDurationShock( float AValue )
		{
			if( DurationShock == AValue )
				return;

			DurationShock = AValue;
			CurieIMU.setDetectionDuration( CURIE_IMU_TAP_SHOCK, DurationShock );
		}

		void SetDurationDoubleTap( uint16_t AValue )
		{
			if( DurationDoubleTap == AValue )
				return;

			DurationDoubleTap = AValue;
			CurieIMU.setDetectionDuration( CURIE_IMU_DOUBLE_TAP, DurationDoubleTap );
		}

	public:
		virtual void HandleInterrupt() override
		{
//			Serial.println( "TEST!!!!!!!!" );
/*
			if( CurieIMU.getInterruptStatus(CURIE_IMU_TAP_SHOCK))
				Serial.println( "TEST-11111111----!!!!!!!!" );

			if( CurieIMU.getInterruptStatus(CURIE_IMU_TAP_QUIET))
				Serial.println( "TEST-22222222----!!!!!!!!" );
*/
//return;
			if( CurieIMU.getInterruptStatus(CURIE_IMU_TAP))
			{
//				Serial.println( "TEST-11111111----!!!!!!!!" );
				FDetectedBits = 0;
				for( int axis = 0; axis < 3; ++axis )
				{
					if( CurieIMU.tapDetected( axis, POSITIVE ) )
						FDetectedBits |= 1 << ( axis * 2 );

					if( CurieIMU.tapDetected( axis, NEGATIVE ) )
						FDetectedBits |= 1 << ( axis * 2 + 1 );

//					OutputPins[ axis * 2 ].SendValue( CurieIMU.shockDetected( axis, POSITIVE ));
//					OutputPins[ axis * 2 + 1 ].SendValue( CurieIMU.shockDetected( axis, NEGATIVE ));
				}
			}

			if( CurieIMU.getInterruptStatus( CURIE_IMU_DOUBLE_TAP ))
				FDetectedDoubleTap = true;

		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			inherited::SystemLoopBegin( currentMicros );

//			CurieIMU.noInterrupts(CURIE_IMU_TAP);
			uint8_t ADetectedBits = FDetectedBits;
			FDetectedBits = 0;
//			CurieIMU.interrupts(CURIE_IMU_TAP);
			if( ADetectedBits )
			{
//				Serial.println( ADetectedBits );
				for( int axis = 0; axis < 3; ++axis )
				{
					if( ADetectedBits & ( 1 << ( axis * 2 )) )
						OutputPins[ axis * 2 ].Notify( nullptr );

					if( ADetectedBits & ( 1 << ( axis * 2 + 1 )) )
						OutputPins[ axis * 2 + 1 ].Notify( nullptr );
				}
			}

			if( FDetectedDoubleTap )
			{
				FDetectedDoubleTap = false;
				DoubleTapOutputPin.Notify( nullptr );
			}
		}

		virtual void SystemStart() override
		{
			inherited::SystemStart();
			Arduino101Accelerometer::FTapInterrupt = this;
			CurieIMU.attachInterrupt(Arduino101Accelerometer::eventCallback);
			CurieIMU.setDetectionThreshold( CURIE_IMU_TAP, Threshold );
			CurieIMU.setDetectionDuration( CURIE_IMU_TAP_QUIET, DurationQuiet );
			CurieIMU.setDetectionDuration( CURIE_IMU_TAP_SHOCK, DurationShock );
			CurieIMU.setDetectionDuration( CURIE_IMU_DOUBLE_TAP, DurationDoubleTap );
			CurieIMU.interrupts( CURIE_IMU_TAP );
			CurieIMU.interrupts( CURIE_IMU_DOUBLE_TAP );
//			CurieIMU.interrupts( CURIE_IMU_TAP_SHOCK );
//			CurieIMU.interrupts( CURIE_IMU_TAP_QUIET );
		}
	};
//---------------------------------------------------------------------------
	class Arduino101CurieDetectShock : public Arduino101BasicDigitalSensor
	{
		typedef Arduino101BasicDigitalSensor inherited;

	public:
		float Threshold = 3.91;
		float Duration = 2.5;

	protected:
		uint8_t	FDetectedBits = 0;

	public:
		void SetThreshold( float AValue )
		{
			if( Threshold == AValue )
				return;

			Threshold = AValue;
			CurieIMU.setDetectionThreshold( CURIE_IMU_SHOCK, Threshold );
		}

		void SetDuration( float AValue )
		{
			if( Duration == AValue )
				return;

			Duration = AValue;
			CurieIMU.setDetectionDuration( CURIE_IMU_SHOCK, Duration );
		}

	public:
		virtual void HandleInterrupt() override
		{
//			Serial.println( "TEST!!!!!!!!" );
			if( CurieIMU.getInterruptStatus( CURIE_IMU_SHOCK ))
			{
//				Serial.println( "TEST-11111111----!!!!!!!!" );
				FDetectedBits = 0;
				for( int axis = 0; axis < 3; ++axis )
				{
					if( CurieIMU.shockDetected( axis, POSITIVE ) )
						FDetectedBits |= 1 << ( axis * 2 );

					if( CurieIMU.shockDetected( axis, NEGATIVE ) )
						FDetectedBits |= 1 << ( axis * 2 + 1 );

//					OutputPins[ axis * 2 ].SendValue( CurieIMU.shockDetected( axis, POSITIVE ));
//					OutputPins[ axis * 2 + 1 ].SendValue( CurieIMU.shockDetected( axis, NEGATIVE ));
				}
			}
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			inherited::SystemLoopBegin( currentMicros );
//			CurieIMU.noInterrupts(CURIE_IMU_SHOCK);
			uint8_t ADetectedBits = FDetectedBits;
			FDetectedBits = 0;
//			CurieIMU.interrupts(CURIE_IMU_SHOCK);
			if( ADetectedBits )
			{
				Serial.println( ADetectedBits );
				for( int axis = 0; axis < 3; ++axis )
				{
					if( ADetectedBits & ( 1 << ( axis * 2 )) )
						OutputPins[ axis * 2 ].Notify( nullptr );

					if( ADetectedBits & ( 1 << ( axis * 2 + 1 )) )
						OutputPins[ axis * 2 + 1 ].Notify( nullptr );
				}
			}
		}

		virtual void SystemStart() override
		{
			inherited::SystemStart();
			Arduino101Accelerometer::FShockInterrupt = this;
			CurieIMU.attachInterrupt(Arduino101Accelerometer::eventCallback);
			CurieIMU.setDetectionThreshold( CURIE_IMU_SHOCK, Threshold );
			CurieIMU.setDetectionDuration( CURIE_IMU_SHOCK, Duration );
			CurieIMU.interrupts( CURIE_IMU_SHOCK );
		}
	};
//---------------------------------------------------------------------------
	class Arduino101CurieDetectZeroMotion : public OpenWire::Component, public Arduino101SensorInterrupt
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		float	Threshold = 0;
		float	Duration = 1.28;

	protected:
		bool	FDetected = false;

	public:
		void SetThreshold( float AValue )
		{
			if( Threshold == AValue )
				return;

			Threshold = AValue;
			CurieIMU.setDetectionThreshold( CURIE_IMU_ZERO_MOTION, Threshold );
		}

		void SetDuration( float AValue )
		{
			if( Duration == AValue )
				return;

			Duration = AValue;
			CurieIMU.setDetectionDuration( CURIE_IMU_ZERO_MOTION, Duration );
		}

	public:
		virtual void HandleInterrupt() override
		{
			if( CurieIMU.getInterruptStatus( CURIE_IMU_ZERO_MOTION ))
				FDetected = true;
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );

			if( FDetected )
			{
				FDetected = false;
				OutputPin.Notify( nullptr );
			}
		}

		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			Arduino101Accelerometer::FZeroMotionInterrupt = this;
			CurieIMU.attachInterrupt(Arduino101Accelerometer::eventCallback);
			CurieIMU.setDetectionThreshold( CURIE_IMU_ZERO_MOTION, Threshold );
			CurieIMU.setDetectionDuration( CURIE_IMU_ZERO_MOTION, Duration );
			CurieIMU.interrupts( CURIE_IMU_ZERO_MOTION );
		}
	};
//---------------------------------------------------------------------------
	class Arduino101CurieDetectMotion : public Arduino101BasicDigitalSensor
	{
		typedef Arduino101BasicDigitalSensor inherited;

	public:
		float	Threshold = 0;
		uint8_t SampleCount = 1;

	protected:
		uint8_t	FDetectedBits = 0;

	public:
		void SetThreshold( float AValue )
		{
			if( Threshold == AValue )
				return;

			Threshold = AValue;
			CurieIMU.setDetectionThreshold( CURIE_IMU_MOTION, Threshold );
		}

		void SetSampleCount( uint8_t AValue )
		{
			if( SampleCount == AValue )
				return;

			SampleCount = AValue;
			CurieIMU.setDetectionDuration( CURIE_IMU_MOTION, SampleCount );
		}

	public:
		virtual void HandleInterrupt() override
		{
//			Serial.println( "TEST!!!!!!!!" );
			if( CurieIMU.getInterruptStatus( CURIE_IMU_SHOCK ))
			{
//				Serial.println( "TEST-11111111----!!!!!!!!" );
				FDetectedBits = 0;
				for( int axis = 0; axis < 3; ++axis )
				{
					if( CurieIMU.shockDetected( axis, POSITIVE ) )
						FDetectedBits |= 1 << ( axis * 2 );

					if( CurieIMU.shockDetected( axis, NEGATIVE ) )
						FDetectedBits |= 1 << ( axis * 2 + 1 );

//					OutputPins[ axis * 2 ].SendValue( CurieIMU.shockDetected( axis, POSITIVE ));
//					OutputPins[ axis * 2 + 1 ].SendValue( CurieIMU.shockDetected( axis, NEGATIVE ));
				}
			}
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			inherited::SystemLoopBegin( currentMicros );
//			CurieIMU.noInterrupts(CURIE_IMU_SHOCK);
			uint8_t ADetectedBits = FDetectedBits;
			FDetectedBits = 0;
//			CurieIMU.interrupts(CURIE_IMU_SHOCK);
			if( ADetectedBits )
			{
				Serial.println( ADetectedBits );
				for( int axis = 0; axis < 3; ++axis )
				{
					if( ADetectedBits & ( 1 << ( axis * 2 )) )
						OutputPins[ axis * 2 ].Notify( nullptr );

					if( ADetectedBits & ( 1 << ( axis * 2 + 1 )) )
						OutputPins[ axis * 2 + 1 ].Notify( nullptr );
				}
			}
		}

		virtual void SystemStart() override
		{
			inherited::SystemStart();
			Arduino101Accelerometer::FMotionInterrupt = this;
			CurieIMU.attachInterrupt(Arduino101Accelerometer::eventCallback);
			CurieIMU.setDetectionThreshold( CURIE_IMU_MOTION, Threshold );
			CurieIMU.setDetectionDuration( CURIE_IMU_MOTION, SampleCount );
			CurieIMU.interrupts( CURIE_IMU_MOTION );
		}
	};
//---------------------------------------------------------------------------
	enum TArduino101StepMode { smNormal = CURIE_IMU_STEP_MODE_NORMAL, smSensitive = CURIE_IMU_STEP_MODE_SENSITIVE, smRobust = CURIE_IMU_STEP_MODE_ROBUST };
//---------------------------------------------------------------------------
	class Arduino101CurieDetectSteps : public OpenWire::Component, public ClockingSupport //, public Arduino101SensorInterrupt
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::TypedSourcePin<uint32_t>	OutputPin;
		OpenWire::SinkPin	ResetInputPin;

	public:
		bool Enabled : 1;
		TArduino101StepMode	Mode : 2;

	public:
		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			CurieIMU.setStepCountEnabled( Enabled );
		}

		void SetMode( TArduino101StepMode AValue )
		{
			if( Mode == AValue )
				return;

			Mode = AValue;
			CurieIMU.setStepDetectionMode( Mode );
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			inherited::SystemLoopBegin( currentMicros );

//			Serial.println( "SystemLoopBegin" );
//			Serial.println( CurieIMU.getStepCount() );

			if( ! ClockInputPin.IsConnected() )
				OutputPin.SetValue( CurieIMU.getStepCount(), true );

		}

		virtual void SystemStart() override
		{
			inherited::SystemStart();
//			Arduino101Accelerometer::FStepInterrupt = this;
//			CurieIMU.attachInterrupt( Arduino101Accelerometer::eventCallback );
			CurieIMU.setStepDetectionMode( Mode );
			CurieIMU.setStepCountEnabled( Enabled );
			CurieIMU.resetStepCount();
			OutputPin.SetValue( 0, false );
//			CurieIMU.interrupts( CURIE_IMU_STEP );
		}

	protected:
		void DoResetReceive( void *_Data )
		{
			CurieIMU.resetStepCount();
		}

		virtual void DoClockReceive( void *_Data ) override
		{
			OutputPin.SetValue( CurieIMU.getStepCount(), true );
		}

	public:
		Arduino101CurieDetectSteps() :
			Enabled( true ),
			Mode( smNormal )
		{
			ResetInputPin.SetCallback( MAKE_CALLBACK( Arduino101CurieDetectSteps::DoResetReceive ));
		}

	};
//---------------------------------------------------------------------------
	enum TArduino101AccelerometerOrientation : uint8_t { aoXUp, aoXDown, aoYUp, aoYDown, aoZUp, aoZDown };
//---------------------------------------------------------------------------
	class Arduino101CalibrateAccelerometer : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef OpenWire::Component inherited;
		
	public:
		OpenWire::SourcePin	CalibratingOutputPin;

	public:
		TArduino101AccelerometerOrientation	Orientation = aoZUp;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			int ADirections[] = {0, 0, 0};
			switch( Orientation )
			{
				case aoXUp:
					ADirections[ 0 ] = 1;
					break;

				case aoXDown:
					ADirections[ 0 ] = -1;
					break;

				case aoYUp:
					ADirections[ 1 ] = 1;
					break;

				case aoYDown:
					ADirections[ 1 ] = -1;
					break;

				case aoZUp:
					ADirections[ 2 ] = 1;
					break;

				case aoZDown:
					ADirections[ 2 ] = -1;
					break;

			}

			CalibratingOutputPin.SendValue( true );
			CurieIMU.autoCalibrateXAccelOffset( ADirections[ 0 ] );
			CurieIMU.autoCalibrateYAccelOffset( ADirections[ 1 ] );
			CurieIMU.autoCalibrateZAccelOffset( ADirections[ 2 ] );
			CalibratingOutputPin.SendValue( false );
		}

	};
//---------------------------------------------------------------------------
	class Arduino101CalibrateGyroscope : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef OpenWire::Component inherited;
		
	public:
		OpenWire::SourcePin	CalibratingOutputPin;

	public:
		TArduino101AccelerometerOrientation	Orientation = aoZUp;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			CalibratingOutputPin.SendValue( true );
			CurieIMU.autoCalibrateGyroOffset();
			CalibratingOutputPin.SendValue( false );
		}

	};

//---------------------------------------------------------------------------
	template <int C_AXIS> class Arduino101AccelerometerOffset : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		int16_t	Offset = 0;

	public:
		void SetOffset( int16_t AValue )
		{
			if( Offset == AValue )
				return;

			Offset = AValue;
			CurieIMU.setAccelerometerOffset( C_AXIS, Offset );
		}

	protected:
		virtual void SystemStart() override
		{
			inherited::SystemStart();
			CurieIMU.setAccelerometerOffset( C_AXIS, Offset );
		}
	};
//---------------------------------------------------------------------------
	template <int C_AXIS> class Arduino101GyroscopeOffset : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		int16_t	Offset = 0;

	public:
		void SetOffset( int16_t AValue )
		{
			if( Offset == AValue )
				return;

			Offset = AValue;
			CurieIMU.setGyroOffset( C_AXIS, Offset );
		}

	protected:
		virtual void SystemStart() override
		{
			inherited::SystemStart();
			CurieIMU.setGyroOffset( C_AXIS, Offset );
		}
	};
//---------------------------------------------------------------------------
	class Arduino101CurieIMU : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		Arduino101Gyroscope		Gyroscope;
		Arduino101Accelerometer	Accelerometer;
		Arduino101Thermometer	Thermometer;

	protected:
		virtual void SystemStart() override
		{
			inherited::SystemStart();
			CurieIMU.begin();
		}
	};
//---------------------------------------------------------------------------
}

#endif

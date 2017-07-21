////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MPU6050_h
#define _MITOV_MPU6050_h

#include <Mitov.h>
#include <Wire.h> //I2C Arduino Library
#include <Mitov_Basic_I2C.h>
#include <Mitov_BasicMPU.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class MPU6050LowPowerMode : public MPUOptionalSensor
	{
	public:
		bool Enabled = false;
		float SampleFrequency = 1.25;
	};
//---------------------------------------------------------------------------
	enum MPU6050Filter
	{
      f_AB_260Hz_GB_256Hz_F_8KHz,
      f_AB_184Hz_GB_188Hz_F_1KHz,
      f_AB_94Hz_GB_98Hz_F_1KHz,
      f_AB_44Hz_GB_42Hz_F_1KHz,
      f_AB_21Hz_GB_20Hz_F_1KHz,
      f_AB_10Hz_GB_10Hz_F_1KHz,
      f_AB_5Hz_GB_5Hz_F_1KHz
	};
//---------------------------------------------------------------------------
	class MPU6050I2C : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef	OpenWire::Component	inherited;

	protected:
		static const byte MPU6050_RA_SMPLRT_DIV			= 0x19;
		static const byte MPU6050_RA_CONFIG				= 0x1A;
		static const byte MPU6050_RA_GYRO_CONFIG		= 0x1B;
		static const byte MPU6050_RA_ACCEL_CONFIG		= 0x1C;

		static const byte MPU6050_RA_FIFO_EN			= 0x23;
		static const byte MPU6050_RA_I2C_MST_CTRL		= 0x24;
		static const byte MPU6050_RA_I2C_SLV0_ADDR		= 0x25;
		static const byte MPU6050_RA_I2C_SLV0_REG		= 0x26;
		static const byte MPU6050_RA_I2C_SLV0_CTRL		= 0x27;
		static const byte MPU6050_RA_I2C_SLV1_ADDR		= 0x28;
		static const byte MPU6050_RA_I2C_SLV1_REG		= 0x29;
		static const byte MPU6050_RA_I2C_SLV1_CTRL		= 0x2A;
		static const byte MPU6050_RA_I2C_SLV2_ADDR		= 0x2B;
		static const byte MPU6050_RA_I2C_SLV2_REG		= 0x2C;
		static const byte MPU6050_RA_I2C_SLV2_CTRL		= 0x2D;
		static const byte MPU6050_RA_I2C_SLV3_ADDR		= 0x2E;
		static const byte MPU6050_RA_I2C_SLV3_REG		= 0x2F;
		static const byte MPU6050_RA_I2C_SLV3_CTRL		= 0x30;
		static const byte MPU6050_RA_I2C_SLV4_ADDR		= 0x31;
		static const byte MPU6050_RA_I2C_SLV4_REG		= 0x32;
		static const byte MPU6050_RA_I2C_SLV4_DO		= 0x33;
		static const byte MPU6050_RA_I2C_SLV4_CTRL		= 0x34;
		static const byte MPU6050_RA_I2C_SLV4_DI		= 0x35;
		static const byte MPU6050_RA_I2C_MST_STATUS		= 0x36;
		static const byte MPU6050_RA_INT_PIN_CFG		= 0x37;
		static const byte MPU6050_RA_INT_ENABLE			= 0x38;
		static const byte MPU6050_RA_INT_STATUS			= 0x3A;

		static const byte MPU6050_RA_ACCEL_XOUT_H		= 0x3B;
		static const byte MPU6050_RA_ACCEL_XOUT_L		= 0x3C;
		static const byte MPU6050_RA_ACCEL_YOUT_H		= 0x3D;
		static const byte MPU6050_RA_ACCEL_YOUT_L		= 0x3E;
		static const byte MPU6050_RA_ACCEL_ZOUT_H		= 0x3F;
		static const byte MPU6050_RA_ACCEL_ZOUT_L		= 0x40;

		static const byte MPU6050_RA_EXT_SENS_DATA_00	= 0x49;
		static const byte MPU6050_RA_EXT_SENS_DATA_01	= 0x4A;
		static const byte MPU6050_RA_EXT_SENS_DATA_02	= 0x4B;
		static const byte MPU6050_RA_EXT_SENS_DATA_03	= 0x4C;
		static const byte MPU6050_RA_EXT_SENS_DATA_04	= 0x4D;
		static const byte MPU6050_RA_EXT_SENS_DATA_05	= 0x4E;
		static const byte MPU6050_RA_EXT_SENS_DATA_06	= 0x4F;
		static const byte MPU6050_RA_EXT_SENS_DATA_07	= 0x50;
		static const byte MPU6050_RA_EXT_SENS_DATA_08	= 0x51;
		static const byte MPU6050_RA_EXT_SENS_DATA_09	= 0x52;
		static const byte MPU6050_RA_EXT_SENS_DATA_10	= 0x53;
		static const byte MPU6050_RA_EXT_SENS_DATA_11	= 0x54;
		static const byte MPU6050_RA_EXT_SENS_DATA_12	= 0x55;
		static const byte MPU6050_RA_EXT_SENS_DATA_13	= 0x56;
		static const byte MPU6050_RA_EXT_SENS_DATA_14	= 0x57;
		static const byte MPU6050_RA_EXT_SENS_DATA_15	= 0x58;
		static const byte MPU6050_RA_EXT_SENS_DATA_16	= 0x59;
		static const byte MPU6050_RA_EXT_SENS_DATA_17	= 0x5A;
		static const byte MPU6050_RA_EXT_SENS_DATA_18	= 0x5B;
		static const byte MPU6050_RA_EXT_SENS_DATA_19	= 0x5C;
		static const byte MPU6050_RA_EXT_SENS_DATA_20	= 0x5D;
		static const byte MPU6050_RA_EXT_SENS_DATA_21	= 0x5E;
		static const byte MPU6050_RA_EXT_SENS_DATA_22	= 0x5F;
		static const byte MPU6050_RA_EXT_SENS_DATA_23	= 0x60;

		static const byte MPU6050_RA_USER_CTRL			= 0x6A;
		static const byte MPU6050_RA_PWR_MGMT_1			= 0x6B;
		static const byte MPU6050_RA_PWR_MGMT_2			= 0x6C;

		static const byte MPU6050_RA_FIFO_COUNTH		= 0x72;
		static const byte MPU6050_RA_FIFO_COUNTL		= 0x73;
		static const byte MPU6050_RA_FIFO_R_W			= 0x74;

	public:
		OpenWire::SinkPin	ResetInputPin;

	public:
		MPUAccelerometer		Accelerometer;
		MPU9250Gyroscope		Gyroscope;
		MPUThermometer			Thermometer;
		MPUFrameSynchronization	FrameSynchronization;
//		MPU9250Queue				Queue;
		MPUInterrupt			Interrupt;
		MPU6050LowPowerMode		LowPowerMode;

	public:
		uint8_t	SampleRateDivider = 0;

		bool	Enabled : 1;
		bool	Address : 1;
		MPUClockSource	ClockSource : 3;
		MPU6050Filter Filter : 4;

	protected:
		TwoWire &FWire;

	public:
		void SetClockSource( MPUClockSource	 AValue )
		{
			if( AValue == ClockSource )
				return;

			ClockSource = AValue;
			UpdatePowerManagementReg1( false );
		}

//	protected:
//		bool	FUseFIFO : 1;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			ReadSensors();
		}

/*
		void DoResetReceive( void *_Data )
		{
		}
*/
	protected:
		void ReadSensors()
		{
//			Serial.println( "ReadSensors" );
			const float AccelerometerCoefficients [] = 
			{ 
				2.0f / 32768, 
				4.0f / 32768, 
				8.0f / 32768, 
				16.0f / 32768 
			};

			const float GyroscopeCoefficients [] = 
			{ 
				250.0f / 32768, 
				500.0f / 32768, 
				1000.0f / 32768, 
				2000.0f / 32768 
			};

			const float GyroscopeUnits [] =
			{
				1,
				PI / 180,
				1 / 360
			};

			uint8_t AIntValues[ 7 * 2 ];

			if( ReadBytes( MPU6050_RA_ACCEL_XOUT_H, sizeof( AIntValues ), AIntValues ))
			{
				for( int i = 0; i < 3; ++i )
				{
					float AValue = (( ((int16_t)AIntValues[ i * 2 ] ) << 8 ) | AIntValues[ i * 2 + 1 ] ) * AccelerometerCoefficients[ Accelerometer.FullScaleRange ];
					Accelerometer.OutputPins[ i ].Notify( &AValue );
				}

				for( int i = 0; i < 3; ++i )
				{
					float AValue = (((int16_t)AIntValues[ ( i + 4 ) * 2 ] ) << 8 | AIntValues[ ( i + 4 ) * 2 + 1 ] ) * GyroscopeCoefficients[ Gyroscope.FullScaleRange ] * GyroscopeUnits[ Gyroscope.Units ];
					Gyroscope.OutputPins[ i ].Notify( &AValue );
				}

//				float AValue = ((((int16_t)AIntValues[ 3 * 2 ] ) << 8 ) | AIntValues[ 3 * 2 + 1 ] ) / 333.87 + 21.0;
				float AValue = (float((((int16_t)AIntValues[ 3 * 2 ] ) << 8 ) | AIntValues[ 3 * 2 + 1 ] ) + 12412.0 ) / 340.0;
				if( Thermometer.InFahrenheit )
					AValue = Func::ConvertCtoF( AValue );

				Thermometer.OutputPin.Notify( &AValue );
			}
		}

	protected:
		inline void WriteTo( byte ARegister, byte AData )
		{
//			Serial.print( "REG: " ); Serial.print( ARegister, HEX ); Serial.print( " = " ); Serial.println( AData, BIN );
			I2C::WriteByte( FWire, MPUConst::Adresses[ Address ], ARegister, AData );
		}

		bool ReadBytes( uint8_t regAddr, uint8_t length, void *data )
		{
			return I2C::ReadBytes( FWire, MPUConst::Adresses[ Address ], regAddr, length, data );
		}

	public:
		void UpdatePowerManagementReg1( bool AReset = false )
		{
			uint8_t AValue;
			if( ClockSource = mcsAutoSelect )
			{
				if( Gyroscope.X.Enabled )
					AValue = mcsGyroX;

				else if( Gyroscope.Y.Enabled )
					AValue = mcsGyroY;

				else if( Gyroscope.Z.Enabled )
					AValue = mcsGyroZ;

				else
					AValue = mcsInternal;
			}

			else
				AValue = ClockSource;

			AValue |=	( AReset				?		0b10000000 : 0 ) |
						( Enabled				? 0 :	0b01000000 ) |
						( LowPowerMode.Enabled	?       0b00100000 : 0 ) |
						( Thermometer.Enabled	? 0 :	0b00001000 );

//			Serial.print( "UpdatePowerManagementReg1: " ); Serial.println( AValue, BIN );
			WriteTo( MPU6050_RA_PWR_MGMT_1, AValue );
		}

		void UpdatePowerManagementReg2()
		{
			const float CLevels[] =
			{
				1.25,
				5,
				20,
				40
			};

			int AValue = -1;
			for( int i = 0; i < sizeof( CLevels ) / sizeof( CLevels[ 0 ] ); ++i )
				if( LowPowerMode.SampleFrequency <= CLevels[ i ] )
				{
					AValue = i;
					break;
				}

			if( AValue == -1 )
				AValue = 0;

			AValue <<= 6;

			AValue |=	( Accelerometer.X.Enabled	? 0 : 0b00100000 ) |
								( Accelerometer.Y.Enabled	? 0 : 0b00010000 ) |
								( Accelerometer.Z.Enabled	? 0 : 0b00001000 ) |
								( Gyroscope.X.Enabled		? 0 : 0b00000100 ) |
								( Gyroscope.Y.Enabled		? 0 : 0b00000010 ) |
								( Gyroscope.Z.Enabled		? 0 : 0b00000001 );

			WriteTo( MPU6050_RA_PWR_MGMT_2, (uint8_t)AValue );
		}

/*
		void UpdateUserControlReg()
		{
			if( ! Compass.DirectAccess )
				WriteTo( MPU9250_RA_USER_CTRL, 0b00100000 ); // Enable master I2C mode
		}
*/
		void UpdateConfigReg()
		{
			uint8_t AValue =	Filter |
//								( Queue.BlockOnFull ? 0b01000000 : 0 ) |
								( FrameSynchronization.Location << 4 );

			WriteTo( MPU6050_RA_CONFIG, AValue );
		}

		void UpdateGyroConfigReg()
		{
			uint8_t AValue =	(( Gyroscope.FullScaleRange & 0b11 ) << 3 ) |
						( Gyroscope.X.SelfTest ? 0b10000000 : 0 ) |
						( Gyroscope.Y.SelfTest ? 0b01000000 : 0 ) |
						( Gyroscope.Z.SelfTest ? 0b00100000 : 0 );

			WriteTo( MPU6050_RA_GYRO_CONFIG, AValue );
		}

		void UpdateAccelerometerConfigReg()
		{
			byte AValue =	(( Accelerometer.FullScaleRange & 0b11 ) << 3 ) |
							( Accelerometer.X.SelfTest ? 0b10000000 : 0 ) |
							( Accelerometer.Y.SelfTest ? 0b01000000 : 0 ) |
							( Accelerometer.Z.SelfTest ? 0b00100000 : 0 );

			WriteTo( MPU6050_RA_ACCEL_CONFIG, AValue );
		}

		void UpdatenterruptPinAndBypassConfigReg()
		{
			byte AValue =	( Interrupt.Inverted								?		0b10000000 : 0 ) |
							( Interrupt.OpenDrain								?		0b01000000 : 0 ) |
							( Interrupt.Latch									?		0b00100000 : 0 ) |
							( FrameSynchronization.InterruptOnLowLevel			?		0b00001000 : 0 ) |
							( FrameSynchronization.EnableInterrupt				?		0b00000100 : 0 );
//							( /*Compass.Queue & */ ( ! ADirectCompassAccess )		? 0 :	0b00000010 );

//			Serial.print( "UpdatenterruptPinAndBypassConfigReg: " ); Serial.println( AValue, BIN );

			WriteTo( MPU6050_RA_INT_PIN_CFG, AValue );
		}

		inline void UpdatenterruptPinAndBypassConfigRegNormal()
		{
			UpdatenterruptPinAndBypassConfigReg();
		}

		void UpdateSampleRateDividerReg()
		{
			WriteTo( MPU6050_RA_SMPLRT_DIV, SampleRateDivider );
		}

		void UpdateFIFOReg()
		{
/*
			byte AValue =	( Thermometer.Queue		?		0b10000000 : 0 ) |
							( Gyroscope.X.Queue		?		0b01000000 : 0 ) |
							( Gyroscope.Y.Queue		?		0b00100000 : 0 ) |
							( Gyroscope.Z.Queue		?		0b00010000 : 0 ) |
							( Accelerometer.Queue	?		0b00001000 : 0 ) |
							( Compass.Queue			?		0b00000001 : 0 );

			FUseFIFO = ( AValue != 0 );

			WriteTo( MPU9250_RA_FIFO_EN, AValue );
*/
		}

		void UpdateInteruptEnableReg()
		{
			byte AValue =
//							( Interrupt.OnQueueOverflow	?	0b00010000 : 0 ) |
							( Interrupt.OnFrameSync		?	0b00001000 : 0 ) |
							( Interrupt.OnRawReady		?	0b00000001 : 0 );

			WriteTo( MPU6050_RA_INT_ENABLE, AValue );
		}

	protected:
		virtual void SystemStart() override
		{
			UpdatePowerManagementReg1();
			UpdatePowerManagementReg2();
			UpdateConfigReg();
			UpdateGyroConfigReg();
			UpdateAccelerometerConfigReg();
			UpdateSampleRateDividerReg();
			UpdatenterruptPinAndBypassConfigReg();
//			UpdateUserControlReg();
//			UpdatenterruptPinAndBypassConfigReg( Compass.DirectAccess && ! Compass.Queue );
			UpdateFIFOReg();
			UpdateInteruptEnableReg();

			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( Enabled )
			{
				if( ! ClockInputPin.IsConnected() )
					ReadSensors();

/*
				if( FUseFIFO )
				{
					if( Queue.SizeOutputPin.IsConnected() )
					{
						uint8_t AIntValues[ 2 ];
						if( ReadBytes( MPU9250_RA_FIFO_COUNTH, 2, AIntValues ))
						{
							uint32_t ASize = (( ((int16_t)AIntValues[ 0 ] ) << 8 ) | AIntValues[ 1 ] );
							Queue.SizeOutputPin.Notify( &ASize );
						}
					}
				}
*/
				uint8_t AIntValue;
//				if( ReadBytes( MPU9250_RA_INT_STATUS, 1, &AIntValue ) )
				if( ReadBytes( MPU6050_RA_I2C_MST_STATUS, 1, &AIntValue ) )
//				if( Queue.OverflowOutputPin.IsConnected() || MotionDetect.OutputPin.IsConnected() || FrameSynchronization.OutputPin.IsConnected() )
				{
//					if( AIntValue & 0x00010000 )
//						Queue.OverflowOutputPin.Notify( nullptr );

					if( AIntValue & 0x10000000 )
						FrameSynchronization.OutputPin.Notify( nullptr );

				}
			}

			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		MPU6050I2C( TwoWire &AWire ) :
			ClockSource( mcsAutoSelect ),
			Filter( f_AB_260Hz_GB_256Hz_F_8KHz ),
			Enabled( true ),
			Address( false ),
			FWire( AWire )
//			FUseFIFO( false )
		{
//			ResetInputPin.SetCallback( MAKE_CALLBACK( MPU9250I2C::DoResetReceive ));
		}
	};
//---------------------------------------------------------------------------
}

#endif

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MPU9250_h
#define _MITOV_MPU9250_h

#include <Mitov.h>
#include <Wire.h> //I2C Arduino Library
#include <Mitov_Basic_I2C.h>
#include <Mitov_BasicMPU.h>
#include <Mitov_Compass_AK8963.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	namespace MPU9250Const
	{
		const byte MPU9150_RA_MAG_ADDRESS	= 0x0C;

		const byte MPU9150_RA_MAG_ST1		= 0x02;
		const byte MPU9150_RA_MAG_XOUT_L	= 0x03;
		const byte MPU9150_RA_MAG_XOUT_H	= 0x04;
		const byte MPU9150_RA_MAG_YOUT_L	= 0x05;
		const byte MPU9150_RA_MAG_YOUT_H	= 0x06;
		const byte MPU9150_RA_MAG_ZOUT_L	= 0x07;
		const byte MPU9150_RA_MAG_ZOUT_H	= 0x08;
		const byte MPU9150_RA_MAG_ST2		= 0x09;
		const byte MPU9150_RA_MAG_CNTL1		= 0x0A;
		const byte MPU9150_RA_MAG_CNTL2		= 0x0B;
		const byte MPU9150_RA_MAG_ASTC		= 0x0C;

		const byte MPU9150_RA_MAG_ASAX		= 0x10;
		const byte MPU9150_RA_MAG_ASAY		= 0x11;
		const byte MPU9150_RA_MAG_ASAZ		= 0x12;
	};
//---------------------------------------------------------------------------
	class MPU9250MotionDetect : public MPUOptionalSensor
	{
	public:
		OpenWire::SourcePin OutputPin;

	public:
		bool	CompareWithPreviousSample = false;
		float	Threshold = 0;

	};
//---------------------------------------------------------------------------
	class MPU9250LowPowerMode : public MPUOptionalSensor
	{
	public:
		bool Enabled = false;
		float SampleFrequency = 0.24;
	};
//---------------------------------------------------------------------------
	class MPU9250Compass : public MPUBasicSensor
	{
	public:
		CompassAK8963Mode	Mode : 2;
		bool Enabled : 1;
		bool SelfTest : 1;
		bool HighResolution : 1;
		bool DirectAccess : 1;
//		bool Queue : 1;

	public:
		OpenWire::SinkPin	ResetInputPin;
		OpenWire::SourcePin OverflowOutputPin;

	protected:
		TwoWire &FWire;

	protected:
		void DoResetReceive( void *_Data )
		{
			I2C::WriteByte( FWire, MPU9250Const::MPU9150_RA_MAG_ADDRESS, MPU9250Const::MPU9150_RA_MAG_CNTL2, 0b00000001 );
		}

	public:
		MPU9250Compass( TwoWire &AWire ) :
			Mode( cm100Hz ),
			Enabled( true ),
			HighResolution( true ),
			DirectAccess( true ),
			SelfTest( false ),
			FWire( AWire )
//			Queue( false )
		{
			ResetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MPU9250Compass::DoResetReceive );
		}
	};
//---------------------------------------------------------------------------
	enum MPU9250GyroscopeThermometerFilter
	{
		gtf_GB_8800Hz_GF_32KHz_TB_4000Hz,
		gtf_GB_3600Hz_GF_32KHz_TB_4000Hz,
		gtf_GB_250Hz_GF_8KHz_TB_4000Hz,
		gtf_GB_184Hz_GF_1KHz_TB_188Hz,
		gtf_GB_92Hz_GF_1KHz_TB_98Hz,
		gtf_GB_41Hz_GF_1KHz_TB_42Hz,
		gtf_GB_20Hz_GF_1KHz_TB_20Hz,
		gtf_GB_10Hz_GF_1KHz_TB_10Hz,
		gtf_GB_5Hz_GF_1KHz_TB_5Hz,
		gtf_GB_3600Hz_GF_8KHz_TB_4000Hz
	};
//---------------------------------------------------------------------------
/*
	class MPU9250Queue
	{
	public:
		OpenWire::SourcePin	OverflowOutputPin;
		OpenWire::SourcePin	SizeOutputPin;

	public:
		bool BlockOnFull = false;
	};
*/
//---------------------------------------------------------------------------
	class MPU9250I2C : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef	OpenWire::Component	inherited;

	protected:
		static const byte MPU9250_RA_SMPLRT_DIV			= 0x19;
		static const byte MPU9250_RA_CONFIG				= 0x1A;
		static const byte MPU9250_RA_GYRO_CONFIG		= 0x1B;
		static const byte MPU9250_RA_ACCEL_CONFIG		= 0x1C;
		static const byte MPU9250_RA_ACCEL_CONFIG_2		= 0x1D;
		static const byte MPU9250_RA_LP_ACCEL_ODR		= 0x1E;

		static const byte MPU9250_RA_MOT_THR			= 0x1F;

		static const byte MPU9250_RA_FIFO_EN			= 0x23;
		static const byte MPU9250_RA_I2C_MST_CTRL		= 0x24;
		static const byte MPU9250_RA_I2C_SLV0_ADDR		= 0x25;
		static const byte MPU9250_RA_I2C_SLV0_REG		= 0x26;
		static const byte MPU9250_RA_I2C_SLV0_CTRL		= 0x27;
		static const byte MPU9250_RA_I2C_SLV1_ADDR		= 0x28;
		static const byte MPU9250_RA_I2C_SLV1_REG		= 0x29;
		static const byte MPU9250_RA_I2C_SLV1_CTRL		= 0x2A;
		static const byte MPU9250_RA_I2C_SLV2_ADDR		= 0x2B;
		static const byte MPU9250_RA_I2C_SLV2_REG		= 0x2C;
		static const byte MPU9250_RA_I2C_SLV2_CTRL		= 0x2D;
		static const byte MPU9250_RA_I2C_SLV3_ADDR		= 0x2E;
		static const byte MPU9250_RA_I2C_SLV3_REG		= 0x2F;
		static const byte MPU9250_RA_I2C_SLV3_CTRL		= 0x30;
		static const byte MPU9250_RA_I2C_SLV4_ADDR		= 0x31;
		static const byte MPU9250_RA_I2C_SLV4_REG		= 0x32;
		static const byte MPU9250_RA_I2C_SLV4_DO		= 0x33;
		static const byte MPU9250_RA_I2C_SLV4_CTRL		= 0x34;
		static const byte MPU9250_RA_I2C_SLV4_DI		= 0x35;
		static const byte MPU9250_RA_I2C_MST_STATUS		= 0x36;
		static const byte MPU9250_RA_INT_PIN_CFG		= 0x37;
		static const byte MPU9250_RA_INT_ENABLE			= 0x38;
		static const byte MPU9250_RA_DMP_INT_STATUS		= 0x39;
		static const byte MPU9250_RA_INT_STATUS			= 0x3A;

		static const byte MPU9250_RA_ACCEL_XOUT_H		= 0x3B;
		static const byte MPU9250_RA_ACCEL_XOUT_L		= 0x3C;
		static const byte MPU9250_RA_ACCEL_YOUT_H		= 0x3D;
		static const byte MPU9250_RA_ACCEL_YOUT_L		= 0x3E;
		static const byte MPU9250_RA_ACCEL_ZOUT_H		= 0x3F;
		static const byte MPU9250_RA_ACCEL_ZOUT_L		= 0x40;

		static const byte MPU9250_RA_EXT_SENS_DATA_00	= 0x49;
		static const byte MPU9250_RA_EXT_SENS_DATA_01	= 0x4A;
		static const byte MPU9250_RA_EXT_SENS_DATA_02	= 0x4B;
		static const byte MPU9250_RA_EXT_SENS_DATA_03	= 0x4C;
		static const byte MPU9250_RA_EXT_SENS_DATA_04	= 0x4D;
		static const byte MPU9250_RA_EXT_SENS_DATA_05	= 0x4E;
		static const byte MPU9250_RA_EXT_SENS_DATA_06	= 0x4F;
		static const byte MPU9250_RA_EXT_SENS_DATA_07	= 0x50;
		static const byte MPU9250_RA_EXT_SENS_DATA_08	= 0x51;
		static const byte MPU9250_RA_EXT_SENS_DATA_09	= 0x52;
		static const byte MPU9250_RA_EXT_SENS_DATA_10	= 0x53;
		static const byte MPU9250_RA_EXT_SENS_DATA_11	= 0x54;
		static const byte MPU9250_RA_EXT_SENS_DATA_12	= 0x55;
		static const byte MPU9250_RA_EXT_SENS_DATA_13	= 0x56;
		static const byte MPU9250_RA_EXT_SENS_DATA_14	= 0x57;
		static const byte MPU9250_RA_EXT_SENS_DATA_15	= 0x58;
		static const byte MPU9250_RA_EXT_SENS_DATA_16	= 0x59;
		static const byte MPU9250_RA_EXT_SENS_DATA_17	= 0x5A;
		static const byte MPU9250_RA_EXT_SENS_DATA_18	= 0x5B;
		static const byte MPU9250_RA_EXT_SENS_DATA_19	= 0x5C;
		static const byte MPU9250_RA_EXT_SENS_DATA_20	= 0x5D;
		static const byte MPU9250_RA_EXT_SENS_DATA_21	= 0x5E;
		static const byte MPU9250_RA_EXT_SENS_DATA_22	= 0x5F;
		static const byte MPU9250_RA_EXT_SENS_DATA_23	= 0x60;

		static const byte MPU9250_RA_MOT_DETECT_CTRL    = 0x69;
		static const byte MPU9250_RA_USER_CTRL			= 0x6A;
		static const byte MPU9250_RA_PWR_MGMT_1			= 0x6B;
		static const byte MPU9250_RA_PWR_MGMT_2			= 0x6C;

		static const byte MPU9250_RA_FIFO_COUNTH		= 0x72;
		static const byte MPU9250_RA_FIFO_COUNTL		= 0x73;
		static const byte MPU9250_RA_FIFO_R_W			= 0x74;

	public:
		OpenWire::SinkPin	ResetInputPin;

	public:
		MPUAccelerometer		Accelerometer;
		MPU9250Gyroscope			Gyroscope;
		MPU9250Compass				Compass;
		MPUThermometer			Thermometer;
		MPUFrameSynchronization	FrameSynchronization;
//		MPU9250Queue				Queue;
		MPUInterrupt			Interrupt;
		MPU9250MotionDetect			MotionDetect;
		MPU9250LowPowerMode			LowPowerMode;

	protected:
		float CompassAdjustmentValues[ 3 ];

	public:
		uint8_t	SampleRateDivider = 0;

		bool	Enabled : 1;
		bool	Address : 1;
		MPUClockSource	ClockSource : 3;
		MPU9250GyroscopeThermometerFilter GyroscopeThermometerFilter : 4;

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

		void SetCompassDirectAccess( bool AValue )
		{
			if( AValue == Compass.DirectAccess )
				return;

			Compass.DirectAccess = AValue;

			UpdateCompassSlaveI2CReg();
			UpdateUserControlReg();
			UpdatenterruptPinAndBypassConfigRegNormal();
		}

//	protected:
//		bool	FUseFIFO : 1;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			ReadSensors();
		}

		void DoResetReceive( void *_Data )
		{
			UpdatePowerManagementReg1( true );
		}

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

			const float CompassCoefficients[] =
			{
				10.0f *4219.0f / 8190.0f,
				10.0f *4219.0f / 32760.0f
			};

			uint8_t AIntValues[ 7 * 2 ];

			if( ReadBytes( MPU9250_RA_ACCEL_XOUT_H, sizeof( AIntValues ), AIntValues ))
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

				float AValue = ((((int16_t)AIntValues[ 3 * 2 ] ) << 8 ) | AIntValues[ 3 * 2 + 1 ] ) / 333.87 + 21.0;
				if( Thermometer.InFahrenheit )
					AValue = Func::ConvertCtoF( AValue );

				Thermometer.OutputPin.Notify( &AValue );
			}

/*
			if( Compass.Queue )
			{
				if( ReadBytes( MPU9250_RA_FIFO_R_W, 1, AIntValues ))
				{
					Compass.OverflowOutputPin.SendValue( AIntValues[ 0 ] & 0b00000010 );
					if( AIntValues[ 0 ] & 0b00000001 )
						for( int i = 0; i < 3; ++i )
							if( ReadBytes( MPU9250_RA_FIFO_R_W, 1, AIntValues ))
								if( ReadBytes( MPU9250_RA_FIFO_R_W, 1, AIntValues + 1 ))
								{
										float AValue = (( ((int16_t)AIntValues[ 1 ] ) << 8 ) | AIntValues[ 0 ] ) * CompassCoefficients[ Compass.HighResolution & 1 ] * CompassAdjustmentValues[ i ];
										Compass.OutputPins[ i ].Notify( &AValue );
								}


				}
			}

			else 
*/
			if( Compass.DirectAccess )
			{
				if( I2C::ReadBytes( FWire, MPU9250Const::MPU9150_RA_MAG_ADDRESS, MPU9250Const::MPU9150_RA_MAG_ST1, 1, AIntValues ))
				{
					Compass.OverflowOutputPin.SendValue( AIntValues[ 0 ] & 0b00000010 );
					if( AIntValues[ 0 ] & 0b00000001 )
						if( I2C::ReadBytes( FWire, MPU9250Const::MPU9150_RA_MAG_ADDRESS, MPU9250Const::MPU9150_RA_MAG_XOUT_L, 7, AIntValues ))
							for( int i = 0; i < 3; ++i )
							{
								float AValue = (( ((int16_t)AIntValues[ i * 2 + 1 ] ) << 8 ) | AIntValues[ i * 2 ] ) * CompassCoefficients[ Compass.HighResolution & 1 ] * CompassAdjustmentValues[ i ];
								Compass.OutputPins[ i ].Notify( &AValue );
							}

				}
			}
			else
			{
				if( ReadBytes( MPU9250_RA_EXT_SENS_DATA_00, 1, AIntValues ))
				{
	//				Serial.println( AIntValues[ 0 ], BIN );
					Compass.OverflowOutputPin.SendValue( AIntValues[ 0 ] & 0b00000010 );
					if( AIntValues[ 0 ] & 0b00000001 )
						if( ReadBytes( MPU9250_RA_EXT_SENS_DATA_01, 7, AIntValues ))
							for( int i = 0; i < 3; ++i )
							{
								float AValue = (( ((int16_t)AIntValues[ i * 2 + 1 ] ) << 8 ) | AIntValues[ i * 2 ] ) * CompassCoefficients[ Compass.HighResolution & 1 ] * CompassAdjustmentValues[ i ];
								Compass.OutputPins[ i ].Notify( &AValue );
							}

				}
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
						( Gyroscope.Standby		?		0b00010000 : 0 ) |
						( Thermometer.Enabled	? 0 :	0b00001000 );

//			Serial.print( "UpdatePowerManagementReg1: " ); Serial.println( AValue, BIN );
			WriteTo( MPU9250_RA_PWR_MGMT_1, AValue );
		}

		void UpdatePowerManagementReg2()
		{
			uint8_t AValue =	( Accelerometer.X.Enabled	? 0 : 0b00100000 ) |
								( Accelerometer.Y.Enabled	? 0 : 0b00010000 ) |
								( Accelerometer.Z.Enabled	? 0 : 0b00001000 ) |
								( Gyroscope.X.Enabled		? 0 : 0b00000100 ) |
								( Gyroscope.Y.Enabled		? 0 : 0b00000010 ) |
								( Gyroscope.Z.Enabled		? 0 : 0b00000001 );

			WriteTo( MPU9250_RA_PWR_MGMT_2, AValue );
		}

		void UpdateUserControlReg()
		{
			if( ! Compass.DirectAccess )
				WriteTo( MPU9250_RA_USER_CTRL, 0b00100000 ); // Enable master I2C mode
		}

		void UpdateConfigReg()
		{
			uint8_t AValue =	(( GyroscopeThermometerFilter - 2 ) & 0b111 ) |
//								( Queue.BlockOnFull ? 0b01000000 : 0 ) |
								( FrameSynchronization.Location << 4 );

			WriteTo( MPU9250_RA_CONFIG, AValue );
		}

		void UpdateGyroConfigReg()
		{
			uint8_t AValue;
			switch( GyroscopeThermometerFilter )
			{
				case gtf_GB_8800Hz_GF_32KHz_TB_4000Hz: AValue = 0b11; break;
				case gtf_GB_3600Hz_GF_32KHz_TB_4000Hz: AValue = 0b10; break;
				default : AValue = 0b00;
			}

			AValue |=	(( Gyroscope.FullScaleRange & 0b11 ) << 3 ) |
						( Gyroscope.X.SelfTest ? 0b10000000 : 0 ) |
						( Gyroscope.Y.SelfTest ? 0b01000000 : 0 ) |
						( Gyroscope.Z.SelfTest ? 0b00100000 : 0 );

			WriteTo( MPU9250_RA_GYRO_CONFIG, AValue );
		}

		void UpdateAccelerometerConfigReg()
		{
			byte AValue =	(( Accelerometer.FullScaleRange & 0b11 ) << 3 ) |
							( Accelerometer.X.SelfTest ? 0b10000000 : 0 ) |
							( Accelerometer.Y.SelfTest ? 0b01000000 : 0 ) |
							( Accelerometer.Z.SelfTest ? 0b00100000 : 0 );

			WriteTo( MPU9250_RA_ACCEL_CONFIG, AValue );
		}

		void UpdateAccelerometerConfig2Reg()
		{
			const uint8_t Values[] =
			{
				0b00001000,
				0,
				1,
				2,
				3,
				4,
				5,
				6
			};

			uint8_t AValue = Values[ Accelerometer.Filter ];

			WriteTo( MPU9250_RA_ACCEL_CONFIG_2, AValue );
		}

		void UpdatenterruptPinAndBypassConfigReg( bool ADirectCompassAccess )
		{
			byte AValue =	( Interrupt.Inverted								?		0b10000000 : 0 ) |
							( Interrupt.OpenDrain								?		0b01000000 : 0 ) |
							( Interrupt.Latch									?		0b00100000 : 0 ) |
							( FrameSynchronization.InterruptOnLowLevel			?		0b00001000 : 0 ) |
							( FrameSynchronization.EnableInterrupt				?		0b00000100 : 0 ) |
							( /*Compass.Queue & */ ( ! ADirectCompassAccess )		? 0 :	0b00000010 );

//			Serial.print( "UpdatenterruptPinAndBypassConfigReg: " ); Serial.println( AValue, BIN );

			WriteTo( MPU9250_RA_INT_PIN_CFG, AValue );
		}

		void UpdatenterruptPinAndBypassConfigRegNormal()
		{
			UpdatenterruptPinAndBypassConfigReg( Compass.DirectAccess );
		}

		void UpdateSampleRateDividerReg()
		{
			WriteTo( MPU9250_RA_SMPLRT_DIV, SampleRateDivider );
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
			byte AValue =	( Interrupt.OnMotion		?	0b01000000 : 0 ) |
//							( Interrupt.OnQueueOverflow	?	0b00010000 : 0 ) |
							( Interrupt.OnFrameSync		?	0b00001000 : 0 ) |
							( Interrupt.OnRawReady		?	0b00000001 : 0 );

			WriteTo( MPU9250_RA_INT_ENABLE, AValue );
		}

		void UpdateMotionThresholdReg()
		{
			byte AValue = MotionDetect.Threshold * 255 / 1020 + 0.5;
			WriteTo( MPU9250_RA_MOT_THR, AValue );
		}

		void UpdateMotionDetectReg()
		{
			byte AValue =	( MotionDetect.Enabled						?	0b10000000 : 0 ) |
							( MotionDetect.CompareWithPreviousSample	?	0b01000000 : 0 );

			WriteTo( MPU9250_RA_MOT_DETECT_CTRL, AValue );
		}


		void UpdateAccelerometerODRControlReg()
		{
			const float CLevels[] =
			{
				0.24,
				0.49,
				0.98,
				1.95,
				3.91,
				7.81,
				15.63,
				31.25,
				62.50,
				125,
				250,
				500
			};

			int AValue = -1;
			for( int i = 0; i < sizeof( CLevels ) / sizeof( CLevels[ 0 ] ); ++i )
				if( LowPowerMode.SampleFrequency <= CLevels[ i ] )
				{
					AValue = i;
					break;
				}

			if( AValue == -1 )
				AValue = 11;

			WriteTo( MPU9250_RA_LP_ACCEL_ODR, AValue );
		}


		void UpdateCompassSlaveI2CReg()
		{
//			WriteTo( MPU9250_RA_I2C_MST_CTRL, 0b10000000 );
			WriteTo( MPU9250_RA_I2C_SLV0_ADDR, 0b10000000 | MPU9250Const::MPU9150_RA_MAG_ADDRESS );
			WriteTo( MPU9250_RA_I2C_SLV0_REG, MPU9250Const::MPU9150_RA_MAG_ST1 );
//			WriteTo( MPU9250_RA_I2C_SLV0_CTRL, ( ( ! Compass.DirectAccess ) || Compass.Queue ) ? 0b10000000 | 8 : 0 );
			WriteTo( MPU9250_RA_I2C_SLV0_CTRL, ( ! Compass.DirectAccess ) ? 0b10000000 | 8 : 0 );
		}

		void UpdateCompassControlReg()
		{
			byte AValue;
			if( Compass.Enabled )
				AValue = CompassAK8963Const::CompassModes[ Compass.Mode ];

			else
				AValue = 0;

			AValue |=	( Compass.HighResolution	? 0b00010000 : 0 );

			I2C::WriteByte( FWire, MPU9250Const::MPU9150_RA_MAG_ADDRESS, MPU9250Const::MPU9150_RA_MAG_CNTL1, AValue );
		}

		void UpdateCompassSelfTestReg()
		{
			byte AValue = ( Compass.SelfTest	? 0b01000000 : 0 );
			I2C::WriteByte( FWire, MPU9250Const::MPU9150_RA_MAG_ADDRESS, MPU9250Const::MPU9150_RA_MAG_ASTC, AValue );
		}

		void ReadCompassAdjustmentValues()
		{
			uint8_t AValues[ 3 ];

			I2C::ReadBytes( FWire, MPU9250Const::MPU9150_RA_MAG_ADDRESS, MPU9250Const::MPU9150_RA_MAG_ASAX, sizeof( AValues ), AValues );
			for( int i = 0; i < 3; ++i )
				CompassAdjustmentValues[ i ] = (((float) AValues[ i ] ) - 128.0f) / 256.0f + 1.0f;

		}

	protected:
		virtual void SystemStart() override
		{
			UpdatePowerManagementReg1( true );
			UpdatePowerManagementReg2();
			UpdateConfigReg();
			UpdateGyroConfigReg();
			UpdateAccelerometerConfigReg();
			UpdateAccelerometerConfig2Reg();
			UpdateSampleRateDividerReg();
			UpdatenterruptPinAndBypassConfigReg( true );
			UpdateCompassControlReg();
			UpdateCompassSlaveI2CReg();
			UpdateCompassSelfTestReg();
			UpdateUserControlReg();
			ReadCompassAdjustmentValues();
//			UpdatenterruptPinAndBypassConfigReg( Compass.DirectAccess && ! Compass.Queue );
			UpdatenterruptPinAndBypassConfigRegNormal();
			UpdateFIFOReg();
			UpdateMotionThresholdReg();
			UpdateMotionDetectReg();
			UpdateAccelerometerODRControlReg();
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
				if( ReadBytes( MPU9250_RA_INT_STATUS, 1, &AIntValue ) )
//				if( Queue.OverflowOutputPin.IsConnected() || MotionDetect.OutputPin.IsConnected() || FrameSynchronization.OutputPin.IsConnected() )
				{
					if( AIntValue & 0x01000000 )
						MotionDetect.OutputPin.Notify( nullptr );

//					if( AIntValue & 0x00010000 )
//						Queue.OverflowOutputPin.Notify( nullptr );

					if( AIntValue & 0x00001000 )
						FrameSynchronization.OutputPin.Notify( nullptr );

				}
			}

			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		MPU9250I2C( TwoWire &AWire ) :
			ClockSource( mcsAutoSelect ),
			GyroscopeThermometerFilter( gtf_GB_250Hz_GF_8KHz_TB_4000Hz ),
			Enabled( true ),
			Address( false ),
			Compass( AWire ),
			FWire( AWire )
//			FUseFIFO( false )
		{
			ResetInputPin.SetCallback( MAKE_CALLBACK( MPU9250I2C::DoResetReceive ));
		}
	};
//---------------------------------------------------------------------------
}

#endif

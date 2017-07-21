////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BASICMPU_h
#define _MITOV_BASICMPU_h

#include <Mitov.h>
#include <Wire.h> //I2C Arduino Library
#include <Mitov_Basic_I2C.h>

namespace Mitov
{
	namespace MPUConst
	{
		const byte Adresses[ 2 ] = { 0x68, 0x69 };
	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	enum MPUClockSource { mcsAutoSelect = 6, mcsInternal = 0, mcsGyroX = 1, mcsGyroY = 2, mcsGyroZ = 3, mcsExt32K = 4, mcsExt19M = 5, mcsReset = 7 };
//---------------------------------------------------------------------------
	class MPUBasicSensor : public OpenWire::Object
	{
	public:
		OpenWire::SourcePin	OutputPins[ 3 ];

	};
//---------------------------------------------------------------------------
	class MPUOptionalSensor
	{
	public:
		bool Enabled : 1;
//		bool Queue : 1;
		bool SelfTest : 1; // Added to save space as bitfield

	public:
		MPUOptionalSensor() :
			Enabled( true ),
//			Queue( false ),
			SelfTest( false )
		{
		}

	};
//---------------------------------------------------------------------------
	typedef MPUOptionalSensor MPUOptionalSelfTestSensor;
//---------------------------------------------------------------------------
	class MPUOptionalAxesSensor : public MPUBasicSensor
	{
	public:
		MPUOptionalSelfTestSensor	X;
		MPUOptionalSelfTestSensor	Y;
		MPUOptionalSelfTestSensor	Z;
	};
//---------------------------------------------------------------------------
	enum ArduinoMPUAccelerometerRange { ar2g, ar4g, ar8g, ar16g };
//---------------------------------------------------------------------------
	enum ArduinoMPU9250AccelerometerFilter
	{
      af_AB_1130Hz_AF_4KHz,
      af_AB_460Hz_AF_1KHz,
      af_AB_184Hz_AF_1KHz,
      af_AB_92Hz_AF_1KHz,
      af_AB_41Hz_AF_1KHz,
      af_AB_20Hz_AF_1KHz,
      af_AB_10Hz_AF_1KHz,
      af_AB_5Hz_AF_1KHz
	};
//---------------------------------------------------------------------------
	class MPUAccelerometer : public MPUOptionalAxesSensor
	{
	public:
		ArduinoMPUAccelerometerRange FullScaleRange : 2;
//		bool	Queue : 1;
		ArduinoMPU9250AccelerometerFilter	Filter : 3; // Added in the base class to save spece with bitfields

	public:
		MPUAccelerometer() :
			FullScaleRange( ar2g ),
//			Queue( false ),
			Filter( af_AB_1130Hz_AF_4KHz )
		{
		}

	};
//---------------------------------------------------------------------------
	enum TArduinoMPUGyroscopeRange { gr250dps, gr500dps, gr1000dps, gr2000dps };
//---------------------------------------------------------------------------
	class MPU9250Gyroscope : public MPUOptionalAxesSensor
	{
	public:
		TArduinoMPUGyroscopeRange FullScaleRange : 2;
		TAngleUnits	Units : 2;
		bool		Standby : 1; // Added in the base class to save spece with bitfields

	public:
		MPU9250Gyroscope() :
			FullScaleRange( gr250dps ),
			Units( auDegree ),
			Standby( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class MPUThermometer : public MPUOptionalSensor
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	InFahrenheit = false;

	};
//---------------------------------------------------------------------------
	enum MPUFrameSynchronizationLocation { fslDisabled, fslThermometer, fslGyroscopeX, fslGyroscopeY, fslGyroscopeZ, fslAccelerometerX, fslAccelerometerY, fslAccelerometerZ };
//---------------------------------------------------------------------------
	class MPUFrameSynchronization
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		MPUFrameSynchronizationLocation Location : 3;
		bool	EnableInterrupt : 1;
		bool	InterruptOnLowLevel : 1;

	public:
		MPUFrameSynchronization() :
			Location( fslDisabled ),
			EnableInterrupt( false ),
			InterruptOnLowLevel( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class MPUInterrupt
	{
	public:
		bool	Inverted : 1; // Added in the base class to save memory trough bitfields!
		bool	OpenDrain : 1; // Added in the base class to save memory trough bitfields!
		bool	Latch : 1; // Added in the base class to save memory trough bitfields!
		bool	OnMotion : 1; // Added in the base class to save memory trough bitfields!
//		bool	OnQueueOverflow : 1;
		bool	OnFrameSync : 1;
		bool	OnRawReady : 1;

	public:
		MPUInterrupt() :
			Inverted( false ),
			OpenDrain( false ),
			Latch( true ),
			OnMotion( false ),
//			OnQueueOverflow( false ),
			OnFrameSync( false ),
			OnRawReady( false )
		{
		}
	};
//---------------------------------------------------------------------------
}

#endif

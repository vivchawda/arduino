////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GASSENSOR_h
#define _MITOV_GASSENSOR_h

#include <Mitov.h>

// Based on ideas from https://github.com/empierre/arduino/blob/master/AirQuality-Multiple_Gas_Sensor1_4.ino

namespace Mitov
{
/*
	class GasSensorMQ7 : public Mitov::CommonFilter
	{
		typedef Mitov::CommonFilter inherited;

    public:
        bool	Enabled = true;
		float	LoadResistor = 10000.0;
		float	CleanAirValue = 0.10;

	protected:
		virtual void DoReceive( void *_Data ) override
		{
//			float Ro = (((1.0 - CleanAirValue) * LoadResistor / CleanAirValue ) / LoadResistor); // Ro =[(Vc - Vo) * RL ] / (Vo * Ro_clean_air_factor)
			float Ro = (((1.0 - CleanAirValue) * LoadResistor ) / ( CleanAirValue * LoadResistor )); // Ro =[(Vc - Vo) * RL ] / (Vo * Ro_clean_air_factor)

			float Vo = *((float *)_Data);
			float Rs = (( 1.0 - Vo) * LoadResistor) / Vo;
			float RsRo_ratio = Rs / Ro;
			float ppm = pow((RsRo_ratio/22.073), (1/-0.66659));

			Serial.print( Ro ); Serial.print( " / " ); Serial.println( Rs );

			inherited::OutputPin.Notify( &ppm );
		}

	};
*/
	class GasSensorMQ135 : public Mitov::CommonSink
	{
		typedef Mitov::CommonSink inherited;

	protected:
		static const float	CCurves[ 6 ][ 2 ];

//		static const float           CO2Curve[2]     =  {113.7105289, -3.019713765};  //MQ135
//		static const float           CO_secCurve[2]  =  {726.7809737, -4.040111669};  //MQ135
//		static const float           NH4Curve[2]     =  {84.07117895, -4.41107687};   //MQ135 Ammonium
//		static const float           C2H50H_Curve[2] =  {74.77989144, 3.010328075};   //MQ135 Ethanol
//		static const float           CH3Curve[2]     =  {47.01770503, -3.281901967};  //MQ135 Methyl
//		static const float           CH3_2COCurve[2] =  {7.010800878, -2.122018939};  //MQ135 Acetone

    public:
		OpenWire::TypedSourcePin<float>	OutputPins[ 6 ];

    public:
		float	LoadResistor = 0.990;
		float	CleanAirValue = 0.04;

        bool	Enabled : 1;

	protected:
		bool	FChangeOnly : 1;

	public:
		bool	FCalibrating : 1;
		bool	FFirstSample : 1;

    public:
		float FTemperature = 20;
		float FHumidity	= 33;

	protected:
		virtual void DoReceive( void *_Data ) override
		{
			float AValue = *((float *)_Data );

			if( FCalibrating )
			{
				if( FFirstSample )
				{
					FFirstSample = false;
					CleanAirValue = AValue;
				}

				else
				{
					CleanAirValue += AValue;
					CleanAirValue /= 2;
				}

				return;
			}

//			Serial.println( CleanAirValue );

			float Ro = ((1000 * LoadResistor) / CleanAirValue - LoadResistor);
			float Rs = ((1000 * LoadResistor) / AValue - LoadResistor);
			for( int i = 0; i < 6; ++i )
				if( OutputPins[ i ].IsConnected() )
				{					
					const float	CORA = 0.00035;
					const float	CORB = 0.02718;
					const float	CORC = 1.39538;
					const float	CORD = 0.0018;

					float PPM = (CCurves[ i ][ 0 ] * pow(( Rs / Ro ), CCurves[ i ][ 1 ]));

//					Serial.println( FTemperature );
//					Serial.println( FHumidity );

					float ACorrectionFactor = CORA * FTemperature * FTemperature - CORB * FTemperature + CORC - ( FHumidity - 33.0 ) * CORD;

					PPM *= ACorrectionFactor;

					OutputPins[ i ].SetValue( PPM, FChangeOnly );
				}
			
			FChangeOnly = true;
		}

	public:
		GasSensorMQ135() :
			Enabled( true ),
			FChangeOnly( false ),
			FCalibrating( false ),
			FFirstSample( true )
		{
		}

	};
//---------------------------------------------------------------------------
	class TArduinoGasSensorMQCorrection : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	TemperatureInputPin;
		OpenWire::SinkPin	HumidityInputPin;

	protected:
		GasSensorMQ135 &FOwner;

	protected:
		void DoTemperatureReceive( void *_Data )
		{
			FOwner.FTemperature = *(float *)_Data;
		}

		void DoHumidityReceive( void *_Data )
		{
			FOwner.FHumidity = *(float *)_Data;
		}

	public:
		TArduinoGasSensorMQCorrection( GasSensorMQ135 &AOwner ) :
			FOwner( AOwner )
		{
			TemperatureInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TArduinoGasSensorMQCorrection::DoTemperatureReceive );
			HumidityInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TArduinoGasSensorMQCorrection::DoHumidityReceive );
		}

	};
//---------------------------------------------------------------------------
	class TArduinoGasSensorMQCallibration : public OpenWire::Component
	{
	public:
		OpenWire::ConnectSinkPin		CallibrateInputPin;
		OpenWire::TypedSourcePin<bool>	CallibratingOutputPin;

	public:
		uint32_t	Period = 25000;

	protected:
		GasSensorMQ135 &FOwner;
		unsigned long	FLastTime = 0;

	protected:
		void DoCallibrateReceive( void *_Data )
		{
			FOwner.FCalibrating = true;
			FOwner.FFirstSample = true;
			FLastTime = millis();
			CallibratingOutputPin.SetValue( true, true );
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ! FOwner.FCalibrating )
				return;

			if( millis() - FLastTime > Period )
			{
				FOwner.FCalibrating = false;
				CallibratingOutputPin.SetValue( false, true );
			}

//			inherited::SystemLoopBegin( currentMicros );
		}

		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			CallibratingOutputPin.SetValue( false, false );
		}

	public:
		TArduinoGasSensorMQCallibration( GasSensorMQ135 &AOwner ) :
			FOwner( AOwner )
		{
			CallibrateInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TArduinoGasSensorMQCallibration::DoCallibrateReceive );
		}

	};
//---------------------------------------------------------------------------
	const float	GasSensorMQ135::CCurves[ 6 ][ 2 ] =
	{
		{113.7105289, -3.019713765},  //MQ135 CO2
		{726.7809737, -4.040111669},  //MQ135 CO
		{47.01770503, -3.281901967},  //MQ135 CH3 - Methyl
		{84.07117895, -4.41107687},   //MQ135 NH4 - Ammonium
		{7.010800878, -2.122018939},  //MQ135 (CH3)2CO - Acetone
		{74.77989144, 3.010328075}    //MQ135 C2H50H - Alcohol, Ethanol
	};
//---------------------------------------------------------------------------
}

#endif

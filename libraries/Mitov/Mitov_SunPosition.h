////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SUN_POSITION_h
#define _MITOV_SUN_POSITION_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	#define _DEG_TO_RAD_	0.01745329

	class SunPosition : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	TimeInputPin;
		OpenWire::SourcePin	ElevationOutputPin;
		OpenWire::SourcePin	AzimuthOutputPin;

	public:
		float Longitude = 0.0;
		float Latitude = 0.0;

	public:
		TDateTime	InitialTime;

	public:
		void CalculateOutput()
		{
			uint32_t JD_whole = InitialTime.JulianDate();

//			Serial.print( "JD_whole: " ); Serial.println( JD_whole );

			uint16_t AHour;
			uint16_t AMin;
			uint16_t ASec;
			uint16_t AMSec;

			InitialTime.DecodeTime( AHour, AMin, ASec, AMSec );

			float JD_frac = ( AHour + AMin / 60.0 + ASec / 3600.0)/24.0 - 0.5;

//			Serial.print( "JD_frac: " ); Serial.println( JD_frac );

			float T = JD_whole-2451545; T=(T+JD_frac) / 36525.0;
//			Serial.print( "T: " ); Serial.println( T );

			float L0 = _DEG_TO_RAD_ * fmod(280.46645+36000.76983*T,360);
//			Serial.print( "L0: " ); Serial.println( L0 );

			float M = _DEG_TO_RAD_ * fmod(357.5291+35999.0503*T,360);
//			Serial.print( "M: " ); Serial.println( M );

			float e = 0.016708617 - 0.000042037 * T;
			float C = _DEG_TO_RAD_ * ((1.9146-0.004847*T) * sin(M) + (0.019993-0.000101*T) * sin(2*M) + 0.00029 * sin(3*M));
			float f = M + C;
//			Serial.print( "f: " ); Serial.println( f );

			float Obl = _DEG_TO_RAD_ * ( 23 + 26 / 60.0 + 21.448 / 3600.0 - 46.815 / 3600 * T );
//			Serial.print( "Obl: " ); Serial.println( Obl );

			int32_t JDx = JD_whole - 2451545;
			float GrHrAngle = 280.46061837 + ( 360 * JDx ) % 360 + 0.98564736629 * JDx + 360.98564736629 * JD_frac;
			GrHrAngle = fmod( GrHrAngle, 360.0 );
//			Serial.print( "GrHrAngle: " ); Serial.println( GrHrAngle );

			float L_true = fmod( C + L0, 2 * PI );
//			Serial.print( "L_true: " ); Serial.println( L_true );

			float R = 1.000001018 * ( 1 - e * e ) / ( 1 + e * cos(f));
//			Serial.print( "R: " ); Serial.println( R );

			float RA = atan2( sin(L_true) * cos(Obl), cos(L_true) );
//			Serial.print( "RA: " ); Serial.println( RA );

			float Decl=asin(sin(Obl)*sin(L_true));
//			Serial.print( "Decl: " ); Serial.println( Decl );

			float ALongitude = Longitude * _DEG_TO_RAD_;
			float ALatitude = Latitude * _DEG_TO_RAD_;

			float HrAngle = _DEG_TO_RAD_ * GrHrAngle + ALongitude - RA;
//			Serial.print( "HrAngle: " ); Serial.println( HrAngle );

			float elev = asin(sin(ALatitude) * sin(Decl) + cos(ALatitude) * (cos(Decl) * cos(HrAngle)));
//			Serial.print( "elev: " ); Serial.println( elev );

			// Azimuth measured eastward from north.
			float azimuth = PI + atan2(sin(HrAngle), cos(HrAngle) * sin(ALatitude) - tan(Decl) * cos(ALatitude));
//			Serial.print( "azimuth: " ); Serial.println( azimuth );

			elev /= _DEG_TO_RAD_;
			azimuth /= _DEG_TO_RAD_;

			ElevationOutputPin.Notify( &elev );
			AzimuthOutputPin.Notify( &azimuth );
		}

	protected:
		void DoTimeReceive( void *_Data )
		{
			TDateTime AValue = *(TDateTime*)_Data;
			if( InitialTime == AValue )
				return;

			InitialTime = AValue;
			CalculateOutput();
		}

	protected:
		virtual void SystemStart() 
		{
//			inherited::SystemStart();
			CalculateOutput();
		}

	public:
		SunPosition()
		{
			TimeInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&SunPosition::DoTimeReceive );
		}

	};
//---------------------------------------------------------------------------
	#undef _DEG_TO_RAD_
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

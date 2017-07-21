////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GPS_h
#define _MITOV_GPS_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class MitovSerialGPS : public OpenWire::Component, public OpenWire::Pin
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	LocationOutputPins[ 5 ];
		OpenWire::SourcePin	CourseOutputPins[ 2 ];
		OpenWire::SourcePin	SatellitesOutputPins[ 7 ];

		OpenWire::SourcePin	DateTimeOutputPin;
		OpenWire::SourcePin	InvalidOutputPin;
		OpenWire::SourcePin	ModeOutputPin;
		OpenWire::SourcePin	HorizontalPrecisionOutputPin;

/*
		OpenWire::SourcePin	LatitudeOutputPin;
		OpenWire::SourcePin	LongitudeOutputPin;
		OpenWire::SourcePin	AltitudeOutputPin;
		OpenWire::SourcePin	SpeedOutputPin;
		OpenWire::SourcePin	CourseOutputPin;
		OpenWire::SourcePin	MagneticVariationOutputPin;
		OpenWire::SourcePin	HeightAboveWGS84EllipsoidOutputPin;
		OpenWire::SourcePin	NumberOfSatellitesOutputPin;
		OpenWire::SourcePin	HorizontalPrecisionOutputPin;
*/
	public:
		bool	Enabled = true;

	protected:
		char	*FBuffer = nullptr; //[110]; // Do not make less than 100!
		uint8_t	FIndex = 0;

//	protected:
//		Mitov::BasicSerialPort &FSerial;

/*
	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			Serial.println( "." );
//			if( FNeedsRead || ( ! ClockInputPin.IsConnected() ))
				ReadSensor();

			inherited::SystemLoopBegin( currentMicros );
		}
*/
	protected:
		virtual void Receive( void *_Data ) override
		{
			Mitov::TDataBlock ADataBlock = *(Mitov::TDataBlock*)_Data;
			for( int i = 0; i < ADataBlock.Size; ++i )
				ProcessChar( ADataBlock.Data[ i ] );
		}

	protected:
		void ProcessChar( char AChar )
		{
//			int AChar = FSerial.GetStream().read();
//			Serial.print( AChar );
//			if( AChar < 0 )
//				return;

//			Serial.print( (char)AChar );
			if( AChar == 13 )
				return;

			if( AChar != 10 )
			{
				if( ! FBuffer )
					FBuffer = new char[ 110 ];

				FBuffer[ FIndex ++ ] = AChar;
				if( FIndex < 109 )
					return;
			}

			FBuffer[ FIndex ] = '\0';

			String AString = FBuffer;

			delete [] FBuffer;
			FBuffer = nullptr;
//			delay( 100 );

//			Serial.println( AString );

			if( AString.startsWith( "$GPRMC" ))
			{
				if( ! ProcessGPRMC( AString ))
					InvalidOutputPin.SendValue( true );
			}
			else if( AString.startsWith( "$GPGGA" ))
				ProcessGPGGA( AString );

			else if( AString.startsWith( "$GPGSV" ))
				ProcessGPGSV( AString );

			FIndex = 0;
		}

		bool EstractSubTextString( String &AString, String ASubText, int &AStartIndex, String &AValue )
		{
			int APos = AString.indexOf( ASubText, AStartIndex );
			if( APos < 0 )
				return false;

			AValue = AString.substring( AStartIndex, APos );

			AStartIndex = APos + 1;

			return true;
		}

		bool EstractSubString( String &AString, int &AStartIndex, String &AValue )
		{
			return EstractSubTextString( AString, ",", AStartIndex, AValue );
		}

		void ProcessGPGSV( String &AString )
		{
//			Serial.println( AString );
//			return;

			int AStartIndex = AString.indexOf( "*", 7 );
			if( AStartIndex < 0 )
				return;

			if( ! IsValidChecksum( AString, AStartIndex + 1 ))
				return;

			AString[ AStartIndex ] = ',';

			AStartIndex = 7;

			String ACountMesages;
			if( ! EstractSubString( AString, AStartIndex, ACountMesages ))
				return;

//			Serial.println( ">>>" );
//			Serial.println( ACountMesages );

			String AMesageNo;
			if( ! EstractSubString( AString, AStartIndex, AMesageNo ))
				return;

			if( AMesageNo == "1" )
				SatellitesOutputPins[ 1 ].Notify( nullptr );

//			Serial.println( AMesageNo );

			String ANumSatellites;
			if( ! EstractSubString( AString, AStartIndex, ANumSatellites ))
				return;

//			Serial.println( "" );
//			Serial.println( ANumSatellites );

//			Serial.println( "-------" );
//			Serial.println( "-------" );

			for( int i = 0; i < 4; ++i )
			{
//				Serial.println( "-------" );
				String APRN;
				if( ! EstractSubString( AString, AStartIndex, APRN ))
					return;

//				Serial.println( APRN );
				if( APRN != "" )
				{
					uint32_t APRNInt = APRN.toInt();
					SatellitesOutputPins[ 2 ].Notify( &APRNInt );
				}


				String AElevation;
				if( ! EstractSubString( AString, AStartIndex, AElevation ))
					return;

//				Serial.println( AElevation );

				if( AElevation != "" )
				{
					float AElevationFloat = AElevation.toFloat();
					SatellitesOutputPins[ 3 ].Notify( &AElevationFloat );
				}

				String AAzimuth;
				if( ! EstractSubString( AString, AStartIndex, AAzimuth ))
					return;

//				Serial.println( AAzimuth );

				if( AAzimuth != "" )
				{
					float AAzimuthFloat = AAzimuth.toFloat();
					SatellitesOutputPins[ 4 ].Notify( &AAzimuthFloat );
				}

				String ASNR;
				if( ! EstractSubString( AString, AStartIndex, ASNR ))
					return;

//				Serial.println( ASNR );

				if( ASNR != "" )
				{
					uint32_t ASNRInt = ASNR.toInt();
					SatellitesOutputPins[ 5 ].Notify( &ASNRInt );
				}

				SatellitesOutputPins[ 6 ].Notify( nullptr );
			}
		}

		void ProcessGPGGA( String &AString )
		{
//			Serial.println( "TEST" );
//			Serial.println( AString );
			int AStartIndex = 7;

			String ASkipValue;
			if( ! EstractSubString( AString, AStartIndex, ASkipValue )) // Time
				return;

//			Serial.println( "" );
//			Serial.println( ASkipValue );
//			Serial.println( AStartIndex );
			if( ! EstractSubString( AString, AStartIndex, ASkipValue )) // Latitude
				return;

//			Serial.println( ASkipValue );
//			Serial.println( AStartIndex );
			if( ! EstractSubString( AString, AStartIndex, ASkipValue )) // Latitude
				return;

//			Serial.println( ASkipValue );
//			Serial.println( AStartIndex );
			if( ! EstractSubString( AString, AStartIndex, ASkipValue )) // Longitude
				return;

//			Serial.println( ASkipValue );
//			Serial.println( AStartIndex );
			if( ! EstractSubString( AString, AStartIndex, ASkipValue )) // Longitude
				return;

//			Serial.println( ASkipValue );
//			Serial.println( AStartIndex );
			if( ! EstractSubString( AString, AStartIndex, ASkipValue )) // Fix Quality // 0 = Invalid // 1 = GPS fix // 2 = DGPS fix
				return;

//			Serial.println( ASkipValue );
//			Serial.println( AStartIndex );

			String ANumSatellites;
			if( ! EstractSubString( AString, AStartIndex, ANumSatellites ))
				return;

//			Serial.println( "ANumSatellites" );
//			Serial.println( ANumSatellites );
//			Serial.println( AStartIndex );

			String APrecision;
			if( ! EstractSubString( AString, AStartIndex, APrecision ))
				return;

//			Serial.println( APrecision );
//			Serial.println( AStartIndex );

			String AAltitude;
			if( ! EstractSubString( AString, AStartIndex, AAltitude ))
				return;

//			Serial.print( "AAltitude: " );
//			Serial.println( AAltitude );
//			Serial.println( AStartIndex );

			if( ! EstractSubString( AString, AStartIndex, ASkipValue )) // Meters
				return;

			String AWGS84;
			if( ! EstractSubString( AString, AStartIndex, AWGS84 ))
				return;

			if( ! EstractSubString( AString, AStartIndex, ASkipValue )) // Meters
				return;

			if( ! EstractSubString( AString, AStartIndex, ASkipValue )) // Time since last DGPS update
				return;

			if( ! EstractSubTextString( AString, "*", AStartIndex, ASkipValue )) // DGPS reference station id
				return;

			if( ! IsValidChecksum( AString, AStartIndex ))
				return;

//			Serial.println( AWGS84 );
			if( ANumSatellites != "" )
			{
				uint32_t ANumSatellitesInt = ANumSatellites.toInt();
				SatellitesOutputPins[ 0 ].Notify( &ANumSatellitesInt );
			}

			if( APrecision != "" )
			{
//				Serial.println( APrecision );
				float APrecisionFloat = APrecision.toFloat();
				HorizontalPrecisionOutputPin.Notify( &APrecisionFloat );
			}

			if( AAltitude != "" )
			{
				float AAltitudeFloat = AAltitude.toFloat();
				LocationOutputPins[ 2 ].Notify( &AAltitudeFloat );
			}

			if( AWGS84 != "" )
			{
				float AWGS84Float = AWGS84.toFloat();
				LocationOutputPins[ 3 ].Notify( &AWGS84Float );
			}

//			AltitudeOutputPin
		}

		bool ProcessGPRMC( String &AString )
		{
//			Serial.println( AString );
			int AStartIndex = 7;

			String ATime;
			if( ! EstractSubString( AString, AStartIndex, ATime ))
				return false;

			if( ATime.length() < 6 )
				ATime = "";

//			Serial.println( ATime );

			String AValid;
			if( ! EstractSubString( AString, AStartIndex, AValid ))
				return false;

//			Serial.println( AValid );

			String ALatitude;
			if( ! EstractSubString( AString, AStartIndex, ALatitude ))
				return false;

			if( ALatitude.length() < 3 )
				ALatitude = "";

//			Serial.println( ALatitude );

			String ANorthSouth;
			if( ! EstractSubString( AString, AStartIndex, ANorthSouth ))
				return false;

//			Serial.println( ANorthSouth );

			String ALongitude;
			if( ! EstractSubString( AString, AStartIndex, ALongitude ))
				return false;

//			Serial.println( ALongitude );

			String AEastWest;
			if( ! EstractSubString( AString, AStartIndex, AEastWest ))
				return false;

//			Serial.println( AEastWest );

			if( ALongitude.length() < 4 )
				ALongitude = "";

			String ASpeed;
			if( ! EstractSubString( AString, AStartIndex, ASpeed ))
				return false;

//			Serial.println( ASpeed );

			String ACourse;
			if( ! EstractSubString( AString, AStartIndex, ACourse ))
				return false;

//			Serial.println( ACourse );

			String ADate;
			if( ! EstractSubString( AString, AStartIndex, ADate ))
				return false;

			if( ADate.length() < 6 )
				ADate = "";

//			Serial.println( ADate );

			String AVariation;
			if( ! EstractSubString( AString, AStartIndex, AVariation ))
				return false;

//			Serial.println( AVariation );

			String AVariationEastWest;
			String AMode;
			if( AString.indexOf( ",", AStartIndex ))
			{
				if( ! EstractSubString( AString, AStartIndex, AVariationEastWest ))
					return false;

				if( ! EstractSubTextString( AString, "*", AStartIndex, AMode ))
					return false;
			}
			else
			{
				if( ! EstractSubTextString( AString, "*", AStartIndex, AVariationEastWest ))
					return false;
			}


//			Serial.println( AVariationEastWest );
//			Serial.println( AMode );

			if( ! IsValidChecksum( AString, AStartIndex ))
				return false;

//			Serial.println( AChecksumText );
//			if( ! EstractSubTextString( AString, "*", AStartIndex, AChecksum ))
//				return false;

			InvalidOutputPin.SendValue( AValid != "A" );

			if( ANorthSouth != "" && ALatitude != "" )
			{
				String ALatitudeValue = ALatitude.substring( 0, 2 );

//				Serial.println( ALatitudeValue );

				String ADegrees = ALatitude.substring( 2 );
//				Serial.println( ADegrees );

				float ALatitudeFloat = ALatitudeValue.toFloat();
				float ADegreesFloat = ADegrees.toFloat();
				ALatitudeFloat += ADegreesFloat / 60;
				if( ANorthSouth == "S" )
					ALatitudeFloat = -ALatitudeFloat;

				LocationOutputPins[ 0 ].Notify( &ALatitudeFloat );
			}

			if( AEastWest != "" && ALongitude != "" )
			{
//				Serial.println( ALongitude );

				String ALongitudeValue = ALongitude.substring( 0, 3 );
//				Serial.println( ALongitudeValue );

				String ADegrees = ALongitude.substring( 3 );
//				Serial.println( ADegrees );

				float ALongitudeFloat = ALongitudeValue.toFloat();
				float ADegreesFloat = ADegrees.toFloat();

				ALongitudeFloat += ADegreesFloat / 60;

				if( AEastWest == "W" )
					ALongitudeFloat = -ALongitudeFloat;

				LocationOutputPins[ 1 ].Notify( &ALongitudeFloat );
			}

			if( ASpeed != "" )
			{
				float ASpeedFloat = ASpeed.toFloat();
				CourseOutputPins[ 0 ].Notify( &ASpeedFloat );
			}

			if( ( ATime != "" ) && ( ADate != "" ) )
			{
				String AValueText = ATime.substring( 0, 2 );
//				Serial.println( AValueText );
				uint16_t AHour = AValueText.toInt();

				AValueText = ATime.substring( 2, 4 );
//				Serial.println( AValueText );
				uint16_t AMinute = AValueText.toInt();

				AValueText = ATime.substring( 4 );
//				Serial.println( AValueText );
				float ASecond = AValueText.toFloat();

				AValueText = ADate.substring( 0, 2 );
//				Serial.println( AValueText );
				uint16_t ADay = AValueText.toInt();

				AValueText = ADate.substring( 2, 4 );
//				Serial.println( AValueText );
				uint16_t AMonth = AValueText.toInt();

				AValueText = ADate.substring( 4 );
//				Serial.println( AValueText );
				uint16_t AYear = AValueText.toInt();

				Mitov::TDateTime ADateTime;

				if( ADateTime.TryEncodeDateTime( 2000 + AYear, AMonth, ADay, AHour, AMinute, ASecond, ( ASecond - int(ASecond )) * 1000 ))
					DateTimeOutputPin.Notify( &ADateTime );
			}

			if( ACourse != "" )
			{
				float ACourseFloat = ACourse.toFloat();
				CourseOutputPins[ 1 ].Notify( &ACourseFloat );
			}

			if( AVariationEastWest != "" && AVariation != "" )
			{
				float AVariationFloat = AVariation.toFloat();
				if( AVariationEastWest == "W" )
					AVariationFloat = -AVariationFloat;

				LocationOutputPins[ 4 ].Notify( &AVariationFloat );
			}

			if( AMode == "N" ) // Data not valid
				ModeOutputPin.SendValue<uint32_t>( 1 );

			else if( AMode == "A" ) // Autonomous
				ModeOutputPin.SendValue<uint32_t>( 2 );

			else if( AMode == "D" ) // Differential
				ModeOutputPin.SendValue<uint32_t>( 3 );

			else if( AMode == "E" ) // Estimated
				ModeOutputPin.SendValue<uint32_t>( 4 );

			else
				ModeOutputPin.SendValue<uint32_t>( 0 );

			return true;
		}

		bool IsValidChecksum( String AString, int AStartIndex )
		{
			String AChecksum = AString.substring( AStartIndex );

//			Serial.println( AChecksum );

			uint8_t AByteCheckSum = 0;
			for( int i = 1; i < AStartIndex - 1; ++i )
				AByteCheckSum ^= AString[ i ];

			String AChecksumText( AByteCheckSum, HEX );
			AChecksumText.toUpperCase();

			return( AChecksumText == AChecksum );
		}

	public:
		MitovSerialGPS( Mitov::BasicSerialPort &ASerial, OpenWire::Pin &ASerialOutputPin )
//			FSerial( ASerial )
		{
			ASerialOutputPin.Connect( *this );
		}

	};
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

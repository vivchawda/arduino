////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef MITOV_ESP8266_PROGRAM_OVER_WIFI_h
#define MITOV_ESP8266_PROGRAM_OVER_WIFI_h

#include <Mitov.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class ProgramESP8266OverWiFi : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	UploadingOutputPin;
//		OpenWire::SourcePin	StartOutputPin;
//		OpenWire::SourcePin	EndOutputPin;
		OpenWire::TypedStartSourcePin<float>	ProgressOutputPin;
		OpenWire::SourcePin	ErrorOutputPin;

	public:
		uint16_t	Port = 8266;
		String		Hostname;
		String		Password;
		
	protected:
		virtual void SystemStart() override
		{
			ArduinoOTA.onStart([ this ]() 
					{
						UploadingOutputPin.SendValue<bool>( true );
//						StartOutputPin.Notify( nullptr );
					}
				);

			ArduinoOTA.onEnd([ this ]() 
					{
						UploadingOutputPin.SendValue<bool>( false );
//						EndOutputPin.Notify( nullptr );
					}
				);

			ArduinoOTA.onProgress([ this ](unsigned int progress, unsigned int total) 
					{
						float AValue = float( progress ) / total;
						ProgressOutputPin.Notify( &AValue );
//						Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
					}
				);

			ArduinoOTA.onError([ this ](ota_error_t error) 
					{
						ErrorOutputPin.SendValue<uint32_t>( error );
					}
				);

			ArduinoOTA.setPort( Port );
//			ArduinoOTA.setRebootOnSuccess( RebootOnSuccess );
//			ArduinoOTA.setMdnsEnabled( MdnsEnabled );

//			ArduinoOTA.setPassword( "admin" );
//			ArduinoOTA.setPassword( nullptr );
//			Serial.println( "TEST1" );
//			delay( 1000 );

			if( Hostname != "" )
				ArduinoOTA.setHostname( (char *)Hostname.c_str() );

			if( Password != "" )
				ArduinoOTA.setPassword( (char *)Password.c_str() );

			else
				ArduinoOTA.setPassword( nullptr );

//			ArduinoOTA.setPassword( "" );
			ArduinoOTA.begin();


//			Serial.println( ArduinoOTA.getHostname());
//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			ArduinoOTA.handle();
//			inherited::SystemLoopBegin( currentMicros );
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

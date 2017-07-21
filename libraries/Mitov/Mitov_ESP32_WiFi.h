////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ESP32_WIFI_h
#define _MITOV_ESP32_WIFI_h

#include <Mitov.h>
#include <Mitov_BasicEthernet.h>
#include <WiFi.h>
//#include "ip_addr.h"
//#include "espconn.h"

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class ESP32WiFiModule;
//---------------------------------------------------------------------------
	class ESP32WiFiModuleOptionalDisabledElement
	{
	public:
		bool	Enabled = false;

	};
//---------------------------------------------------------------------------
	class ESP32WiFiModuleOptionalChannel : public ESP32WiFiModuleOptionalDisabledElement
	{
	public:
		unsigned long	Channel = 1;

	};
//---------------------------------------------------------------------------
	class ESP32WiFiModuleOptionalIPConfig : public ESP32WiFiModuleOptionalDisabledElement
	{
	public:
		::IPAddress	IP;

	};
//---------------------------------------------------------------------------
	class ESP32WiFiModuleAccessPointConfig
	{
	public:
		bool		Enabled = false;

		::IPAddress	IP;
		::IPAddress	Gateway;
		::IPAddress	Subnet;

	};
//---------------------------------------------------------------------------
	class ESP32WiFiModuleRemoteConfig : public ESP32WiFiModuleAccessPointConfig
	{
	public:
		 ESP32WiFiModuleOptionalIPConfig	DNS;
	};
//---------------------------------------------------------------------------
	class ESP32WiFiModuleOptionalMacAddress : public ESP32WiFiModuleOptionalDisabledElement
	{
	public:
		TMACAddress MacAddress;

	};
//---------------------------------------------------------------------------
	class ESP32ModuleRemoteAccessPoint
	{
	public:
		bool	Enabled = true;
		String	SSID;
	    String	Password;
		ESP32WiFiModuleOptionalChannel	Channel;
		ESP32WiFiModuleOptionalMacAddress	MacAddress;
		ESP32WiFiModuleRemoteConfig		Config;

//	protected:
//		ESP32WiFiModule &FOwner;

	public:
		bool Connect()
		{
			if( ! Enabled )
				return false;

			if( SSID == "" )
				return false;

			const char *APassword;
			if( Password == "" )
				APassword = NULL;

			else
				APassword = Password.c_str();

			int32_t channel;

			if( Channel.Enabled )
				channel = Channel.Channel;

			else
				channel = 0;

			const uint8_t* bssid;

			if( MacAddress.Enabled )
				bssid = MacAddress.MacAddress.FMacAddress;

			else
				bssid = NULL;

//			Serial.println( SSID );
//			Serial.println( APassword );

			if( Config.Enabled )
			{
				if( Config.DNS.Enabled )
					WiFi.config( Config.IP, Config.Gateway, Config.Subnet, Config.DNS.IP );

				else
					WiFi.config( Config.IP, Config.Gateway, Config.Subnet);
			}

			WiFi.begin( (char *)SSID.c_str(), APassword, channel, bssid );

			for(;;)
			{
//				Serial.print( "." ) ;
				int ARes = WiFi.status();
				if( ARes == WL_CONNECTED )
					return true;

				if( ARes == WL_CONNECT_FAILED )
					return false;

				delay(500);
			}

/*
			while (WiFi.status() != WL_CONNECTED) {
			WL_CONNECT_FAILED

			return ( ARes == WL_CONNECTED );
*/
//			return ( WiFi.begin( (char *)SSID.c_str(), APassword, channel, bssid ) == WL_CONNECTED );


//			return ( WiFi.begin( (char *)SSID.c_str() ) == WL_CONNECTED );


/*
			if( Password == "" )
				return ( LWiFi.connect( SSID.c_str() ) > 0 );

			switch( Encription )
			{
				case liweAuto:
				{
					if( LWiFi.connectWPA( SSID.c_str(), Password.c_str() ) > 0 )
						return true;

					return( LWiFi.connectWEP( SSID.c_str(), Password.c_str() ) > 0 );
				}

				case liweWEP:
					return( LWiFi.connectWEP( SSID.c_str(), Password.c_str() ) > 0 );

				case liweWPA:
					return( LWiFi.connectWPA( SSID.c_str(), Password.c_str() ) > 0 );
			}
*/
		}

	protected:
		void RegisterIn( ESP32WiFiModule &AOwner );

	public:
		ESP32ModuleRemoteAccessPoint( ESP32WiFiModule &AOwner, TMACAddress AMacAddress ) :
			ESP32ModuleRemoteAccessPoint( AOwner )
		{
			MacAddress.MacAddress = AMacAddress;
		}

		ESP32ModuleRemoteAccessPoint( ESP32WiFiModule &AOwner )
		{
			RegisterIn( AOwner );
		}

	};
//---------------------------------------------------------------------------
	class ESP32WiFiModuleAccessPoint
	{
	protected:
		ESP32WiFiModule *FOwner;

	public:
		bool	Enabled : 1;
		bool	IsHidden : 1;
		String	SSID;
		String	Password;

		unsigned long	Channel = 1;

		ESP32WiFiModuleAccessPointConfig	Config;

	public:
		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			if( Enabled )
				TryStart();

			else
				WiFi.softAPdisconnect();

		}

	public:
		void TryStart();

	public:
		ESP32WiFiModuleAccessPoint( ESP32WiFiModule * AOwner ) :
			FOwner( AOwner ),
			Enabled( true ),
			IsHidden( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class ESP32ModuleScanNetworksOperation : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	ScanInputPin;

		OpenWire::SourcePin	CountOutputPin;

		OpenWire::SourcePin	SignalStrengthOutputPin;
		OpenWire::SourcePin	SSIDOutputPin;
		OpenWire::SourcePin	ChannelOutputPin;
		OpenWire::SourcePin	EncryptionOutputPin;
//		OpenWire::SourcePin	IsHiddenOutputPin;
		OpenWire::SourcePin	MACAddressOutputPin;

		OpenWire::TypedSourcePin<bool>	ScanningOutputPin;
		OpenWire::SourcePin	FailedOutputPin;
		OpenWire::SourcePin	FoundNetworkOutputPin;

	public:
		bool	ShowHidden = false;

	protected:
		bool	FScanRequest = false;

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			inherited::SystemLoopBegin( currentMicros );
			ScanningOutputPin.SetValue( FScanRequest, true );
			if( FScanRequest )
			{
				int8_t AComplete = WiFi.scanComplete();
				if( AComplete == WIFI_SCAN_FAILED )
				{
					FailedOutputPin.Notify( NULL );
					CountOutputPin.SendValue( 0 );
					FScanRequest = false;
				}

				else if( AComplete >= 0 )
				{
					CountOutputPin.SendValue( (int)AComplete );
					for( int i = 0; i < AComplete; i ++ )
					{
						String ssid;
						uint8_t encryptionType;
						int32_t ASetrength;
						uint8_t* BSSID;
						int32_t channel;
//						bool isHidden;
						if( WiFi.getNetworkInfo( i, ssid, encryptionType, ASetrength, BSSID, channel )) //, isHidden ))
						{
							String BSSIDStr = WiFi.BSSIDstr( i );

							SSIDOutputPin.Notify( (void *)ssid.c_str() );
							SignalStrengthOutputPin.Notify( &ASetrength );
							EncryptionOutputPin.Notify( &encryptionType );
							MACAddressOutputPin.Notify( (void *)BSSIDStr.c_str() );
							ChannelOutputPin.Notify( &channel );
//							IsHiddenOutputPin.Notify( &isHidden );
						}

						FoundNetworkOutputPin.Notify( NULL );
					}

					FScanRequest = false;
				}
			}
		}

	protected:
		void DoScanNetworks( void *_Data )
		{
			if( WiFi.scanComplete() != WIFI_SCAN_RUNNING )
			{
//				WiFi.scanNetworks( true, ShowHidden );
				WiFi.scanNetworks( false, ShowHidden );
				FScanRequest = true;
			}
/*
			for( int i = 0; i < nearbyAccessPointCount; i ++ )
			{
				FoundSSIDOutputPin.Notify( LWiFi.SSID( i ));

				int32_t ASetrength = LWiFi.RSSI( i );
				FoundSignalStrengthOutputPin.Notify( &ASetrength );
			}
*/
		}

	public:
		ESP32ModuleScanNetworksOperation()
		{			
			ScanInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ESP32ModuleScanNetworksOperation::DoScanNetworks );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, typename T_CLIENT> class TCPSecureClientSocket : public TCPClientSocket<T_OWNER, T_CLIENT>
	{
		typedef TCPClientSocket<T_OWNER, T_CLIENT> inherited;

	public:
		String	CACert;
		String	Certificate;
		String	PrivateKey;

	protected:
		virtual void StartSocket() override
		{
			if( CACert != "" )
				inherited::FClient.setCACert( CACert.c_str() );

			if( Certificate != "" )
				inherited::FClient.setCertificate( Certificate.c_str() );

			if( PrivateKey != "" )
				inherited::FClient.setPrivateKey( PrivateKey.c_str() );

			inherited::StartSocket();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class ESP32WiFiModule : public Mitov::BasicEthernetShield
	{
		typedef Mitov::BasicEthernetShield inherited;

	public:
		OpenWire::SourcePin	AddressOutputPin;
		OpenWire::SourcePin	MACOutputPin;

		OpenWire::SourcePin	BSSIDOutputPin;
		OpenWire::SourcePin	GatewayIPOutputPin;
		OpenWire::SourcePin	SubnetMaskIPOutputPin;

		OpenWire::TypedSourcePin<bool>	RemoteConnectedOutputPin;

	public:
		ESP32WiFiModuleAccessPoint	AccessPoint;
		Mitov::SimpleObjectList<ESP32ModuleRemoteAccessPoint*>	AccessPoints;

	public:
		String	HostName;
		bool	AutoReconnect : 1;

	public:
		bool	IsStarted : 1;

	public:
		virtual bool GetIPFromHostName( String AHostName, ::IPAddress &AAdress )
		{
			bool AResult = ( WiFi.hostByName( AHostName.c_str(), AAdress ) == 1 );
			if( ! AResult )
				AAdress = INADDR_NONE;

			return AResult;
		}

	protected:
		void StopEthernet()
		{
			inherited::StopEthernet();

			WiFi.disconnect( true );
			IsStarted = false;
		}

		void StartEthernet()
		{
/*
			if( ! AccessPoints.length() )
				return;

			if( ! Sockets.length() )
				return;
*/
//			Serial.println( "TRY CONNECT" );
			if( HostName != "" )
			{
				WiFi.setHostname( (char *)HostName.c_str() );
//				espconn_mdns_set_hostname( (char *) HostName.c_str() ); 
			}


			bool AConnected = false;
			for( int i = 0; i < AccessPoints.size(); ++i )
				if( AccessPoints[ i ]->Connect() )
				{
					AConnected = true;
//					Serial.println( "CONNECT" );

					if( AddressOutputPin.IsConnected() )
					{
						String IPAddress = IPAdressToString( WiFi.localIP());
						AddressOutputPin.Notify( (void *)IPAddress.c_str() );
					}

					if( MACOutputPin.IsConnected() )
					{
						String AAddress = WiFi.macAddress();
						MACOutputPin.Notify( (void *)AAddress.c_str() );
					}

					IsStarted = true;
					break;
				}

			if( ! AConnected )
				WiFi.begin();

			RemoteConnectedOutputPin.SetValue( AConnected, true );

//			WiFi.setAutoReconnect( AutoReconnect ); // For some reason not yet implemented!

			AccessPoint.TryStart();

			if( BSSIDOutputPin.IsConnected() )
				BSSIDOutputPin.SendValue( WiFi.BSSIDstr() );

			if( GatewayIPOutputPin.IsConnected() )
				GatewayIPOutputPin.SendValue( IPAdressToString( WiFi.gatewayIP() ));

			if( SubnetMaskIPOutputPin.IsConnected() )
				SubnetMaskIPOutputPin.SendValue( IPAdressToString( WiFi.subnetMask() ));

			for( int i = 0; i < Sockets.size(); ++i )
				Sockets[ i ]->TryStartSocket();

		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros )
		{
			if( RemoteConnectedOutputPin.IsConnected() )
				RemoteConnectedOutputPin.SetValue( WiFi.isConnected(), true );

			inherited::SystemLoopBegin( currentMicros );
		}

/*
	void DoCheckSignalStrength( void *_Data )
		{
			if( IsStarted )
				if( SignalStrengthOutputPin.IsConnected() )
				{
					int32_t ASetrength = WiFi.RSSI();
					SignalStrengthOutputPin.Notify( &ASetrength );
				}

		}
*/
	public:
		ESP32WiFiModule() :
			IsStarted( false ),
			AccessPoint( this ),
			AutoReconnect( true )
		{			
//			ScanNetworksInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ESP32WiFiModule::DoScanNetworks );
//			CheckSignalStrengthInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ESP32WiFiModule::DoCheckSignalStrength );
		}

	};
//---------------------------------------------------------------------------
	class ESP32ModuleClockedOperation : public Mitov::ClockingSupport
	{
	protected:
		ESP32WiFiModule &FOwner;

	public:
		ESP32ModuleClockedOperation( ESP32WiFiModule &AOwner ) :
			FOwner( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
	class ESP32ModuleSignalStrengthOperation : public ESP32ModuleClockedOperation
	{
		typedef ESP32ModuleClockedOperation	inherited;

	public:
		OpenWire::SourcePin	SignalStrengthOutputPin;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			if( inherited::FOwner.IsStarted )
				if( SignalStrengthOutputPin.IsConnected() )
				{
					int32_t ASetrength = WiFi.RSSI();
					SignalStrengthOutputPin.Notify( &ASetrength );
				}

		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class ESP32ModuleReconnectOperation : public ESP32ModuleClockedOperation
	{
		typedef ESP32ModuleClockedOperation	inherited;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			WiFi.reconnect();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	void ESP32WiFiModuleAccessPoint::TryStart()
	{
		if( FOwner->Enabled )
			if( Enabled )
				if( SSID != "" )
				{
//Serial.println( "TEST1" );
//Serial.println( SSID );
//Serial.println( IsHidden );
					if( Config.Enabled )
						WiFi.softAPConfig( Config.IP, Config.Gateway, Config.Subnet );

					if( Password != "" )
						WiFi.softAP( SSID.c_str(), Password.c_str(), Channel, IsHidden );

					else
//						WiFi.softAP( SSID.c_str() );
						WiFi.softAP( SSID.c_str(), NULL, Channel, IsHidden );

					WiFi.softAPIP();
				}

	}
//---------------------------------------------------------------------------
	void ESP32ModuleRemoteAccessPoint::RegisterIn( ESP32WiFiModule &AOwner )
	{
		AOwner.AccessPoints.push_back( this );
	}
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SERVO_h
#define _MITOV_SERVO_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class MitovMQTTClient;
//---------------------------------------------------------------------------
	enum TArduinoMQTTDelivery { dcOnceOrLess, dcOnceOrMore, dcOnceOnly };
//---------------------------------------------------------------------------
	class TCLMQTTClientWill
	{
	public:
		String	Topic;
		String	Message;
		TArduinoMQTTDelivery	Delivery;
	};
//---------------------------------------------------------------------------
	static const uint8_t PROGMEM MQIsdp_V[] =
	{
		0, 6, 
		'M', 'Q', 'I', 's', 'd', 'p',
		3
	};
//---------------------------------------------------------------------------
	static const uint8_t PROGMEM MQTT_V[] =
	{
		0, 4,
		'M', 'Q', 'T', 'T',
		4
	};
//---------------------------------------------------------------------------
	class ArduinoMQTTClientTextBasicTopic : public OpenWire::Object
	{
	public:
		String	Topic;
		String	InitialValue;

		TArduinoMQTTDelivery SendDelivery : 2;
		TArduinoMQTTDelivery ReceiveDelivery : 2;

		bool	Enabled	: 1;
		bool	Retain	: 1;

		bool	FSubscribed : 1; // In base class to save memory!

	protected:
		bool	FInitialValueSent : 1;

/*
		void SetEnabled( bool AValue )
		{
			Enabled = AValue;
		}
*/
	protected:
		MitovMQTTClient &FOwner;

	public:
		inline void ConnectionClosed()
		{
			FSubscribed = false;
		}

		virtual void Process()
		{
		}

		virtual bool NeedsSubscription()
		{
			return false;
		}

		virtual bool NeedsUnsubscription()
		{
			return false;
		}

		virtual void SendPayload( String APayload )
		{
		}

	public:
		ArduinoMQTTClientTextBasicTopic( MitovMQTTClient &AOwner );

	};
//---------------------------------------------------------------------------
	class TMQTTPacket
	{
	public:
		enum TMQTTMessageType 
		{
//          Reserved0,    //  0  Reserved
          BROKERCONNECT,  //  0  Broker request to connect to Broker
          CONNECT,        //  1  Client request to connect to Broker
          CONNACK,        //  2  Connect Acknowledgment
          PUBLISH,        //  3  Publish message
          PUBACK,         //  4  Publish Acknowledgment
          PUBREC,         //  5  Publish Received (assured delivery part 1)
          PUBREL,         //  6  Publish Release (assured delivery part 2)
          PUBCOMP,        //  7  Publish Complete (assured delivery part 3)
          SUBSCRIBE,      //  8  Client Subscribe request
          SUBACK,         //  9  Subscribe Acknowledgment
          UNSUBSCRIBE,    // 10  Client Unsubscribe request
          UNSUBACK,       // 11  Unsubscribe Acknowledgment
          PINGREQ,        // 12  PING Request
          PINGRESP,       // 13  PING Response
          DISCONNECT,     // 14  Client is Disconnecting
          Reserved15      // 15
		};

	public:
		SimpleList< uint8_t, uint32_t>	FBytes;

	public:
		static void AddLengthStringPayload( SimpleList<uint8_t, uint32_t>	&ABytes, String AText )
		{
			uint16_t ALength = AText.length();
/*
			Serial.println( AText );
			Serial.println( "AddLengthStringPayload" );
			Serial.println( ALength );
			return;
*/
			ABytes.push_back( ALength >> 8 );
			ABytes.push_back( ALength );


			ABytes.append( (uint8_t *)AText.c_str(), ALength );
		}

		static void IntToMSBLSB( SimpleList<uint8_t, uint32_t>	&ABytes, uint16_t AValue )
		{
			ABytes.push_back( AValue >> 8 );
			ABytes.push_back( AValue );
		}

		static uint16_t MSBLSBToInt( uint8_t *ABytes )
		{
			return ( uint16_t( ABytes[ 0 ] ) << 8 ) | ABytes[ 1 ];
		}

		static String ExtractUTF8String( uint8_t *&ABytes, uint32_t &ALenght )
		{
			uint16_t AStrLength = MSBLSBToInt( ABytes );

//			Serial.print( "AStrLength: " ); Serial.println( int( AStrLength ));

			char *Array = new char[ AStrLength + 1 ];
			memcpy( Array, ABytes + 2, AStrLength );

			Array[ AStrLength ] ='\0';

//			Serial.print( "Array: " ); Serial.println( Array );

			String AResult = Array;

//			Serial.print( "AResult: " ); Serial.println( AResult );
			delete [] Array;

//			String AResult;
//			AResult.reserve( AStrLength );

			ABytes += AStrLength + 2;
			ALenght -= AStrLength + 2;

			return AResult;
		}
/*
	protected:
		inline void SetRetain( bool AValue )
		{
		}

		inline void SetQoSLevel( uint8_t AValue )
		{
		}

		inline void SetDuplicate( bool AValue )
		{
		}
*/
	public:
		virtual bool CanPost( ArduinoMQTTClientTextBasicTopic *ATopic ) { return true; }
		virtual bool IsProcessed( MitovMQTTClient *AOwner, TMQTTPacket::TMQTTMessageType AMessageType, uint8_t *AData, uint32_t ALength ) { return false; }
		virtual void SystemLoop( MitovMQTTClient *AOwner ) {}

	protected:
		void AddRemainingLength( uint32_t ALength )
		{
			if( ALength == 0 )
			{
				FBytes.push_back( 0 );
				return;
			}
  
			while( ALength )
			{
				uint8_t	ADigit = ALength % 128;
				ALength /= 128;
				if( ALength > 0 )
					ADigit != 0x80;
      
				FBytes.push_back( ADigit );
			}
		}

	public:
		TMQTTPacket( TMQTTMessageType AMessageType )
		{
			FBytes.push_back( AMessageType << 4 );
		}

	};
//---------------------------------------------------------------------------
	class TMQTTConnectPacket : public TMQTTPacket
	{
		typedef TMQTTPacket inherited;

	public:
		virtual bool IsProcessed( MitovMQTTClient *AOwner, TMQTTPacket::TMQTTMessageType AMessageType, uint8_t *AData, uint32_t ALength ) override
		{
			return( AMessageType = CONNACK );
		}

	public:
		TMQTTConnectPacket( String AClientID, bool AIs3_1_1, bool ACleanSession, uint16_t AKeepAliveInterval, TCLMQTTClientWill &AWill, String AUserName, String APassword ) :
			inherited( CONNECT )
		{
			SimpleList<uint8_t, uint32_t>	ABytes;
			if( AIs3_1_1 )
			{
				for( int i = 0; i < sizeof( MQTT_V ); ++ i )
					ABytes.push_back( pgm_read_byte( MQTT_V + i ));
			}

			else
			{
				for( int i = 0; i < sizeof( MQIsdp_V ); ++ i )
					ABytes.push_back( pgm_read_byte( MQIsdp_V + i ));
			}

			uint8_t AFlagsOffset = ABytes.size();

			ABytes.push_back( 0 ); // Connection Flags
			IntToMSBLSB( ABytes, AKeepAliveInterval );
			uint8_t &AConnectFlags = ((uint8_t *)ABytes )[ AFlagsOffset ];

			if( ACleanSession )
				AConnectFlags |= 0b00000010;

			AddLengthStringPayload( ABytes, AClientID );
			if( AWill.Topic != "" )
			{
				AConnectFlags |= 0b00000100;
				AConnectFlags |= ( AWill.Delivery << 3 );

				AddLengthStringPayload( ABytes, AWill.Topic );
				AddLengthStringPayload( ABytes, AWill.Message );
			}

			if( AUserName != "" )
			{
				uint8_t &AConnectFlags = ((uint8_t *)ABytes )[ AFlagsOffset ];
				AConnectFlags |= 0b10000000;
				AddLengthStringPayload( ABytes, AUserName );
				if( APassword != "" )
				{
					uint8_t &AConnectFlags = ((uint8_t *)ABytes )[ AFlagsOffset ];
					AConnectFlags |= 0b01000000;
					AddLengthStringPayload( ABytes, APassword );
				}
			}

			AddRemainingLength( ABytes.size() );
			FBytes.append( ABytes, ABytes.size() );
		}
	};
//---------------------------------------------------------------------------
	class TMQTTBasicRetryPacket : public TMQTTPacket
	{
		typedef TMQTTPacket inherited;

	protected:
		unsigned long	FLastTime;
		uint32_t		FRetryInterval;

	public:
		virtual void SystemLoop( MitovMQTTClient *AOwner ) override;

	public:
		TMQTTBasicRetryPacket( TMQTTMessageType AMessageType, uint32_t ARetryInterval ) :
			inherited( AMessageType ),
			FRetryInterval( ARetryInterval )
		{
		}

	};
//---------------------------------------------------------------------------
	class TMQTTReleasePacket : public TMQTTBasicRetryPacket
	{
		typedef TMQTTBasicRetryPacket inherited;

	protected:
		uint16_t FPacketID;
		ArduinoMQTTClientTextBasicTopic	*FTopic;

	public:
		virtual bool CanPost( ArduinoMQTTClientTextBasicTopic *ATopic ) override
		{
			return ( FTopic != ATopic );
		}

		virtual bool IsProcessed( MitovMQTTClient *AOwner, TMQTTPacket::TMQTTMessageType AMessageType, uint8_t *AData, uint32_t ALength ) override
		{
//			Serial.println( "PUBCOMP Test" );
			if( AMessageType != PUBCOMP )
				return( false );

			return( MSBLSBToInt( AData ) == FPacketID );
		}

	public:
		TMQTTReleasePacket( ArduinoMQTTClientTextBasicTopic	*ATopicObject, uint16_t APacketID, uint32_t ARetryInterval ) :
			inherited( PUBREL, ARetryInterval ),
			FPacketID( APacketID )
		{
			FTopic = ATopicObject;
			FBytes[ 0 ] = FBytes[ 0 ] | 0b0010; // Set required bit!
			FBytes.push_back( 2 );
			IntToMSBLSB( FBytes, APacketID );

			FLastTime = millis();
		}

	};
//---------------------------------------------------------------------------
	class TMQTTPublishPacket : public TMQTTBasicRetryPacket
	{
		typedef TMQTTBasicRetryPacket inherited;

	protected:
		uint16_t	FPacketID;
		ArduinoMQTTClientTextBasicTopic	*FTopic;

	public:
		virtual bool CanPost( ArduinoMQTTClientTextBasicTopic *ATopic ) override
		{
			return ( FTopic != ATopic );
		}

		virtual bool IsProcessed( MitovMQTTClient *AOwner, TMQTTPacket::TMQTTMessageType AMessageType, uint8_t *AData, uint32_t ALength ) override;

	public:
		TMQTTPublishPacket( ArduinoMQTTClientTextBasicTopic	*ATopicObject, uint16_t &APacketID, uint32_t ARetryInterval ) :
			inherited( PUBLISH, ARetryInterval )
		{
			FTopic = ATopicObject;
			FBytes[ 0 ] |= ( ATopicObject->SendDelivery << 1 ) | ( ATopicObject->Retain ? 1 : 0 );

			SimpleList<uint8_t, uint32_t>	ABytes;
			AddLengthStringPayload( ABytes, ATopicObject->Topic );
			if( ATopicObject->SendDelivery != dcOnceOrLess )
			{
				FPacketID = APacketID;
				IntToMSBLSB( ABytes, FPacketID );
				if( APacketID == 0xFFFF )
					APacketID = 1;

				else
					++ APacketID;

				FLastTime = millis();
			}

			ABytes.append( (uint8_t *)ATopicObject->InitialValue.c_str(), ATopicObject->InitialValue.length() );

			AddRemainingLength( ABytes.size() );
			FBytes.append( ABytes, ABytes.size() );
		}
	};
//---------------------------------------------------------------------------
	class TMQTTPublishAckPacket : public TMQTTPacket
	{
		typedef TMQTTPacket inherited;

	public:
		TMQTTPublishAckPacket( uint16_t APacketID ) :
			inherited( PUBACK )
		{
			FBytes.push_back( 2 );
			IntToMSBLSB( FBytes, APacketID );
		}
	};
//---------------------------------------------------------------------------
	class TMQTTPublishReceivedPacket : public TMQTTBasicRetryPacket
	{
		typedef TMQTTBasicRetryPacket inherited;

	protected:
		uint16_t	FPacketID;

	public:
		virtual bool IsProcessed( MitovMQTTClient *AOwner, TMQTTPacket::TMQTTMessageType AMessageType, uint8_t *AData, uint32_t ALength ) override
		{
			if( AMessageType != PUBREL )
				return false;

			return( MSBLSBToInt( AData ) == FPacketID );
		}

	public:
		TMQTTPublishReceivedPacket( uint16_t APacketID, uint32_t ARetryInterval ) :
			inherited( PUBREC, ARetryInterval )
		{
			FBytes.push_back( 2 );
			IntToMSBLSB( FBytes, APacketID );
		}

	};
//---------------------------------------------------------------------------
	class TMQTTSubscribePacket : public TMQTTBasicRetryPacket
	{
		typedef TMQTTBasicRetryPacket inherited;

	protected:
		uint16_t	FPacketID;

	public:
		virtual bool IsProcessed( MitovMQTTClient *AOwner, TMQTTPacket::TMQTTMessageType AMessageType, uint8_t *AData, uint32_t ALength ) override
		{
			if( AMessageType != SUBACK )
				return false;

			return( MSBLSBToInt( AData ) == FPacketID );
		}

	public:
		TMQTTSubscribePacket( SimpleList<ArduinoMQTTClientTextBasicTopic *> &AList, uint16_t &APacketID, uint32_t ARetryInterval ) :
			inherited( SUBSCRIBE, ARetryInterval )
		{
			FBytes[ 0 ] = FBytes[ 0 ] | 0b0010; // Set required bit!

			SimpleList<uint8_t, uint32_t>	ABytes;

			FPacketID = APacketID;
			IntToMSBLSB( ABytes, FPacketID );
			if( APacketID == 0xFFFF )
				APacketID = 1;

			else
				++ APacketID;

			for( int i = 0; i < AList.size(); ++i )
			{
				AddLengthStringPayload( ABytes, AList[ i ]->Topic );
				ABytes.push_back( AList[ i ]->ReceiveDelivery );
			}

			AddRemainingLength( ABytes.size() );
			FBytes.append( ABytes, ABytes.size() );
		}
	};
//---------------------------------------------------------------------------
	class TMQTTUnsubscribePacket : public TMQTTBasicRetryPacket
	{
		typedef TMQTTBasicRetryPacket inherited;

	protected:
		uint16_t	FPacketID;

	public:
		virtual bool IsProcessed( MitovMQTTClient *AOwner, TMQTTPacket::TMQTTMessageType AMessageType, uint8_t *AData, uint32_t ALength ) override
		{
			if( AMessageType != UNSUBACK )
				return false;

			return( MSBLSBToInt( AData ) == FPacketID );
		}

	public:
		TMQTTUnsubscribePacket( SimpleList<ArduinoMQTTClientTextBasicTopic *> &AList, uint16_t &APacketID, uint32_t ARetryInterval ) :
			inherited( UNSUBSCRIBE, ARetryInterval )
		{
			FBytes[ 0 ] = FBytes[ 0 ] | 0b0010; // Set required bit!

			SimpleList<uint8_t, uint32_t>	ABytes;

			FPacketID = APacketID;
			IntToMSBLSB( ABytes, FPacketID );
			if( APacketID == 0xFFFF )
				APacketID = 1;

			else
				++ APacketID;

			for( int i = 0; i < AList.size(); ++i )
				AddLengthStringPayload( ABytes, AList[ i ]->Topic );

			AddRemainingLength( ABytes.size() );
			FBytes.append( ABytes, ABytes.size() );
		}
	};
//---------------------------------------------------------------------------
/*
	class TMQTTPingRequestPacket : public TMQTTPacket
	{
		typedef TMQTTPacket inherited;

	public:
		TMQTTPingRequestPacket() :
			inherited( PINGREQ )
		{
			FBytes.push_back( 0 );
		}
	};
*/
//---------------------------------------------------------------------------
	class TMQTTSimplePacket : public TMQTTPacket
	{
		typedef TMQTTPacket inherited;

	public:
		TMQTTSimplePacket( TMQTTMessageType AMessageType ) :
			inherited( AMessageType )
		{
			FBytes.push_back( 0 );
		}
	};
//---------------------------------------------------------------------------
	class MitovMQTTClient : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;
		OpenWire::SinkPin	InputPin;

		OpenWire::ConnectSinkPin	ConnectedInputPin;
		OpenWire::SourcePin	DisconnectOutputPin;
		OpenWire::SourcePin	SessionPresentOutputPin;

	public:
		String	ClientID;

		String	UserName;
		String	Password;

		TCLMQTTClientWill	Will;

		uint16_t	KeepAliveInterval = 10;

		uint16_t	RetryInterval = 20;

		bool	Enabled : 1;
		bool	CleanSession : 1;

	protected:
		bool	FSocketConnected : 1;
		bool	FConnected : 1;
		bool	FIs3_1_1 : 1;

	protected:
		SimpleObjectDeleteList<TMQTTPacket *>	FRequests;
		SimpleList<uint8_t, uint32_t>	FReceivedData;

		unsigned long	FCurrentTime;
		unsigned long	FLastComTime;

		uint16_t	FCurrentPacketID = 1;

	public:
		SimpleList<ArduinoMQTTClientTextBasicTopic *>	FTopics;

	public:
		void SendResponsePacket( TMQTTPacket *APacket )
		{
			FRequests.push_back( APacket );

//			Serial.println( "SendResponsePacket" );
//			Serial.println( APacket->FBytes.size() );
//			Serial.println( Mitov::TDataBlock( APacket->FBytes.size(), APacket->FBytes ).ToString());

			OutputPin.SendValue( Mitov::TDataBlock( APacket->FBytes.size(), APacket->FBytes ));
		}

		void SendPacket( TMQTTPacket *APacket )
		{
//			Serial.println( "SendPacket" );
//			Serial.println( APacket->FBytes.size() );
//			Serial.println( Mitov::TDataBlock( APacket->FBytes.size(), APacket->FBytes ).ToString());

			OutputPin.SendValue( Mitov::TDataBlock( APacket->FBytes.size(), APacket->FBytes ));
			delete APacket;
		}

		void ResendPacket( TMQTTPacket *APacket )
		{
//			Serial.println( "ResendPacket" );
//			Serial.println( APacket->FBytes.size() );
//			Serial.println( Mitov::TDataBlock( APacket->FBytes.size(), APacket->FBytes ).ToString());
			OutputPin.SendValue( Mitov::TDataBlock( APacket->FBytes.size(), APacket->FBytes ));
		}

	protected:
		void Connect()
		{
//			Serial.println( "Connect()" );
//			return;
			if( FConnected )
				return;

			if( FRequests.size() )
				return;

			FLastComTime = millis();
			SendResponsePacket( new TMQTTConnectPacket( ClientID, FIs3_1_1, CleanSession, KeepAliveInterval, Will, UserName, Password ));
		}

		void Disconnect()
		{
			if( ! FConnected )
				return;

			FConnected = false;
			SendPacket( new TMQTTSimplePacket( TMQTTPacket::DISCONNECT ));
			for( int i = 0; i < FTopics.size(); ++ i )
				FTopics[ i ]->ConnectionClosed();
		}

	public:
		void Publish( ArduinoMQTTClientTextBasicTopic *ATopicObject )
		{
			if( FRequests.size() > 4 * FTopics.size() )
				return;

			for( int i = 0; i < FRequests.size(); ++i )
				if( ! FRequests[ i ]->CanPost( ATopicObject ))
					return;

//			Serial.println( "Publish" );
			TMQTTPacket *APacket = new TMQTTPublishPacket( ATopicObject, FCurrentPacketID, RetryInterval );
			if( ATopicObject->SendDelivery != dcOnceOrLess )
				SendResponsePacket( APacket );

			else
				SendPacket( APacket );

		}

		void Subscribe( SimpleList<ArduinoMQTTClientTextBasicTopic *> &AList )
		{
//			Serial.println( "Subscribe" );
			SendResponsePacket( new TMQTTSubscribePacket( AList, FCurrentPacketID, RetryInterval ));
		}

		void Unsubscribe( SimpleList<ArduinoMQTTClientTextBasicTopic *> &AList )
		{
//			Serial.println( "Unsubscribe" );
			SendResponsePacket( new TMQTTUnsubscribePacket( AList, FCurrentPacketID, RetryInterval ));
		}

	public:
		void UpdateEnabled()
		{
			if( Enabled )
			{
				if( ! ConnectedInputPin.IsConnected() )
					Connect();

			}
			else
				Disconnect();

		}

	protected:
		virtual void SystemStart() override
		{
			UpdateEnabled();

//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( FConnected )
			{
				unsigned long	ACurrentTime = millis();
				if( ACurrentTime - FLastComTime > KeepAliveInterval * 5 * 1000 + 2000 )
				{
					Error(); // Timedout!
//						inherited::SystemLoopBegin( currentMicros );
					return;
				}

				if( ACurrentTime - FCurrentTime > KeepAliveInterval * 800 )
				{
//					Serial.println( "PING" );
					FCurrentTime = ACurrentTime;
					SendPacket( new TMQTTSimplePacket( TMQTTPacket::PINGREQ ));
				}

				ProcessTopics();
				StartSubscriptions();
				EndsSubscriptions();

				for( int i = FRequests.size(); i--; )
					FRequests[ i ]->SystemLoop( this );

			}

//			inherited::SystemLoopBegin( currentMicros );
		}

	protected:
		void Error()
		{
			DisconnectOutputPin.Notify( nullptr );
		}

		void ProcessTopics()
		{
			for( int i = 0; i < FTopics.size(); ++ i )
				FTopics[ i ]->Process();
		}

		void StartSubscriptions()
		{
			SimpleList<ArduinoMQTTClientTextBasicTopic *> AList;
			for( int i = 0; i < FTopics.size(); ++ i )
				if( FTopics[ i ]->NeedsSubscription())
					AList.push_back( FTopics[ i ] );

			if( AList.size() )
				Subscribe( AList );
		}

		void EndsSubscriptions()
		{
			SimpleList<ArduinoMQTTClientTextBasicTopic *> AList;
			for( int i = 0; i < FTopics.size(); ++ i )
				if( FTopics[ i ]->NeedsUnsubscription())
					AList.push_back( FTopics[ i ] );

			if( AList.size() )
				Unsubscribe( AList );
		}

	protected:
		void DoConnectedReceive( void *_Data )
		{
			if( FSocketConnected == *((bool *)_Data))
				return;

			FSocketConnected = *((bool *)_Data);

//			Serial.println( "FSocketConnected" );
//			Serial.println( FSocketConnected );

			if( FSocketConnected )
				Connect();
		}

		void DoInputReceive( void *_Data )
		{
//			Serial.println( "Received" );

			FLastComTime = millis();

			Mitov::TDataBlock ABlock = *(Mitov::TDataBlock *)_Data;
//			Serial.println( ABlock.ToString() );
			FReceivedData.append( ABlock.Data, ABlock.Size );

			if( FReceivedData.size() < 2 )
				return;

//			Serial.println( "FReceivedData" );
//			Serial.println( APacket->FBytes.size() );
//			Serial.println( Mitov::TDataBlock( FReceivedData.size(), FReceivedData ).ToString());

//			Serial.print( "ReceivedSize = " ); Serial.println( FReceivedData.size() );
//			Serial.println( "Received - 2" );
			uint8_t	ADataOffset = 1;
			uint32_t	ALength = 0;

			uint32_t	AReceivedLength = FReceivedData.size();

			uint32_t	AMultiplier = 1;

			for( int i = 1; i <= 4; ++ i )
			{
				++ ADataOffset;
				ALength += ( FReceivedData[ i ] & 0x7F ) * AMultiplier;
//				Serial.print( "i = " ); Serial.println( i );
//				Serial.print( "ALength = " ); Serial.println( ALength );
				if( !( FReceivedData[ i ] & 0x80 ))
					break;

				if( AReceivedLength < i + 2 )
					return;

				AMultiplier *= 128;
				if( i == 4 )
				{
					Error();
					return;
				}
			}

//			Serial.println( "TEST1" );
//			Serial.println( AReceivedLength );
//			Serial.println( ADataOffset );
//			Serial.println( ALength );
			if( AReceivedLength < ADataOffset + ALength )
				return;

//			Serial.println( "TEST2" );
//			Serial.println( ALength );
//			Serial.println( FReceivedData[ 0 ] );

			TMQTTPacket::TMQTTMessageType AMessageType = TMQTTPacket::TMQTTMessageType( FReceivedData[ 0 ] >> 4 );
			switch( AMessageType )
			{
				case TMQTTPacket::CONNACK :
				    if( ALength != 2 )
					{
						Error();
						return;
					}

					if( FReceivedData[ 3 ] )
					{
						Error(); // Connection refused
						break;
					}

					SessionPresentOutputPin.SendValue<bool>( FReceivedData[ 2 ] & 1 );

//					Serial.print( "CONNACK: " ); Serial.println( FReceivedData[ 3 ] );

				    FConnected = true;
					FCurrentTime = millis();
//					Serial.println( "CONNACK" );
					ProcessRequests( AMessageType, FReceivedData + ADataOffset, ALength );
					ProcessTopics();
					StartSubscriptions();
					break;

				case TMQTTPacket::PINGRESP :
//					Serial.println( "PINGRESP" );
					break;

				case TMQTTPacket::PUBACK : // Acknowleges a message in QoS 1
//					Serial.println( "PUBACK" );
					ProcessRequests( AMessageType, FReceivedData + ADataOffset, ALength );
					break;

				case TMQTTPacket::PUBREC : // Acknowleges a message in QoS 2
//					Serial.println( "PUBREC" );
					ProcessRequests( AMessageType, FReceivedData + ADataOffset, ALength );
					break;

				case TMQTTPacket::PUBCOMP : // Complete a message in QoS 2
//					Serial.println( "PUBCOMP" );
					ProcessRequests( AMessageType, FReceivedData + ADataOffset, ALength );
					break;

				case TMQTTPacket::SUBACK :
//					Serial.println( "SUBACK" );
					ProcessRequests( AMessageType, FReceivedData + ADataOffset, ALength );
					break;

				case TMQTTPacket::UNSUBACK :
//					Serial.println( "UNSUBACK" );
					ProcessRequests( AMessageType, FReceivedData + ADataOffset, ALength );
					break;

				case TMQTTPacket::PUBLISH :
//					Serial.println( "PUBLISH" );
					ProcessPublish( FReceivedData[ 0 ], FReceivedData + ADataOffset, ALength );
					break;
			}

			FReceivedData.Delete( 0, ADataOffset + ALength );

//			Serial.print( "FReceivedData.Delete = " ); Serial.println( ADataOffset + ALength );
//			Serial.println( APacket->FBytes.size() );
//			Serial.println( Mitov::TDataBlock( FReceivedData.size(), FReceivedData ).ToString());
		}

		void ProcessPublish( uint8_t AHeader, uint8_t *AData, uint32_t ALength )
		{
			TArduinoMQTTDelivery AQoS = TArduinoMQTTDelivery( ( AHeader >> 1 ) & 0b11 );
			uint16_t AMessageID;
			if( AQoS != dcOnceOrLess )
			{
				AMessageID = TMQTTPacket::MSBLSBToInt( AData );
				AData += 2;
				ALength -= 2;
			}

			String ATopic = TMQTTPacket::ExtractUTF8String( AData, ALength );

//			Serial.print( "ATopic: " ); Serial.println( ATopic );

			char *Array = new char[ ALength + 1 ];
			memcpy( Array, AData, ALength );

			Array[ ALength ] ='\0';
			String APayload = Array;

			delete [] Array;

//			Serial.print( "APayload: " ); Serial.println( APayload );
			switch( AQoS )
			{
				case dcOnceOrMore:
					SendPacket( new TMQTTPublishAckPacket( AMessageID ));
					break;

				case dcOnceOnly:
					SendResponsePacket( new TMQTTPublishReceivedPacket( AMessageID, RetryInterval ));
					break;

			}

			for( int i = 0; i < FTopics.size(); ++ i )
				if( FTopics[ i ]->Topic == ATopic )
					FTopics[ i ]->SendPayload( APayload );
		}

		void ProcessRequests( TMQTTPacket::TMQTTMessageType AMessageType, uint8_t *AData, uint32_t ALength )
		{
//			for( int i = 0; i < FRequests.size(); ++i )
//			Serial.println( FRequests.size() );
			for( int i = FRequests.size(); i--; )
				if( FRequests[ i ]->IsProcessed( this, AMessageType, AData, ALength ))
				{
//					Serial.println( "IsProcessed" );
//					Serial.println( FRequests.size() );
					FRequests.Delete( i );
//					Serial.println( "IsProcessed after delete" );
//					Serial.println( FRequests.size() );
					break;
				}
		}

	public:
		MitovMQTTClient( bool AIs3_1_1, String AClientID ) :
			ClientID( AClientID ),
			Enabled( true ),
			CleanSession( true ),
			FSocketConnected( false ),
			FConnected( false ),
			FIs3_1_1( AIs3_1_1 )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MitovMQTTClient::DoInputReceive );
			ConnectedInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MitovMQTTClient::DoConnectedReceive );
		}

	};
//---------------------------------------------------------------------------
	class ArduinoMQTTClientTextReadTopic : public ArduinoMQTTClientTextBasicTopic
	{
		typedef ArduinoMQTTClientTextBasicTopic	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		bool FModified = true;

	public:
		virtual void Process() override
		{
			if( Enabled )
				if( FModified )
				{
					FModified = false;
					if( FInitialValueSent )
						FOwner.Publish( this );

					else
					{
						FInitialValueSent = true;

						if( InitialValue != "" )
							FOwner.Publish( this );

					}
				}

		}

	protected:
		void DoReceive( void *_Data )
		{
			InitialValue = ( char * )_Data;
			FModified = true;
		}

	public:
		ArduinoMQTTClientTextReadTopic( MitovMQTTClient &AOwner ) :
			inherited( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoMQTTClientTextReadTopic::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class ArduinoMQTTClientTextWriteTopic : public ArduinoMQTTClientTextBasicTopic
	{
		typedef ArduinoMQTTClientTextBasicTopic	inherited;

	public:
		OpenWire::SourcePin	OutputPin;

/*
	public:
		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
		}
*/
	public:
		virtual bool NeedsSubscription() override
		{
			bool AResult = Enabled && (!FSubscribed );
			if( AResult )
			{
				FSubscribed = true;
				if( ! FInitialValueSent )
				{
					FInitialValueSent = true;
					if( InitialValue != "" )
						OutputPin.SendValue( InitialValue );
				}
			}

			return AResult;
		}

		virtual bool NeedsUnsubscription() override
		{
			bool AResult = ( ! Enabled ) && FSubscribed;
/*
			if( AResult )
			{
				Serial.print( "Enabled " ); Serial.println( Enabled );
				Serial.print( "FSubscribed " ); Serial.println( FSubscribed );
			}
*/
			if( AResult )
				FSubscribed = false;

			return AResult;
		}

		virtual void SendPayload( String APayload ) override
		{
			OutputPin.SendValue( APayload );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class ArduinoMQTTClientTextReadWriteTopic : public ArduinoMQTTClientTextReadTopic
	{
		typedef ArduinoMQTTClientTextReadTopic	inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		virtual bool NeedsSubscription() override
		{
			bool AResult = Enabled && (!FSubscribed );
			if( AResult )
			{
				FSubscribed = true;
				if( ! FInitialValueSent )
				{
					FInitialValueSent = true;
					if( InitialValue != "" )
						OutputPin.SendValue( InitialValue );
				}
			}

			return AResult;
		}

		virtual bool NeedsUnsubscription() override
		{
			bool AResult = ( ! Enabled ) && FSubscribed;
//			Serial.print( "bool AResult " ); Serial.println( AResult );
			if( AResult )
				FSubscribed = false;

			return AResult;
		}

		virtual void SendPayload( String APayload ) override
		{
			OutputPin.SendValue( APayload );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	ArduinoMQTTClientTextBasicTopic::ArduinoMQTTClientTextBasicTopic( MitovMQTTClient &AOwner ) :
		FOwner( AOwner ),
		SendDelivery( dcOnceOrLess ),
		ReceiveDelivery( dcOnceOrLess ),
		Enabled( true ),
		Retain( true ),
		FSubscribed( false ),
		FInitialValueSent( false )
	{
		AOwner.FTopics.push_back( this );
	}
//---------------------------------------------------------------------------
	void TMQTTBasicRetryPacket::SystemLoop( MitovMQTTClient *AOwner )
	{
		unsigned long ATime = millis();
		if( ATime - FLastTime > FRetryInterval * 1000 )
		{
/*
			Serial.println( "TIME DIFF:" );
			Serial.println( ATime - FLastTime );
			Serial.println( FRetryInterval );
*/
			FBytes[ 0 ] = FBytes[ 0 ] | 0b1000; // Set the Dublicate flag
			AOwner->ResendPacket( this );
			FLastTime = ATime;
		}
	}
//---------------------------------------------------------------------------
	bool TMQTTPublishPacket::IsProcessed( MitovMQTTClient *AOwner, TMQTTPacket::TMQTTMessageType AMessageType, uint8_t *AData, uint32_t ALength )
	{
//		Serial.println( "Test PUBREC1" );
//		Serial.println( FBytes[ 0 ] & 0b110 );

		if( ( FBytes[ 0 ] & 0b110 ) == 0b100 ) // QoS Level 2 - dcOnceOnly
		{
//			Serial.println( "Test PUBREC" );
			if( AMessageType != PUBREC )
				return( false );

//			Serial.print( "My ID: " ); Serial.print( FPacketID ); Serial.print( " ? " ); Serial.println( MSBLSBToInt( AData ) );
			if( MSBLSBToInt( AData ) == FPacketID )
			{
//				Serial.println( "Send Response" );
				AOwner->SendResponsePacket( new TMQTTReleasePacket( FTopic, FPacketID, FRetryInterval ));
				return true;
			}
    
//				FOnRelease( FPacketID );
    
			return false;
		}
    
		if( AMessageType != PUBACK )
			return( false );
    
		return( MSBLSBToInt( AData ) == FPacketID );
	}
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

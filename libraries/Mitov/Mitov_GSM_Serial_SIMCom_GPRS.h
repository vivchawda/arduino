////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GSM_SERIAL_SIMCOM_GPRS_h
#define _MITOV_GSM_SERIAL_SIMCOM_GPRS_h

#include <Mitov.h>
#include <Mitov_BasicEthernet.h>
#include <Mitov_GSM_Serial.h>
#include <Mitov_StringPrint.h>

//#define __SIMGPRS__DEBUG__

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class SIMComGPRSModule;
	class SIMComGPRSTCPClientSocket;
//---------------------------------------------------------------------------
	class BasicSIMComGPRSSocket : public Mitov::BasicSocket, public MitovGSMSerialBasicFunction
	{
		typedef Mitov::BasicSocket inherited;

	public:
		OpenWire::SinkPin	DisconnectInputPin;

	protected:
		SIMComGPRSModule &FModule;
		bool	FRunning = false;

	protected:
		BufferPrint	FStringPrint;

	protected:
		virtual void DoDisconnect( void * )
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.println( "DoDisconnect" );
#endif
			SetEnabled( false );
		}

	public:
		virtual bool IsEnabled()
		{
			return Enabled; // && FRunning; //FModule.Enabled;
		}

		virtual Print *GetPrint()
		{
			return &FStringPrint;
		}

		virtual bool CanSend()
		{
			return Enabled && FRunning;
		}

		virtual void BeginPacket()
		{
			FStringPrint.Value.clear();
		}

//		virtual void DataReceived( int connectionId, unsigned char AData ) = 0;

	public:
		BasicSIMComGPRSSocket( SIMComGPRSModule &AModule );
	};
//---------------------------------------------------------------------------
	class TMitovGSMSerialGPRSGetIPFunctionStatus : public MitovGSMSerialBasicFunction
	{
		typedef MitovGSMSerialBasicFunction inherited;

	protected:
		SIMComGPRSModule *FOwnerModule;

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override;

	public:
		TMitovGSMSerialGPRSGetIPFunctionStatus( SIMComGPRSModule *AOwnerModule );

	};
//---------------------------------------------------------------------------
	class SIMComGPRSModule : public OpenWire::Component, public MitovGSMSerialBasicFunction
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	AddressOutputPin;

	public:
		bool	Enabled : 1;

	protected:
		bool	FConnected : 1;
		bool	FFromStart : 1;
		bool	FInConnecting : 1; 
		bool	FMultiConnection : 1;
		bool	FTryNextConnection : 1;
		uint8_t	FAPNContext;

	public:
		Mitov::SimpleList<BasicSIMComGPRSSocket *>	Sockets;

	public:
		Mitov::SimpleObjectList<GPRSAccessPoint *>	AccessPoints;

	public:
		void SetEnabled( bool AValue )
		{
            if( Enabled == AValue )
                return;

            Enabled = AValue;
			if( Enabled )
				StartEthernet();

			else
				StopEthernet();

		}

	public:
/*
		bool AssignServerID( BasicSIMComGPRSSocket *ASocket )
		{
			bool AResult = false;
			for( int i = 0; i < sizeof( FOwner.FSocketConnections ) / sizeof( FOwner.FSocketConnections[ 0 ] ); ++i )
				if( ! FOwner.FSocketConnections[ i ] )
				{
					FOwner.FSocketConnections[ i ] = ASocket;
//					FServerInstance = ASocket;
					AResult = true;
				}

			return AResult;
		}
*/
		void AddServerConnection( BasicSIMComGPRSSocket *ASocket, int AIndex )
		{
			if( ! FOwner.FSocketConnections[ AIndex ] )
				FOwner.FSocketConnections[ AIndex ] = ASocket;

		}

		bool RemoveServerConnection( BasicSIMComGPRSSocket *ASocket, int AIndex )
		{
			if( FOwner.FSocketConnections[ AIndex ] == ASocket )
			{
				FOwner.FSocketConnections[ AIndex ] = nullptr;
				return true;
			}

			return false;
		}

		bool IsServerConnection( BasicSIMComGPRSSocket *ASocket, int AIndex )
		{
			return( FOwner.FSocketConnections[ AIndex ] == ASocket );
		}

		void ReleaseServerID( BasicSIMComGPRSSocket *ASocket )
		{
//			if( FServerInstance == ASocket )
//				FServerInstance = NULL;

			for( int i = 0; i < sizeof( FOwner.FSocketConnections ) / sizeof( FOwner.FSocketConnections[ 0 ] ); ++i )
				if( FOwner.FSocketConnections[ i ] == ASocket )
					FOwner.FSocketConnections[ i ] = NULL;
		}

		bool AssignConnectionID( BasicSIMComGPRSSocket *ASocket, int &AID )
		{
			for( int i = 0; i < sizeof( FOwner.FSocketConnections ) / sizeof( FOwner.FSocketConnections[ 0 ] ); ++i )
				if( ! FOwner.FSocketConnections[ i ] )
				{
					FOwner.FSocketConnections[ i ] = ASocket;
					AID = i;
					return true;
				}

			return false;
		}

		void ReleaseConnectionID( int AID )
		{
//			FOwner.FSocketConnections[ AID ] = FServerInstance;
		}

	public:
		void ConnectResult( bool AValue )
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.print( "STATUS: " ); Serial.println( AValue );
#endif
			if( AValue )
			{
				FConnected = true;
				for( int i = 0; i < AccessPoints.size(); ++ i )
					AccessPoints[ i ]->FRetryCount = 0;

				StartConnection();
			}

			else
				TryNextConnection();
		}

		void GetIPConfigResult( String AIPAddress )
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.println( "TEST5" );
#endif
			AddressOutputPin.SendValue( AIPAddress );

			for( int i = 0; i < Sockets.size(); ++i )
				Sockets[ i ]->TryStartSocket();
		}

	protected:
		void SetMultiConnectionResult( bool AValue )
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.println( "TEST3_1" );
#endif
			delay( 200 );
			if( AValue )
				SetContext();

#ifdef __SIMGPRS__DEBUG__
			Serial.println( "TEST3_2" );
#endif
			delay( 200 );
		}

		void SetContextResult( bool AValue )
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.println( "TEST3" );
#endif
			delay( 200 );
			if( AValue )
				TryNextConnection();
		}

		void StartConnectionResult( bool AValue )
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.println( "TEST4" );
#endif
			if( AValue )
				GetIPConfig();

		}

		void StartConnection()
		{
//			FOwner.SendQueryRegisterResponse( new TMitovGSMSerialFunctionStatus( this, (TOnMitovGSMSerialFunctionResult)&SIMComGPRSModule::StartConnectionResult ), String( "AT+CIICR" ), true, 5 );
			FOwner.SendQueryRegisterResponse( new TMitovGSMSerialFunctionStatus( FOwner, MAKE_CALLBACK_3( OpenWire::Component, TOnMitovGSMSerialFunctionResult, SIMComGPRSModule::StartConnectionResult, bool )), String( "AT+CIICR" ), true, 5 );
		}

		void GetIPConfig()
		{
			FOwner.SendQueryRegisterResponse( new TMitovGSMSerialGPRSGetIPFunctionStatus( this ), String( "AT+CIFSR" ), true, 5 );
		}

	protected:
		void TryNextConnection()
		{
			FTryNextConnection = false;
			unsigned long currentMicros = micros();
			bool ATryNextConnection = false;

			for( int i = 0; i < AccessPoints.size(); ++ i )
			{
				if( AccessPoints[ i ]->Enabled )
				{
//					Serial.println( "TryNextConnection" );
					bool ACanRetry = true;
					if( ! FFromStart )
						if( ! AccessPoints[ i ]->CanRetry( currentMicros, ACanRetry ) )
						{
							ATryNextConnection |= ACanRetry;
							continue;
						}

					ATryNextConnection |= ACanRetry;
					String AUserName = AccessPoints[ i ]->UserName;

					MitovGSMSerialBasicFunction *AFunction = new TMitovGSMSerialFunctionStatus( FOwner, MAKE_CALLBACK_3( OpenWire::Component, TOnMitovGSMSerialFunctionResult, SIMComGPRSModule::ConnectResult, bool ));

					if( AUserName == "" )
						FOwner.SendQueryRegisterResponse( AFunction, String( "AT+CSTT=\"" ) + AccessPoints[ i ]->AccessPointName + "\"", true );

					else
						FOwner.SendQueryRegisterResponse( AFunction, String( "AT+CSTT=\"" ) + AccessPoints[ i ]->AccessPointName + "\",\"" + AccessPoints[ i ]->UserName + "\",\"" + AccessPoints[ i ]->Password + "\"", true );

					AccessPoints[ i ]->FLastTime = currentMicros;
					++ AccessPoints[ i ]->FRetryCount;
					return;
				}
			}

			FTryNextConnection = ATryNextConnection;
//			Serial.print( "FTryNextConnection: " ); Serial.println( FTryNextConnection );
		}

		void SetMultiConnection()
		{
			if( FMultiConnection )
				TryNextConnection();

			else
			{
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "TEST1" );
#endif
//				FOwner.SendQueryRegisterResponse( this, String( "AT+CIPMUX=1" ), true );
				FOwner.SendQueryRegisterResponse( new TMitovGSMSerialFunctionStatus( FOwner, MAKE_CALLBACK_3( OpenWire::Component, TOnMitovGSMSerialFunctionResult, SIMComGPRSModule::SetMultiConnectionResult, bool )), String( "AT+CIPMUX=1" ), true, 5 );
			}
		}

		void SetContext()
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.println( "TEST2" );
			Serial.print( "FAPNContext: " ); Serial.println( FAPNContext );
#endif
			FOwner.SendQueryRegisterResponse( new TMitovGSMSerialFunctionStatus( FOwner, MAKE_CALLBACK_3( OpenWire::Component, TOnMitovGSMSerialFunctionResult, SIMComGPRSModule::SetContextResult, bool )), String( "AT+CIPSGTXT=" ) + FAPNContext, true, 5 );
		}

		void TryConnect( bool FromStart, unsigned long currentMicros )
		{
			if( FInConnecting )
				return;

			FInConnecting = true;
			FFromStart = FromStart;
			SetMultiConnection();


/*
			for( int i = 0; i < AccessPoints.size(); i ++ )
				if( AccessPoints[ i ]->Enabled )
				{
					bool ACanRetry = false;
					if( ! FromStart )
						if( ! AccessPoints[ i ]->CanRetry( currentMicros, ACanRetry ) )
							continue;

					String AUserName = AccessPoints[ i ]->UserName;

					if( AUserName == "" )
						FOwner.SendQueryRegisterResponse( &FRequestConnectAccessPoint, String( "AT+CSTT=\"" ) + AccessPoints[ i ]->AccessPointName + "\"" );

					else
						FOwner.SendQueryRegisterResponse( &FRequestConnectAccessPoint, String( "AT+CSTT=\"" ) + AccessPoints[ i ]->AccessPointName + "\",\"" + AccessPoints[ i ]->UserName + "\",\"" + AccessPoints[ i ]->Password + "\"" );
/ *
					FConnected = FGprs.attachGPRS( (char *)AccessPoints[ i ]->AccessPointName.c_str(), (char *)AccessPoints[ i ]->UserName.c_str(), (char *)AccessPoints[ i ]->Password.c_str() );
					if( FConnected )
						break;

					AccessPoints[ i ]->FLastTime = currentMicros;
					++ AccessPoints[ i ]->FRetryCount;
* /
				}
*/
		}

		void StopEthernet()
		{
			for( int i = 0; i < Sockets.size(); ++i )
				Sockets[ i ]->StopSocket();

//			LWiFi.end();
//			IsStarted = false;
			FConnected = false;
		}

		void StartEthernet()
		{
			if( ! FOwner.IsStarted )
				return;
/*
			if( ! AccessPoints.length() )
				return;

			if( ! Sockets.length() )
				return;
*/
			TryConnect( true, 0 );
//			if( AccessPointName != "" )
//				IsStarted = FGprs.attachGPRS( (char *)AccessPointName.c_str(), (char *)UserName.c_str(), (char *)Password.c_str() );

//			else
//				AConnected = FGprs.attachGPRS();

/*
			if( AConnected )
			{
			}
*/
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( Enabled )
				if( ! FConnected )
					if( FOwner.IsStarted )
						TryConnect( false, currentMicros );

			if( FTryNextConnection )
				TryNextConnection();

			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		virtual void ElementSystemStart() override
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.print( "FAPNContext: " ); Serial.println( FAPNContext );
#endif

			if( Enabled )
				if( ! FConnected )
					if( FOwner.IsStarted )
						TryConnect( false, micros() );

		}

	public:
		SIMComGPRSModule( MitovGSMSerial &AOwner, uint8_t AAPNContext ) :
			MitovGSMSerialBasicFunction( AOwner ),
			Enabled( true ),
			FFromStart( false ),
			FInConnecting( false ),
			FMultiConnection( false ),
			FTryNextConnection( false ),
			FConnected( false ),
			FAPNContext( AAPNContext )
		{
		}

	};
//---------------------------------------------------------------------------
	class TSIMComGPRSTCPSocketSendPacket : public MitovGSMSerialBasicFunction
	{
		typedef MitovGSMSerialBasicFunction inherited;

	protected:
		BasicSIMComGPRSSocket* FOwnerSocket;
		BufferPrint	FStringPrint;

	public:
		virtual bool TryProcessPromptInput( bool &ASuccess ) override
		{
			FOwner.SendDirect( FStringPrint.Value, FStringPrint.Value.size() );
			ASuccess = true;
			return true;
		}

/*
		virtual bool TryProcessRequestedInput( String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
			if( ALine.startsWith( ">" ) )
			{
				FOwner.SendDirect( FStringPrint.Value, FStringPrint.Value.size() );
				ALockInput = false;
				ASuccess = true;
				return true;
			}

			return false;
		}
*/
	public:
		TSIMComGPRSTCPSocketSendPacket( BasicSIMComGPRSSocket *AOwnerSocket, BufferPrint AStringPrint );

	};
//---------------------------------------------------------------------------
	class SIMComGPRSTCPServerSocket : public BasicSIMComGPRSSocket
	{
		typedef BasicSIMComGPRSSocket inherited;

//		EthernetServer	*FServer;
//		EthernetClient	FClient;

		int	FClientCurrentID = -1;

	public:
		OpenWire::SourcePin	ListeningOutputPin;

	protected:
		bool	FInOpening = false;
		int		FReadSize = 0;
		int		FReadCounter;
		uint8_t	*FBuffer = nullptr;
		uint8_t	*FBufferPtr;

	public:
		virtual bool ProcessCharacter( char AChar, bool &ASuccess ) override
		{ 
			if( FReadCounter )
			{
				*FBufferPtr++ = *(uint8_t*)&AChar;
				--FReadCounter;
//				Serial.println( FReadSize );
//				Serial.println( AChar );
				if( !FReadCounter )
				{
#ifdef __SIMGPRS__DEBUG__
					Serial.println( "SERVER RECEIVE COMPLETED" );
#endif
					ASuccess = true;
					OutputPin.SendValue( Mitov::TDataBlock( FReadSize, FBuffer ));
					FClientCurrentID = -1;
//					Serial.println( (char *)FBuffer );
					delete [] FBuffer;
					FBuffer = nullptr;
				}

				return true;
			}

			return false; 
		}

		virtual bool TryProcessInput( String ALine, bool &ALockInput ) override
		{ 
			ALine.trim();
			if( FInOpening )
			{
				if( ALine == "SERVER OK" )
				{
#ifdef __SIMGPRS__DEBUG__
					Serial.println( "ServerSocketListening" );
#endif
					ListeningOutputPin.SendValue( true );
					FInOpening = false;
					FRunning = true;
					return true;
				}
			}

			else if( Func::ContainsTextAtPos( ALine, 1, ", REMOTE IP:" ))
			{
				int AIndex = ALine[ 0 ] - '0';
				if( ( AIndex >= 0 ) && ( AIndex <= 7 ))
				{
					FModule.AddServerConnection( this, AIndex );
#ifdef __SIMGPRS__DEBUG__
					Serial.print( "ServerSocket: Client Connected: " ); Serial.println( AIndex );
#endif
				}

				return true;
			}

			else if( Func::ContainsTextAtPos( ALine, 1, ", CLOSE OK" ) || Func::ContainsTextAtPos( ALine, 1, ", CLOSED" ))
			{
				int AIndex = ALine[ 0 ] - '0';
				if( ( AIndex >= 0 ) && ( AIndex <= 7 ))
				{
					if( FModule.RemoveServerConnection( this, AIndex ))
					{
#ifdef __SIMGPRS__DEBUG__
						Serial.print( "ServerSocket: Client Close: " ); Serial.println( AIndex );
#endif
						return true;
					}
				}
			}

			else if( ALine.startsWith( "+RECEIVE," ))
			{
				int AIndex = ALine[ 9 ] - '0';
				if( ( AIndex >= 0 ) && ( AIndex <= 7 ))
				{
					if( FModule.IsServerConnection( this, AIndex ))
					{
						FClientCurrentID = AIndex;
						ALine.remove( 0, 11 );
						FReadSize = ALine.toInt();
#ifdef __SIMGPRS__DEBUG__
						Serial.println( "SERVER RECEIVE " + String( AIndex ) + " -> " + FReadSize );
#endif
						if( FReadSize )
						{
							FReadCounter = FReadSize;
							if( FBuffer )
								delete [] FBuffer;

							FBuffer = new uint8_t[ FReadSize ];
							FBufferPtr = FBuffer;
							ALockInput = true;
							return true;
						}
					}
				}
			}

			return false; 
		}

	protected:
		void OpenedResult( bool AResult )
		{
			if( AResult )
			{
				FInOpening = true;
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "ServerSocketConnected" );
#endif
			}
		}

	protected:
		virtual void SystemStart() override
		{
			inherited::SystemStart();
			ListeningOutputPin.SendValue( false );
		}

		virtual void StartSocket() override
		{
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "SERVER::StartSocket" );
#endif
//			if( FModule.AssignConnectionID( this ) )
//			if( FModule.AssignServerID( this ) )
//			{
////				FModule.SendData( "AT+CIPSERVER=1," + String( Port ), 2000 );
				FModule.FOwner.SendQueryRegisterResponse( new TMitovGSMSerialFunctionExpectOk( FModule.FOwner, MAKE_CALLBACK_3( OpenWire::Component, TOnMitovGSMSerialFunctionResult, SIMComGPRSTCPServerSocket::OpenedResult, bool ), "AT+CIPSERVER=1," + String( Port )), "AT+CIPSERVER=1," + String( Port ), true, 5 );
//			}

		}

/*
		virtual void SystemLoopBegin( unsigned long currentMicros ) 
		{
			inherited::SystemLoopBegin( currentMicros );
		}
*/
/*
		virtual void DataReceived( int connectionId, unsigned char AData )
		{
#ifdef __SIMGPRS__DEBUG__
				Serial.print( "RECEIVED: " );
				Serial.println( connectionId );
#endif
			FClientCurrentID = connectionId;
			OutputPin.Notify( &AData );
//			FClientCurrentID = -1;
		}
*/
	public:
		virtual void EndPacket()
		{
			if( FStringPrint.Value.size() == 0 )
				return;

			if( FClientCurrentID >= 0 )
			{
				FModule.FOwner.SendQueryRegisterResponse( new TSIMComGPRSTCPSocketSendPacket( this, FStringPrint ), "AT+CIPSEND=" + String( FClientCurrentID ) +"," + String( FStringPrint.Value.size()), true, 5, 60000 );
//				if( FModule.SendData( "AT+CIPSEND=" + String( FClientCurrentID ) + "," + String( FStringPrint.Value.size()), "\r\n> ", 2000 ))
//					FModule.SendDirect( (uint8_t *)FStringPrint.Value, FStringPrint.Value.size() );

//				Serial.println( FStringPrint.Value );
			}

			else
			{
				for( int i = 0; i < 7; ++i )
					if( FModule.IsServerConnection( this, i ))
						FModule.FOwner.SendQueryRegisterResponse( new TSIMComGPRSTCPSocketSendPacket( this, FStringPrint ), "AT+CIPSEND=" + String( i ) +"," + String( FStringPrint.Value.size()), true, 5, 60000 );
			}

//			FStringPrint.Value.clear();
		}

	public:
		virtual void StopSocket()
		{
			FInOpening = false;
			if( FRunning )
			{
				ListeningOutputPin.SendValue( false );
//				FModule.SendData( "AT+CIPSERVER=0," + String( Port ), 2000 );
				FModule.FOwner.SendQueryRegisterResponse( new TMitovGSMSerialFunctionExpectOk( FModule.FOwner, MAKE_CALLBACK_3( OpenWire::Component, TOnMitovGSMSerialFunctionResult, SIMComGPRSTCPServerSocket::OpenedResult, bool ), "AT+CIPSERVER=0"), "AT+CIPSERVER=0", true, 5 );
				FModule.ReleaseServerID( this );
				FRunning = false;
			}
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SIMComGPRSTCPClientSocket : public BasicSIMComGPRSSocket
	{
		typedef BasicSIMComGPRSSocket inherited;

	public:
		OpenWire::SourcePin	ConnectedOutputPin;

	public:
		String	Host;
		int		FID;

	protected:
		int		FReadSize;
		int		FReadCounter;
		uint8_t	*FBuffer = nullptr;
		uint8_t	*FBufferPtr;
		bool	FConnectingRunning = false;

	public:
		virtual bool ProcessCharacter( char AChar, bool &ASuccess ) override
		{ 
			if( FReadCounter )
			{
				*FBufferPtr++ = *(uint8_t*)&AChar;
				--FReadCounter;
//				Serial.println( FReadSize );
//				Serial.println( AChar );
//				OutputPin.SendValue( Mitov::TDataBlock( 1, &AChar ));
				if( !FReadCounter )
				{
					ASuccess = true;
					OutputPin.SendValue( Mitov::TDataBlock( FReadSize, FBuffer ));
//					Serial.println( (char *)FBuffer );
					delete [] FBuffer;
					FBuffer = nullptr;
				}

				return true;
			}

			return false; 
		}

		virtual bool TryProcessInput( String ALine, bool &ALockInput ) override
		{ 
			if( ! FConnectingRunning )
				return false;

			ALine.trim();
			if( ALine == String( FID ) + ", CONNECT OK" )
			{
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "CLIENT::CONNECT OK " + String( FID ) );
#endif
				FRunning = true;
				ConnectedOutputPin.SendValue( true );
				return true;
			}

			if( ALine == String( FID ) + ", CLOSED" )
			{
				FConnectingRunning = false;
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "CLIENT::CLOSED " + String( FID ) );
#endif
				StopSocket();
				return true;
			}

			if( ALine == String( FID ) + ", CLOSE OK" )
			{
				FConnectingRunning = false;
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "CLIENT::CLOSE OK " + String( FID ) );
#endif
				StopSocket();
				return true;
			}

			if( ALine == String( FID ) + ", SEND OK" )
			{
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "CLIENT::SEND OK " + String( FID ) );
#endif
				return true;
			}

			if( ALine.startsWith( "+RECEIVE," + String( FID ) + "," ))
			{
				ALine.remove( 0, 11 );
				FReadSize = ALine.toInt();
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "RECEIVE " + String( FID ) + " -> " + FReadSize );
#endif
				if( FReadSize )
				{
					FReadCounter = FReadSize;
					if( FBuffer )
						delete [] FBuffer;

					FBuffer = new uint8_t[ FReadSize ];
					FBufferPtr = FBuffer;
					ALockInput = true;
				}

				return true;
			}

			return false; 
		}

	protected:
		void OpenedResult( bool AResult )
		{
#ifdef __SIMGPRS__DEBUG__
			if( AResult )
				Serial.println( "SocketConnected" );
#endif
		}

	protected:
		virtual void SystemStart() override
		{
			inherited::SystemStart();
			ConnectedOutputPin.SendValue( false );
		}

		virtual void StartSocket() override
		{
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "CLIENT::StartSocket" );
#endif
			if( FModule.AssignConnectionID( this, FID ) )
			{
#ifdef __SIMGPRS__DEBUG__
				Serial.print( "CLIENT::StartSocket ID: " ); Serial.println( FID );
#endif
				FConnectingRunning = true;
				FModule.FOwner.SendQueryRegisterResponse( new TMitovGSMSerialFunctionStatus( FModule.FOwner, MAKE_CALLBACK_3( OpenWire::Component, TOnMitovGSMSerialFunctionResult, SIMComGPRSTCPClientSocket::OpenedResult, bool )), "AT+CIPSTART=" + String( FID ) +",\"TCP\",\"" + Host + "\"," + String( Port ), true, 5 );
//				FModule.SendData( "AT+CIPSTART=" + String( FID ) +",\"TCP\",\"" + Host + "\"," + String( Port ), 2000 );
			}

//			Serial.println( "StartSocket" );
		}

		virtual void StopSocket()
		{
//			FConnectingRunning = false;
			if( FRunning )
			{
#ifdef __SIMGPRS__DEBUG__
				Serial.print( "CLIENT::CloseSocket ID: " ); Serial.println( FID );
#endif
				ConnectedOutputPin.SendValue( false );
				FModule.ReleaseConnectionID( FID );
				FModule.FOwner.TrySendQuery( "AT+CIPCLOSE=" + String( FID ), nullptr );
//				FModule.SendData( "AT+CIPCLOSE=" + String( FID ), 2000 );
				FRunning = false;
			}
//			FClient.stop();
		}

/*
		virtual void DataReceived( int connectionId, unsigned char AData )
		{
			OutputPin.Notify( &AData );
		}
*/

	public:
		virtual void EndPacket()
		{
			if( FStringPrint.Value.size() == 0 )
				return;

			FModule.FOwner.SendQueryRegisterResponse( new TSIMComGPRSTCPSocketSendPacket( this, FStringPrint ), "AT+CIPSEND=" + String( FID ) +"," + String( FStringPrint.Value.size()), true, 5, 60000 );

//			if( FModule.SendData( "AT+CIPSEND=" + String( FID ) + "," + String( FStringPrint.Value.size()), "\r\n> ", 2000 ))
//				FModule.SendDirect( (uint8_t *)FStringPrint.Value, FStringPrint.Value.size() );

//			FStringPrint.Value.clear();
		}

	public:
/*
		virtual void SystemLoopBegin( unsigned long currentMicros ) 
		{
			inherited::SystemLoopBegin( currentMicros );
		}
*/
	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SIMComGPRSUDPClientSocket : public SIMComGPRSTCPClientSocket
	{
		typedef SIMComGPRSTCPClientSocket inherited;

	protected:
		virtual void StartSocket() override
		{
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "UDP CLIENT::StartSocket" );
#endif
			if( FModule.AssignConnectionID( this, FID ) )
			{
#ifdef __SIMGPRS__DEBUG__
				Serial.print( "UDP CLIENT::StartSocket ID: " ); Serial.println( FID );
#endif
				FConnectingRunning = true;
				FModule.FOwner.SendQueryRegisterResponse( new TMitovGSMSerialFunctionStatus( FModule.FOwner, MAKE_CALLBACK_3( OpenWire::Component, TOnMitovGSMSerialFunctionResult, SIMComGPRSUDPClientSocket::OpenedResult, bool )), "AT+CIPSTART=" + String( FID ) +",\"UDP\",\"" + Host + "\"," + String( Port ), true, 5 );
//				FModule.SendData( "AT+CIPSTART=" + String( FID ) +",\"TCP\",\"" + Host + "\"," + String( Port ), 2000 );
			}

//			Serial.println( "StartSocket" );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	BasicSIMComGPRSSocket::BasicSIMComGPRSSocket( SIMComGPRSModule &AModule ) :
		MitovGSMSerialBasicFunction( AModule.FOwner ),
		FModule( AModule )
	{
		DisconnectInputPin.SetCallback( MAKE_CALLBACK( BasicSIMComGPRSSocket::DoDisconnect ));
		AModule.Sockets.push_back( this );
	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	TMitovGSMSerialGPRSGetIPFunctionStatus::TMitovGSMSerialGPRSGetIPFunctionStatus( SIMComGPRSModule *AOwnerModule ) :
		inherited( AOwnerModule->FOwner ),
		FOwnerModule( AOwnerModule )
	{
	}
//---------------------------------------------------------------------------
	bool TMitovGSMSerialGPRSGetIPFunctionStatus::TryProcessRequestedInput( String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess )
	{
//			Serial.println( "TryProcessRequestedInput" );
//			Serial.println( ALine );
//			delay( 100 );
		if( ALine != "" )
		{
			ALine.trim();
#ifdef __SIMGPRS__DEBUG__
			Serial.print( "RES IP: " ); Serial.println( ALine );
#endif

			bool AIsValid = true;
			int ACountDots = 0;
			for( int i = 0; i < ALine.length(); ++i )
			{
				if( ALine[ i ] == '.' )
					++ACountDots;

				else if( ! isdigit( ALine[ i ] ))
				{
					AIsValid = false;
					break;
				}
			}

#ifdef __SIMGPRS__DEBUG__
			Serial.println( AIsValid );
			Serial.println( ACountDots );
#endif

			ASuccess = ( ( ACountDots == 3 ) && AIsValid );
			if( ASuccess)
				FOwnerModule->GetIPConfigResult( ALine ); //AddressOutputPin.SendValue( ALine, false );

			AResponseCompleted = true;
			ALockInput = false;
			return true;
		}

		return false;
	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	TSIMComGPRSTCPSocketSendPacket::TSIMComGPRSTCPSocketSendPacket( BasicSIMComGPRSSocket* AOwnerSocket, BufferPrint AStringPrint ) :
		inherited( AOwnerSocket->FOwner ),
		FStringPrint( AStringPrint )
	{
/*
#ifdef __SIMGPRS__DEBUG__
		Serial.println( "TCPSocketSendPacket" );

		for( int i = 0; i < FStringPrint.Value.size(); ++i )
			Serial.println( FStringPrint.Value[ i ] );

		Serial.println( "" );

#endif
*/
	}
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

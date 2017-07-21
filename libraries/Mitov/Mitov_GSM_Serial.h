////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GSM_SERIAL_h
#define _MITOV_GSM_SERIAL_h

#include <Mitov.h>

//#define __SIMGSM__DEBUG__

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class MitovGSMSerial;
	class MitovGSMSerialVoiceCallFunction;
	class MitovGSMSerialSMSMessageReceivedFunction;
	class MitovGSMSerialSMSModule;
	class MitovGSMSerialSMSBasicMessageFunction;
//---------------------------------------------------------------------------
	namespace GSMFinc
	{
		bool ExtractTimeStamp( String ATimeStamp, TDateTime &ADateTime, int32_t &ATimeZone )
		{
			if( ATimeStamp.length() < 17 )
				return false;

			String AText = ATimeStamp.substring( 0, 2 );
			int16_t AYear = 2000 + AText.toInt();

			AText = ATimeStamp.substring( 3, 5 );
			int16_t AMonth = AText.toInt();

			AText = ATimeStamp.substring( 6, 8 );
			int16_t ADay = AText.toInt();

			AText = ATimeStamp.substring( 9, 11 );
			int16_t AHour = AText.toInt();

			AText = ATimeStamp.substring( 12, 14 );
			int16_t AMinute = AText.toInt();

			AText = ATimeStamp.substring( 15, 17 );
			int16_t ASecond = AText.toInt();

			AText = ATimeStamp.substring( 17, 20 );
			ATimeZone = AText.toInt();

			return ADateTime.TryEncodeDateTime( AYear, AMonth, ADay, AHour, AMinute, ASecond, 0 );
		}
	}
//---------------------------------------------------------------------------
	class MitovGSMSerialBasicFunction : public OpenWire::Object
	{
	public:
		MitovGSMSerial &FOwner;

	public:
		virtual void ElementSystemStart() {} // Make sure it is different than SystemStart
		virtual bool TryProcessPromptInput( bool &ASuccess ) { ASuccess = false; return false; }
		virtual bool TryProcessRequestedInput( String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) { return false; }
		virtual bool TryProcessInput( String ALine, bool &ALockInput ) { return false; }
		virtual bool ProcessCharacter( char AChar, bool &ASuccess ) { return false; }
		virtual void TimedOut() {}

	public:
		MitovGSMSerialBasicFunction( MitovGSMSerial &AOwner );
		virtual ~MitovGSMSerialBasicFunction();
	};
//---------------------------------------------------------------------------
	class MitovGSMSerialBasicExpectOKFunction : public MitovGSMSerialBasicFunction
	{
		typedef MitovGSMSerialBasicFunction inherited;

	protected:
		bool	FEmptyLineDetected = false;

	public:
		void Reset()
		{
			FEmptyLineDetected = false;
		}

	protected:
		virtual void OKReceived()
		{
		}

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
//			Serial.println( "TryProcessRequestedInput" );
//			Serial.println( ALine );
			if( FEmptyLineDetected )
			{
				ALine.trim();
				if( ALine != "" )
				{
					ASuccess = ( ALine == "OK" );
					if( ASuccess )
					{
//						Serial.println( "OK Processed" );
		//					Serial.println( "ALockInput = false" );
						OKReceived();
					}

					AResponseCompleted = true;
					FEmptyLineDetected = false;
					ALockInput = false;
					return true;
				}
			}

			else if( ALine == "" )
			{
//				Serial.println( "FEmptyLineDetected" );
				FEmptyLineDetected = true;
				return true;
			}

			return false;
		}

	public:
		using inherited::inherited;


	};
//---------------------------------------------------------------------------
	typedef void (OpenWire::Object::*TOnMitovGSMSerialFunctionResult) ( bool AResult );
	typedef bool (OpenWire::Object::*TOnMitovGSMSerialFunctionLine) ( String ALine );
//---------------------------------------------------------------------------
	class TMitovGSMSerialFunctionExpectOk : public MitovGSMSerialBasicFunction
	{
		typedef MitovGSMSerialBasicFunction inherited;

	protected:
		OpenWire::Object *FOwnerFunction;
		TOnMitovGSMSerialFunctionResult FCallback;

	protected:
		bool	FLocked = false;
		bool	FEmptyLineDetected = false;
		String	FLockStringStart;

	public:
		void Reset()
		{
			FEmptyLineDetected = false;
		}

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
//			Serial.println( "TryProcessRequestedInput" );
//			Serial.println( ALine );
			if( FEmptyLineDetected )
			{
				ALine.trim();
#ifdef __SIMGSM__DEBUG__
				Serial.print( "RES1: " ); Serial.println( ALine );
#endif
				ASuccess = ( ALine == "OK" );
				if( ASuccess || ( ALine == "ERROR" ))
				{
					( FOwnerFunction->*FCallback )( ALine == "OK" );

					AResponseCompleted = true;
					FEmptyLineDetected = false;
					ALockInput = false;
					return true;
				}
			}

			else if( ALine == "" )
			{
//				Serial.println( "FEmptyLineDetected" );
				FEmptyLineDetected = true;
				return true;
			}

			else
			{
				ALine.trim();
	//			Serial.print( "SHORT_LINE: " ); Serial.println( AShortLine );
				if( ALine.startsWith( FLockStringStart ))
				{
					ALockInput = true;
					FLocked = true;
	//				Serial.println( "+CMGR: DETECTED!!!"  );
				}
			}

			return false;
		}

	public:
		TMitovGSMSerialFunctionExpectOk( MitovGSMSerial &AOwner, OpenWire::Object *AOwnerFunction, TOnMitovGSMSerialFunctionResult ACallback, String ALockStringStart ) :
			inherited( AOwner ),
			FOwnerFunction( AOwnerFunction ),
			FCallback( ACallback )
		{
		}

	};
	//---------------------------------------------------------------------------
	class TMitovGSMSerialFunctionStatus : public MitovGSMSerialBasicFunction
	{
		typedef MitovGSMSerialBasicFunction inherited;

	protected:
		OpenWire::Object *FOwnerFunction;
		TOnMitovGSMSerialFunctionResult FCallback;

	protected:
		bool	FEmptyLineDetected = false;

	public:
		void Reset()
		{
			FEmptyLineDetected = false;
		}

	public:
		virtual void TimedOut() override
		{
			if( FCallback )
				( FOwnerFunction->*FCallback )( false );
		}

		virtual bool TryProcessRequestedInput( String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
//			Serial.println( "TryProcessRequestedInput" );
//			Serial.println( ALine );
			if( FEmptyLineDetected )
			{
				ALine.trim();
#ifdef __SIMGSM__DEBUG__
				Serial.print( "RES1: " ); Serial.println( ALine );
#endif
				ASuccess = ( ALine == "OK" );
				if( ASuccess || ( ALine == "ERROR" ))
				{
					if( FCallback )
						( FOwnerFunction->*FCallback )( ALine == "OK" );

					AResponseCompleted = true;
					FEmptyLineDetected = false;
					ALockInput = false;
					return true;
				}
			}

			else if( ALine == "" )
			{
//				Serial.println( "FEmptyLineDetected" );
				FEmptyLineDetected = true;
				return true;
			}

			return false;
		}

	public:
/*
		TMitovGSMSerialFunctionStatus( MitovGSMSerialBasicFunction *AOwnerFunction, TOnMitovGSMSerialFunctionResult ACallback ) :
			inherited( AOwnerFunction->FOwner ),
			FOwnerFunction( AOwnerFunction ),
			FCallback( ACallback )
		{
		}
*/
		TMitovGSMSerialFunctionStatus( MitovGSMSerial &AOwner, OpenWire::Object *AOwnerFunction, TOnMitovGSMSerialFunctionResult ACallback ) :
			inherited( AOwner ),
			FOwnerFunction( AOwnerFunction ),
			FCallback( ACallback )
		{
		}

	};
//---------------------------------------------------------------------------
	class TArduinoGSMSerialDelays
	{
	public:
		uint32_t	PowerPulse = 2000000;
		uint32_t	Shutdown = 10000000;
		uint32_t	Startup = 20000000;
	};
//---------------------------------------------------------------------------
	class MitovGSMSerial : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	protected:
		struct TResponseElement
		{
		public:
			MitovGSMSerialBasicFunction *Handler;
			bool Delete;
			uint32_t Timeout;
			String Command;
			uint8_t RetryCount;

		public:
			TResponseElement() :
				Handler( nullptr ),
				Delete( false ),
				Timeout( 0 )
			{
			}

			TResponseElement( MitovGSMSerialBasicFunction *AHandler, bool ADelete, uint32_t ATimeout, String ACommand, uint8_t ARetryCount ) :
				Handler( AHandler ),
				Delete( ADelete ),
				Timeout( ATimeout ),
				Command( ACommand ),
				RetryCount( ARetryCount )
			{
#ifdef __SIMGSM__DEBUG__
				Serial.print( "TResponseElement: " ); Serial.println( Command );
#endif
			}

			~TResponseElement()
			{
				if( Delete )
					delete Handler;
			}

		};

		struct TQueryElement
		{
		public:
			String Command;
			TResponseElement *ResponseElement;

		public:
			TQueryElement() :
				ResponseElement( nullptr )
			{
			}

			TQueryElement( String ACommand, TResponseElement *AResponseElement ) :
				Command( ACommand ),
				ResponseElement( AResponseElement )
			{
			}

		};


	public:
		OpenWire::SourcePin	PowerOutputPin;
		OpenWire::SourcePin	ReadyOutputPin;

	public:
		bool	PowerOn = true;

		TArduinoGSMSerialDelays	Delays;

	public:
		bool	IsStarted = false;

	protected:
		bool	FPowerChecked = false;
		bool	FInPowerCheckWait = false;
		bool	FInPowerSwitch = false;
		bool	FInPowerSwitchDelay = false;
		bool	FInStartupWait = false;
		TResponseElement *FCurrentElement = nullptr;
		unsigned long FLastTime = 0;
		unsigned long FCurrentMillis = 0;

	public:
		void SetPowerOn( bool AValue )
		{
			if( PowerOn == AValue )
				return;

			PowerOn = AValue;
			if( !PowerOn )
				IsStarted = false;
//			Serial.println( "SetPowerOn" );
//			Serial.println( PowerOn );
			QueryPowerOn();
		}

	protected:
		Mitov::SimpleList<MitovGSMSerialBasicFunction *>	FFunctions;
		Mitov::SimpleList<TResponseElement *>				FResponseHandlersQueue;
		Mitov::SimpleList<TQueryElement>					FQueryQueue;

	public:
		void	*FSocketConnections[ 7 ];

	public:
		void AddFunction( MitovGSMSerialBasicFunction *AFunction )
		{
			FFunctions.push_back( AFunction );
		}

		void RemoveFunction( MitovGSMSerialBasicFunction *AFunction )
		{
			FFunctions.erase( AFunction );
		}

	public:
		void SendDirect( void *AData, int ASize )
		{
#ifdef __SIMGSM__DEBUG__
			Serial.print( "SENDING : " ); Serial.println( ASize );

			char *APtr = (char *)AData;
			for( int i = 0; i < ASize; ++i )
				Serial.print( *APtr++ );
#endif

			FStream.write( (uint8_t *)AData, ASize );
		}

		void SendQuery( String AQuery, TResponseElement *AElement = nullptr )
		{
#ifdef __SIMGSM__DEBUG__
			Serial.print( "QUERY : \"" );	Serial.print( AQuery ); Serial.println( "\"" );
#endif
			FCurrentMillis = millis();

			FStream.println( AQuery );
			FCurrentElement = AElement;
		}

		void TrySendQuery( String AQuery, TResponseElement *AElement )
		{
			if( ( PowerOn || ( AQuery == "AT" ) ) && ( FResponseHandlersQueue.size() == 0 ) && ( !FInPowerSwitch ))
				SendQuery( AQuery, AElement );

			else
			{
#ifdef __SIMGSM__DEBUG__
				Serial.print( "ADD TO QUERY : \"" );	Serial.print( AQuery ); Serial.println( "\"" );
#endif
				FQueryQueue.push_back( TQueryElement( AQuery, AElement ));

#ifdef __SIMGSM__DEBUG__
				Serial.println( "QUEUE>>" );
				for( int i = 0; i < FQueryQueue.size(); ++i )
					Serial.println( FQueryQueue[ i ].Command );
				Serial.println( "<<QUEUE" );
#endif
			}

		}

		void SendQueryRegisterResponse( MitovGSMSerialBasicFunction *ASender, String AQuery, bool ADelete = false, uint8_t ARetryCount = 0, uint32_t ATimeout = 10000 )
		{
#ifdef __SIMGSM__DEBUG__
			Serial.print( "PUSHING: " ); Serial.println( AQuery );
#endif
			TResponseElement *AElement = new TResponseElement( ASender, ADelete, ATimeout, ( ARetryCount ) ? AQuery : "", ARetryCount );
			TrySendQuery( AQuery, AElement );
			FResponseHandlersQueue.push_back( AElement );
			
//			SendQuery( AQuery );
//			Serial.print( "PUSHING: " ); Serial.println( AQuery );
//			Serial.println( "PUSHED" );
		}

		void AbortResponseHandler( MitovGSMSerialBasicFunction *ASender )
		{
			if( FLockRequestedInputIndex )
				if( FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Handler == ASender)
					FLockRequestedInputIndex = 0;

#ifdef __SIMGSM__DEBUG__
			Serial.println( "FResponseHandlersQueue.erase" );
#endif
			for( int i = 0; i < FResponseHandlersQueue.size(); ++i )
				if( FResponseHandlersQueue[ i ]->Handler == ASender )
				{
					if( FCurrentElement == FResponseHandlersQueue[ i ] )
						FCurrentElement =nullptr;

					delete FResponseHandlersQueue[ i ];
					FResponseHandlersQueue.Delete( i );
					break;
				}

//			FResponseHandlersQueue.erase( ASender );
		}

	public:
		Stream &FStream;

		TMitovGSMSerialFunctionStatus		FPowerOnFunction;

	protected:
		char		FBuffer[ 256 ];
		uint8_t		FIndex = 0;
		uint16_t	FLockInputIndex = 0;
		uint16_t	FLockRequestedInputIndex = 0;

	protected:
		void ProcessNextCommand()
		{
//			Serial.println( "ProcessNextCommand" );
//				Serial.print( "RESP_QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
			if( FQueryQueue.size() )
			{
//					Serial.print( "SEND_QUERY: " ); Serial.println( FQueryQueue.size() );
				TQueryElement AElement = FQueryQueue[ 0 ];
//					Serial.print( "ESTRACT_QUERY: " ); Serial.println( ACommand );
				FQueryQueue.pop_front();

#ifdef __SIMGSM__DEBUG__
				Serial.println( "QUEUE>>" );
				for( int i = 0; i < FQueryQueue.size(); ++i )
					Serial.println( FQueryQueue[ i ].Command );
				Serial.println( "<<QUEUE" );
#endif

				SendQuery( AElement.Command, AElement.ResponseElement );
//				Serial.print( "SEND_QUERY: " ); Serial.println( FQueryQueue.size() );
			}
		}

		void ReadSerial()
		{
//			if( FInPowerSwitch )
//				return;

			int AChar = FStream.read();
//			Serial.print( AChar );
			if( AChar < 0 )
				return;

			if( FLockInputIndex )
			{
				bool ASuccess = false;
				if( FFunctions[ FLockInputIndex - 1 ]->ProcessCharacter( AChar, ASuccess ))
				{
					if( ASuccess )
						FLockInputIndex = 0;

					return;
				}
			}

//			Serial.print( (char)AChar );
//			if( AChar < ' ' )
//				Serial.println( AChar );

			if( AChar == 13 )
				return;

			if( AChar != 10 )
			{
				if( FIndex == 0 )
					if( AChar == '>' )
/*
						if( FLockRequestedInputIndex )
						{
							bool ASuccess = false;
							if( FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Handler->TryProcessPromptInput( ASuccess ))
							{
								if( FCurrentElement == FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ] )
									FCurrentElement = nullptr;

								if( ( ! ASuccess ) && FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Command && FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->RetryCount )
								{
									Serial.println( "RETRY5" );
									Serial.println( FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Command );
									-- FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->RetryCount;
									SendQuery( FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Command, FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ] );
								}

								else
								{
									Serial.println( "Queue Delete 5" );
		//							Serial.println( ALockInput );
		//							Serial.print( "RESP_QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
									delete FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ];
									FResponseHandlersQueue.Delete( FLockRequestedInputIndex - 1 );
		//							Serial.println( "ProcessNextCommand 1" );
									ProcessNextCommand();
								}
							}

*/

						for( int i = 0; i < FResponseHandlersQueue.size(); ++i )
						{
							bool ASuccess = false;
							if( FResponseHandlersQueue[ i ]->Handler->TryProcessPromptInput( ASuccess ))
							{
								if( FCurrentElement == FResponseHandlersQueue[ i ] )
									FCurrentElement = nullptr;

								if( ( ! ASuccess ) && FResponseHandlersQueue[ i ]->Command && FResponseHandlersQueue[ i ]->RetryCount )
								{
#ifdef __SIMGSM__DEBUG__
									Serial.println( "RETRY5" );
									Serial.println( FResponseHandlersQueue[ i ]->Command );
#endif
									-- FResponseHandlersQueue[ i ]->RetryCount;
									SendQuery( FResponseHandlersQueue[ i ]->Command, FResponseHandlersQueue[ i ] );
								}

								else
								{
#ifdef __SIMGSM__DEBUG__
									Serial.println( "Queue Delete 5" );
		//							Serial.println( ALockInput );
		//							Serial.print( "RESP_QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
#endif
									delete FResponseHandlersQueue[ i ];
									FResponseHandlersQueue.Delete( i );
		//							Serial.println( "ProcessNextCommand 1" );
									ProcessNextCommand();
								}
							}
						}

				FBuffer[ FIndex ++ ] = AChar;
				if( FIndex < 255 )
					return;
			}

//			Serial.println( "TEST!!!" );
//			Serial.println( "" );
//			Serial.println( FIndex );

			FBuffer[ FIndex ] = '\0';
			FIndex = 0;

			String AString = FBuffer;

#ifdef __SIMGSM__DEBUG__
			Serial.print( "LINE: " ); Serial.println( AString );

//			Serial.print( "QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
#endif

			bool	ALockInput;
			bool	AResponseCompleted = false;

//			Serial.print( "FLockRequestedInputIndex : " ); Serial.println( FLockRequestedInputIndex );
//			Serial.print( "FLockInputIndex : " ); Serial.println( FLockInputIndex );
			if( FLockRequestedInputIndex )
			{
				ALockInput = true;
				bool ASuccess = false;
				if( FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Handler->TryProcessRequestedInput( AString, ALockInput, AResponseCompleted, ASuccess ))
				{
					if( AResponseCompleted )
					{
						if( FCurrentElement == FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ] )
							FCurrentElement = nullptr;

						if( ( ! ASuccess ) && FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Command && FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->RetryCount )
						{
#ifdef __SIMGSM__DEBUG__
							Serial.println( "RETRY1" );
							Serial.println( FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Command );
#endif
							-- FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->RetryCount;
							SendQuery( FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Command, FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ] );
						}

						else
						{
#ifdef __SIMGSM__DEBUG__
							Serial.println( "Queue Delete 1" );
//							Serial.println( ALockInput );
//							Serial.print( "RESP_QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
#endif
							delete FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ];
							FResponseHandlersQueue.Delete( FLockRequestedInputIndex - 1 );
//							Serial.println( "ProcessNextCommand 1" );
							ProcessNextCommand();
						}
					}

					if( ! ALockInput )
						FLockRequestedInputIndex = 0;
				}

				return;
			}

			ALockInput = false;
			AResponseCompleted = false;
			for( int i = 0; i < FResponseHandlersQueue.size(); ++i )
			{
				bool ASuccess = false;
				if( FResponseHandlersQueue[ i ]->Handler->TryProcessRequestedInput( AString, ALockInput, AResponseCompleted, ASuccess ))
				{
					if( ALockInput )
						FLockRequestedInputIndex = i + 1;

					if( AResponseCompleted )
					{
						if( FCurrentElement == FResponseHandlersQueue[ i ] )
							FCurrentElement = nullptr;

						if( ( ! ASuccess ) && FResponseHandlersQueue[ i ]->Command && FResponseHandlersQueue[ i ]->RetryCount )
						{
#ifdef __SIMGSM__DEBUG__
							Serial.println( "RETRY2" );
							Serial.println( FResponseHandlersQueue[ i ]->Command );
#endif
							-- FResponseHandlersQueue[ i ]->RetryCount;
							SendQuery( FResponseHandlersQueue[ i ]->Command, FResponseHandlersQueue[ i ] );
						}

						else
						{
#ifdef __SIMGSM__DEBUG__
							Serial.println( "Queue Delete 2" );
							Serial.print( "DELETING: " ); Serial.println( FResponseHandlersQueue[ i ]->Command );
//							Serial.print( "RESP_QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
#endif
							delete FResponseHandlersQueue[ i ];
							FResponseHandlersQueue.Delete( i );
//							Serial.println( "ProcessNextCommand 2" );
							ProcessNextCommand();
						}
					}

					return;
				}
			}

			if( FLockInputIndex )
			{
//				Serial.println( "FLockInputIndex" );
				ALockInput = true;
				FFunctions[ FLockInputIndex - 1 ]->TryProcessInput( AString, ALockInput );
				if( ! ALockInput )
					FLockInputIndex = 0;

				return;
			}

//			Serial.println( "*****" );
			ALockInput = false;
			for( int i = 0; i < FFunctions.size(); ++i )
				if( FFunctions[ i ]->TryProcessInput( AString, ALockInput ))
				{
					if( ALockInput )
					{
						FLockInputIndex = i + 1;
#ifdef __SIMGSM__DEBUG__
						Serial.print( "FLockInputIndex = " ); Serial.println( FLockInputIndex );
#endif
					}

					return;
				}
/*
			ALine.trim();
			if( ALine == "RDY" )
			{
				return true;
			}

			if( ALine == "NORMAL POWER DOWN" )
			{
				return true;
			}

			if( ALine == "Call Ready" )
			{
			}

*/
		}

	public:
		void PulsePower()
		{
#ifdef __SIMGSM__DEBUG__
			Serial.println( "PULSE" );
#endif
			FInPowerSwitch = true;
			PowerOutputPin.SendValue( true );
			FLastTime = micros();
		}

		void PulsePowerDelay()
		{
#ifdef __SIMGSM__DEBUG__
			Serial.println( "PULSE DELAY" );
#endif
			FInPowerSwitchDelay = true;
			FLastTime = micros();
		}

		void DoPowerResult( bool ASucess )
		{
			if( ! ASucess )
				return;

//			Serial.println( "PowerOKReceived()" );
			FInPowerCheckWait = false;
			if( PowerOn && FPowerChecked )
				StartStartupWait();

			else
			{
#ifdef __SIMGSM__DEBUG__
				Serial.println( "FPowerChecked = true" );
#endif
				if( PowerOn )
					PulsePower();

				else
					FPowerChecked = true;
			}

		}

	protected:
		void QueryPowerOn()
		{
//			Serial.print( "FInPowerSwitch = " ); Serial.println( FInPowerSwitch );

			if( FInPowerSwitch )
				return;

			if( ! PowerOutputPin.IsConnected() )
			{
				ModuleReady();
				return;
			}

//			Serial.println( "AT..." );
			SendQueryRegisterResponse( &FPowerOnFunction, "AT" );
			FLastTime = micros();
			FInPowerCheckWait = true;
		}

		void ModuleReady()
		{
			IsStarted = PowerOn;
			ReadyOutputPin.Notify( nullptr );

			for( int i = 0; i < FFunctions.size(); ++i )
				FFunctions[ i ]->ElementSystemStart();
		}

		void StartStartupWait()
		{
#ifdef __SIMGSM__DEBUG__
			Serial.println( "STARTUP_WAIT" );
#endif
			FInStartupWait = true;
			FLastTime = micros();
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			inherited::SystemLoopBegin( currentMicros );
			if( FInStartupWait )
			{
				if( currentMicros - FLastTime >= Delays.Startup ) // 20000000 )
				{
#ifdef __SIMGSM__DEBUG__
					Serial.println( "FInStartupWait" );
#endif
					FInStartupWait = false;
					ModuleReady();
				}
			}

			else if( FInPowerCheckWait )
			{
				if( currentMicros - FLastTime >= 2000000 )
				{
					FInPowerCheckWait = false;
					FPowerOnFunction.Reset();
					AbortResponseHandler( &FPowerOnFunction );
#ifdef __SIMGSM__DEBUG__
					Serial.println( "FInPowerCheckWait" );
#endif
					if( PowerOn )
					{
						FPowerChecked = true;
						PulsePower();
					}

					else
						StartStartupWait();

//					PowerOutputPin.SendValue( false );
				}
			}

			else if( FInPowerSwitchDelay )
			{
				if( currentMicros - FLastTime >= Delays.Shutdown ) //10000000 )
				{
#ifdef __SIMGSM__DEBUG__
					Serial.println( "SECOND_PULSE" );
#endif
					FInPowerSwitchDelay = false;	
					PulsePower();
				}
			}

			else if( FInPowerSwitch )
			{
				if( currentMicros - FLastTime >= Delays.PowerPulse ) //2000000 )
				{
					PowerOutputPin.SendValue( false );
					if( FPowerChecked )
					{
#ifdef __SIMGSM__DEBUG__
						Serial.println( "FInPowerSwitch" );
#endif
						FInPowerSwitch = false;	
						StartStartupWait();
					}

					else
					{
#ifdef __SIMGSM__DEBUG__
						Serial.println( "FPowerChecked = true" );
#endif
						FPowerChecked = true;
						PulsePowerDelay();
					}
				}
			}

			ReadSerial();
//			Serial.println( "TEST6" );
			if( FCurrentElement )
			{
				unsigned long AMillis = millis();
				if( ( AMillis - FCurrentMillis ) > FCurrentElement->Timeout )
				{
#ifdef __SIMGSM__DEBUG__
					Serial.println( "TIMEOUT" );
#endif
					FLockRequestedInputIndex = 0;

					if( FCurrentElement->Command && FCurrentElement->RetryCount )
					{
#ifdef __SIMGSM__DEBUG__
						Serial.println( "RETRY3" );
						Serial.println( FCurrentElement->Command );
#endif
						-- FCurrentElement->RetryCount;
						SendQuery( FCurrentElement->Command, FCurrentElement );
					}

					else
					{			
						FCurrentElement->Handler->TimedOut();
						FResponseHandlersQueue.erase( FCurrentElement );
						delete FCurrentElement;
	//					Serial.println( "ProcessNextCommand 3" );
						ProcessNextCommand();
					}

					FCurrentElement = nullptr;
				}
			}
/*
			if( FResponseHandlersQueue.size() > 0 )
			{
				int AIndex = ( FLockRequestedInputIndex ) ? ( FLockRequestedInputIndex - 1 ) : 0;
				unsigned long AMillis = millis();
//				Serial.print( "TEST7: " ); Serial.println( AIndex );
//				Serial.print( "SIZE: " ); Serial.println( FResponseHandlersQueue.size() );
				if( ( AMillis - FCurrentMillis ) > FResponseHandlersQueue[ AIndex ]->Timeout )
				{
					Serial.print( "TIMEOUT: " ); Serial.println( AIndex );
					FLockRequestedInputIndex = 0;

					if( FResponseHandlersQueue[ AIndex ]->Command && FResponseHandlersQueue[ AIndex ]->RetryCount )
					{
						Serial.println( "RETRY3" );
						Serial.println( FResponseHandlersQueue[ AIndex ]->Command );
						-- FResponseHandlersQueue[ AIndex ]->RetryCount;
						TrySendQuery( FResponseHandlersQueue[ AIndex ]->Command );
					}

					else
					{			
						FResponseHandlersQueue[ AIndex ]->Handler->TimedOut();
						delete FResponseHandlersQueue[ AIndex ];
						FResponseHandlersQueue.Delete( AIndex );
	//					Serial.println( "ProcessNextCommand 3" );
						ProcessNextCommand();
					}
				}
			}
*/
			if( FResponseHandlersQueue.size() == 0 )
			{
//				Serial.println( "FResponseHandlersQueue.size() == 0" );
				ProcessNextCommand();
			}
		}

		virtual void SystemStart() override
		{
//			Serial.println( "SYSTEM_START" );
			PowerOutputPin.SendValue( false );
			QueryPowerOn();
		}

	public:
		MitovGSMSerial( Mitov::BasicSerialPort &ASerial ) :
			FStream( ASerial.GetStream() ),
			FPowerOnFunction( *this, this, (TOnMitovGSMSerialFunctionResult)&MitovGSMSerial::DoPowerResult )
		{
			memset( FSocketConnections, 0, sizeof( FSocketConnections ) );
		}

	};
//---------------------------------------------------------------------------
	class TArduinoGSMReceivingVoiceCallAutoAnswer
	{
	public:
		bool Enabled : 1;
		uint8_t NumberRings : 7;

	public:
		TArduinoGSMReceivingVoiceCallAutoAnswer() :
			Enabled( false ),
			NumberRings( 1 )
		{
		}
	};
//---------------------------------------------------------------------------
    class MitovGSMSerialReceivingVoiceCall
	{
		typedef MitovGSMSerialBasicFunction inherited;

	public:
		OpenWire::SourcePin	CallingOutputPin;
		OpenWire::SourcePin	NumberOutputPin;
		OpenWire::SourcePin	AddressTypeOutputPin;
		OpenWire::SourcePin	SubAddressOutputPin;
		OpenWire::SourcePin	SubAddressTypeOutputPin;
		OpenWire::SourcePin	PhoneBookAddressOutputPin;
		OpenWire::SinkPin	AnswerInputPin;

	public:
		TArduinoGSMReceivingVoiceCallAutoAnswer	AutoAnswer;

	protected:
		uint8_t	FRingCount = 0;

	public:
		void Ringing();

		inline void ClearRingCount()
		{
			FRingCount = 0;
		}

	protected:
		MitovGSMSerialVoiceCallFunction	&FOwner;

	public:
		MitovGSMSerialReceivingVoiceCall( MitovGSMSerialVoiceCallFunction &AOwner );

	};
//---------------------------------------------------------------------------
	class TMitovGSMSerialFunctionResponseLineAndStatus : public MitovGSMSerialBasicFunction
	{
		typedef MitovGSMSerialBasicFunction inherited;

	protected:
		OpenWire::Object *FOwnerFunction;
		TOnMitovGSMSerialFunctionResult FCallback;
		TOnMitovGSMSerialFunctionLine	FLineCallback;

	protected:
		bool	FEmptyLineDetected = false;
		bool	FLocked = false;

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
//			Serial.println( "TryProcessRequestedInput" );
//			Serial.println( ALine );
			if( FLocked )
			{
				if( FEmptyLineDetected )
				{
					ALine.trim();
#ifdef __SIMGSM__DEBUG__
					Serial.print( "RES1: " ); Serial.println( ALine );
#endif
					ASuccess = ( ALine == "OK" );
					if( ASuccess || ( ALine == "ERROR" ))
					{
	//					Serial.print( "RES2: " ); Serial.println( ALine );
						if( FCallback )
							( FOwnerFunction->*FCallback )( ALine == "OK" );

						AResponseCompleted = true;
						FEmptyLineDetected = false;
						ALockInput = false;
						FLocked = false;
						return true;
					}
				}

				else if( ALine == "" )
				{
	//				Serial.println( "FEmptyLineDetected" );
					FEmptyLineDetected = true;
					return true;
				}
			}

			else if(( FOwnerFunction->*FLineCallback )( ALine ) )
			{
#ifdef __SIMGSM__DEBUG__
				Serial.println( "READY!!!" );
#endif
				ALockInput = true;
				FLocked = true;
			}

			return false;
		}

	public:
		TMitovGSMSerialFunctionResponseLineAndStatus( MitovGSMSerial &AOwner, OpenWire::Object *AOwnerFunction, TOnMitovGSMSerialFunctionResult ACallback, TOnMitovGSMSerialFunctionLine ALineCallback ) :
			inherited( AOwner ),
			FOwnerFunction( AOwnerFunction ),
			FCallback( ACallback ),
			FLineCallback( ALineCallback )
		{
		}

	};
//---------------------------------------------------------------------------
	class MitovGSMSerialVoiceCallAnswerFunction : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	protected:
//		bool	FEmptyLineDetected = false;
		MitovGSMSerial &FOwner;
		TMitovGSMSerialFunctionStatus	FQuery;

	public:
		void Send()
		{
			FOwner.SendQueryRegisterResponse( &FQuery, "ATA" );
		}

	public:
//		virtual bool TryProcessRequestedInput( String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override;

	public:
		MitovGSMSerialVoiceCallAnswerFunction( MitovGSMSerial &AOwner ) :
			FOwner( AOwner ),
			FQuery( AOwner, this, nullptr )
		{
		}

	};
//---------------------------------------------------------------------------
	enum TArduinoGSMSerialVoiceModuleExistingCallMode {ccmDrop, ccmHold};
//---------------------------------------------------------------------------
	class MitovGSMSerialVoiceCallFunction : public MitovGSMSerialBasicFunction
	{
		typedef MitovGSMSerialBasicFunction inherited;

	public:
		OpenWire::SourcePin BusyOutputPin;
		OpenWire::SourcePin InUseOutputPin;

	protected:
		enum TState { sIdle, sCalling, sReceivingCall, sTalking };

	public:
		MitovGSMSerialReceivingVoiceCall *FReceivingCall = nullptr;

	protected:
		TState	FState = sIdle;

		MitovGSMSerialVoiceCallAnswerFunction	FVoiceCallAnswerFunction;

	protected:
		void SetState( TState AValue )
		{
			FState = AValue;
//			Serial.print( "STATE: " ); Serial.println( AValue );
		}

	public:
		void DoReceiveAnswer( void * )
		{
			FVoiceCallAnswerFunction.Send();
		}

	public:
		void CallAnswered()
		{
			InUseOutputPin.SendValue( true );
			SetState( sTalking );
		}

		void MakingCall()
		{
			SetState( sCalling );
		}

		void DropCall()
		{
			if( sIdle )
				return;

			FOwner.SendQueryRegisterResponse( new MitovGSMSerialBasicExpectOKFunction( FOwner ), "ATH", true );
		}

		void PlaceOnHold()
		{
			if( sIdle )
				return;

			FOwner.SendQueryRegisterResponse( new MitovGSMSerialBasicExpectOKFunction( FOwner ), "AT+CHLD=2", true ); // Place on Hold
		}

	public:
		virtual bool TryProcessInput( String ALine, bool &ALockInput ) override
		{
//			Serial.println( "????" );
//			Serial.println( ALine );
			ALine.trim();
//			Serial.println( "TEST3333" );
			if( ALine.startsWith( "+CLIP:" )) //"RDY" )
			{
				ALine.remove( 0, 6 );
				ALine.trim();
//				Serial.println( "TEST111" );
//				ProcessLine( ALine, false );
				if( FReceivingCall )
				{
					String ANumber;
					if( Func::ExtractOptionallyQuotedCommaText( ALine, ANumber ))
					{
						String AAddressType;
						if( Func::ExtractOptionallyQuotedCommaText( ALine, AAddressType ))
						{
							FReceivingCall->NumberOutputPin.SendValue( ANumber );
							FReceivingCall->AddressTypeOutputPin.SendValue<uint32_t>( AAddressType.toInt() );
							ALine.trim();
							String ASubAddress;
							if( Func::ExtractOptionallyQuotedCommaText( ALine, ASubAddress ))
							{
								String ASubAddressType;
								if( Func::ExtractOptionallyQuotedCommaText( ALine, ASubAddressType ))
								{
									FReceivingCall->SubAddressOutputPin.SendValue( ASubAddress );
									FReceivingCall->SubAddressTypeOutputPin.SendValue( ASubAddressType.toInt() );
									ALine.trim();

									String APhoneBookAddress;
									if( Func::ExtractOptionallyQuotedCommaText( ALine, APhoneBookAddress ))
										FReceivingCall->PhoneBookAddressOutputPin.SendValue( APhoneBookAddress );

									else
										FReceivingCall->PhoneBookAddressOutputPin.SendValue( "" );
								}
							}
							else
							{
								FReceivingCall->SubAddressOutputPin.SendValue( "" );
								FReceivingCall->SubAddressTypeOutputPin.SendValue( 0 );
								FReceivingCall->PhoneBookAddressOutputPin.SendValue( "" );
							}

						}
					}
				}

				InUseOutputPin.SendValue( true );
				SetState( sReceivingCall );
				return true;
			}

			if( ALine == "RING" )
			{
//				Serial.println( "RRRRR" );
				if( FReceivingCall )
					FReceivingCall->Ringing();
//					FReceivingCall->CallingOutputPin.Notify( nullptr );

				InUseOutputPin.SendValue( true );
				SetState( sReceivingCall );
				return true;
			}

			if( ALine == "BUSY" )
			{
				BusyOutputPin.Notify( nullptr );
				InUseOutputPin.SendValue( false );
				SetState( sIdle );
				if( FReceivingCall )
					FReceivingCall->ClearRingCount();

				return true;
			}

			if( ALine == "NO CARRIER" )
			{
				InUseOutputPin.SendValue( false );
				SetState( sIdle );
				if( FReceivingCall )
					FReceivingCall->ClearRingCount();

				return true;
			}

			if( ALine.startsWith( "+COLP:" ))
			{
				InUseOutputPin.SendValue( true );
				SetState( sTalking );
				return true;
			}

			return false;
		}

		virtual void ElementSystemStart() override
		{
			InUseOutputPin.SendValue( false );
		}

	public:
		MitovGSMSerialVoiceCallFunction( MitovGSMSerial &AOwner ) :
			inherited( AOwner ),
			FVoiceCallAnswerFunction( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class MitovVoiceModuleExpectOKFunction : public MitovGSMSerialBasicExpectOKFunction, public ClockingSupport
	{
		typedef MitovGSMSerialBasicExpectOKFunction inherited;

	protected:
		MitovGSMSerialVoiceCallFunction &FOwnerFunction;

	public:
		virtual void ElementSystemStart() override 
		{
			if( ! ClockInputPin.IsConnected() )
				DoClockReceive( nullptr );
		}

	public:
		MitovVoiceModuleExpectOKFunction( MitovGSMSerialVoiceCallFunction &AOwnerFunction ) :
			inherited( AOwnerFunction.FOwner ),
			FOwnerFunction( AOwnerFunction )
		{
		}

	};
//---------------------------------------------------------------------------
	class MitovVoiceModuleCallFunction : public MitovVoiceModuleExpectOKFunction
	{
		typedef MitovVoiceModuleExpectOKFunction inherited;

	public:
		String Number;
		TArduinoGSMSerialVoiceModuleExistingCallMode ExistingCallMode = ccmDrop;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			if( ExistingCallMode == ccmDrop )
				FOwnerFunction.DropCall();

			else
				FOwnerFunction.PlaceOnHold();

			FOwner.SendQueryRegisterResponse( this, String( "ATD" ) + Number + ";" );
			FOwnerFunction.MakingCall();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MitovGSMSerialVoiceSelectAudioChannelFunction : public MitovVoiceModuleExpectOKFunction
	{
		typedef MitovVoiceModuleExpectOKFunction inherited;

	public:
		uint8_t	Channel = 0;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.SendQueryRegisterResponse( this, String( "AT+CHFA=" ) + Channel );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MitovGSMSerialVoiceSetVolumeFunction : public MitovVoiceModuleExpectOKFunction
	{
		typedef MitovVoiceModuleExpectOKFunction inherited;

	public:
		uint8_t	Channel = 0;
		float	Volume = 0.5;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			int AVolume = ( Volume * 15 ) + 0.5;
			FOwner.SendQueryRegisterResponse( this, String( "AT+CMIC=" ) + Channel + "," + AVolume );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MitovGSMSerialSMSSendMessageFunction : public MitovGSMSerialBasicFunction, public ClockingSupport
	{
		typedef MitovGSMSerialBasicFunction inherited;

	public:
		OpenWire::SourcePin	ReferenceOutputPin;
		OpenWire::SourcePin	FailedOutputPin;

	public:
		String Address;
		String Text;

	protected:
		bool	FEmptyLineDetected = false;
		bool	FTextSent = false;

	public:
		void SetText( String AValue )
		{
			if( Text == AValue )
				return;

			Text = AValue;

			if( ! ClockInputPin.IsConnected() )
				TrySendValue();

		}

	protected:
		bool FLocked = false;

	protected:
		void TrySendValue()
		{
			if( Address == "" )
				return;

			if( Text == "" )
				return;

//			Serial.println( "TEST555" );
			FTextSent = false;

//			FOwner.SendQueryRegisterResponse( this, String( "AT+CMGS=\"" ) + Address + "\"\r\n" + Text + "\x1a"  );
			FOwner.SendQueryRegisterResponse( this, String( "AT+CMGS=\"" ) + Address + "\"" );
//			FOwner.FStream.print( Text );
//			FOwner.FStream.print((char)26);	//the ASCII code of the ctrl+z is 26
		}

	public:
		virtual bool TryProcessPromptInput( bool &ASuccess ) override
		{ 
			if( FTextSent)
				return true;

//			Serial.println( "SEND TEXT!!!" );
			FOwner.FStream.print( Text );
			FOwner.FStream.print((char)26);	//the ASCII code of the ctrl+z is 26
			FTextSent = true;
			ASuccess = true;
			return true; 
		}

		virtual void TimedOut() override
		{
			FailedOutputPin.Notify( nullptr );
		}

		virtual bool TryProcessRequestedInput( String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
			if( FLocked )
			{
				if( FEmptyLineDetected )
				{
					ALine.trim();
					if( ALine != "" )
					{
//						Serial.println( "COMPLETE!!!" );
						ASuccess = ( ALine == "OK" );

						if( !ASuccess )
							FailedOutputPin.Notify( nullptr );
/*
						if( ALine == "OK" )
						{
			//				Serial.println( "OK Processed" );
			//					Serial.println( "ALockInput = false" );
						}
*/
						AResponseCompleted = true;
						FEmptyLineDetected = false;						
						FLocked = false;
						ALockInput = false;
						return true;
					}
				}

				else if( ALine == "" )
				{
//					Serial.println( "FEmptyLineDetected = true" );
					FEmptyLineDetected = true;
					return true;
				}

				return false;
			}

			ALine.trim();
			if( ALine.startsWith( "+CMGS:" ) )
			{
//				Serial.println( "+CMGS: DETECTED!!!" );
				ALine.remove( 0, 6 );
				ALine.trim();
				uint32_t AReference = ALine.toInt();

				ReferenceOutputPin.Notify( &AReference );

				ALockInput = true;
				FLocked = true;
				return true;
			}

/*
			else 
			{
				if( ALine == ">" ) )
				{
				}
			}
*/

			return false;
		}

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			TrySendValue();
		}

		void DoInputChange( void *_Data )
		{
			if( ClockInputPin.IsConnected() )
				return;

			TrySendValue();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
/*
	class MitovGSMSerialSMSSetModeFunction : public MitovGSMSerialBasicExpectOKFunction, public ClockingSupport
	{
		typedef MitovGSMSerialBasicExpectOKFunction inherited;

	public:
		bool	PDUMode : 1;

	protected:
		bool	FEmptyLineDetected : 1;

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
			if( FEmptyLineDetected )
			{
				ALine.trim();
				if( ALine != "" )
				{
					ASuccess = ( ALine == "OK" );
/ *
					if( ALine == "OK" )
					{
		//				Serial.println( "OK Processed" );
		//					Serial.println( "ALockInput = false" );
					}
* /
					AResponseCompleted = true;
					FEmptyLineDetected = false;
					ALockInput = false;
					return true;
				}
			}

			else if( ALine == "" )
			{
				FEmptyLineDetected = true;
				return true;
			}

			return false;
		}

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.SendQueryRegisterResponse( this, String( "AT+CMGF=" ) + ( ( PDUMode ) ? "0" : "1" ) );
		}

	public:
		virtual void ElementSystemStart() override 
		{
//			inherited::SystemStart();
//			Serial.println( "CLOCK!" );
			if( ! ClockInputPin.IsConnected() )
				DoClockReceive( nullptr );

		}

	public:
		MitovGSMSerialSMSSetModeFunction( MitovGSMSerial &AOwner ) :
			inherited( AOwner ),
			PDUMode( false ),
			FEmptyLineDetected( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class MitovGSMSerialSMSGetModeFunction : public MitovGSMSerialBasicFunction, public ClockingSupport
	{
		typedef MitovGSMSerialBasicFunction inherited;

	public:
		OpenWire::SourcePin	InPDUModeOutputPin;

	protected:
		bool	FLocked : 1;
		bool	FEmptyLineDetected : 1;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.SendQueryRegisterResponse( this, "AT+CMGF?" );
		}

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
			if( FLocked )
			{
				if( FEmptyLineDetected )
				{
					ALine.trim();
					if( ALine != "" )
					{
						ASuccess = ( ALine == "OK" );
/ *
						if( ALine == "OK" )
						{
			//				Serial.println( "OK Processed" );
			//					Serial.println( "ALockInput = false" );
						}
* /
						AResponseCompleted = true;
						FEmptyLineDetected = false;
						FLocked = false;
						ALockInput = false;
						return true;
					}
				}

				else if( ALine == "" )
				{
					FEmptyLineDetected = true;
					return true;
				}
			}

			if( ALine.startsWith( "+CMGF:" ) )
			{
				ALine.remove( 0, 6 );
				ALine.trim();
				uint8_t AReference = ALine.toInt();

				InPDUModeOutputPin.SendValue<bool>( AReference == 0 );

				ALockInput = true;
				FLocked = true;
				return true;
			}

			return false;
		}

	public:
		virtual void ElementSystemStart() override
		{
//			inherited::SystemStart();
			if( ! ClockInputPin.IsConnected() )
				DoClockReceive( nullptr );

		}

	public:
		MitovGSMSerialSMSGetModeFunction( MitovGSMSerial &AOwner ) :
			inherited( AOwner ),
			FLocked( false ),
			FEmptyLineDetected( false )
		{
		}

	};
*/
//---------------------------------------------------------------------------
	class MitovGSMSerialSMSBasicMessageFunction : public MitovGSMSerialBasicFunction
	{
		typedef MitovGSMSerialBasicFunction inherited;

	protected:
		bool	FLocked = false;
//		bool	FEmptyLineDetected = false;
		String	FLines;

	protected:
		virtual void ProcessLine( String ALine, bool AIsSecondLine ) {}

	public:
		virtual void ReceivedMessageInfo( String AMessageStatus, String AAddress, String AName, Mitov::TDateTime ADateTime, int32_t ATimeZone ) {}
		virtual void ReceivedMessageText( String AText ) {}

	public:
		virtual bool TryProcessInput( String ALine, bool &ALockInput ) override
		{
			if( FLocked )
			{
/*
				if( FEmptyLineDetected )
				{
//				Serial.println( ALine );
					FEmptyLineDetected = false;
					ProcessLine( FLines, true );
					ALockInput = false;
					FLocked = false;
//					ReceivedMessageText( FLines );

					Serial.print( "LINES: " ); Serial.println( FLines );
					FLines = "";

					return true;
				}

				else if( ALine == "" )
				{
					Serial.println( "FEmptyLineDetected" );
					FEmptyLineDetected = true;
					return true;
				}
*/
				if( ALine == "" )
				{
//					Serial.println( "FEmptyLineDetected" );
//					FEmptyLineDetected = false;
					ProcessLine( FLines, true );
					ALockInput = false;
					FLocked = false;
//					ReceivedMessageText( FLines );

//					Serial.print( "LINES: " ); Serial.println( FLines );
					FLines = "";

					return true;
				}
			}

			String AShortLine = ALine;
			AShortLine.trim();
//			Serial.println( "TEST3333" );
			if( AShortLine.startsWith( "+CMT:" )) //"RDY" )
			{
//				Serial.println( "+CMT: DETECTED!" );
				ProcessLine( ALine, false );

				FLocked = true;
				ALockInput = true;
				return true;
			}

			else if( FLocked )
				FLines += ALine + "\r\n";

			return false;
		}

	public:
		MitovGSMSerialSMSBasicMessageFunction( MitovGSMSerialSMSModule &AOwnerModule );

	};
//---------------------------------------------------------------------------
	class MitovGSMSerialSMSMessageFunction : public MitovGSMSerialSMSBasicMessageFunction
	{
		typedef MitovGSMSerialSMSBasicMessageFunction inherited;

	public:
		OpenWire::SourcePin	OutputPin;
		OpenWire::SourcePin	AddressOutputPin;
		OpenWire::SourcePin	NameOutputPin;
		OpenWire::SourcePin	TimeOutputPin;
		OpenWire::SourcePin	TimeZoneOutputPin;

	protected:
		bool FIsPDU = false;

	public:
		virtual void ReceivedMessageInfo( String AMessageStatus, String AAddress, String AName, Mitov::TDateTime ADateTime, int32_t ATimeZone ) override
		{
			AddressOutputPin.SendValue( AAddress );
			NameOutputPin.SendValue( AName );
			TimeOutputPin.Notify( &ADateTime );
			TimeZoneOutputPin.Notify( &ATimeZone );
		}

		virtual void ReceivedMessageText( String AText ) override
		{
			OutputPin.SendValue( AText );
		}

	public:
		virtual void ProcessLine( String ALine, bool AIsSecondLine ) override
		{
			if( AIsSecondLine )
			{
				if( FIsPDU )
				{
//					Serial.println( ALine );
					// DODO: Decode!
					// http://soft.laogu.com/download/sms_pdu-mode.pdf
					// https://www.diafaan.com/sms-tutorials/gsm-modem-tutorial/online-sms-submit-pdu-decoder/
					// http://jazi.staff.ugm.ac.id/Mobile%20and%20Wireless%20Documents/s_gsm0705pdu.pdf
				}

				else
					OutputPin.SendValue( ALine );
			}

			else
			{
				FIsPDU = false;
				ALine.remove( 0, 5 );
				ALine.trim();
				String AAddressOrNameOrLength;
				if( Func::ExtractOptionallyQuotedCommaText( ALine, AAddressOrNameOrLength ))
				{
//					Serial.println( "TTT1" );
//					Serial.println( AAddressOrName );
					String ANameOrLength;
					if( Func::ExtractOptionallyQuotedCommaText( ALine, ANameOrLength ))
					{
//						Serial.println( "TTT2" );
						String ATimeStamp;
						if( Func::ExtractOptionallyQuotedCommaText( ALine, ATimeStamp ))
						{ 
							// Text Mode
							AddressOutputPin.SendValue( AAddressOrNameOrLength );
							NameOutputPin.SendValue( ANameOrLength );

							Mitov::TDateTime ADateTime;
							int32_t ATimeZone;
							if( GSMFinc::ExtractTimeStamp( ATimeStamp, ADateTime, ATimeZone ))
							{
								TimeOutputPin.Notify( &ADateTime );
								TimeZoneOutputPin.Notify( &ATimeZone );
							}
						}

						else 
						{
//							Serial.println( "YYYYYYYYY" );
							FIsPDU = true;
//							int ALength = ANameOrLength.toInt();
							NameOutputPin.SendValue( AAddressOrNameOrLength );
						}
					}

					else
					{
//						Serial.println( "YYYYYYYYY" );
						FIsPDU = true;
	//					int ALength = ANameOrLength.toInt();
						NameOutputPin.Notify( (void *)"" );
					}
				}
			}
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MitovArduinoGSMSerialDetectDefinedTextFunction : public MitovGSMSerialBasicFunction
	{
		typedef MitovGSMSerialBasicFunction inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		const char *FText;

	public:
		virtual bool TryProcessInput( String ALine, bool &ALockInput ) override
		{
			ALine.trim();
			if( ALine == FText ) //"RDY" )
			{
				OutputPin.Notify( nullptr );
				return true;
			}

			return false;
		}

	public:
		MitovArduinoGSMSerialDetectDefinedTextFunction(  MitovGSMSerial &AOwner, const char *AText ) : 
			inherited( AOwner ),
			FText( AText )
		{
		}

	};
//---------------------------------------------------------------------------
	class MitovGSMSerialSMSMessageReceivedFunctionRequestSMSDetails : public MitovGSMSerialBasicFunction
	{
		typedef MitovGSMSerialBasicFunction inherited;

	protected:
		MitovGSMSerialSMSMessageReceivedFunction &FOwnerFunction;

	protected:
		bool	FLocked = false;
		bool	FEmptyLineDetected = false;
		String	FLines;

	public:
		virtual bool TryProcessRequestedInput( String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override;

	public:
		MitovGSMSerialSMSMessageReceivedFunctionRequestSMSDetails( MitovGSMSerialSMSMessageReceivedFunction &AOwnerFunction );

	};
//---------------------------------------------------------------------------
	class MitovGSMSerialSMSMessageReceivedFunction : public MitovGSMSerialBasicFunction
	{
		typedef MitovGSMSerialBasicFunction inherited;

	public:
		OpenWire::SourcePin	StorageOutputPin;
		OpenWire::SourcePin	IndexOutputPin;
		OpenWire::SourcePin	ReceivedOutputPin;

	public:
		MitovGSMSerialSMSModule &FOwnerModule;

	protected:
		void RequestDetails( int32_t AIndex )
		{
			FOwner.SendQueryRegisterResponse( new MitovGSMSerialSMSMessageReceivedFunctionRequestSMSDetails( *this ), String( "AT+CMGR=" ) + AIndex + String( ",1" ), true );
		}

	public:
		virtual bool TryProcessInput( String ALine, bool &ALockInput ) override
		{
//			Serial.println( "+CMTI: TEST!!!" );
			ALine.trim();
			if( ALine.startsWith( "+CMTI:" ) )
			{
//				Serial.println( "+CMTI: DETECTED!!!" );
//				Serial.println( "ALine.startsWith" );
//				Serial.println( ALine );
				String AStorageType;
				if( Func::ExtractOptionallyQuotedCommaText( ALine, AStorageType ))
				{
					String AIndexText;
					if( Func::ExtractOptionallyQuotedCommaText( ALine, AIndexText ))
					{
						StorageOutputPin.SendValue( AStorageType );

						int32_t	AIndex = AIndexText.toInt();
						IndexOutputPin.Notify( &AIndex );

						RequestDetails( AIndex );

//						ReceivedOutputPin.Notify( nullptr );
					}
				}

				return true;
			}

			return false;
		}

		void ReportMessageDetails()
		{
			ReceivedOutputPin.Notify( nullptr );
		}

	public:
		MitovGSMSerialSMSMessageReceivedFunction( MitovGSMSerialSMSModule &AOwnerModule );

	};
//---------------------------------------------------------------------------
	class MitovGSMSerialSMSModule : public MitovGSMSerialBasicFunction
	{
		typedef MitovGSMSerialBasicFunction inherited;

	public:
		MitovGSMSerialSMSBasicMessageFunction *FMessageInfo = nullptr;

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MitovSIM900ReadADCFunction : public ClockingSupport
	{
		typedef ClockingSupport inherited;

	public:
		OpenWire::TypedSourcePin<float> OutputPin;
		OpenWire::TypedSourcePin<bool>	ErrorOutputPin;

	protected:
		MitovGSMSerial &FOwner;
		bool FStarted : 1;
		bool FErrorStarted : 1;

		TMitovGSMSerialFunctionResponseLineAndStatus FQuery;

	protected:
		bool DoProcessLine( String ALine )
		{
			ALine.trim();
#ifdef __SIMGSM__DEBUG__
			Serial.print( "TESTING: " ); Serial.println( ALine );
#endif
			if( ! ALine.startsWith( "+CADC:" ))
				return false;

			int APos = ALine.indexOf( ",", 6 );
			if( APos >= 0 )
			{
				String ALeft = ALine.substring( 6, APos );
				String ARight = ALine.substring( APos + 1 );
				ALeft.trim();
				ARight.trim();
				int ASuccess = ALeft.toInt();
				float AValue = ARight.toInt();

//					Serial.println( ASuccess );
//					Serial.println( AValue );

				ErrorOutputPin.SetValue( ASuccess != 0, FErrorStarted );
				FErrorStarted = true;

				if( ASuccess )
				{
					AValue /= 2800;
					OutputPin.SetValue( AValue, FStarted );
					FStarted = true;
				}
			}

			return true;
		}

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			if( ! FOwner.IsStarted )
				return;

			FOwner.AbortResponseHandler( &FQuery );
			FOwner.SendQueryRegisterResponse( &FQuery, "AT+CADC?" );
		}

	public:
		MitovSIM900ReadADCFunction( MitovGSMSerial &AOwner ) :
			FOwner( AOwner ),
			FStarted( false ),
			FErrorStarted( false ),
			FQuery( AOwner, this, nullptr, (TOnMitovGSMSerialFunctionLine)&MitovSIM900ReadADCFunction::DoProcessLine )
		{
		}

	};
//---------------------------------------------------------------------------
	class MitovGSMSerialGetSignalStrengthFunction : public MitovGSMSerialBasicFunction, public ClockingSupport
	{
		typedef MitovGSMSerialBasicFunction inherited;

	public:
		OpenWire::TypedSourcePin<float> SignalStrengthOutputPin;
		OpenWire::TypedSourcePin<bool>	SignalStrengthUnknownOutputPin;
		OpenWire::TypedSourcePin<float> BitErrorRateOutputPin;
		OpenWire::TypedSourcePin<bool>	BitErrorRateUnknownOutputPin;

	protected:
		bool FStarted = false;
		TMitovGSMSerialFunctionResponseLineAndStatus FQuery;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
//			Serial.println( "DoClockReceive" );
			FOwner.AbortResponseHandler( &FQuery );
			FOwner.SendQueryRegisterResponse( &FQuery, "AT+CSQ" );
		}

		bool DoProcessLine( String ALine )
		{
			ALine.trim();
			if( ! ALine.startsWith( "+CSQ:" ) )
				return false;
//				Serial.println( "ALine.startsWith" );
//				Serial.println( ALine );

			int APos = ALine.indexOf( ",", 6 );
			if( APos >= 0 )
			{
				String ALeft = ALine.substring( 6, APos );
				String ARight = ALine.substring( APos + 1 );
				ALeft.trim();
				ARight.trim();
				int AStrength = ALeft.toInt();
				int ABitErrorRate = ARight.toInt();

//					Serial.println( AStrength );
//					Serial.println( ABitErrorRate );

				if( AStrength > 31 )
				{
					SignalStrengthOutputPin.SetValue( 0, FStarted );
					SignalStrengthUnknownOutputPin.SetValue( true, FStarted );
				}

				else
				{
					SignalStrengthOutputPin.SetValue( float( AStrength ) / 31, FStarted );
					SignalStrengthUnknownOutputPin.SetValue( false, FStarted );
				}

				if( ABitErrorRate > 31 )
				{
					BitErrorRateOutputPin.SetValue( 0, FStarted );
					BitErrorRateUnknownOutputPin.SetValue( true, FStarted );
				}

				else
				{
					BitErrorRateOutputPin.SetValue( float( ABitErrorRate ) / 7, FStarted );
					BitErrorRateUnknownOutputPin.SetValue( false, FStarted );
				}

				FStarted = true;
			}

			return true;
		}

	public:
		virtual void ElementSystemStart() override
		{
			DoClockReceive( nullptr );
		}

	public:
		MitovGSMSerialGetSignalStrengthFunction( MitovGSMSerial &AOwner ) :
			inherited( AOwner ),
			FQuery( AOwner, (OpenWire::Component *)this, nullptr, MAKE_CALLBACK_3_1( OpenWire::Component, TOnMitovGSMSerialFunctionLine, MitovGSMSerialGetSignalStrengthFunction::DoProcessLine, String ))
		{
		}

	};
//---------------------------------------------------------------------------
	MitovGSMSerialBasicFunction::MitovGSMSerialBasicFunction( MitovGSMSerial &AOwner ) :
		FOwner( AOwner )
	{
//		Serial.println( "MitovGSMSerialBasicFunction" );
		FOwner.AddFunction( this );
	}
//---------------------------------------------------------------------------
	MitovGSMSerialBasicFunction::~MitovGSMSerialBasicFunction()
	{
		FOwner.RemoveFunction( this );
	}
//---------------------------------------------------------------------------
	MitovGSMSerialReceivingVoiceCall::MitovGSMSerialReceivingVoiceCall( MitovGSMSerialVoiceCallFunction &AOwner ) :
		FOwner( AOwner )
	{
		AOwner.FReceivingCall = this;
		AnswerInputPin.SetCallback( &AOwner, (OpenWire::TOnPinReceive)&MitovGSMSerialVoiceCallFunction::DoReceiveAnswer );
	}
//---------------------------------------------------------------------------
	void MitovGSMSerialReceivingVoiceCall::Ringing()
	{
//		Serial.println( "RINGING!!!" );
		CallingOutputPin.Notify( nullptr );
		if( AutoAnswer.Enabled )
		{
			++FRingCount;
			if( FRingCount >= AutoAnswer.NumberRings )
				FOwner.DoReceiveAnswer( nullptr );
		}
	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	MitovGSMSerialSMSMessageReceivedFunctionRequestSMSDetails::MitovGSMSerialSMSMessageReceivedFunctionRequestSMSDetails( MitovGSMSerialSMSMessageReceivedFunction &AOwnerFunction ) : 
		inherited( AOwnerFunction.FOwner ),
		FOwnerFunction( AOwnerFunction )
	{
	}
//---------------------------------------------------------------------------
	bool MitovGSMSerialSMSMessageReceivedFunctionRequestSMSDetails::TryProcessRequestedInput( String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess )
	{
//		Serial.println( "MitovGSMSerialSMSMessageReceivedFunctionRequestSMSDetails::TryProcessRequestedInput" );
//		Serial.println( ALine );
		if( FEmptyLineDetected )
		{
//			Serial.println( "FEmptyLineDetected" );
			ALine.trim();
			if( ALine != "" )
			{
				ASuccess = ( ALine == "OK" );
/*
				if( ALine == "OK" )
				{
//					Serial.println( "OK Processed" );
//						Serial.println( "ALockInput = false" );
				}
*/
				AResponseCompleted = true;
				FEmptyLineDetected = false;
				FLocked = false;
//				Serial.print( "LINES: " ); Serial.println( FLines );
				if( FOwnerFunction.FOwnerModule.FMessageInfo )
					FOwnerFunction.FOwnerModule.FMessageInfo->ReceivedMessageText( FLines );

				FLines = "";
				ALockInput = false;
				return true;
			}
		}

		else if( FLocked && ( ALine == "" ))
		{
//			Serial.println( "FEmptyLineDetected = true" );
			FEmptyLineDetected = true;
			return true;
		}

		else
		{
			String AShortLine = ALine;
			AShortLine.trim();
//			Serial.print( "SHORT_LINE: " ); Serial.println( AShortLine );
			if( ! AShortLine.startsWith( "+CMGR:" ) )
			{
				if( FLocked )
					FLines += ALine + "\r\n";
			}

			else
			{
				FLocked = true;
//				Serial.println( "+CMGR: DETECTED!!!"  );
				ALine.remove( 0, 6 );
				ALine.trim();
				String AMessageStatus;
				if( Func::ExtractOptionallyQuotedCommaText( ALine, AMessageStatus ))
				{
//					Serial.println( AMessageStatus );
					String AAddress;
					if( Func::ExtractOptionallyQuotedCommaText( ALine, AAddress ))
					{
//						Serial.println( AAddress );
						String AName;
						if( Func::ExtractOptionallyQuotedCommaText( ALine, AName ))
						{
//							Serial.println( AName );
							String ATimeStamp;
							if( Func::ExtractOptionallyQuotedCommaText( ALine, ATimeStamp ))
							{ 
								Mitov::TDateTime ADateTime;
								int32_t ATimeZone;
								if( GSMFinc::ExtractTimeStamp( ATimeStamp, ADateTime, ATimeZone ))
								{
									if( FOwnerFunction.FOwnerModule.FMessageInfo )
										FOwnerFunction.FOwnerModule.FMessageInfo->ReceivedMessageInfo( AMessageStatus, AAddress, AName, ADateTime, ATimeZone );
//									TimeOutputPin.Notify( &ADateTime );
//									TimeZoneOutputPin.Notify( &ATimeZone );
								}
							}
						}
					}
				}
			}
		}

		return false;
	}
//---------------------------------------------------------------------------
	MitovGSMSerialSMSMessageReceivedFunction::MitovGSMSerialSMSMessageReceivedFunction( MitovGSMSerialSMSModule &AOwnerModule ) :
		inherited( AOwnerModule.FOwner ),
		FOwnerModule( AOwnerModule )
	{
	}
//---------------------------------------------------------------------------
	MitovGSMSerialSMSBasicMessageFunction::MitovGSMSerialSMSBasicMessageFunction( MitovGSMSerialSMSModule &AOwnerModule ) :
		inherited( AOwnerModule.FOwner )
	{
		AOwnerModule.FMessageInfo = this;
	}
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

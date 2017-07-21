////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_INFRARED_REMOTE_h
#define _MITOV_INFRARED_REMOTE_h

#include <Mitov.h>
#include <IRremote.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class InfraredRemoteReceiver;
//---------------------------------------------------------------------------
	class InfraredRemoteReceiverBasicCommandElementIntf
	{
	public:
		virtual bool TryDecide( decode_results &AResult, uint32_t ALastCode ) = 0;

	public:
		InfraredRemoteReceiverBasicCommandElementIntf( InfraredRemoteReceiver &AOwner );

	};
//---------------------------------------------------------------------------
	template<typename TYPE, int C_TYPE> class InfraredRemoteReceiverTypedCommandElement : public InfraredRemoteReceiverBasicCommandElementIntf
	{
		typedef InfraredRemoteReceiverBasicCommandElementIntf inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		TYPE Value = 0;

	public:
		virtual bool TryDecide( decode_results &AResult, uint32_t ALastCode ) override
		{
			if( AResult.decode_type != C_TYPE )
				return false;

			if( AResult.value != Value )
				return false;

			OutputPin.Notify( nullptr );
			return true;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteReceiverNECCommandElement : public OpenWire::Component, public InfraredRemoteReceiverBasicCommandElementIntf
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::TypedSourcePin<bool>	OutputPin;
		OpenWire::SourcePin	RepeatOutputPin;

	public:
		uint32_t Value = 0;
		uint32_t RepeatInterval = 300;

	protected:
		unsigned long	FLastTime = 0;

	public:
		virtual bool TryDecide( decode_results &AResult, uint32_t ALastCode ) override
		{
			if( AResult.decode_type != NEC )
				return false;

			if(( AResult.value == REPEAT ) && ( ALastCode == Value ))
			{
				FLastTime = millis();
				OutputPin.SetValue( true, true );
				RepeatOutputPin.Notify( nullptr );
				return true;
			}

			if( AResult.value == Value )
			{
				FLastTime = millis();
				OutputPin.SetValue( false, true );
				OutputPin.SetValue( true, true );
				return true;
			}

			return false;
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );
			if( OutputPin.Value )
				if( millis() - FLastTime > RepeatInterval )
					OutputPin.SetValue( false, true );

		}

		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			OutputPin.SetValue( false, false );
		}

	public:
		using InfraredRemoteReceiverBasicCommandElementIntf::InfraredRemoteReceiverBasicCommandElementIntf;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteReceiverPanasonicCommandElement : public InfraredRemoteReceiverBasicCommandElementIntf
	{
		typedef InfraredRemoteReceiverBasicCommandElementIntf inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t Value = 0;
		uint32_t Address = 0;

	public:
		virtual bool TryDecide( decode_results &AResult, uint32_t ALastCode ) override
		{
			if( AResult.decode_type != PANASONIC )
				return false;

			if( AResult.value != Value )
				return false;

			if( AResult.address != Address )
				return false;

			OutputPin.Notify( nullptr );
			return true;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteReceiverAiwaRCT501CommandElement : public InfraredRemoteReceiverBasicCommandElementIntf
	{
		typedef InfraredRemoteReceiverBasicCommandElementIntf inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		int16_t Code = 0;

	public:
		virtual bool TryDecide( decode_results &AResult, uint32_t ALastCode ) override
		{
			if( AResult.decode_type != PANASONIC )
				return false;

			if( AResult.value != Code )
				return false;

			OutputPin.Notify( nullptr );
			return true;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteReceiver : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;
		OpenWire::SourcePin	AddressOutputPin;
		OpenWire::SourcePin	DataTypeOutputPin;
		OpenWire::SourcePin	RawOutputPin;

	public:
		uint32_t	ClearInterval = 1000;
		bool		Enabled : 1;
		bool		ReplaceNECRepeatCode : 1;

	public:
		Mitov::SimpleList<InfraredRemoteReceiverBasicCommandElementIntf *> FElements;

	public:
		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			if( Enabled )
				FSensor.enableIRIn();

		}

	protected:
		static InfraredRemoteReceiver *FInstance;

	public:
		static void GlobalResume()
		{
			if( FInstance )
				if( FInstance->Enabled )
					FInstance->FSensor.enableIRIn();
		}

	protected:
		IRrecv		FSensor;
		uint32_t	FLastCode = 0;
		
	protected:
		unsigned long	FLastTime = 0;

	protected:
		virtual void SystemStart() override
		{
			if( Enabled )
				FSensor.enableIRIn();

//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( FLastCode )
				if( millis() - FLastTime > ClearInterval )
					FLastCode = 0;

			if( Enabled )
			{
				decode_results AResults;
				if ( FSensor.decode( &AResults ))
				{
					uint32_t *ARawData;
					uint32_t ARawDataLength = 0;
					if( RawOutputPin.IsConnected() )
						{
							ARawDataLength = AResults.rawlen;
							if( ARawDataLength )
							{
								ARawData = new uint32_t[ ARawDataLength ];
								for( int i = 0; i < ARawDataLength; ++ i )
									ARawData[ i ] = AResults.rawbuf[ i ];
							}
						}

//					Serial.print( "Len: " ); Serial.println( AResults.rawlen );
					FSensor.resume();

					for( int i = 0; i < FElements.size(); ++i )
						if( FElements[ i ]->TryDecide( AResults, FLastCode ))
							break;

					uint32_t AValue = AResults.value;

					if( AResults.decode_type == NEC )
					{
						if( ReplaceNECRepeatCode )
							if( AValue == REPEAT )
							{
								AValue = FLastCode;
								if( FLastCode != 0 )
									FLastTime = millis();
							}

						if( AValue != REPEAT )
						{
							FLastCode = AValue;
							FLastTime = millis();
						}
					}

					DataTypeOutputPin.SendValue<int32_t>( AResults.decode_type );
					AddressOutputPin.SendValue<uint32_t>( AResults.address );
					if( RawOutputPin.IsConnected() )
						if( ARawDataLength )
						{
							RawOutputPin.SendValue( TArray<uint32_t>( ARawDataLength, ARawData ));
							delete [] ARawData;
						}

					OutputPin.Notify( &AValue );
				}				
			}
				

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		InfraredRemoteReceiver( int APinNumber ) :
			FSensor( APinNumber ),
			Enabled( true ),
			ReplaceNECRepeatCode( true )
		{
			FInstance = this;
		}

	};
//---------------------------------------------------------------------------
	InfraredRemoteReceiver *InfraredRemoteReceiver::FInstance = nullptr;
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitter : public Mitov::CommonSink
	{
		typedef Mitov::CommonSink inherited;

	public:
		IRsend	FSensor;

	protected:
		bool		FSentOneJVC = false;
		uint32_t	FLastValueJVC = 0;

	public:
		bool RepeatedJVCCommand( uint32_t AValue )
		{
			bool AResult = ( FLastValueJVC == AValue );
			FLastValueJVC = AValue;
			FSentOneJVC = true;

			return AResult;
		}

	protected:
		virtual void DoReceive( void *_Data ) override
		{
			TArray<uint32_t> *AData = (TArray<uint32_t>*)_Data;
			unsigned int * code = new unsigned int[ AData->Size ];
			for( int i = 0; i < AData->Size; ++i )
				code[ i ] = AData->Data[ i ];

			FSensor.sendRaw(code, AData->Size, 38 ); // 38KHz
			delete [] code;
		}
	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterBasicSendCommandElement : public OpenWire::Component, public ClockingSupport
	{
		typedef OpenWire::Component inherited;

	protected:
		InfraredRemoteTransmitter	&FOwner;

	public:
		InfraredRemoteTransmitterBasicSendCommandElement( InfraredRemoteTransmitter	&AOwner ) :
			FOwner( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendRC5CommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint32_t	Value = 0;
		uint8_t		NumberBits = 14;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendRC5( Value, NumberBits );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendRC6CommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint32_t	Value = 0;
		uint8_t		NumberBits = 32;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendRC6( Value, NumberBits );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendNECCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint32_t	Value = 0;
		uint8_t		NumberBits = 32;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendNEC( Value, NumberBits );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendSonyCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint32_t	Value = 0;
		uint8_t		NumberBits = 12;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendSony( Value, NumberBits );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendPanasonicCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint32_t	Address = 0;
		uint32_t	Value = 0;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendPanasonic( Address, Value );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendJVCCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint32_t	Value = 0;
		uint8_t		NumberBits = 12;
		bool		UseRepeat = true;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			bool ARepeat = FOwner.RepeatedJVCCommand( Value );
			if( UseRepeat && ARepeat )
				FOwner.FSensor.sendJVC( Value, NumberBits, true );

			else
				FOwner.FSensor.sendJVC( Value, NumberBits, false );

			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendSAMSUNGCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint32_t	Value = 0;
		uint8_t		NumberBits = 32;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendSAMSUNG( Value, NumberBits );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendWhynterCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint32_t	Value = 0;
		uint8_t		NumberBits = 32;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendWhynter( Value, NumberBits );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendAiwaRCT501CommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		int16_t	Code = 0;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendAiwaRCT501( Code );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendLGCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint32_t	Value = 0;
		uint8_t		NumberBits = 28;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendLG( Value, NumberBits );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendDISHCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint32_t	Value = 0;
		uint8_t		NumberBits = 32;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendDISH( Value, NumberBits );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendSharpRawCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint32_t	Value = 0;
		uint8_t		NumberBits = 32;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendSharpRaw( Value, NumberBits );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendSharpCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint32_t	Address = 0;
		uint32_t	Command = 0;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendSharp( Address, Command );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendDenonCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint32_t	Value = 0;
		uint8_t		NumberBits = 32;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendDenon( Value, NumberBits );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendProntoCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		String	Code;
		bool	Repeat = false;
		bool	Fallback = false;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendPronto( (char *)Code.c_str(), Repeat, Fallback );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
/*
	class InfraredRemoteTransmitterSendLEGOPowerFunctionsCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint16_t	Value = 0;
		uint8_t		NumberBits = 32;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendLegoPowerFunctions( Value, NumberBits );
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
*/
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendRawCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		uint16_t *_Values;
		uint32_t _Count;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.FSensor.sendRaw( _Values, _Count, 38 ); // 38KHz
			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class InfraredRemoteTransmitterSendConstRawCommandElement : public InfraredRemoteTransmitterBasicSendCommandElement
	{
		typedef InfraredRemoteTransmitterBasicSendCommandElement inherited;

	public:
		const /*PROGMEM*/ uint16_t *_Values;
		uint32_t _Count;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
//			Serial.println( "TEST" );
			uint16_t *AValues = new uint16_t[ _Count ];
			for( int i = 0; i < _Count; ++i )
			{
				AValues[ i ] = pgm_read_word( _Values + i ) * USECPERTICK;
//				Serial.println( AValues[ i ] );
			}

			FOwner.FSensor.sendRaw( AValues, _Count, 38 ); // 38KHz

			delete [] _Values;

			InfraredRemoteReceiver::GlobalResume();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	InfraredRemoteReceiverBasicCommandElementIntf::InfraredRemoteReceiverBasicCommandElementIntf( InfraredRemoteReceiver &AOwner )
	{
		AOwner.FElements.push_back( this );
	}
//---------------------------------------------------------------------------
}

#endif

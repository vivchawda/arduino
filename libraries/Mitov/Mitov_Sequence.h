////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SEQUENCE_h
#define _MITOV_SEQUENCE_h

#include <Mitov.h>

namespace Mitov
{
	class Sequence;
//---------------------------------------------------------------------------
	class SequenceClockElement : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t Delay;

	protected:
		Sequence &FOwner;

	public:
		void DoClockReceive( void *_Data );

	public:
		SequenceClockElement( Sequence &AOwner, uint32_t ADelay );

	};
//---------------------------------------------------------------------------
	class Sequence : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ConnectSinkPin	StartInputPin;

	public:
		bool	Repeat = false;

	protected:
//		SequenceClockElement *FCurrentElement = nullptr;
		uint16_t	FCurrentIndex = -1;
		uint32_t	FStartTime;

	public:
		SimpleList<SequenceClockElement *, uint16_t>	FElements;

	public:
		void SignalElement( SequenceClockElement *AElement )
		{
			if( FCurrentIndex < 0 )
				return;

			SequenceClockElement *AElementItem = FElements[ FCurrentIndex ];

//			Serial.print( "SignalElement: " );
//			Serial.println( FCurrentIndex );

			if( AElement != AElementItem )
				return;

//			FElements[ FCurrentIndex ]->DoClockReceive( nullptr );
			AElementItem->OutputPin.Notify( nullptr );
			if( ++FCurrentIndex < FElements.size() )
				FStartTime = millis();

			else if( Repeat )
				DoStartReceive( nullptr );

			else
				FCurrentIndex = -1;
		}

	protected:
		void DoStartReceive( void *_Data )
		{
			if( FElements.size() )
			{
				FCurrentIndex = 0;
				FStartTime = millis();
			}
		}

	protected:
		virtual void SystemStart() override
		{
			if( ! StartInputPin.IsConnected() )
				DoStartReceive( nullptr );
//				OutputPin.Notify( &FValue );

//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( FCurrentIndex >= 0 )
				if( ( millis() - FStartTime ) >= FElements[ FCurrentIndex ]->Delay )
					SignalElement( FElements[ FCurrentIndex ] );

/*
			if( FDetectTimerRunning )
				if( ( millis() - FDetectStartTime ) >= PacketDetectInterval )
					TryProcessAcumulattor();

			if( FTimeoutTimerRunning )
				if( ( millis() - FTimeoutStartTime ) >= ResponseTimeout )
				{
					// Timeout
					FDetectTimerRunning = false;

					if( FCurrentTransaction )
						FCurrentTransaction->Release();

					FCurrentTransaction = nullptr;
					FReceivedData.clear();
					TrySend();
				}
*/
//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		Sequence()
		{
			StartInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Sequence::DoStartReceive );
		}

	};
//---------------------------------------------------------------------------
	SequenceClockElement::SequenceClockElement( Sequence &AOwner, uint32_t ADelay ) :
		FOwner( AOwner ),
		Delay( ADelay )
	{
		FOwner.FElements.push_back( this );
		ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&SequenceClockElement::DoClockReceive );
	}
//---------------------------------------------------------------------------
	void SequenceClockElement::DoClockReceive( void *_Data )
	{
		FOwner.SignalElement( this );		
	}
//---------------------------------------------------------------------------
}

#endif

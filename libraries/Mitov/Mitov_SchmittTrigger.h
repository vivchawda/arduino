////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SCHMITT_TRIGGER_h
#define _MITOV_SCHMITT_TRIGGER_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T> class SchmittTrigger : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	InitialValue : 1;
		bool	Inverted : 1;

	protected:
        T	FValue;
		T	FThreshold;
		T	FCurrentValue;

    public:
		void SetValue( T AValue )
		{
			if( FValue == AValue )
				return;

			FValue = AValue;
			ProcessOutput();
		}

		void SetThreshold( T AValue )
		{
			if( FThreshold == AValue )
				return;

			FThreshold = AValue;
			ProcessOutput();
		}

		void SetInverted( bool AValue )
		{
			if( Inverted == AValue )
				return;

			Inverted = AValue;
			ProcessOutput();
		}

	protected:
		void ProcessOutput()
		{
			if( ! Enabled )
				return;

			bool AValue;

			if( Inverted )
			{
				if( InitialValue )
					AValue = ( FCurrentValue < FValue + FThreshold );

				else
					AValue = ( FCurrentValue < FValue - FThreshold );

			}

			else
			{
				if( InitialValue )
					AValue = ( FCurrentValue > FValue - FThreshold );

				else
					AValue = ( FCurrentValue > FValue + FThreshold );

			}

			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			OutputPin.Notify( &AValue );
		}

	protected:
		void DoReceive( void *_Data )
		{
			T AValue = *(T*)_Data;
			if( AValue == FCurrentValue )
				return;

			FCurrentValue = AValue;
			ProcessOutput();
		}

	protected:
		virtual void SystemStart() override
		{
			OutputPin.SendValue<bool>( InitialValue );
//			inherited::SystemStart();
		}

	public:
		SchmittTrigger( T AValue, T AThreshold ) : 
			Enabled( true ),
			InitialValue( false ),
			Inverted( false ),
			FValue( AValue ),
			FThreshold( AThreshold )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&SchmittTrigger::DoReceive );
		}

	};
//---------------------------------------------------------------------------
}

#endif
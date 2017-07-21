////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SETVALUEITEMS_h
#define _MITOV_SETVALUEITEMS_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T> class ArduinoSetValueElement : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;

	protected:
		ValueSource<T> &FOwner;

	public:
		T	Value;

	protected:
		void DoReceived( void *_Data )
		{
			FOwner.ExternalSetValue( nullptr, Value );
		}

	public:
		ArduinoSetValueElement( ValueSource<T> &AOwner, T AValue ) :
			FOwner( AOwner ),
			Value( AValue )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoSetValueElement::DoReceived );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class ArduinoStateSetValueElement : public Mitov::BasicValueStateElement<T>
	{
	public:
		OpenWire::SinkPin	InputPin;

	protected:
		ValueStateSource<T> &FOwner;
		bool	FInputValue = false;

	public:
		T	Value;

	public:
		virtual bool TryActive() override
		{
			if( FInputValue )
			{
				FOwner.ExternalSetValue( nullptr, Value );
				return true;
			}

			return false;
		}

	protected:
		void DoReceived( void *_Data )
		{
			if( FInputValue == *(bool *)_Data )
				return;

//			Serial.println( FInputValue );

			FInputValue = *(bool *)_Data;
			if( FInputValue )
				FOwner.ExternalSetValue( nullptr, Value );

			else
				FOwner.ResetValue();
		}

	public:
		ArduinoStateSetValueElement( ValueStateSource<T> &AOwner, T AValue ) :
			FOwner( AOwner ),
			Value( AValue )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoStateSetValueElement::DoReceived );
			FOwner.RegisterElement( this );
		}

	};
//---------------------------------------------------------------------------    
	class ArduinoRampToValueAnalogElement : public OpenWire::Component, public RampToValueElementIntf
	{
		typedef Mitov::CommonFilter inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		ValueRampSource<float> &FOwner;

	public:
		float	Value = 0.0f;
		float	Slope = 1.0f;

//	protected:
//		unsigned long	FLastTime = 0;

	public:
		virtual void UpdateRamp( float ACurrentValue, unsigned long AOldTime, unsigned long ANewTime ) override
		{
//			Serial.println( "UpdateRamp" );
			if( ACurrentValue == Value )
				FOwner.RegisterForTimeUpdate( this, false );

			else
			{
				float ARamp = abs( ( ANewTime - AOldTime ) * Slope / 1000000 );
				if( ACurrentValue < Value )
				{
					ACurrentValue += ARamp;
//					Serial.println( "UpdateRamp 1" );
					if( ACurrentValue > Value )
						ACurrentValue = Value;

				}
				else
				{
//					Serial.println( "UpdateRamp 2" );
					ACurrentValue -= ARamp;
					if( ACurrentValue < Value )
						ACurrentValue = Value;

				}
				
				FOwner.ExternalSetValue( this, ACurrentValue );
			}
		}

	protected:
		void DoReceived( void *_Data )
		{
//			Serial.println( "DoReceived" );
			FOwner.RegisterForTimeUpdate( this, true );
//			FOwner.ExternalSetValue( this, Value );
		}

	public:
		ArduinoRampToValueAnalogElement( ValueRampSource<float> &AOwner ) :
			FOwner( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoRampToValueAnalogElement::DoReceived );
		}

	};
//---------------------------------------------------------------------------
	class ArduinoStateRampToValueAnalogElement : public Mitov::BasicValueStateElement<float>, public RampToValueElementIntf
	{
		typedef Mitov::BasicValueStateElement<float> inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		ValueRampStateSource<float> &FOwner;
		bool	FInputValue = false;

	public:
		float	Value = 0.0f;
		float	Slope = 1.0f;

	public:
		virtual void UpdateRamp( float ACurrentValue, unsigned long AOldTime, unsigned long ANewTime ) override
		{
//			Serial.println( "UpdateRamp" );
			if( ACurrentValue == Value )
				FOwner.RegisterForTimeUpdate( this, false );

			else
			{
				float ARamp = abs( ( ANewTime - AOldTime ) * Slope / 1000000 );
				if( ACurrentValue < Value )
				{
					ACurrentValue += ARamp;
//					Serial.println( "UpdateRamp 1" );
					if( ACurrentValue > Value )
						ACurrentValue = Value;

				}
				else
				{
//					Serial.println( "UpdateRamp 2" );
					ACurrentValue -= ARamp;
					if( ACurrentValue < Value )
						ACurrentValue = Value;

				}
				
				FOwner.ExternalSetValue( this, ACurrentValue );
			}
		}

	public:
		virtual bool TryActive() override
		{
//			Serial.println( "TryActive" );
			if( FInputValue )
			{
				FOwner.RegisterForTimeUpdate( this, true );
				return true;
			}

			return false;
		}

	protected:
		void DoReceived( void *_Data )
		{
			if( FInputValue == *(bool *)_Data )
				return;

//			Serial.println( FInputValue );

			FInputValue = *(bool *)_Data;
			if( FInputValue )
				FOwner.RegisterForTimeUpdate( this, true );
//				FOwner.ExternalSetValue( nullptr, Value );

			else
			{
				FOwner.RegisterForTimeUpdate( this, false );
				FOwner.ResetValue();
			}
		}

	public:
		ArduinoStateRampToValueAnalogElement( ValueRampStateSource<float> &AOwner ) :
			FOwner( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoStateRampToValueAnalogElement::DoReceived );
			FOwner.RegisterElement( this );
		}

	};
//---------------------------------------------------------------------------
}

#endif

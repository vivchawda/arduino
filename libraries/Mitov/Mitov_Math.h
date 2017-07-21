////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MATH_h
#define _MITOV_MATH_h

#include <Mitov.h>

namespace Mitov
{
#define Min Min
#define Max Max
//---------------------------------------------------------------------------
	#define MITOV_PI 3.14159265359
//---------------------------------------------------------------------------
    template<typename T> class CommonValueMathFilter : public OpenWire::Object
    {
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        T   Value;

    public:
        bool Enabled = true;

	protected:
        virtual T FilterValue( T AValue ) = 0;

	protected:
		virtual void DoReceive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			T AInValue = *(T*)_Data;

            T AOutValue = FilterValue( AInValue );

			OutputPin.Notify( &AOutValue );
        }

    public:
        CommonValueMathFilter( T AValue = 0 ) : 
			Value( AValue )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CommonValueMathFilter::DoReceive );
		}

    };
//---------------------------------------------------------------------------
    template<typename T> class AddValue : public CommonValueMathFilter<T>
	{
		typedef CommonValueMathFilter<T> inherited;

	protected:
        virtual T FilterValue( T AValue )
        {
            return AValue + inherited::Value;
        }

	};
//---------------------------------------------------------------------------
	template<typename T> class SubtractValue : public CommonValueMathFilter<T>
	{
		typedef CommonValueMathFilter<T> inherited;

	protected:
        virtual T FilterValue( T AValue )
        {
            return AValue - inherited::Value;
        }

	public:
		SubtractValue( T AValue = 1 ) : 
			inherited( AValue )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class SubtractFromValue : public CommonValueMathFilter<T>
	{
		typedef CommonValueMathFilter<T> inherited;

	protected:
        virtual T FilterValue( T AValue )
        {
            return inherited::Value - AValue;
        }

	public:
		SubtractFromValue( T AValue = 1 ) : 
			inherited( AValue )
		{
		}
	};
//---------------------------------------------------------------------------
    template<typename T> class MultiplyByValue : public CommonValueMathFilter<T>
	{
		typedef CommonValueMathFilter<T> inherited;

	protected:
        virtual T FilterValue( T AValue )
        {
            return AValue * inherited::Value;
        }

	public:
		MultiplyByValue( T AValue = 1 ) : 
			inherited( AValue )
		{
		}
	};
//---------------------------------------------------------------------------
    template<typename T> class DivideByValue : public CommonValueMathFilter<T>
	{
		typedef CommonValueMathFilter<T> inherited;

	protected:
        virtual T FilterValue( T AValue )
        {
            return AValue / inherited::Value;
        }

	public:
		DivideByValue( T AValue = 1 ) : 
			inherited( AValue )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class DivideValue : public CommonValueMathFilter<T>
	{
		typedef CommonValueMathFilter<T> inherited;

	protected:
        virtual T FilterValue( T AValue )
        {
            return inherited::Value / AValue;
        }

	};
//---------------------------------------------------------------------------
	template<typename T> class Inverse : public CommonTypedFilter<T>
	{
		typedef CommonTypedFilter<T> inherited;

	protected:
        virtual T FilterValue( T AValue )
        {
            return -AValue;
        }

	};
//---------------------------------------------------------------------------
	template<typename T> class Abs : public CommonTypedFilter<T>
	{
		typedef CommonTypedFilter<T> inherited;

	protected:
        virtual T FilterValue( T AValue )
        {
            return abs( AValue );
        }

	};
//---------------------------------------------------------------------------
	class MinLimit : public CommonValueMathFilter<float>
	{
	protected:
        virtual float FilterValue( float AValue )
        {
			return ( AValue < Value ) ? Value : AValue;
        }
	};
//---------------------------------------------------------------------------
	class MaxLimit : public CommonValueMathFilter<float>
	{
		typedef CommonValueMathFilter<float> inherited;

	protected:
        virtual float FilterValue( float AValue )
        {
			return ( AValue > Value ) ? Value : AValue;
        }

	public:
		MaxLimit( float AValue = 1.0 ) :
			inherited( AValue )
		{
		}
	};
//---------------------------------------------------------------------------
	class Limit : public CommonTypedFilter<float>
	{
	public:
		float	Min = 0.0;
		float	Max = 1.0;

	protected:
        virtual float FilterValue( float AValue )
        {
			return ( AValue > Max ) ? 
					Max : 
					( AValue < Min ) ? Min : AValue;
        }

	};
//---------------------------------------------------------------------------
	enum CompareValueType { ctEqual, ctNotEqual, ctBigger, ctSmaller, ctBiggerOrEqual, ctSmallerOrEqual };
//---------------------------------------------------------------------------
	template<typename T> class CompareValue : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        T	Value;

	protected:
		T	FCurrentValue;

    public:
		CompareValueType	CompareType : 3;
        bool				Enabled : 1;
		bool				OnlyChanged : 1;
		bool				IgnoreCase : 1; // Added in root to save space!

	protected:
		bool	FOutputValue : 1;
		bool	FStarted : 1;
		bool	FProcessedOnce : 1;

    public:
		void SetValue( T AValue )
		{
			if( OnlyChanged )
				if( Value == AValue )
					return;

			Value = AValue;
			ProcessOutput();
		}

	protected:
		bool PerformCompare( T ACurrentValue, T AValue )
		{
			switch( CompareType )
			{
			default:
			case ctEqual:
				return ( ACurrentValue == AValue );

			case ctNotEqual:
				return ( ACurrentValue != AValue );

			case ctBigger:
				return ( ACurrentValue > AValue );

			case ctSmaller:
				return ( ACurrentValue < AValue );

			case ctBiggerOrEqual:
				return ( ACurrentValue >= AValue );

			case ctSmallerOrEqual:
				return ( ACurrentValue <= AValue );

			}
		}

	protected:
		void ProcessOutput()
		{
			if( ! Enabled )
				return;

//			Serial.print( FCurrentValue ); Serial.print( " ? " ); Serial.println( Value );

			bool ABoolValue = PerformCompare( FCurrentValue, Value );

			if( ! OnlyChanged )
				if( FOutputValue )
				{
					FOutputValue = false;
					OutputPin.SendValue( false );
				}

			if( ( !FStarted ) || ( FOutputValue != ABoolValue ) )
			{
				OutputPin.Notify( &ABoolValue );
				FOutputValue = ABoolValue;
				FStarted = true;
			}
		}

	protected:
		void DoReceive( void *_Data )
		{
			T AValue = *(T*)_Data;

			if( OnlyChanged )
				if( FProcessedOnce )
					if( AValue == FCurrentValue )
						return;

			FCurrentValue = AValue;
			FProcessedOnce = true;

			ProcessOutput();
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			if( ! FStarted )
				OutputPin.SendValue<bool>( false );

		}

	public:
		CompareValue( T AInitialValue ) : 
			Value( AInitialValue ),
			FCurrentValue( AInitialValue ),
			CompareType( ctEqual ),
			Enabled( true ),
			OnlyChanged( true ),
			IgnoreCase( false ),
			FOutputValue( false ),
			FStarted( false ),
			FProcessedOnce( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CompareValue::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	template<> bool CompareValue<char>::PerformCompare( char ACurrentValue, char AValue )
	{
		if( IgnoreCase )
		{
			ACurrentValue = toupper( ACurrentValue );
			AValue = toupper( AValue );
		}

		switch( CompareType )
		{
			default:
			case ctEqual:
				return ( ACurrentValue == AValue );

			case ctNotEqual:
				return ( ACurrentValue != AValue );

			case ctBigger:
				return ( ACurrentValue > AValue );

			case ctSmaller:
				return ( ACurrentValue < AValue );

			case ctBiggerOrEqual:
				return ( ACurrentValue >= AValue );

			case ctSmallerOrEqual:
				return ( ACurrentValue <= AValue );

		}
	}
//---------------------------------------------------------------------------
	template<> bool CompareValue<String>::PerformCompare( String ACurrentValue, String AValue )
	{
		if( IgnoreCase )
		{
			ACurrentValue.toUpperCase();
			AValue.toUpperCase();
		}

		switch( CompareType )
		{
			default:
			case ctEqual:
				return ( ACurrentValue == AValue );

			case ctNotEqual:
				return ( ACurrentValue != AValue );

			case ctBigger:
				return ( ACurrentValue > AValue );

			case ctSmaller:
				return ( ACurrentValue < AValue );

			case ctBiggerOrEqual:
				return ( ACurrentValue >= AValue );

			case ctSmallerOrEqual:
				return ( ACurrentValue <= AValue );

		}
	}
//---------------------------------------------------------------------------
	template<> void CompareValue<String>::DoReceive( void *_Data )
	{
		String AValue = String( (char*)_Data );

		if( OnlyChanged )
			if( FProcessedOnce )
				if( AValue == FCurrentValue )
					return;

		FCurrentValue = AValue;
		FProcessedOnce = true;

		ProcessOutput();
	}
//---------------------------------------------------------------------------
	class CompareDateTimeValue : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        Mitov::TDateTime	Value;

	protected:
		Mitov::TDateTime	FCurrentValue;

    public:
		TDateTimeCompareItems	Items;

    public:
		CompareValueType	CompareType : 3;
        bool				Enabled : 1;
		bool				OnlyChanged : 1;
		bool				IgnoreCase : 1; // Added in root to save space!

	protected:
		bool	FOutputValue : 1;
		bool	FStarted : 1;
		bool	FProcessedOnce : 1;

    public:
		void SetValue( Mitov::TDateTime AValue )
		{
			if( OnlyChanged )
				if( Value == AValue )
					return;

			Value = AValue;
			ProcessOutput();
		}

	protected:
		bool PerformCompare( Mitov::TDateTime ACurrentValue, Mitov::TDateTime AValue )
		{
			switch( CompareType )
			{
			default:
			case ctEqual:
				return ( ACurrentValue.IsEqual( AValue, Items ) );

			case ctNotEqual:
				return ( ! ACurrentValue.IsEqual( AValue, Items ) );

			case ctBigger:
				return ( ACurrentValue.IsBigger( AValue, Items ) );

			case ctSmaller:
				return ( AValue.IsBigger( ACurrentValue, Items ) );

			case ctBiggerOrEqual:
				return ( ACurrentValue.IsBiggerOrEqual( AValue, Items ) );

			case ctSmallerOrEqual:
				return ( AValue.IsBiggerOrEqual( ACurrentValue, Items ) );

			}
		}

	protected:
		void ProcessOutput()
		{
			if( ! Enabled )
				return;

//			Serial.print( FCurrentValue ); Serial.print( " ? " ); Serial.println( Value );

			bool ABoolValue = PerformCompare( FCurrentValue, Value );

			if( ! OnlyChanged )
				if( FOutputValue )
				{
					FOutputValue = false;
					OutputPin.SendValue( false );
				}

			if( ( !FStarted ) || ( FOutputValue != ABoolValue ) )
			{
				OutputPin.Notify( &ABoolValue );
				FOutputValue = ABoolValue;
				FStarted = true;
			}
		}

	protected:
		void DoReceive( void *_Data )
		{
			Mitov::TDateTime AValue = *(Mitov::TDateTime*)_Data;

			if( OnlyChanged )
				if( FProcessedOnce )
					if( AValue == FCurrentValue )
						return;

			FCurrentValue = AValue;
			FProcessedOnce = true;

			ProcessOutput();
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			if( ! FStarted )
				OutputPin.SendValue<bool>( false );

		}

	public:
		CompareDateTimeValue() : 
			CompareType( ctEqual ),
			Enabled( true ),
			OnlyChanged( true ),
			IgnoreCase( false ),
			FOutputValue( false ),
			FStarted( false ),
			FProcessedOnce( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CompareDateTimeValue::DoReceive );
		}

	};
//---------------------------------------------------------------------------
/*
	class CompareStringValue : public CompareValue<String>
	{
		typedef CompareValue<String> inherited;

	protected:
		virtual bool PerformCompare( String ACurrentValue, String AValue ) override
		{
			if( IgnoreCase )
			{
				ACurrentValue.toUpperCase();
				AValue.toUpperCase();
			}

			return inherited::PerformCompare( ACurrentValue, AValue );
		}

	protected:
		virtual void DoReceive( void *_Data ) override
		{
			String AValue = String( (char*)_Data );

			if( OnlyChanged )
				if( FProcessedOnce )
					if( AValue == FCurrentValue )
						return;

			FCurrentValue = AValue;
			FProcessedOnce = true;

			ProcessOutput();
		}

	public:
		CompareStringValue() : 
			inherited( String() )
		{
		}
	};
*/
//---------------------------------------------------------------------------
	class AveragePeriod : public CommonFilter
	{
	public:
		unsigned long	Period = 1000;

	protected:
		float FSum = 0.0f;
		unsigned long	FCount = 0;
		unsigned long	FLastTime = 0;

	protected:
		virtual void DoReceive( void *_Data ) override
		{
			FSum += *(float *)_Data;
			++FCount;
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( currentMicros - FLastTime < Period )
				return;

			if( ! FCount )
				return;

			float AValue = FSum / FCount;

			FSum = 0.0;
			FCount = 0;
			FLastTime = currentMicros;

			OutputPin.Notify( &AValue );
		}

	};
//---------------------------------------------------------------------------
	template<typename T, int C_NUM_INPUTS> class CommonMathMultiInput : public CommonClockedMultiInput<T, C_NUM_INPUTS>
	{
		typedef CommonClockedMultiInput<T, C_NUM_INPUTS> inherited;

    public:
        bool Enabled = true;

	};
//---------------------------------------------------------------------------
	template<typename T, int C_NUM_INPUTS> class Add : public CommonMathMultiInput<T, C_NUM_INPUTS>
	{
		typedef CommonMathMultiInput<T, C_NUM_INPUTS> inherited;

	protected:
		virtual T CalculateOutput() override
		{
			T AValue = 0;
			for( int i = 0; i < C_NUM_INPUTS; ++i )
				AValue += inherited::InputPins[ i ].Value;

			return AValue;
		}
	};
//---------------------------------------------------------------------------
	template<typename T, int C_NUM_INPUTS> class Multiply : public CommonMathMultiInput<T, C_NUM_INPUTS>
	{
		typedef CommonMathMultiInput<T, C_NUM_INPUTS> inherited;

	protected:
		virtual T CalculateOutput() override
		{
			T AValue = 1;
			for( int i = 0; i < C_NUM_INPUTS; ++i )
				AValue *= inherited::InputPins[ i ].Value;

			return AValue;
		}
	};
//---------------------------------------------------------------------------
	class ValueRange
	{
	public:
		float Min = 0.0f;
		float Max = 1.0f;

	};
//---------------------------------------------------------------------------
	class MapRange : public CommonTypedFilter<float>
	{
		typedef CommonTypedFilter<float> inherited;

	public:
		ValueRange	InputRange;
		ValueRange	OutputRange;

	protected:
        virtual float FilterValue( float AValue ) override
        {
			return Func::MapRange( AValue, InputRange.Min, InputRange.Max, OutputRange.Min, OutputRange.Max );
//			return (( AValue - InputRange.Min ) * (OutputRange.Max - OutputRange.Min) / (InputRange.Max - InputRange.Min)) + OutputRange.Min;
        }
	};
//---------------------------------------------------------------------------
	class AnalogDeadZone : public CommonTypedFilter<float>
	{
		typedef CommonTypedFilter<float> inherited;

	public:
		float Value = 0.5f;
		float Span = 0.1f;

	protected:
        virtual float FilterValue( float AValue ) override
        {
			float AFalfSpan = Span / 2;
			if( AValue > Value + AFalfSpan )
				return AValue - AFalfSpan;

			if( AValue < Value - AFalfSpan )
				return AValue + AFalfSpan;

			return Value;
        }
	};
//---------------------------------------------------------------------------
	class AnalogDeadZoneScaled : public CommonTypedFilter<float>
	{
		typedef CommonTypedFilter<float> inherited;

	public:
		ValueRange	InputRange;
		ValueRange	OutputRange;
		float Value = 0.5f;
		float Span = 0.1f;

	protected:
        virtual float FilterValue( float AValue ) override
        {
			float AFalfSpan = Span / 2;

			if( AValue > Value + AFalfSpan )
				AValue -= AFalfSpan;

			else if( AValue < Value - AFalfSpan )
				AValue += AFalfSpan;

			else
				AValue = Value;

			return Func::MapRange( AValue, InputRange.Min + AFalfSpan, InputRange.Max - AFalfSpan, OutputRange.Min, OutputRange.Max );
        }
	};
//---------------------------------------------------------------------------
    template<typename T> class RaiseToPower : public CommonValueMathFilter<T>
	{
		typedef CommonValueMathFilter<T> inherited;

	protected:
        virtual T FilterValue( T AValue ) override
        {
            return pow( AValue, inherited::Value );
        }

	public:
		RaiseToPower( T AValue = 1 ) : 
			inherited( AValue )
		{
		}
	};
//---------------------------------------------------------------------------
	class Sine : public CommonTypedFilter<float>
	{
		typedef CommonTypedFilter<float> inherited;

	protected:
        virtual float FilterValue( float AValue ) override
        {
            return sin( AValue );
        }

	};
//---------------------------------------------------------------------------
	class Cosine : public CommonTypedFilter<float>
	{
		typedef CommonTypedFilter<float> inherited;

	protected:
        virtual float FilterValue( float AValue ) override
        {
            return cos( AValue );
        }

	};
//---------------------------------------------------------------------------
	class RadToDegrees : public CommonTypedFilter<float>
	{
		typedef CommonTypedFilter<float> inherited;

	protected:
        virtual float FilterValue( float AValue ) override
        {
            return AValue * 180 / MITOV_PI;
        }

	};
//---------------------------------------------------------------------------
	class DegreesToRad : public CommonTypedFilter<float>
	{
		typedef CommonTypedFilter<float> inherited;

	protected:
        virtual float FilterValue( float AValue ) override
        {
            return AValue * MITOV_PI / 180;
        }

	};
//---------------------------------------------------------------------------
    class AndUnsignedValue : public CommonValueMathFilter<uint32_t>
	{
		typedef CommonValueMathFilter<uint32_t> inherited;

	protected:
        virtual uint32_t FilterValue( uint32_t AValue ) override
        {
            return AValue & Value;
        }

	};
//---------------------------------------------------------------------------
    class OrUnsignedValue : public CommonValueMathFilter<uint32_t>
	{
		typedef CommonValueMathFilter<uint32_t> inherited;

	protected:
        virtual uint32_t FilterValue( uint32_t AValue ) override
        {
            return AValue | Value;
        }

	};
//---------------------------------------------------------------------------
    class XorUnsignedValue : public CommonValueMathFilter<uint32_t>
	{
		typedef CommonValueMathFilter<uint32_t> inherited;

	protected:
        virtual uint32_t FilterValue( uint32_t AValue ) override
        {
            return AValue ^ Value;
        }

	};
//---------------------------------------------------------------------------
    class NotUnsignedValue : public CommonTypedFilter<uint32_t>
	{
		typedef CommonTypedFilter<uint32_t> inherited;

	protected:
        virtual uint32_t FilterValue( uint32_t AValue ) override
        {
            return ~AValue;
        }

	};
//---------------------------------------------------------------------------
	template<typename T> class Accumulate : public CommonTypedFilter<T>
	{
		typedef CommonTypedFilter<T> inherited;

	public:
		T	InitialValue = 0;

	public:
		void SetInitialValue( float AValue )
		{
			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			inherited::OutputPin.Notify( &InitialValue );
		}

	protected:
		bool			FHasTime = false;
		unsigned long	FLastTime = 0;

	protected:
        virtual T FilterValue( T AValue ) override
        {
			unsigned long ATime = micros();
			if( ! FHasTime )			
			{
				FHasTime = true;
				return InitialValue;
			}

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			InitialValue += AValue * dT;
            return InitialValue;
        }

	};
//---------------------------------------------------------------------------
	class AccumulateAngle : public Accumulate<float>
	{
	public:
		TAngleUnits	Units = auDegree;

	protected:
        virtual float FilterValue( float AValue ) override
        {
			if( ! FHasTime )			
			{
				FHasTime = true;
				return InitialValue;
			}

			unsigned long ATime = micros();
			float dT = ( ATime - FLastTime ) / 1000000.0f;
			InitialValue += AValue * dT;
			switch( Units )
			{
				case auDegree:
					InitialValue = posmod( InitialValue, 360.0f );
					break;

				case auRadians:
					InitialValue = posmod( InitialValue, 2 * PI );
					break;

				case auNormalized:
					InitialValue = posmod( InitialValue, 1.0f );
					break;

			}

            return InitialValue;
        }
	};
//---------------------------------------------------------------------------
#undef Min
#undef Max
}

#endif

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TEXT_h
#define _MITOV_TEXT_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class TextStateValue;
//---------------------------------------------------------------------------
	class TextValue : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		String Value;

	public:
		virtual void ExternalSetValue( String AValue )
		{
			if( Value == AValue )
				return;

			Value = AValue;
			OutputPin.Notify( (void *)Value.c_str() );
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			if( ! ClockInputPin.IsConnected() )
				OutputPin.Notify( (void *)Value.c_str() );

		}

		virtual void DoClockReceive( void *_Data ) override
		{
			OutputPin.Notify( (void *)Value.c_str() );
		}

	public:
		TextValue( char *AValue ) :
			Value( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	class BindableTextValue : public TextValue
	{
		typedef TextValue inherited;

	protected:
		String OldValue = inherited::Value;

	protected:
		virtual void SystemInit()
		{
			inherited::SystemInit();
			OldValue = inherited::Value;
		}

		virtual void SystemLoopBegin( unsigned long currentMicros )
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			inherited::OutputPin.Notify( (void *)OldValue.c_str() );
		}

	public:
		BindableTextValue( char *AValue ) :
			inherited( AValue ),
			OldValue( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	class ArduinoSetTextValueElement : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;

	protected:
		TextValue &FOwner;

	public:
		String	Value;

	protected:
		void DoReceived( void *_Data )
		{
			FOwner.ExternalSetValue( Value );
		}

	public:
		ArduinoSetTextValueElement( TextValue &AOwner ) :
			FOwner( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoSetTextValueElement::DoReceived );
		}

	};
//---------------------------------------------------------------------------
	class ArduinoStateSetTextValueElement : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;

	protected:
		TextStateValue &FOwner;
		bool	FInputValue = false;

	public:
		String	Value;

	public:
		bool TryActive();

	protected:
		void DoReceived( void *_Data );

	public:
		ArduinoStateSetTextValueElement( TextStateValue &AOwner );

	};
//---------------------------------------------------------------------------
	class TextStateValue : public TextValue
	{
		typedef Mitov::TextValue inherited;

	protected:
		String FCurrentValue;

	protected:
		Mitov::SimpleList<ArduinoStateSetTextValueElement *>	FElements;

	public:
		virtual void ExternalSetValue( String AValue ) override
		{
			if( FCurrentValue == AValue )
				return;

			FCurrentValue = AValue;
			OutputPin.Notify( (void *)FCurrentValue.c_str() );
		}

		void ResetValue()
		{
			for( int i = FElements.size(); i -- ; )
				if( FElements[ i ]->TryActive() )
					return;

			FCurrentValue = Value;
			OutputPin.Notify( (void *)FCurrentValue.c_str() );
		}

	public:
		void RegisterElement( ArduinoStateSetTextValueElement *AValue )
		{
			FElements.push_back( AValue );
		}

	protected:
		virtual void SystemStart() override
		{
			FCurrentValue = Value;
			inherited::SystemStart();
//			if( ! ClockInputPin.IsConnected() )
//				OutputPin.Notify( (void *)FCurrentValue.c_str() );

		}

		virtual void DoClockReceive( void *_Data ) override
		{
			OutputPin.Notify( (void *)FCurrentValue.c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class BindableTextStateValue : public TextStateValue
	{
		typedef TextValue inherited;

	protected:
		String OldValue;

	protected:
		virtual void SystemInit()
		{
			inherited::SystemInit();
			OldValue = inherited::Value;
		}

		virtual void SystemLoopBegin( unsigned long currentMicros )
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			ResetValue();
//			inherited::OutputPin.Notify( (void *)OldValue.c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class BasicFormattedText;
//---------------------------------------------------------------------------
	class FormattedTextElementBasic : public OpenWire::Object // : public OpenWire::Component
	{
//		typedef OpenWire::Component inherited;
	protected:
		BasicFormattedText &FOwner;

	public:
		virtual String GetText() = 0;

		virtual void ClearModified() {}
		virtual bool GetModified() { return false; }
		virtual void SystemStart()
		{
		}

	public:
		FormattedTextElementBasic( BasicFormattedText &AOwner );

	};
//---------------------------------------------------------------------------
	class BasicFormattedText : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::SimpleList<FormattedTextElementBasic *>	FElements;

	};
//---------------------------------------------------------------------------
	template <typename T_STRING> class BasicTypedFormattedText : public Mitov::BasicFormattedText
	{
		typedef Mitov::BasicFormattedText inherited;

	protected:
		struct TStringItem
		{
			T_STRING	Text;
			FormattedTextElementBasic *Element;
		};

	protected:
		Mitov::SimpleList<TStringItem *>	FReadyElements;

	public:
		void AddReadyElement( T_STRING ATextItem, int AIndex )
		{
//			Serial.print( "AddElement: " ); Serial.print( ATextItem ); Serial.println( AIndex );

			TStringItem	*AItem = new TStringItem;
			AItem->Text = ATextItem;
//			Serial.print( "AddElement: " ); Serial.println( AItem->Text );
			if( AIndex < FElements.size() )
				AItem->Element = FElements[ AIndex ];

			else
				AItem->Element = NULL;

			FReadyElements.push_back( AItem );

//			Serial.println( FReadyElements[ FReadyElements.size() - 1 ]->Text );
//			Serial.println( "DEBUG>>" );
//			for( Mitov::SimpleList<TStringItem *>::iterator Iter = FReadyElements.begin(); Iter != FReadyElements.end(); ++Iter )
//				Serial.println( ( *Iter )->Text );

//			Serial.println( "<<DEBUG" );
		}

	protected:
		void ProcessSendOutput()
		{
//			Serial.println( "ProcessSendOutput" );
			String AText;
			for( int i = 0; i < FReadyElements.size(); ++i )
			{
//				Serial.println( ( *Iter )->Text );
				AText += FReadyElements[ i ]->Text;
				FormattedTextElementBasic *AElemen = FReadyElements[ i ]->Element;
				if( AElemen )
				{
					AText += AElemen->GetText();
					AElemen->ClearModified();
				}
			}

//			Serial.println( AText );
			inherited::OutputPin.Notify( (void *)AText.c_str() );
		}

	protected:
		virtual void SystemLoopEnd() override
		{
			bool AModified = false;
			for( int i = 0; i < FReadyElements.size(); ++i )
			{
				FormattedTextElementBasic *AElemen = FReadyElements[ i ]->Element;
				if( AElemen )
					if( AElemen->GetModified() )
						AModified= true;
			}

			if( AModified )
				if( ! ClockInputPin.IsConnected() )
					ProcessSendOutput();

			inherited::SystemLoopEnd();
		}

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			ProcessSendOutput();
		}

	};
//---------------------------------------------------------------------------
	class FormattedText_Fixed : public Mitov::BasicTypedFormattedText<char *>
	{
		typedef Mitov::BasicTypedFormattedText<char *> inherited;

	public:
		void AddNullElement( const char *ATextItem )
		{
			TStringItem	*AItem = new TStringItem;

			AItem->Text = (char *)ATextItem;
			AItem->Element = NULL;

			inherited::FReadyElements.push_back( AItem );
		}

	protected:
		virtual void SystemStart() override
		{
			for( int i = 0; i < FElements.size(); ++i )
				FElements[ i ]->SystemStart();
			
			inherited::SystemStart();
			ProcessSendOutput();
		}

	};
//---------------------------------------------------------------------------
	class FormattedText : public Mitov::BasicTypedFormattedText<String>
	{
		typedef Mitov::BasicTypedFormattedText<String> inherited;

	public:
		String Text;

	protected:
		void InitElements()
		{
			FReadyElements.clear();
			String	ATextItem;
			String	AIndexText;
			bool	AInEscape = false;

//			Serial.println( "INIT" );
//			Serial.println( Text );
//			delay( 1000 );

			for( unsigned int i = 0; i < Text.length(); ++ i )
			{
				char AChar = Text[ i ];
				if( AInEscape )
				{
					if( AChar >= '0' && AChar <= '9' )
						AIndexText += AChar;

					else
					{
						if( AChar == '%' )
						{
							if( AIndexText.length() == 0 )
								ATextItem += '%';

							else
							{
								AddReadyElement( ATextItem, AIndexText.toInt() );
								ATextItem = "";
							}

						}

						else
						{
							if( AIndexText.length() == 0 )
								ATextItem += '%';

							else
							{
								AddReadyElement( ATextItem, AIndexText.toInt() );
								ATextItem = "";
							}

							ATextItem += AChar;
						}

						AInEscape = false;
					}
				}

				else
				{
					if( AChar == '%' )
					{
						AInEscape = true;
						AIndexText = "";
					}

					else
						ATextItem += AChar;

				}

			}

			if( AInEscape )
				AddReadyElement( ATextItem, AIndexText.toInt() );

			else if( ATextItem.length() )
			{
				TStringItem	*AItem = new TStringItem;

				AItem->Text = ATextItem;
				AItem->Element = NULL;

				FReadyElements.push_back( AItem );
			}

//			Serial.println( "DEBUG>>" );
//			for( Mitov::SimpleList<TStringItem *>::iterator Iter = FReadyElements.begin(); Iter != FReadyElements.end(); ++Iter )
//				Serial.println( ( *Iter )->Text );

//			Serial.println( "<<DEBUG" );
		}

	protected:
		virtual void SystemStart() override
		{
			for( int i = 0; i < FElements.size(); ++i )
				FElements[ i ]->SystemStart();
			
			InitElements();
			inherited::SystemStart();
			ProcessSendOutput();
		}

	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD *T_LCD_INSTANCE, typename T> class TextFormatElementInput : public Mitov::CommonSink
	{
		typedef Mitov::CommonSink	inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
		{
			T_LCD_INSTANCE->SetValue( String( *(T*)_Data ));
		}
	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD *T_LCD_INSTANCE> class TextFormatElementStringInput : public Mitov::CommonSink
	{
		typedef Mitov::CommonSink	inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
		{
			T_LCD_INSTANCE->SetValue( (char*)_Data );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD *T_LCD_INSTANCE> class TextFormatElementClockInput : public Mitov::CommonSink
	{
		typedef Mitov::CommonSink	inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
		{
			T_LCD_INSTANCE->SetValue( "(Clock)" );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD *T_LCD_INSTANCE, typename T_OBJECT> class TextFormatElementObjectInput : public Mitov::CommonSink
	{
		typedef Mitov::CommonSink	inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
		{
			T_LCD_INSTANCE->SetValue( ((T_OBJECT *)_Data)->ToString() );
		}
	};
//---------------------------------------------------------------------------
	class FormattedTextElementText : public FormattedTextElementBasic
	{
		typedef Mitov::FormattedTextElementBasic inherited;

	public:
		String	InitialValue;
		String	FValue;
		bool	FModified;

	public:
		void SetValue( String AValue )
		{
			FModified = true;
			FValue = AValue;
		}

	public:
		virtual String GetText()
		{
			return FValue;
		}

		virtual void ClearModified() override
		{
			FModified = false;
		}

		virtual bool GetModified() override
		{ 
			return FModified;
		}

	public:
		virtual void SystemInit()
		{
//			inherited::SystemStart();
			FValue = InitialValue;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T> class FormattedTextElementTyped : public FormattedTextElementBasic
	{
		typedef Mitov::FormattedTextElementBasic inherited;

	public:
		T	InitialValue;

	public:
		OpenWire::ValueSimpleModifiedSinkPin<T>	InputPin;

		virtual void ClearModified() override
		{
			InputPin.Modified = false;
		}

		virtual bool GetModified() override
		{ 
			return InputPin.Modified;
		}

	public:
		virtual void SystemInit()
		{
//			inherited::SystemStart();
			InputPin.Value = InitialValue;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T> class FormattedTextLengthElementTyped : public T
	{
		typedef T inherited;

	public:
		uint8_t	Length = 0;
		char	FillCharacter = ' ';

	public:
		virtual String GetText() override
		{
			String AText = inherited::GetText();
			for( int i = AText.length(); i < Length; ++i )
				AText = FillCharacter + AText;

			return AText;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class FormattedTextElementInteger : public Mitov::FormattedTextElementTyped<int32_t>
	{
		typedef Mitov::FormattedTextElementTyped<int32_t> inherited;

	public:
		uint8_t	Base = 10;

	public:
		virtual String GetText() override
		{
			char AText[ 16 ];
			itoa( InputPin.Value, AText, Base );

			return AText;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class FormattedTextElementUnsigned : public Mitov::FormattedTextElementTyped<uint32_t>
	{
		typedef Mitov::FormattedTextElementTyped<uint32_t> inherited;

	public:
		uint8_t	Base = 10;

	public:
		virtual String GetText() override
		{
			char AText[ 16 ];
			itoa( InputPin.Value, AText, Base );

			return AText;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class FormattedTextElementAnalog : public Mitov::FormattedTextElementTyped<float>
	{
		typedef Mitov::FormattedTextElementTyped<float> inherited;

	public:
		uint8_t	MinWidth = 1;
		uint8_t	Precision = 3;

	public:
		virtual String GetText() override
		{
			char AText[ 16 ];
			dtostrf( InputPin.Value,  MinWidth, Precision, AText );

			return AText;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class FormattedTextElementDigital : public Mitov::FormattedTextElementTyped<bool>
	{
		typedef Mitov::FormattedTextElementTyped<bool> inherited;

	public:
		String	TrueValue = "true";
		String	FalseValue = "false";

	public:
		virtual String GetText() override
		{
			if( InputPin.Value )
				return TrueValue;

			return FalseValue;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class TextLength : public CommonFilter
	{
		typedef CommonFilter inherited;

    public:
        bool Enabled = true;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
                return;

			int32_t AValue = strlen( (char *)_Data );

			inherited::OutputPin.Notify( &AValue );
        }
	};
//---------------------------------------------------------------------------
	class CommonTextMeasureFilter : public CommonFilter
	{
		typedef CommonFilter inherited;

    public:
        bool Enabled : 1;

	public:
		bool	IgnoreCase : 1;
		String	Text;

	public:
		CommonTextMeasureFilter() :
			Enabled( true ),
			IgnoreCase( false )
		{
		}

	};

//---------------------------------------------------------------------------
	class FindTextPosition : public CommonTextMeasureFilter
	{
		typedef CommonTextMeasureFilter inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
                return;

			String		AText = Text;
			String		ATextValue = String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			int32_t AValue = ATextValue.indexOf( AText );

			inherited::OutputPin.Notify( &AValue );
        }
	};
//---------------------------------------------------------------------------
	class FindLastTextPosition : public CommonTextMeasureFilter
	{
		typedef CommonTextMeasureFilter inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
                return;

			String		AText = Text;
			String		ATextValue = String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			int32_t AValue = ATextValue.lastIndexOf( AText );

			inherited::OutputPin.Notify( &AValue );
        }
	};
//---------------------------------------------------------------------------
	class ContainsText : public CommonTextMeasureFilter
	{
		typedef CommonTextMeasureFilter inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
                return;

			String		AText = Text;
			String		ATextValue = String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			bool AValue = ( ATextValue.indexOf( AText ) >= 0 );

			inherited::OutputPin.Notify( &AValue );
        }
	};
//---------------------------------------------------------------------------
	class TextStartsWith : public CommonTextMeasureFilter
	{
		typedef CommonTextMeasureFilter inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
                return;

			String		AText = Text;
			String		ATextValue = String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			bool AValue = ATextValue.startsWith( AText );

			inherited::OutputPin.Notify( &AValue );
        }
	};
//---------------------------------------------------------------------------
	class TextEndsWith : public CommonTextMeasureFilter
	{
		typedef CommonTextMeasureFilter inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
                return;

			String		AText = Text;
			String		ATextValue = String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			bool AValue = ATextValue.endsWith( AText );

			inherited::OutputPin.Notify( &AValue );
        }
	};
//---------------------------------------------------------------------------
	class CommonTextLengthFilter : public CommonFilter
	{
    public:
        bool Enabled = true;

	public:
		uint32_t	Length = 1;

	};
//---------------------------------------------------------------------------
	class CommonTextPositionLengthFilter : public CommonTextLengthFilter
	{
	public:
		uint32_t	Position = 0;

	};
//---------------------------------------------------------------------------
	class SubText : public CommonTextPositionLengthFilter
	{
		typedef CommonTextPositionLengthFilter inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
                return;

			String AValue = String( (char *)_Data ).substring( Position, Position + Length );

			inherited::OutputPin.SendValue( AValue );
        }
	};
//---------------------------------------------------------------------------
	class LeftSubText : public CommonTextLengthFilter
	{
		typedef CommonTextLengthFilter inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
                return;

			String AValue = String( (char *)_Data ).substring( 0, Length );

			inherited::OutputPin.SendValue( AValue );
        }
	};
//---------------------------------------------------------------------------
	class RightSubText : public CommonTextLengthFilter
	{
		typedef CommonTextLengthFilter inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
                return;

			String AValue = String( (char *)_Data );
			AValue = AValue.substring( AValue.length() - Length );

			inherited::OutputPin.SendValue( AValue );
        }
	};
//---------------------------------------------------------------------------
	class DeleteSubText : public CommonTextPositionLengthFilter
	{
		typedef CommonTextPositionLengthFilter inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
                return;

			String AValue = String( (char *)_Data );
			AValue.remove( Position, Length );

			inherited::OutputPin.SendValue( AValue );
        }
	};
//---------------------------------------------------------------------------
	class DeleteLeftSubText : public CommonTextLengthFilter
	{
		typedef CommonTextLengthFilter inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
                return;

			String AValue = String( (char *)_Data );
			AValue.remove( 0, Length );

			inherited::OutputPin.SendValue( AValue );
        }
	};
//---------------------------------------------------------------------------
	class DeleteRightSubText : public CommonTextLengthFilter
	{
		typedef CommonTextLengthFilter inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
                return;

			String AValue = String( (char *)_Data );
			AValue.remove( AValue.length() - Length );

			inherited::OutputPin.SendValue( AValue );
        }
	};
//---------------------------------------------------------------------------
	class ToUpperCase : public CommonFilter	
	{
		typedef CommonFilter inherited;

    public:
        bool Enabled = true;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
            {
			    inherited::OutputPin.Notify( _Data );
                return;
            }

			String AValue = String( (char *)_Data );
			AValue.toUpperCase();

			inherited::OutputPin.SendValue( AValue );
        }
	};
//---------------------------------------------------------------------------
	class ToLowerCase : public CommonFilter	
	{
		typedef CommonFilter inherited;

    public:
        bool Enabled = true;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
            {
			    inherited::OutputPin.Notify( _Data );
                return;
            }

			String AValue = String( (char *)_Data );
			AValue.toLowerCase();

			inherited::OutputPin.SendValue( AValue );
        }
	};
//---------------------------------------------------------------------------
	class TrimText : public CommonFilter	
	{
		typedef CommonFilter inherited;

    public:
        bool Enabled = true;

	protected:
		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
            {
			    inherited::OutputPin.Notify( _Data );
                return;
            }

			String AValue = String( (char *)_Data );
			AValue.trim();

			inherited::OutputPin.SendValue( AValue );
        }
	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	FormattedTextElementBasic::FormattedTextElementBasic( BasicFormattedText &AOwner ) :
		FOwner( AOwner )
	{
		AOwner.FElements.push_back( this );
	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	ArduinoStateSetTextValueElement::ArduinoStateSetTextValueElement( TextStateValue &AOwner ) :
		FOwner( AOwner )
	{
		InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoStateSetTextValueElement::DoReceived );
		FOwner.RegisterElement( this );
	}
//---------------------------------------------------------------------------
	bool ArduinoStateSetTextValueElement::TryActive()
	{
		if( FInputValue )
		{
			FOwner.ExternalSetValue( Value );
			return true;
		}

		return false;
	}
//---------------------------------------------------------------------------
	void ArduinoStateSetTextValueElement::DoReceived( void *_Data )
	{
		if( FInputValue == *(bool *)_Data )
			return;

		FInputValue = *(bool *)_Data;
		if( FInputValue )
			FOwner.ExternalSetValue( Value );

		else
			FOwner.ResetValue();
	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}

#endif

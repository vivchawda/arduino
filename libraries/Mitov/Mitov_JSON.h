////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_JSON_h
#define _MITOV_JSON_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class MakeJSONBasicCommonElement;
	class SplitJSONBasicCommonElement;
//---------------------------------------------------------------------------
	class JSONMakeIntf
	{
	public:
		Mitov::SimpleObjectList<MakeJSONBasicCommonElement*>	Elements;
	};
//---------------------------------------------------------------------------
	class JSONSplitIntf
	{
	public:
		Mitov::SimpleObjectList<SplitJSONBasicCommonElement*>	Elements;

	protected:
		static bool ParseHex( String &AText, int &AIndex, String &AName )
		{
			++AIndex;
//			String AResult;
			int AStartIndex = AIndex;

			if( ! IsHexChar( AText[ AIndex ] ))
				return false;

//			while( IsHexChar( AText[ AIndex ] ))
//				++ AIndex;
//				AResult += AText[ AIndex ++ ];

			const char *ACharText = AText.c_str() + AStartIndex;
			char *AEnd;

			int AValue = strtol( ACharText, &AEnd, 16 );

			AIndex += ( AEnd - ACharText );

			AName += char( AValue );
		}

		static bool IsHexChar( char AChar )
		{
			if( AChar >= '0' && AChar <= '9' )
				return true;

			if( AChar >= 'a' && AChar <= 'f' )
				return true;

			if( AChar >= 'A' && AChar <= 'F' )
				return true;

			return false;
		}

	public:
		static bool ExtractString( String &AText, String &AName )
		{
			if( AText[ 0 ] != '"' )
				return false;

			bool AInEscape = false;			

			for( int i = 1; i < AText.length(); ++ i )
			{
				char AChar = AText[ i ];
				if( AInEscape )
				{
					switch( AChar )
					{
						case '"' :
						case '\\' :
						case '/' :
							AName += AChar;
							break;

						case 'b' :	AName += char( 8 );		break;
						case 'f' :	AName += char( 12 );	break;
						case 'n' :	AName += char( 10 );	break;
						case 'r' :	AName += char( 13 );	break;
						case 't' :	AName += char( 9 );	break;
						case 'u' :	
							if( ! ParseHex( AText, i, AName ))
								return false;

							break;

						default:
							return false;
					}
				}

				else
				{
					if( AChar == '"' )
					{
						AText.remove( 0, i + 1 );
						return true;
					}

					if( AChar == '\\' )
						AInEscape = true;

					else
						AName += AChar;
				}
			}

			return false;
		}

		static bool ExtractObject( String &AText )
		{
//			Serial.println( "OBJECT" );

			AText.remove( 0, 1 );
			AText.trim();
			while( AText.length() )
			{
				if( AText.startsWith( "}" ) )
				{
					AText.remove( 0, 1 );
					AText.trim();
					break;
				}

				String AValue;
				if( ! ExtractString( AText, AValue ))
					return false;

				AText.trim();
				if( ! AText.startsWith( ":" ) )
					return false;

				AText.remove( 0, 1 );
				AText.trim();

				if( ! ExtractElement( AText ))
					return false;

				if( AText.startsWith( "," ) )
				{
					AText.remove( 0, 1 );
					AText.trim();
				}
			}

			return true;
		}

		static bool ExtractArray( String &AText )
		{
			AText.remove( 0, 1 );
			AText.trim();
			while( AText.length() )
			{
				if( AText.startsWith( "]" ) )
				{
					AText.remove( 0, 1 );
					AText.trim();
					break;
				}

				if( ! ExtractElement( AText ))
					return false;

				if( AText.startsWith( "," ) )
				{
					AText.remove( 0, 1 );
					AText.trim();
				}
			}
		}

		static bool IsNumberChar( char AChar )
		{
			const char numberChars[] = { '-', '+', '.', 'e', 'E' };

			if( ( AChar >= '0' ) && ( AChar <= '9' ) )
				return true;

			for( int i = 0; i < MITOV_ARRAY_SIZE( numberChars ); ++ i )
				if( AChar == numberChars[ i ] )
					return true;

			return false;
		}

		static bool ExtractNumber( String &AText, String &AValue )
		{
			for( int i = 0; i < AText.length(); ++ i )
			{
				if( ! IsNumberChar( AText[ i ] ))
				{
					if( i == 0 )
						return false;

					AValue = AText.substring( 0, i );

//					Serial.println( AValue );

					AText.remove( 0, i );
					return true;
				}
			}

			return true;
		}

		static bool ExtractElement( String &AText )
		{
//			Serial.println( "ExtractElement" );
//			Serial.println( AText );

			String AValue;
			if( AText.startsWith( "\"" ))
				return ExtractString( AText, AValue );

			if( AText.startsWith( "true" ))
			{
				AText.remove( 0, 4 );
				return true;
			}

			if( AText.startsWith( "false" ))
			{
				AText.remove( 0, 5 );
				return true;
			}

			if( AText.startsWith( "null" ))
			{
//				Serial.println( "NULL" );
				AText.remove( 0, 4 );
				return true;
			}

			if( AText.startsWith( "{" ))
				return ExtractObject( AText );

			if( AText.startsWith( "[" ))
				return ExtractArray( AText );

			return ExtractNumber( AText, AValue );
		}

	protected:
		bool ProcessObject( String &AText );
		bool ProcessArray( String &AText );

	};
//---------------------------------------------------------------------------
	class MakeJSONBasicCommonElement : public OpenWire::Component
	{
	protected:
		bool FModified = false;

	public:
		virtual bool GetIsModified() { return FModified; }
		virtual String GetText() = 0;

	public:
		MakeJSONBasicCommonElement( JSONMakeIntf &AOwner )
		{
			AOwner.Elements.push_back( this );
		}
	};
//---------------------------------------------------------------------------
	class SplitJSONBasicCommonElement : public OpenWire::Component
	{
	public:
		virtual bool Extract( String &AText ) = 0;
		virtual void SendDefault() = 0;

	public:
		SplitJSONBasicCommonElement( JSONSplitIntf &AOwner )
		{
			AOwner.Elements.push_back( this );
		}
	};
//---------------------------------------------------------------------------
	class SplitJSONBasicObjectElement : public SplitJSONBasicCommonElement
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		const char *FName;

	public:
		SplitJSONBasicObjectElement( JSONSplitIntf &AOwner, const char *AName ) :
			inherited( AOwner ),
			FName( AName )
		{
		}

	};
//---------------------------------------------------------------------------
	class MakeJSON : public OpenWire::Component, public JSONMakeIntf, public Mitov::ClockingSupport
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	OnlyModified = false;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
//			Serial.println( "++++++" );
			int AElementCount = Elements.size();
			if( OnlyModified )
			{
				bool AModified = false;
				for( int i = 0; i < AElementCount; ++i )
					if( Elements[ i ]->GetIsModified() )
					{
						AModified = true;
						break;
					}

				if( ! AModified )
					return;
			}

/*
			else
			{
				for( int i = 0; i < AElementCount; ++i )
					if( ! Elements[ i ]->GetIsPopulated() )
						return;

			}
*/
			String AResult = "{";

			for( int i = 0; i < AElementCount; ++i )
			{
				AResult += Elements[ i ]->GetText();
				if( i < AElementCount - 1 )
					AResult += ",";

			}

			AResult += "}";

			OutputPin.Notify( (void *)AResult.c_str() );
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ! ClockInputPin.IsConnected() )
				DoClockReceive( NULL );

//			inherited::SystemLoopBegin( currentMicros );
		}
	};
//---------------------------------------------------------------------------
	class MakeJSONBasicObjectElement : public MakeJSONBasicCommonElement
	{
		typedef MakeJSONBasicCommonElement inherited;

	protected:
		const char *FName;

	protected:
		virtual	String GetValueText() = 0;

	public:
		virtual String GetText() override
		{
			return String( "\"" ) + FName + "\":" + GetValueText();
		}

	public:
		MakeJSONBasicObjectElement( JSONMakeIntf &AOwner, const char *AName ) :
			inherited( AOwner ),
			FName( AName )
		{
		}

	};
//---------------------------------------------------------------------------
	class SplitJSON : public CommonSink, public JSONSplitIntf
	{
		typedef CommonSink inherited;

	public:
		OpenWire::SourcePin	ErrorOutputPin;

	protected:
		virtual void DoReceive( void *_Data ) override
		{
			String AText = (char *)_Data;

			if( ! ProcessObject( AText ))
			{
				ErrorOutputPin.Notify( nullptr );
				return;
			}

			if( AText.length() )
			{
				ErrorOutputPin.Notify( nullptr );
				return;
			}

			for( int i = 0; i < Elements.size(); ++i )
				Elements[ i ]->SendDefault();
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONTextElement : public MakeJSONBasicObjectElement
	{
		typedef MakeJSONBasicObjectElement inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		String InitialValue;

	protected:
		virtual	String GetValueText() override
		{
			FModified = false;
			return "\"" + InitialValue + "\"";
		}

	protected:
		void DoReceive( void *_Data )
		{
			InitialValue = (char *)_Data;
			FModified = true;
		}

	public:
		MakeJSONTextElement( JSONMakeIntf &AOwner, const char *AName ) :
			inherited( AOwner, AName )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MakeJSONTextElement::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONDigitalElement : public MakeJSONBasicObjectElement
	{
		typedef MakeJSONBasicObjectElement inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		bool InitialValue = false;

	protected:
		virtual	String GetValueText() override
		{
			FModified = false;
			if( InitialValue )
				return "true";

			return "false";
		}

	protected:
		void DoReceive( void *_Data )
		{
			InitialValue = *(bool *)_Data;
			FModified = true;
		}

	public:
		MakeJSONDigitalElement( JSONMakeIntf &AOwner, const char *AName ) :
			inherited( AOwner, AName )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MakeJSONDigitalElement::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONIntegerElement : public MakeJSONBasicObjectElement
	{
		typedef MakeJSONBasicObjectElement inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		int32_t InitialValue = 0;

	protected:
		virtual	String GetValueText() override
		{
			FModified = false;
			char AText[ 16 ];
			itoa( InitialValue, AText, 10 );
			return AText;
		}

	protected:
		void DoReceive( void *_Data )
		{
			InitialValue = *(int32_t *)_Data;
			FModified = true;
		}

	public:
		MakeJSONIntegerElement( JSONMakeIntf &AOwner, const char *AName ) :
			inherited( AOwner, AName )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MakeJSONIntegerElement::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONUnsignedElement : public MakeJSONBasicObjectElement
	{
		typedef MakeJSONBasicObjectElement inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		uint32_t InitialValue = 0;

	protected:
		virtual	String GetValueText() override
		{
			FModified = false;
			char AText[ 16 ];
			itoa( InitialValue, AText, 10 );
			return AText;
		}

	protected:
		void DoReceive( void *_Data )
		{
			InitialValue = *(uint32_t *)_Data;
			FModified = true;
		}

	public:
		MakeJSONUnsignedElement( JSONMakeIntf &AOwner, const char *AName ) :
			inherited( AOwner, AName )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MakeJSONUnsignedElement::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONAnalogElement : public MakeJSONBasicObjectElement
	{
		typedef MakeJSONBasicObjectElement inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		float InitialValue = 0;
		uint8_t	MinWidth = 1;
		uint8_t	Precision = 3;

	protected:
		virtual	String GetValueText() override
		{
			FModified = false;
			char AText[ 16 ];
			dtostrf( InitialValue,  MinWidth, Precision, AText );
			return AText;
		}

	protected:
		void DoReceive( void *_Data )
		{
			InitialValue = *(float *)_Data;
			FModified = true;
		}

	public:
		MakeJSONAnalogElement( JSONMakeIntf &AOwner, const char *AName ) :
			inherited( AOwner, AName )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MakeJSONAnalogElement::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONNullElement : public MakeJSONBasicObjectElement
	{
		typedef MakeJSONBasicObjectElement inherited;

	protected:
		virtual	String GetValueText() override
		{
			return "null";
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONObjectElement : public MakeJSONBasicObjectElement, public JSONMakeIntf
	{
		typedef MakeJSONBasicObjectElement inherited;

	public:
		virtual bool GetIsModified() 
		{ 
			for( int i = 0; i < Elements.size(); ++i )
				if( Elements[ i ]->GetIsModified() )
					return true;

			return false;
		}

	protected:
		virtual	String GetValueText() override
		{
			int AElementCount = Elements.size();

			String AResult = "{";

			for( int i = 0; i < AElementCount; ++i )
			{
				AResult += Elements[ i ]->GetText();
				if( i < AElementCount - 1 )
					AResult += ",";

			}

			return AResult + "}";
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONArrayElement : public MakeJSONObjectElement
	{
		typedef MakeJSONObjectElement inherited;

	protected:
		virtual	String GetValueText() override
		{
			int AElementCount = Elements.size();

			String AResult = "[";

			for( int i = 0; i < AElementCount; ++i )
			{
				AResult += Elements[ i ]->GetText();
				if( i < AElementCount - 1 )
					AResult += ",";

			}

			return AResult + "]";
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONTextArrayElement : public MakeJSONBasicCommonElement
	{
		typedef MakeJSONBasicCommonElement inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		String InitialValue;

	protected:
		virtual	String GetText() override
		{
			FModified = false;
			return "\"" + InitialValue + "\"";
		}

	protected:
		void DoReceive( void *_Data )
		{
			InitialValue = (char *)_Data;
			FModified = true;
		}

	public:
		MakeJSONTextArrayElement( JSONMakeIntf &AOwner ) :
			inherited( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MakeJSONTextArrayElement::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONDigitalArrayElement : public MakeJSONBasicCommonElement
	{
		typedef MakeJSONBasicCommonElement inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		bool InitialValue = false;

	protected:
		virtual	String GetText() override
		{
			FModified = false;
			if( InitialValue )
				return "true";

			return "false";
		}

	protected:
		void DoReceive( void *_Data )
		{
			InitialValue = *(bool *)_Data;
			FModified = true;
		}

	public:
		MakeJSONDigitalArrayElement( JSONMakeIntf &AOwner ) :
			inherited( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MakeJSONDigitalArrayElement::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONIntegerArrayElement : public MakeJSONBasicCommonElement
	{
		typedef MakeJSONBasicCommonElement inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		int32_t InitialValue = 0;

	protected:
		virtual	String GetText() override
		{
			FModified = false;
			char AText[ 16 ];
			itoa( InitialValue, AText, 10 );
			return AText;
		}

	protected:
		void DoReceive( void *_Data )
		{
			InitialValue = *(int32_t *)_Data;
			FModified = true;
		}

	public:
		MakeJSONIntegerArrayElement( JSONMakeIntf &AOwner ) :
			inherited( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MakeJSONIntegerArrayElement::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONUnsignedArrayElement : public MakeJSONBasicCommonElement
	{
		typedef MakeJSONBasicCommonElement inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		uint32_t InitialValue = 0;

	protected:
		virtual	String GetText() override
		{
			FModified = false;
			char AText[ 16 ];
			itoa( InitialValue, AText, 10 );
			return AText;
		}

	protected:
		void DoReceive( void *_Data )
		{
			InitialValue = *(uint32_t *)_Data;
			FModified = true;
		}

	public:
		MakeJSONUnsignedArrayElement( JSONMakeIntf &AOwner ) :
			inherited( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MakeJSONUnsignedArrayElement::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONAnalogArrayElement : public MakeJSONBasicCommonElement
	{
		typedef MakeJSONBasicCommonElement inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		float InitialValue = 0;
		uint8_t	MinWidth = 1;
		uint8_t	Precision = 3;

	protected:
		virtual	String GetText() override
		{
			FModified = false;
			char AText[ 16 ];
			dtostrf( InitialValue,  MinWidth, Precision, AText );
			return AText;
		}

	protected:
		void DoReceive( void *_Data )
		{
			InitialValue = *(float *)_Data;
			FModified = true;
		}

	public:
		MakeJSONAnalogArrayElement( JSONMakeIntf &AOwner ) :
			inherited( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MakeJSONAnalogArrayElement::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONNullArrayElement : public MakeJSONBasicCommonElement
	{
		typedef MakeJSONBasicCommonElement inherited;

	protected:
		virtual	String GetText() override
		{
			return "null";
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONObjectArrayElement : public MakeJSONBasicCommonElement, public JSONMakeIntf
	{
		typedef MakeJSONBasicCommonElement inherited;

	public:
		virtual bool GetIsModified() 
		{ 
			for( int i = 0; i < Elements.size(); ++i )
				if( Elements[ i ]->GetIsModified() )
					return true;

			return false;
		}

	protected:
		virtual	String GetText() override
		{
			int AElementCount = Elements.size();

			String AResult = "{";

			for( int i = 0; i < AElementCount; ++i )
			{
				AResult += Elements[ i ]->GetText();
				if( i < AElementCount - 1 )
					AResult += ",";

			}

			return AResult + "}";
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONArrayArrayElement : public MakeJSONObjectArrayElement
	{
		typedef MakeJSONObjectArrayElement inherited;

	protected:
		virtual	String GetText() override
		{
			int AElementCount = Elements.size();

			String AResult = "[";

			for( int i = 0; i < AElementCount; ++i )
			{
				AResult += Elements[ i ]->GetText();
				if( i < AElementCount - 1 )
					AResult += ",";

			}

			return AResult + "]";
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONTextElement : public SplitJSONBasicObjectElement
	{
		typedef SplitJSONBasicObjectElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		String	InitialValue;

	public:
		virtual bool Extract( String &AText ) override
		{
			String AValue;
			if( ! JSONSplitIntf::ExtractString( AText, AValue ))
				return false;

//			Serial.println( AValue );

			InitialValue = AValue;
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( (void *)InitialValue.c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONDigitalElement : public SplitJSONBasicObjectElement
	{
		typedef SplitJSONBasicObjectElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	InitialValue = false;

	public:
		virtual bool Extract( String &AText ) override
		{
//			Serial.println( AText );

			if( AText.startsWith( "true" ))
			{
				AText.remove( 0, 4 );
//				Serial.println( "TRUE" );
				InitialValue = true;
				return true;
			}

			if( AText.startsWith( "false" ))
			{
				AText.remove( 0, 5 );
//				Serial.println( "FALSE" );
				InitialValue = false;
				return true;
			}

			return false;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONIntegerElement : public SplitJSONBasicObjectElement
	{
		typedef SplitJSONBasicObjectElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		int32_t	InitialValue = 0;

	public:
		virtual bool Extract( String &AText ) override
		{
			String AValue;
			if( ! JSONSplitIntf::ExtractNumber( AText, AValue ))
				return false;

//			Serial.println( "INTEGER" );
//			Serial.println( AValue );
			InitialValue = atoi( AValue.c_str() );
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONUnsignedElement : public SplitJSONBasicObjectElement
	{
		typedef SplitJSONBasicObjectElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t	InitialValue = 0;

	public:
		virtual bool Extract( String &AText ) override
		{
			String AValue;
			if( ! JSONSplitIntf::ExtractNumber( AText, AValue ))
				return false;

//			Serial.println( "UNSIGNED" );
//			Serial.println( AValue );
			InitialValue = atol( AValue.c_str() );
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONAnalogElement : public SplitJSONBasicObjectElement
	{
		typedef SplitJSONBasicObjectElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		float	InitialValue = 0;

	public:
		virtual bool Extract( String &AText ) override
		{
			String AValue;
			if( ! JSONSplitIntf::ExtractNumber( AText, AValue ))
				return false;

//			Serial.println( "ANALOG" );
//			Serial.println( AValue );
			InitialValue = strtod( AValue.c_str(), NULL );
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONObjectElement : public SplitJSONBasicObjectElement, public JSONSplitIntf
	{
		typedef SplitJSONBasicObjectElement inherited;

	public:
		virtual bool Extract( String &AText ) override
		{
			return ProcessObject( AText );
//			if( !AText.startsWith( "{" ))
//				return false;
		}

		virtual void SendDefault() override
		{
			for( int i = 0; i < Elements.size(); ++i )
				Elements[ i ]->SendDefault();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONArrayElement : public SplitJSONBasicObjectElement, public JSONSplitIntf
	{
		typedef SplitJSONBasicObjectElement inherited;

	public:
		virtual bool Extract( String &AText ) override
		{
			return ProcessArray( AText );
//			if( !AText.startsWith( "{" ))
//				return false;
		}

		virtual void SendDefault() override
		{
			for( int i = 0; i < Elements.size(); ++i )
				Elements[ i ]->SendDefault();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONTextArrayElement : public SplitJSONBasicCommonElement
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		String	InitialValue;

	public:
		virtual bool Extract( String &AText ) override
		{
			String AValue;
			if( ! JSONSplitIntf::ExtractString( AText, AValue ))
				return false;

//			Serial.println( AValue );

			InitialValue = AValue;
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( (void *)InitialValue.c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONDigitalArrayElement : public SplitJSONBasicCommonElement
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	InitialValue = false;

	public:
		virtual bool Extract( String &AText ) override
		{
//			Serial.println( AText );

			if( AText.startsWith( "true" ))
			{
				AText.remove( 0, 4 );
//				Serial.println( "TRUE" );
				InitialValue = true;
				return true;
			}

			if( AText.startsWith( "false" ))
			{
				AText.remove( 0, 5 );
//				Serial.println( "FALSE" );
				InitialValue = false;
				return true;
			}

			return false;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONIntegerArrayElement : public SplitJSONBasicCommonElement
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		int32_t	InitialValue = 0;

	public:
		virtual bool Extract( String &AText ) override
		{
			String AValue;
			if( ! JSONSplitIntf::ExtractNumber( AText, AValue ))
				return false;

//			Serial.println( "INTEGER" );
//			Serial.println( AValue );
			InitialValue = atoi( AValue.c_str() );
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONUnsignedArrayElement : public SplitJSONBasicCommonElement
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t	InitialValue = 0;

	public:
		virtual bool Extract( String &AText ) override
		{
			String AValue;
			if( ! JSONSplitIntf::ExtractNumber( AText, AValue ))
				return false;

//			Serial.println( "UNSIGNED" );
//			Serial.println( AValue );
			InitialValue = atoi( AValue.c_str() );
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONAnalogArrayElement : public SplitJSONBasicCommonElement
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		float	InitialValue = 0;

	public:
		virtual bool Extract( String &AText ) override
		{
			String AValue;
			if( ! JSONSplitIntf::ExtractNumber( AText, AValue ))
				return false;

//			Serial.println( "ANALOG" );
//			Serial.println( AValue );
			InitialValue = strtod( AValue.c_str(), NULL );
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONObjectArrayElement : public SplitJSONBasicCommonElement, public JSONSplitIntf
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		virtual bool Extract( String &AText ) override
		{
			return ProcessObject( AText );
//			if( !AText.startsWith( "{" ))
//				return false;
		}

		virtual void SendDefault() override
		{
			for( int i = 0; i < Elements.size(); ++i )
				Elements[ i ]->SendDefault();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONArrayArrayElement : public SplitJSONBasicCommonElement, public JSONSplitIntf
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		virtual bool Extract( String &AText ) override
		{
			return ProcessArray( AText );
//			if( !AText.startsWith( "{" ))
//				return false;
		}

		virtual void SendDefault() override
		{
			for( int i = 0; i < Elements.size(); ++i )
				Elements[ i ]->SendDefault();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	bool JSONSplitIntf::ProcessObject( String &AText )			
	{
		AText.trim();

//			Serial.println( AText );

		if( ! AText.startsWith( "{" ))
			return false;

		AText.remove( 0, 1 );
		AText.trim();

		if( AText.startsWith( "}" ))
		{
			AText.remove( 0, 1 );
			AText.trim();
			return true;
		}

		for(;;)
		{

//				Serial.println( AText );

			String AName;

			if( ! ExtractString( AText, AName ) )
				return false;

//			Serial.println( AName );
//			Serial.println( AText );

			AText.trim();
			if( ! AText.startsWith( ":" ))
				return false;

			AText.remove( 0, 1 );
			AText.trim();

//			Serial.println( AText );

			bool AParsed = false;
			for( int i = 0; i < Elements.size(); ++i )
				if( AName == ((SplitJSONBasicObjectElement *) Elements[ i ] )->FName )
				{
//					Serial.println( "MATCH" );
//					Serial.println( AName );
//					Serial.println( ((SplitJSONBasicObjectElement *) Elements[ i ] )->FName );
					if( ! Elements[ i ]->Extract( AText ))
						return false;

//					Serial.println( "SUCCESS" );

					AParsed = true;
					break;
				}

//			Serial.println( AParsed );

			if( ! AParsed )
				if( ! ExtractElement( AText ))
					return false;

//			Serial.println( "SUCCESS1" );
//			Serial.println( AText );
			AText.trim();
			if( AText.startsWith( "}" ))
			{
				AText.remove( 0, 1 );
				AText.trim();
				break;
			}

//			Serial.println( AText );

			AText.trim();
			if( !AText.startsWith( "," ))
				break;

			AText.remove( 0, 1 );
			AText.trim();

//			Serial.println( "SUCCESS2" );
//			Serial.println( AText );
		}

		return true;
	}
//---------------------------------------------------------------------------
	bool JSONSplitIntf::ProcessArray( String &AText )
	{
		AText.trim();

//		Serial.println( "->> ProcessArray" );

//			Serial.println( AText );

		if( ! AText.startsWith( "[" ))
			return false;

		AText.remove( 0, 1 );
		AText.trim();

		if( AText.startsWith( "]" ))
		{
			AText.remove( 0, 1 );
			AText.trim();
			return true;
		}

		int AElementIndex = 0;

		for(;;)
		{

//				Serial.println( AText );

			if( AElementIndex < Elements.size() )
			{
				if( ! Elements[ AElementIndex ++ ]->Extract( AText ))
					return false;

			}

			else if( ! ExtractElement( AText ))
				return false;

			AText.trim();
			if( AText.startsWith( "]" ))
			{
				AText.remove( 0, 1 );
				AText.trim();
				break;
			}

//			Serial.println( AText );

			AText.trim();
			if( !AText.startsWith( "," ))
				break;

			AText.remove( 0, 1 );
			AText.trim();

//			Serial.println( AText );
		}
	}
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
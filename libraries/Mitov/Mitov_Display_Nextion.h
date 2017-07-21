////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_NEXTION_h
#define _MITOV_DISPLAY_NEXTION_h

#include <Mitov.h>
#include <Mitov_Graphics.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

// #define __NEXTION__DEBUG__

namespace Mitov
{
#define Min Min
#define Max Max

	class ArduinoDisplayNextionElementBasic;
//---------------------------------------------------------------------------
	struct TDisplayNextionResponseElementBasic
	{
	public:
		virtual bool ProcessResponse( uint8_t *ABuffer ) { return false; }
		virtual bool ProcessTextResponse( String AText ) { return false; }
	};
//---------------------------------------------------------------------------
	class DisplayNextionEventsIntf
	{
	public:
		virtual	void ResetEvent() {}
		virtual	bool TryProcessEvent( uint8_t *ABuffer ) = 0;

	};
//---------------------------------------------------------------------------
	class DisplayNextionIntf
	{
	public:
		virtual void GetPosition( TGraphicsPos &AX, TGraphicsPos &AY ) { AX = 0; AY = 0; }
		virtual void SendCommand( const char *ACommand ) = 0;
		virtual void RegisterRender( ArduinoDisplayNextionElementBasic *AItem ) = 0;
		virtual	void ActiveatePage( ArduinoDisplayNextionElementBasic *APage ) = 0;
		virtual	bool IsPageActive( ArduinoDisplayNextionElementBasic *APage ) = 0;
		virtual	void AddRequest( String ARequest, TDisplayNextionResponseElementBasic *AResponse ) = 0;
		virtual	void AddRequestFront( String ARequest, TDisplayNextionResponseElementBasic *AResponse ) = 0;
		virtual	void RegisterEvents( DisplayNextionEventsIntf *AEvents ) = 0;
		virtual	uint8_t GetActivePageNumber() = 0;
		virtual	void RetryRequests() = 0;
		virtual	String GetPageNamePath() { return ""; }

	};
//---------------------------------------------------------------------------
	class ArduinoDisplayNextionElementBasic : public OpenWire::Object
	{
	public:
		DisplayNextionIntf	&FOwner;

	protected:
		struct TResponseSendMe : public TDisplayNextionResponseElementBasic
		{
		public:
			ArduinoDisplayNextionElementBasic *FOwner;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
#ifdef __NEXTION__DEBUG__
				Serial.println( "Page Number TEST" );
#endif //__NEXTION__DEBUG__

				if( ABuffer[ 0 ] != 0x66 )
					return false;

#ifdef __NEXTION__DEBUG__
				Serial.print( "Page Number: " ); Serial.println( ABuffer[ 1 ] );
				Serial.println( FOwner->ElementName );
#endif //__NEXTION__DEBUG__
				FOwner->PageNumber = ABuffer[ 1 ];
				FOwner->PageIdentified = true;
				return true;
			}

		public:
			TResponseSendMe( ArduinoDisplayNextionElementBasic *AOwner ) :
				FOwner( AOwner )
			{
			}
		};

		struct TResponseElementGetID : public TDisplayNextionResponseElementBasic
		{
		public:
			ArduinoDisplayNextionElementBasic *FOwner;
			uint8_t	FExpectedCode;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
#ifdef __NEXTION__DEBUG__
				Serial.println( "ID TEST" );
#endif //__NEXTION__DEBUG__
				if( ABuffer[ 0 ] != FExpectedCode )
					return false;

				if( FOwner->PageIdentified )
					if( FOwner->PageNumber != FOwner->FOwner.GetActivePageNumber() )
						return false;

#ifdef __NEXTION__DEBUG__
				Serial.print( "ID: " ); Serial.println( ABuffer[ 1 ] );
				Serial.println( FOwner->ElementName );
#endif //__NEXTION__DEBUG__
				FOwner->ID = ABuffer[ 1 ];
				FOwner->Identified = true;
				if( ! FOwner->PageIdentified )
					FOwner->FOwner.AddRequestFront( "sendme", new TResponseSendMe( FOwner ));

				return true;
			}

		public:
			TResponseElementGetID( ArduinoDisplayNextionElementBasic *AOwner, uint8_t AExpectedCode ) :
				FOwner( AOwner ),
				FExpectedCode( AExpectedCode )
			{
			}
		};

	public:
		uint8_t	ID;
		uint8_t	PageNumber;
		String	ElementName;
		bool	Identified : 1;
		bool	PageIdentified : 1;

//	public:
//		virtual bool RequestInfo() { return false; }

	public:
		virtual void Render() {}
		virtual void SendRequest() {}

		String GetElementName() 
		{ 
			return FOwner.GetPageNamePath() + ElementName;
		}

		void ActivatePageNo( uint8_t APageNo ) 
		{
			if( PageNumber == APageNo )
				Render();
		}

	protected:
		void RequestElementID( String AName )
		{
			FOwner.AddRequest( String( "get " ) + AName + ".id", new TResponseElementGetID( this, 0x71 ));
		}

	public:
		ArduinoDisplayNextionElementBasic( DisplayNextionIntf &AOwner, String AElementName, int APageID, int AElementID ) :
			FOwner( AOwner ),
			ID( AElementID ),
			PageNumber( APageID ),
			ElementName( AElementName ),
			Identified( AElementID >= 0 ),
			PageIdentified( APageID >= 0 )
		{
			FOwner.RegisterRender( this );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionClockedElement : public ArduinoDisplayNextionElementBasic, public Mitov::ClockingSupport
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		virtual void DoClockReceive( void *_Data ) override
		{
			Render();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementFillScreen : public DisplayNextionClockedElement
	{
		typedef DisplayNextionClockedElement inherited;

	public:
		TColor Color;

	public:
		virtual void Render() override
		{
			FOwner.SendCommand( ( String( "cls " ) + Color.To565Color() ).c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementFillRectangle : public DisplayNextionClockedElement
	{
		typedef DisplayNextionClockedElement inherited;

	public:
		TColor Color = TColor( 255, 0, 0 );
		TGraphicsPos		X = 0;
		TGraphicsPos		Y = 0;
		TGraphicsSize	Width = 100;
		TGraphicsSize	Height = 100;

	public:
		virtual void Render() override
		{
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );
			FOwner.SendCommand( ( String( "fill " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + Width + "," + Height + "," + Color.To565Color() ).c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementDrawRectangle : public DisplayNextionClockedElement
	{
		typedef DisplayNextionClockedElement inherited;

	public:
		TColor Color = TColor( 255, 0, 0 );
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		TGraphicsSize	Width = 100;
		TGraphicsSize	Height = 100;

	public:
		virtual void Render() override
		{
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );
			FOwner.SendCommand( ( String( "draw " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + ( AParentX + X + Width ) + "," + ( AParentY + Y + Height ) + "," + Color.To565Color() ).c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------	
	class DisplayNextionElementDrawPicture : public DisplayNextionClockedElement
	{
		typedef DisplayNextionClockedElement inherited;

	public:
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		TGraphicsSize	Width = 100;
		TGraphicsSize	Height = 100;
		bool			Crop = true;
		uint8_t			PictureIndex = 0;

	public:
		virtual void Render() override
		{
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );
			if( Crop )
				FOwner.SendCommand( ( String( "picq " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + Width + "," + Height + "," + PictureIndex ).c_str() );

			else
				FOwner.SendCommand( ( String( "pic " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + PictureIndex ).c_str() );

		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementActivatePage : public DisplayNextionClockedElement, public DisplayNextionIntf
	{
		typedef DisplayNextionClockedElement inherited;

	public:
		virtual	void ActiveatePage( ArduinoDisplayNextionElementBasic *APage ) override
		{
			FOwner.ActiveatePage( APage );
		}

		virtual	bool IsPageActive( ArduinoDisplayNextionElementBasic *APage ) override
		{
			return FOwner.IsPageActive( APage );
		}

		virtual	void AddRequest( String ARequest, TDisplayNextionResponseElementBasic *AResponse ) override
		{
			FOwner.AddRequest( ARequest, AResponse );
		}

		virtual	void AddRequestFront( String ARequest, TDisplayNextionResponseElementBasic *AResponse ) override
		{
			FOwner.AddRequestFront( ARequest, AResponse );
		}

		virtual	void RegisterEvents( DisplayNextionEventsIntf *AEvents ) override
		{
			FOwner.RegisterEvents( AEvents );
		}


		virtual void SendCommand( const char *ACommand ) override
		{
			if( FOwner.IsPageActive( this ))
				FOwner.SendCommand( ACommand );

		}

		virtual	uint8_t GetActivePageNumber() override
		{
			return FOwner.GetActivePageNumber();
		}

//		virtual	uint8_t GetPageNumber() override
//		{
//			return ID;
//		}

		virtual	void RetryRequests() override
		{
			FOwner.RetryRequests();
		}

		virtual	String GetPageNamePath() override
		{
			return ElementName + ".";
		}

		virtual void RegisterRender( ArduinoDisplayNextionElementBasic *AItem ) override
		{
			FOwner.RegisterRender( AItem );
		}

/*
		virtual void SendRequest() override
		{
			if( ! Identified )
				FOwner.AddRequest( "sendme", new TResponseElementGetID( this ));

			inherited::SendRequest();
		}
*/
	public:
		virtual void Render() override
		{
			FOwner.ActiveatePage( this );
//			TGraphicsPos AParentX, AParentY;
//			FOwner.GetPosition( AParentX, AParentY );
			FOwner.SendCommand( ( String( "page " ) + ElementName ).c_str() );
			if( ! Identified )
			{
				PageIdentified = true;
				FOwner.AddRequest( "sendme", new TResponseElementGetID( this, 0x66 ));
			}

//			FOwner.SendCommand( "sendme"
			FOwner.RetryRequests();
		}

/*
		virtual void SendRequest() override
		{
			if( ! Identified )
				sendme
//				RequestElementID( ElementName );

			inherited::SendRequest();
		}
*/
	public:
		DisplayNextionElementActivatePage( DisplayNextionIntf &AOwner, String AElementName ) :
			inherited( AOwner, AElementName, -1, -1 )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionAnalogGaugeValue
	{
	public:
		float	Value;
		int32_t	Angle;

	public:
		DisplayNextionAnalogGaugeValue( float AValue, int32_t AAngle ) :
			Value( AValue ),
			Angle( AAngle )
		{
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementAnalogGauge : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		OpenWire::ValueChangeDetectSinkPin<float>	InputPin;

	public:
		DisplayNextionAnalogGaugeValue	Min;
		DisplayNextionAnalogGaugeValue	Max;

//	public:
//		virtual bool RequestInfo() override;

	public:
		virtual void Render() override
		{
			if( !Identified )
				return;

			if( !PageIdentified )
				return;

			if( PageNumber != FOwner.GetActivePageNumber() )
				return;

			int16_t AAngle = Func::MapRange( constrain( InputPin.Value, Min.Value, Max.Value ), Min.Value, Max.Value, float( Min.Angle ), float( Max.Angle ) ) + 0.5;
			if( AAngle < 0 )
				AAngle += 360;

			FOwner.SendCommand( ( GetElementName() + ".val=" + AAngle ).c_str() );
		}

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
				RequestElementID( GetElementName() );

			inherited::SendRequest();
		}

	protected:
		void DoReceive( void *_Data )
		{
			Render();
		}

	public:
		DisplayNextionElementAnalogGauge( DisplayNextionIntf &AOwner, String AElementName, int APageID, int AElementID ) :
			inherited( AOwner, AElementName, APageID, AElementID ),
			Min( 0.0, 0 ),
			Max( 1.0, 180 )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DisplayNextionElementAnalogGauge::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementAnalogProgressBar : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		OpenWire::ValueChangeDetectSinkPin<float>	InputPin;

	public:
		float	Min = 0.0f;
		float	Max = 1.0f;

	public:
		virtual void Render() override
		{
			if( !Identified )
				return;

			if( !PageIdentified )
				return;

			if( PageNumber != FOwner.GetActivePageNumber() )
				return;

			int16_t AAngle = Func::MapRange( constrain( InputPin.Value, Min, Max ), Min, Max, 0.0f, 100.0f ) + 0.5;
			FOwner.SendCommand( ( GetElementName() + ".val=" + AAngle ).c_str() );
		}

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
				RequestElementID( GetElementName() );

			inherited::SendRequest();
		}

	protected:
		void DoReceive( void *_Data )
		{
			Render();
		}

	public:
		DisplayNextionElementAnalogProgressBar( DisplayNextionIntf &AOwner, String AElementName, int APageID, int AElementID ) :
			inherited( AOwner, AElementName, APageID, AElementID )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DisplayNextionElementAnalogProgressBar::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementText : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		OpenWire::ValueChangeDetectSinkPin<String>	InputPin;

	public:
		virtual void Render() override
		{
//			Serial.println( "RENDER"  );
			FOwner.SendCommand( ( GetElementName() + ".txt=\"" + InputPin.Value + "\"" ).c_str() );
		}

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
				RequestElementID( GetElementName() );

			inherited::SendRequest();
		}

	protected:
		void DoReceive( void *_Data )
		{
			Render();
		}

	public:
		DisplayNextionElementText( DisplayNextionIntf &AOwner, String AElementName, int APageID, int AElementID ) :
			inherited( AOwner, AElementName, APageID, AElementID )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DisplayNextionElementText::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementNumber : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		OpenWire::ValueChangeDetectSinkPin<uint32_t>	InputPin;

	public:
		virtual void Render() override
		{
			if( !Identified )
				return;

			if( !PageIdentified )
				return;

			if( PageNumber != FOwner.GetActivePageNumber() )
				return;

			FOwner.SendCommand( ( GetElementName() + ".val=" + InputPin.Value ).c_str() );
		}

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
				RequestElementID( GetElementName() );

			inherited::SendRequest();
		}

	protected:
		void DoReceive( void *_Data )
		{
			Render();
		}

	public:
		DisplayNextionElementNumber( DisplayNextionIntf &AOwner, String AElementName, int APageID, int AElementID ) :
			inherited( AOwner, AElementName, APageID, AElementID )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DisplayNextionElementNumber::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementFillCircle : public DisplayNextionClockedElement
	{
		typedef DisplayNextionClockedElement inherited;

	public:
		TColor Color = TColor( 255, 0, 0 );
		uint32_t	X = 50;
		uint32_t	Y = 50;
		uint32_t	Radius = 50;

	public:
		virtual void Render() override
		{
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );
			FOwner.SendCommand( ( String( "cirs " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + Radius + "," + Color.To565Color() ).c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementDrawCircle : public DisplayNextionClockedElement
	{
		typedef DisplayNextionClockedElement inherited;

	public:
		TColor Color = TColor( 255, 0, 0 );
		uint32_t	X = 50;
		uint32_t	Y = 50;
		uint32_t	Radius = 50;

	public:
		virtual void Render() override
		{
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );
			FOwner.SendCommand( ( String( "cir " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + Radius + "," + Color.To565Color() ).c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementDrawLine : public DisplayNextionClockedElement
	{
		typedef DisplayNextionClockedElement inherited;

	public:
		TColor Color = TColor( 255, 0, 0 );
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		TGraphicsPos	Width = 100;
		TGraphicsPos	Height = 100;

	public:
		virtual void Render() override
		{
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );
			FOwner.SendCommand( ( String( "line " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + ( AParentX + X + Width ) + "," + ( AParentY + Y + Height ) + "," + Color.To565Color() ).c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	enum TArduinoDisplayNextionTextBackgroundMode { bmClear, bmColor, bmPictureCrop, bmPicture };
//---------------------------------------------------------------------------
	class DisplayNextionTextBackground
	{
	public:
		TColor		Color;
		uint32_t	PictureIndex = 0;
		TArduinoDisplayNextionTextBackgroundMode Mode = bmClear;

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementDrawText : public DisplayNextionClockedElement, OpenWire::Component
	{
		typedef DisplayNextionClockedElement inherited;

	public:
		TColor Color = TColor( 255, 0, 0 );
		DisplayNextionTextBackground	Background;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		TGraphicsSize	Width = 100;
		TGraphicsSize	Height = 20;
		uint8_t			FontIndex = 0;
		TArduinoTextHorizontalAlign	HorizontalAlign : 2;
		TArduinoTextVerticalAlign	VerticalAlign : 2;
		String	InitialValue;

	protected:
		String	FValue;

	public:
		template<typename T> void Print( T AValue )
		{
			 FValue = String( AValue );
			 if( ! ClockInputPin.IsConnected() )
				 Render();
		}

	public:
		virtual void Render() override
		{
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );
			String ABackgroundColorText;
			String ABackgroundTypeText;
			switch( Background.Mode )
			{
				case bmClear:
					ABackgroundColorText = "NULL";
					ABackgroundTypeText = "1";
					break;

				case bmColor:
					ABackgroundColorText = String( Background.Color.To565Color());
					ABackgroundTypeText = "1";
					break;

				case bmPictureCrop:
					ABackgroundColorText = String( Background.PictureIndex );
					ABackgroundTypeText = "0";
					break;

				case bmPicture:
					ABackgroundColorText = String( Background.PictureIndex );
					ABackgroundTypeText = "2";
					break;

			}

			FOwner.SendCommand( ( String( "xstr " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + Width + "," + Height + "," + FontIndex + "," + Color.To565Color() + "," + ABackgroundColorText + "," + uint16_t( HorizontalAlign ) + "," + uint16_t( VerticalAlign ) + "," + ABackgroundTypeText + ",\"" + FValue + "\"" ).c_str() );
//			FOwner.SendCommand( ( String( "xstr " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + Width + "," + Height + "," + FontIndex + "," + Color.To565Color() + ",NULL," + uint16_t( HorizontalAlign ) + "," + uint16_t( VerticalAlign ) + ",1,\"" + FValue + "\"" ).c_str() );
		}

	protected:
		virtual void SystemInit() override
		{
			FValue = InitialValue;
//			inherited::SystemInit();
		}

	public:
		DisplayNextionElementDrawText( DisplayNextionIntf &AOwner ) :
			inherited( AOwner, "", -1, -1 ),
			HorizontalAlign( thaCenter ),
			VerticalAlign( tvaCenter )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementDrawScene : public DisplayNextionClockedElement, public DisplayNextionIntf
	{
		typedef Mitov::DisplayNextionClockedElement inherited;

	public:
		uint32_t	X = 0;
		uint32_t	Y = 0;

	protected:
		Mitov::SimpleList<ArduinoDisplayNextionElementBasic *>	FElements;

	public:
		virtual	void ActiveatePage( ArduinoDisplayNextionElementBasic *APage ) override
		{
			FOwner.ActiveatePage( APage );
		}

		virtual	bool IsPageActive( ArduinoDisplayNextionElementBasic *APage ) override
		{
			return FOwner.IsPageActive( APage );
		}

		virtual void SendCommand( const char *ACommand ) override
		{
			FOwner.SendCommand( ACommand );
		}

		virtual	uint8_t GetActivePageNumber() override
		{
			return FOwner.GetActivePageNumber();
		}

		virtual	void RetryRequests() override
		{
			FOwner.RetryRequests();
		}

	public:
		virtual void GetPosition( TGraphicsPos &AX, TGraphicsPos &AY )
		{ 
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			AX = AParentX + X;
			AY = AParentY + Y;
		}

		virtual void RegisterRender( ArduinoDisplayNextionElementBasic *AItem ) override
		{
			FElements.push_back( AItem );
			FOwner.RegisterRender( AItem );
		}

	public:
		virtual void Render() override
		{
			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Render();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class DisplayNextion : public OpenWire::Component, public DisplayNextionIntf
	{
		typedef OpenWire::Component inherited;

	protected:
		ArduinoDisplayNextionElementBasic *FActivePage = nullptr;

	protected:
		struct TRequestElement
		{
		public:
			String Request;
			TDisplayNextionResponseElementBasic *Response;

		public:
			TRequestElement( String ARequest, TDisplayNextionResponseElementBasic *AResponse ) :
				Request( ARequest ),
				Response( AResponse )
			{
			}
		};

		struct TResponseReset : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextion *FOwner;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
				if( ABuffer[ 0 ] != 0x88 )
					return false;

#ifdef __NEXTION__DEBUG__
							Serial.println( "RESET Confirmed" );
#endif // __NEXTION__DEBUG__

//				FOwner->Restarted();
				return true;
			}

		public:
			TResponseReset( DisplayNextion *AOwner ) :
				FOwner( AOwner )
			{
			}
		};

	public:
		Mitov::SimpleObjectList<DisplayNextionEventsIntf *>	FEventElements;
		Mitov::SimpleObjectDeleteList<TRequestElement *>	FRequestElements;
		TDisplayNextionResponseElementBasic *FLastResponse = nullptr;
//		Mitov::SimpleObjectDeleteList<TDisplayNextionResponseElementBasic *>	FResponseHandlersQueue;

	protected:
		Mitov::SimpleList<ArduinoDisplayNextionElementBasic *>	FElements;

	protected:
		uint8_t		FBuffer[ 10 ];
		uint8_t		FIndex = 0;
		uint8_t		FLength = 0;
		unsigned long	FLastTime = 0;
		String	FCurrentRequest;
		String	FResultText;
		bool	FStarted = false;
		uint8_t		FActivePageNumber = 0;

	public:
		virtual	void ActiveatePage( ArduinoDisplayNextionElementBasic *APage ) override
		{
			FActivePage = APage;
			if( FActivePage )
				FActivePageNumber = FActivePage->ID;
		}

		virtual	bool IsPageActive( ArduinoDisplayNextionElementBasic *APage ) override
		{
			if( ! FActivePage )
//				if( APage )
//					if( APage->GetElementName() == "page0" )
						return true;

			return ( APage == FActivePage );
		}

		virtual void SendCommand( const char *ACommand ) override
		{
#ifdef __NEXTION__DEBUG__
			Serial.print( "ACommand: " ); Serial.println( ACommand );
#endif // __NEXTION__DEBUG__

			FStream.print( ACommand );
			FStream.print( "\xFF\xFF\xFF" );
		}

		virtual	uint8_t GetActivePageNumber() override
		{
			return FActivePageNumber;
		}

		virtual	void RetryRequests() override
		{
#ifdef __NEXTION__DEBUG__
			Serial.print( "FElements: " ); Serial.println( FElements.size() );
#endif // __NEXTION__DEBUG__

			for( int i = 0; i < FElements.size(); ++i )
				FElements[ i ]->SendRequest();

//			FOwner.RetryRequests();
		}

		virtual void RegisterRender( ArduinoDisplayNextionElementBasic *AItem )
		{
			FElements.push_back( AItem );
		}

	protected:
		Stream &FStream;

	protected:
		void RequestElementIDs()
		{
//			if( ! FStarted )
//				return;

//			Serial.println( FRequestElements.size() );
			if( FLastResponse )
				return;

//			Serial.println( FRequestElements.size() );
			if( FRequestElements.size() )
			{
				TDisplayNextionResponseElementBasic *AElement = FRequestElements[ 0 ]->Response;
				FCurrentRequest = FRequestElements[ 0 ]->Request;
				FRequestElements.pop_front();
				SendCommand( FCurrentRequest.c_str() );

#ifdef __NEXTION__DEBUG__
				Serial.println( "REQUEST" );
				Serial.println( FCurrentRequest );
#endif // __NEXTION__DEBUG__

				FLastResponse = AElement;
//				FResponseHandlersQueue.push_back( AElement );
//				if( AElement->RequestInfo())
//					break;

//				String AText = FRequestElements[ 0 ].ID;
//				FResponseHandlersQueue.push_back( new TResponseElementGetID( FRequestElements[ 0 ] ));
//				FRequestElements.pop_front();
//				SendCommand( AText );
			}
		}

		void ReadSerial()
		{
			int AChar = FStream.read();
			if( AChar < 0 )
				return;

			if( ! FStarted )
			{
				if( AChar != 0x88 )
					return;

				FStarted = true;
			}
			
#ifdef __NEXTION__DEBUG__
			Serial.print( "FIndex: " ); Serial.print( FIndex ); Serial.print( " - "); Serial.println( AChar, HEX );
#endif // __NEXTION__DEBUG__

			if( FIndex == 0 )
			{
#ifdef __NEXTION__DEBUG__
				Serial.print( "RESULT CODE: " );	Serial.println( AChar, HEX );
//				Serial.print( "FIndex: " ); Serial.println( FIndex );
#endif // __NEXTION__DEBUG__
				switch( AChar )
				{
					case 0x00: // Invalid instruction
					case 0x03: // Page ID invalid
					case 0x04: // Picture ID invalid
					case 0x05: // Font ID invalid
					case 0x1A: // Variable name invalid
					case 0x1B: // 	Variable operation invalid
						FLength = 4;
						break;

					case 0x65:
						FLength = 7;
						break;

					case 0x66:
						FLength = 5;
						break;

					case 0x67:
						FLength = 9;
						break;

					case 0x68:
						FLength = 9;
						break;

					case 0x70: // String variable data returns
						FLength = 4; // Skip the sring content
						break;

					case 0x71:
						FLength = 8;
//						Serial.println( "FLength = 8" );
						break;

					case 0x86:
						FLength = 4;
						break;

					case 0x87:
						FLength = 4;
						break;

					case 0x88:
						FLength = 4; // ?
						break;

					case 0x89:
						FLength = 4; // ?
						break;

					case 0xFE:
						FLength = 4; // ?
						break;

					default:
						return; // Unknown Command

				}
			}

			if( FIndex > 0 )
				if( FBuffer[ 0 ] == 0x70 ) // String variable data returns
					if( AChar != 0xFF )
					{
						FResultText += char( AChar );
						return;
					}

			FBuffer[ FIndex ++ ] = AChar;
			if( FIndex < FLength )
				return;

			FIndex = 0;
			if( ( FBuffer[ FLength - 1 ] != 0xFF ) && ( FBuffer[ FLength - 2 ] != 0xFF ) && ( FBuffer[ FLength - 3 ] != 0xFF ) )
			{
//				Serial.println( "Bad command ending" );
				// Bad command ending
				return;
			}

			switch( FBuffer[ 0 ] )
			{
					case 0x65: // Touch event return data
						for( int i = 0; i < FEventElements.size(); ++i )
							if( FEventElements[ i ]->TryProcessEvent( FBuffer ))
								break;

						break;

					case 0x1A:
#ifdef __NEXTION__DEBUG__
						Serial.println( "INVALID" );
#endif // __NEXTION__DEBUG__
//						FLastResponse->ProcessResponse( nullptr );
						delete FLastResponse;
						FLastResponse = nullptr;
						FCurrentRequest = "";
						RequestElementIDs();
						break;

					case 0x66:
						FActivePageNumber = FBuffer[ 1 ];

						for( int i = 0; i < FElements.size(); ++i )
							FElements[ i ]->ActivatePageNo( FActivePageNumber );

					case 0x88: // System Startup

					case 0x71: // Numeric variable data returns
#ifdef __NEXTION__DEBUG__
						Serial.println( "TEST" );
#endif // __NEXTION__DEBUG__
						if( FLastResponse )
						{
#ifdef __NEXTION__DEBUG__
							Serial.println( "TEST11" );
#endif // __NEXTION__DEBUG__
							if( FLastResponse->ProcessResponse( FBuffer ))
							{
								delete FLastResponse;
								FLastResponse = nullptr;
								FCurrentRequest = "";
								RequestElementIDs();
							}
							else if( FBuffer[ 0 ] == 0x66 )
								RetryRequests();

//							Serial.println( "TEST12" );
						}
						else if( FBuffer[ 0 ] == 0x66 )
							RetryRequests();

						break;

					case 0x70: // String variable data returns
//						Serial.println( "TEST1" );
						if( FLastResponse )
						{
//							Serial.println( "TEST2" );
//							Serial.println( FResultText );
							if( FLastResponse->ProcessTextResponse( FResultText ))
							{
								delete FLastResponse;
								FLastResponse = nullptr;
								FCurrentRequest = "";
								RequestElementIDs();
							}
						}
						FResultText = "";
						break;
			}

/*
			if( AChar != 10 )
			{
				FBuffer[ FIndex ++ ] = AChar;
				if( FIndex < 255 )
					return;
			}
*/
//			FBuffer[ FIndex ] = '\0';
//			FIndex = 0;

//			String AString = FBuffer;

		}

	public:
		virtual void AddRequest( String ARequest, TDisplayNextionResponseElementBasic *AResponse ) override
		{
			FRequestElements.push_back( new TRequestElement( ARequest, AResponse ));
			RequestElementIDs();
//			FResponseHandlersQueue.push_back( AResponse );
		}

		virtual	void AddRequestFront( String ARequest, TDisplayNextionResponseElementBasic *AResponse ) override
		{
			FRequestElements.push_front( new TRequestElement( ARequest, AResponse ));
			RequestElementIDs();
		}

		virtual	void RegisterEvents( DisplayNextionEventsIntf *AEvents ) override
		{
			FEventElements.push_back( AEvents );
		}

	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
//			FStarted = true;
			AddRequest( "rest", new TResponseReset( this ) );
//			delay( 1000 );
			RetryRequests();
			RequestElementIDs();
			for( int i = 0; i < FEventElements.size(); ++i )
				FEventElements[ i ]->ResetEvent();

			FLastTime = millis();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );
			ReadSerial();
			if( FCurrentRequest != "" )
				if( millis() - FLastTime > 1000 )
				{
					FLastTime = millis();
					SendCommand( FCurrentRequest.c_str() );
//					RequestElementIDs();
				}
		}
/*
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			Serial.println( "." );
//			if( FNeedsRead || ( ! ClockInputPin.IsConnected() ))
				ReadSensor();

			inherited::SystemLoopBegin( currentMicros );
		}
*/
	public:
		DisplayNextion( Mitov::BasicSerialPort &ASerial ) :
			FStream( ASerial.GetStream() )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementScope : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		OpenWire::SinkPin	ClearInputPin;

	public:
		uint8_t	ID = 1;
		uint8_t	Height = 255;
		bool	Ready = false;

	protected:
		struct TResponseElementGetValue : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextionElementScope *FOwner;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
//				Serial.print( "Height: " ); Serial.println( ABuffer[ 1 ] );
				FOwner->Height = ABuffer[ 1 ];
				FOwner->Ready = true;
				return true;
			}

		public:
			TResponseElementGetValue( DisplayNextionElementScope *AOwner ) :
				FOwner( AOwner )
			{
			}
		};

/*
	public:
		virtual bool RequestInfo() override
		{
			Serial.println( ElementName );
			RequestElementID( ElementName );

			FOwner.AddRequest( String( "get " ) + ElementName + ".h", new TResponseElementGetValue( this )); // Get the Height

//			FOwner.FResponseHandlersQueue.push_back( new TResponseElementGetValue( this ));
//			FOwner.SendCommand( ( String( "get " ) + ElementName + ".h" ).c_str() ); // Get the Height
			return true;
		}
*/
	protected:
		void DoReceiveClear( void * )
		{
			FOwner.SendCommand( ( String( "cle " ) + ID + ",255" ).c_str() );
		}

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
			{
				RequestElementID( GetElementName() );
				FOwner.AddRequest( String( "get " ) + GetElementName() + ".h", new TResponseElementGetValue( this ) );
			}

			inherited::SendRequest();
		}

	public:
		DisplayNextionElementScope( DisplayNextionIntf &AOwner, String AElementName, int APageID, int AElementID ) :
			inherited( AOwner, AElementName, APageID, AElementID )
		{
			ClearInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DisplayNextionElementScope::DoReceiveClear );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionScopeChannel : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SinkPin	ClearInputPin;

	protected:
		DisplayNextionElementScope &FOwner;
		uint8_t	FIndex;

	protected:
		void DoReceiveData( void * _Data )
		{
			if( ! FOwner.Ready )
				return;

			float AFloatValue = MitovMax( 0.0f, MitovMin( 1.0f, *(float *)_Data ));
//			Serial.println( AFloatValue );
			int AIntValue = int( AFloatValue * FOwner.Height + 0.5 );
			FOwner.FOwner.SendCommand( ( String( "add " ) + FOwner.ID + "," + FIndex + "," + AIntValue ).c_str() );
		}

		void DoReceiveClear( void * )
		{
			FOwner.FOwner.SendCommand( ( String( "cle " ) + FOwner.ID + "," + FIndex ).c_str() );
		}

	public:
		DisplayNextionScopeChannel( DisplayNextionElementScope &AOwner, uint8_t AIndex ) :
			FOwner( AOwner ),
			FIndex( AIndex )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DisplayNextionScopeChannel::DoReceiveData );
			ClearInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DisplayNextionScopeChannel::DoReceiveClear );
		}
	};
//---------------------------------------------------------------------------
	enum TArduinoDisplayNextionElementButtonEvents { nbeBoth, nbeDown, nbeUp };
//---------------------------------------------------------------------------
	class DisplayNextionElementButton : public ArduinoDisplayNextionElementBasic, public DisplayNextionEventsIntf
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		TArduinoDisplayNextionElementButtonEvents	Events : 2;

	public:
		virtual	void ResetEvent() override
		{
			OutputPin.SendValue( false );
		}

		virtual	bool TryProcessEvent( uint8_t *ABuffer ) override
		{
			if( !Identified )
				return false;

			if( ABuffer[ 2 ] != ID )
				return false;

			if( ABuffer[ 1 ] != PageNumber ) //FOwner.GetPageNumber() )
				return false;

			if( ABuffer[ 3 ] )
			{
				if( Events == nbeBoth )
					OutputPin.SendValue( true );

				else if( Events == nbeDown )
				{
					OutputPin.SendValue( true );
					OutputPin.SendValue( false );
				}
			}
			else
			{
				if( Events == nbeBoth )
					OutputPin.SendValue( false );

				else if( Events == nbeUp )
				{
					OutputPin.SendValue( true );
					OutputPin.SendValue( false );
				}
			}
		}

	public:
		virtual void SendRequest() override
		{
#ifdef __NEXTION__DEBUG__
			Serial.print( "BUTTON Identified: " ); Serial.println( Identified );
#endif //__NEXTION__DEBUG__
			if( ! Identified )
				RequestElementID( GetElementName() );

			inherited::SendRequest();
		}

	public:
		DisplayNextionElementButton( DisplayNextionIntf &AOwner, String AElementName, int APageID, int AElementID ) :
			inherited( AOwner, AElementName, APageID, AElementID ),
			Events( nbeBoth )
		{
			FOwner.RegisterEvents( this );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementCheckBox : public ArduinoDisplayNextionElementBasic, public DisplayNextionEventsIntf
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		OpenWire::TypedStartSourcePin<bool>	OutputPin;

	protected:
		struct TResponseElementGetValue : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextionElementCheckBox *FOwner;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
//				Serial.print( "Height: " ); Serial.println( ABuffer[ 1 ] );
				FOwner->OutputPin.SetValue( ABuffer[ 1 ] != 0 );
				return true;
			}

		public:
			TResponseElementGetValue( DisplayNextionElementCheckBox *AOwner ) :
				FOwner( AOwner )
			{
			}
		};

	public:
		virtual	bool TryProcessEvent( uint8_t *ABuffer ) override
		{
			if( ! Identified )
				return false;

			if( ABuffer[ 2 ] != ID )
				return false;

			if( ABuffer[ 1 ] != PageNumber ) // FOwner.GetPageNumber() )
				return false;

			RequestState();
		}

	protected:
		void RequestState()
		{
			FOwner.AddRequest( String( "get " ) + GetElementName() + ".val", new TResponseElementGetValue( this ) );
		}

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
			{
				RequestElementID( GetElementName() );
				RequestState();
			}

			inherited::SendRequest();
		}

	public:
		DisplayNextionElementCheckBox( DisplayNextionIntf &AOwner, String AElementName, int APageID, int AElementID ) :
			inherited( AOwner, AElementName, APageID, AElementID )
		{
			FOwner.RegisterEvents( this );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementSlider : public ArduinoDisplayNextionElementBasic, public DisplayNextionEventsIntf, public ClockingSupport
#ifdef __NEXTION_SCAN_SLIDER__
		, public OpenWire::Component
#endif
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		uint32_t	Min = 0;
		uint32_t	Max = 100;

#ifdef __NEXTION_SCAN_SLIDER__
		uint32_t	PressedScanPeriod = 0;

	protected:
		unsigned long	FLastTime = 0;
		bool			FPressed = false;
#endif

	protected:
		struct TResponseElementGetValue : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextionElementSlider *FOwner;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
				uint32_t AValue = ABuffer[ 1 ] | ( int32_t( ABuffer[ 2 ] ) << 8 ) | ( int32_t( ABuffer[ 3 ] ) << 16 ) | ( int32_t( ABuffer[ 4 ] ) << 24 );
				float AFloatValue = float( AValue - FOwner->Min ) / ( FOwner->Max - FOwner->Min );
				FOwner->OutputPin.SetValue( AFloatValue );
//				FOwner->OutputPin.SendValue<int32_t>( ABuffer[ 1 ] | ( int32_t( ABuffer[ 2 ] ) << 8 ) | ( int32_t( ABuffer[ 3 ] ) << 16 ) | ( int32_t( ABuffer[ 4 ] ) << 24 ) );
//				Serial.print( "Height: " ); Serial.println( ABuffer[ 1 ] );
//				FOwner->OutputPin.SetValue( ABuffer[ 1 ] != 0 );
				return true;
			}

		public:
			TResponseElementGetValue( DisplayNextionElementSlider *AOwner ) :
				FOwner( AOwner )
			{
			}
		};

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			RequestState();
		}

	public:
		virtual	bool TryProcessEvent( uint8_t *ABuffer ) override
		{
			if( ! Identified )
				return false;

			if( ABuffer[ 2 ] != ID )
				return false;

			if( ABuffer[ 1 ] != PageNumber ) //FOwner.GetPageNumber() )
				return false;

			RequestState();
#ifdef __NEXTION_SCAN_SLIDER__
			FPressed = ( ABuffer[ 3 ] != 0 );
			if ( FPressed )
				if( PressedScanPeriod )
					FLastTime = millis();
#endif
		}

	protected:
		void RequestState()
		{
			FOwner.AddRequest( String( "get " ) + GetElementName() + ".val", new TResponseElementGetValue( this ) );
		}

#ifdef __NEXTION_SCAN_SLIDER__
	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );
			if( FPressed )
				if( PressedScanPeriod )
					if( millis() - FLastTime > PressedScanPeriod )
					{
						FLastTime = millis();
//						Serial.println( "SCAN" );
						RequestState();
					}
		}
#endif

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
			{
				RequestElementID( GetElementName() );
				RequestState();
			}

			inherited::SendRequest();
		}

	public:
		DisplayNextionElementSlider( DisplayNextionIntf &AOwner, String AElementName, int APageID, int AElementID ) :
			inherited( AOwner, AElementName, APageID, AElementID )
		{
			FOwner.RegisterEvents( this );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementModifierPropertySetterString : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		String	PropertyName;
		String	Value;

	protected:
		ArduinoDisplayNextionElementBasic &FOwner;

	protected:
		void DoClockReceive( void * )
		{
			FOwner.FOwner.SendCommand(( FOwner.GetElementName() + "." + PropertyName + "=\"" + Value + "\"" ).c_str() );
		}

	public:
		DisplayNextionElementModifierPropertySetterString( ArduinoDisplayNextionElementBasic &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DisplayNextionElementModifierPropertySetterString::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementModifierPropertySetterInteger : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		String	PropertyName;
		int	Value = 0;

	protected:
		ArduinoDisplayNextionElementBasic &FOwner;

	protected:
		void DoClockReceive( void * )
		{
			FOwner.FOwner.SendCommand(( FOwner.GetElementName() + "." + PropertyName + "=" + Value ).c_str() );
		}

	public:
		DisplayNextionElementModifierPropertySetterInteger( ArduinoDisplayNextionElementBasic &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DisplayNextionElementModifierPropertySetterInteger::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementModifierPropertyGetterInteger : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	ClockInputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		String	PropertyName;

	protected:
		ArduinoDisplayNextionElementBasic &FOwner;

	protected:
		struct TResponseElementGetValue : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextionElementModifierPropertyGetterInteger *FOwner;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
//				Serial.println( "RESPONSE" );
				FOwner->OutputPin.SendValue<int32_t>( ABuffer[ 1 ] | ( int32_t( ABuffer[ 2 ] ) << 8 ) | ( int32_t( ABuffer[ 3 ] ) << 16 ) | ( int32_t( ABuffer[ 4 ] ) << 24 ) );
				return true;
			}

		public:
			TResponseElementGetValue( DisplayNextionElementModifierPropertyGetterInteger *AOwner ) :
				FOwner( AOwner )
			{
			}
		};

	protected:
		void DoClockReceive( void * )
		{
//			Serial.println( "GET" );
			FOwner.FOwner.AddRequest( String( "get " ) + FOwner.GetElementName() + "." + PropertyName, new TResponseElementGetValue( this ) );
		}

	public:
		DisplayNextionElementModifierPropertyGetterInteger( ArduinoDisplayNextionElementBasic &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DisplayNextionElementModifierPropertyGetterInteger::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementModifierPropertyGetterString : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	ClockInputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		String	PropertyName;

	protected:
		ArduinoDisplayNextionElementBasic &FOwner;

	protected:
		struct TResponseElementGetValue : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextionElementModifierPropertyGetterString *FOwner;

		public:
			virtual bool ProcessTextResponse( String AText ) override
			{
//				Serial.println( "RESPONSE" );
//				FOwner->OutputPin.SendValue<int32_t>( ABuffer[ 1 ] | ( int32_t( ABuffer[ 2 ] ) << 8 ) | ( int32_t( ABuffer[ 3 ] ) << 16 ) | ( int32_t( ABuffer[ 4 ] ) << 24 ) );
				FOwner->OutputPin.Notify( (void *)AText.c_str() );
				return true;
			}

		public:
			TResponseElementGetValue( DisplayNextionElementModifierPropertyGetterString *AOwner ) :
				FOwner( AOwner )
			{
			}
		};

	protected:
		void DoClockReceive( void * )
		{
//			Serial.println( "GET" );
			FOwner.FOwner.AddRequest( String( "get " ) + FOwner.GetElementName() + "." + PropertyName, new TResponseElementGetValue( this ) );
		}

	public:
		DisplayNextionElementModifierPropertyGetterString( ArduinoDisplayNextionElementBasic &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DisplayNextionElementModifierPropertyGetterString::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementModifierPressedState : public DisplayNextionEventsIntf
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		ArduinoDisplayNextionElementBasic &FOwner;

	public:
		virtual	void ResetEvent() override
		{
			OutputPin.SendValue( false );
		}

		virtual	bool TryProcessEvent( uint8_t *ABuffer ) override
		{
			if( ! FOwner.Identified )
				return false;

			if( ABuffer[ 2 ] != FOwner.ID )
				return false;

			if( ABuffer[ 1 ] != FOwner.PageNumber ) // FOwner.FOwner.GetPageNumber() )
				return false;

			OutputPin.SendValue( ABuffer[ 3 ] != 0 );
		}

	public:
		DisplayNextionElementModifierPressedState( ArduinoDisplayNextionElementBasic &AOwner ) :
			FOwner( AOwner )
		{
			FOwner.FOwner.RegisterEvents( this );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementModifierPressEvent : public DisplayNextionEventsIntf
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		ArduinoDisplayNextionElementBasic &FOwner;

	public:
		virtual	bool TryProcessEvent( uint8_t *ABuffer ) override
		{
			if( ! FOwner.Identified )
				return false;

			if( ABuffer[ 2 ] != FOwner.ID )
				return false;

			if( ABuffer[ 1 ] != FOwner.PageNumber ) // FOwner.FOwner.GetPageNumber() )
				return false;

			if( ABuffer[ 3 ] )
				OutputPin.Notify( nullptr );
		}

	public:
		DisplayNextionElementModifierPressEvent( ArduinoDisplayNextionElementBasic &AOwner ) :
			FOwner( AOwner )
		{
			FOwner.FOwner.RegisterEvents( this );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementModifierReleaseEvent : public DisplayNextionEventsIntf
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		ArduinoDisplayNextionElementBasic &FOwner;

	public:
		virtual	bool TryProcessEvent( uint8_t *ABuffer ) override
		{
			if( ! FOwner.Identified )
				return false;

			if( ABuffer[ 2 ] != FOwner.ID )
				return false;

			if( ABuffer[ 1 ] != FOwner.PageNumber ) // FOwner.FOwner.GetPageNumber() )
				return false;

			if( ! ABuffer[ 3 ] )
				OutputPin.Notify( nullptr );
		}

	public:
		DisplayNextionElementModifierReleaseEvent( ArduinoDisplayNextionElementBasic &AOwner ) :
			FOwner( AOwner )
		{
			FOwner.FOwner.RegisterEvents( this );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementCommand : public Mitov::ClockingSupport
	{
		typedef Mitov::ClockingSupport inherited;

	public:
		String Command;

	protected:
		DisplayNextionIntf &FOwner;

	public:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.SendCommand( Command.c_str() );
		}

	public:
		DisplayNextionElementCommand( DisplayNextionIntf &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementGetterCommandString : public Mitov::ClockingSupport
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		String Command;

	protected:
		DisplayNextionIntf &FOwner;

	protected:
		struct TResponseElementGetValue : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextionElementGetterCommandString *FOwner;

		public:
			virtual bool ProcessTextResponse( String AText ) override
			{
//				Serial.println( "RESPONSE" );
				FOwner->OutputPin.Notify( (void *)AText.c_str() );
				return true;
			}

		public:
			TResponseElementGetValue( DisplayNextionElementGetterCommandString *AOwner ) :
				FOwner( AOwner )
			{
			}
		};

	public:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.AddRequest( Command, new TResponseElementGetValue( this ) );
		}

	public:
		DisplayNextionElementGetterCommandString( DisplayNextionIntf &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementGetterCommandInteger : public Mitov::ClockingSupport
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		String Command;

	protected:
		DisplayNextionIntf &FOwner;

	protected:
		struct TResponseElementGetValue : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextionElementGetterCommandInteger *FOwner;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
//				Serial.println( "RESPONSE" );
				FOwner->OutputPin.SendValue<int32_t>( ABuffer[ 1 ] | ( int32_t( ABuffer[ 2 ] ) << 8 ) | ( int32_t( ABuffer[ 3 ] ) << 16 ) | ( int32_t( ABuffer[ 4 ] ) << 24 ) );
				return true;
			}

		public:
			TResponseElementGetValue( DisplayNextionElementGetterCommandInteger *AOwner ) :
				FOwner( AOwner )
			{
			}
		};

	public:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.AddRequest( Command, new TResponseElementGetValue( this ) );
		}

	public:
		DisplayNextionElementGetterCommandInteger( DisplayNextionIntf &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
#undef Min
#undef Max
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

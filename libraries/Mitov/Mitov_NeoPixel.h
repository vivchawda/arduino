////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_NEO_PIXEL_h
#define _MITOV_NEO_PIXEL_h

#include <Mitov.h>
#include <Mitov_Graphics_RGBW.h>

#include <MitovEmbedded_Adafruit_NeoPixel/MitovEmbedded_Adafruit_NeoPixel.h>

namespace Mitov
{
	class NeoPixelsRunningColorGroup;
	class NeoPixelsCommonGroup;
//---------------------------------------------------------------------------
	class NeoPixelsController
	{
	public:
		virtual void AddPixelGroup( NeoPixelsCommonGroup *AGroup ) = 0;
		virtual void SetPixelColor( int AIndex, TRGBWColor AColor ) = 0;
		virtual TRGBWColor GetPixelColor( int AIndex ) = 0;

	};
//---------------------------------------------------------------------------
	class NeoPixelsCommonGroup : public OpenWire::Component
	{
	public:
		NeoPixelsController	&FOwner;
		int			FStartPixel;

	public:
		virtual void StartPixels( int &AStartPixel )
		{
			FStartPixel = AStartPixel;
		}

		virtual void PixelsClock( unsigned long currentMicros )
		{
		}

	public:
		NeoPixelsCommonGroup( NeoPixelsController &AOwner );

	};
//---------------------------------------------------------------------------
	class NeoPixelsBasicGroup : public NeoPixelsCommonGroup
	{
		typedef NeoPixelsCommonGroup inherited;

	public:
		uint32_t	CountPixels = 10;

	public:
		virtual void StartPixels( int &AStartPixel )
		{
			inherited::StartPixels( AStartPixel );
			AStartPixel += CountPixels;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class NeoPixels : public OpenWire::Component, public NeoPixelsController, public Mitov::ClockingSupport
	{
		typedef OpenWire::Component inherited;

	public:
		float   Brightness = 1.0f;
		Mitov::SimpleObjectList<NeoPixelsCommonGroup*>	PixelGroups;

	public:
		bool	FModified;

	protected:
		int	FPinNumber;

	public:
		void SetBrightness( float AValue )
		{
			if( Brightness == AValue )
				return;

			IntSetBrightness( AValue );
		}

	public:
		virtual void AddPixelGroup( NeoPixelsCommonGroup *AGroup ) override
		{
			PixelGroups.push_back( AGroup );
		}

		virtual void SetPixelColor( int AIndex, TRGBWColor AColor ) override
		{
			FPixel.setPixelColor( AIndex, AColor.Red, AColor.Green, AColor.Blue, AColor.White );
//			FPixel.setPixelColor( AIndex, 255, AColor.Green, AColor.Blue, AColor.White );
			FModified = true;
		}

		virtual TRGBWColor GetPixelColor( int AIndex ) override
		{
			return TRGBWColor( FPixel.getPixelColor( AIndex ), true );
		}

	protected:
		MitovEmbedded_Adafruit_NeoPixel	&FPixel;

	protected:
		void IntSetBrightness( float AValue )
		{
			Brightness = AValue;
			FPixel.setBrightness( AValue * 255 );
			FModified = true;
		}

		virtual void SystemInit() override
		{
			FPixel.setPin( FPinNumber );

			FPixel.begin();
			IntSetBrightness( Brightness );

			int AStartPixel = 0;
			for( int i = 0; i < PixelGroups.size(); ++i )
				PixelGroups[ i ]->StartPixels( AStartPixel );
			

			inherited::SystemInit();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			for( int i = 0; i < PixelGroups.size(); ++i )
				PixelGroups[ i ]->PixelsClock( currentMicros );

			inherited::SystemLoopBegin( currentMicros );
		}

		virtual void SystemLoopEnd() override
		{
			if( FModified )
				if( ! ClockInputPin.IsConnected())
				{
					FPixel.show();
					FModified = false;
				}

			inherited::SystemLoopEnd();
		}

		virtual void DoClockReceive(void *) override
		{
			if( FModified )
			{
				FPixel.show();
				FModified = false;
			}
		}

	public:
		NeoPixels( int APinNumber, MitovEmbedded_Adafruit_NeoPixel &APixel ) :
			FPinNumber( APinNumber ),
		    FPixel( APixel )
		{
		}

	};
//---------------------------------------------------------------------------
	class NeoPixelsBasicInitialColorGroup : public NeoPixelsBasicGroup
	{
		typedef NeoPixelsBasicGroup inherited;

	public:
		TRGBWColor	InitialColor;

		virtual void StartPixels( int &AStartPixel )
		{
			inherited::StartPixels( AStartPixel );

			for( uint32_t i = 0; i < CountPixels; ++i )
				FOwner.SetPixelColor( FStartPixel + i, InitialColor );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class NeoPixelsBasicColorGroup : public NeoPixelsBasicInitialColorGroup
	{
		typedef NeoPixelsBasicInitialColorGroup inherited;

	protected:
		TRGBWColor	FColor;

	public:
		OpenWire::SinkPin	ColorInputPin;

	public:
		void SetInitialColor( TRGBWColor AValue )
		{
			if( InitialColor == AValue )
				return;

			InitialColor = AValue;
			FColor = AValue;
			ApplyColorsAll(); 
//			FOwner.FModified = true;
		}

	protected:
		virtual void ApplyColorsAll()
		{
			for( uint32_t i = 0; i < CountPixels; ++i )
				FOwner.SetPixelColor( FStartPixel + i, FColor );
		}

		virtual void ApplyColors() {}

		void IntSetColor( TRGBWColor AValue )
		{
			if( FColor == AValue )
				return;

			FColor = AValue;
			ApplyColors(); 
//			FOwner.FModified = true;
		}

		void DoReceiveColor( void *_Data )
		{
			IntSetColor( *(TRGBWColor *)_Data );
		}

	protected:
		virtual void StartPixels( int &AStartPixel )
		{
			inherited::StartPixels( AStartPixel );
//			Serial.println( CountPixels );
			IntSetColor( InitialColor );
//			FOwner.FModified = true;
//			Serial.println( FStartPixel );
		}

	public:
		NeoPixelsBasicColorGroup( NeoPixelsController &AOwner ) :
			inherited( AOwner )
		{
			ColorInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&NeoPixelsBasicColorGroup::DoReceiveColor );
		}

	};
//---------------------------------------------------------------------------
	class NeoPixelsGroup : public NeoPixelsBasicInitialColorGroup
	{
		typedef NeoPixelsBasicInitialColorGroup inherited;

	protected:
		class PixelValueSinkPin : public OpenWire::ValueSimpleSinkPin<TRGBWColor>
		{
			typedef OpenWire::ValueSimpleSinkPin<TRGBWColor> inherited;

		public:
			NeoPixelsGroup *FOwner;
			int				FIndex;

		public:
			virtual void Receive( void *_Data )
			{
				TRGBWColor AValue = *(TRGBWColor *)_Data;
				if( AValue != Value )
					FOwner->FOwner.SetPixelColor( FIndex, AValue );

				inherited::Receive( _Data );
			}
		};

	public:
		Mitov::SimpleList<PixelValueSinkPin> InputPins;

	protected:
		virtual void StartPixels( int &AStartPixel )
		{
			inherited::StartPixels( AStartPixel );
			for( int i = 0; i < InputPins.size(); ++i )
			{
				InputPins[ i ].FOwner = this;
				InputPins[ i ].FIndex = FStartPixel + i;
				InputPins[ i ].Value = InitialColor;
//				FOwner.SetPixelColor( FStartPixel + i, InitialColor );
//				Iter->SetCallback( this, (OpenWire::TOnPinReceive)&NeoPixelsGroup::DoReceive );
//				Iter->Value = T_VALUE;
			}

		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class NeoPixelsRepeatGroup : public NeoPixelsBasicInitialColorGroup, public NeoPixelsController
	{
		typedef NeoPixelsBasicInitialColorGroup inherited;

	public:
		Mitov::SimpleObjectList<NeoPixelsCommonGroup*>	PixelGroups;

	protected:
		int FSubPixelCount;
		int FRepeatCount;

	public:
		virtual void AddPixelGroup( NeoPixelsCommonGroup *AGroup ) override
		{
			PixelGroups.push_back( AGroup );
		}

		virtual void SetPixelColor( int AIndex, TRGBWColor AColor ) override
		{
			for( int i = 0; i < FRepeatCount; ++i )
				FOwner.SetPixelColor( FStartPixel + AIndex + i * FSubPixelCount, AColor );
		}

		virtual TRGBWColor GetPixelColor( int AIndex ) override
		{
			return FOwner.GetPixelColor( FStartPixel + AIndex );
		}

	public:
		virtual void StartPixels( int &AStartPixel ) 
		{
			inherited::StartPixels( AStartPixel );

			FSubPixelCount = 0;
			for( int i = 0; i < PixelGroups.size(); ++i )
				PixelGroups[ i ]->StartPixels( FSubPixelCount );

			if( FSubPixelCount == 0 )
				FRepeatCount = 0;

			else
				FRepeatCount = ( CountPixels + FSubPixelCount - 1 ) / FSubPixelCount;

		}

		virtual void PixelsClock( unsigned long currentMicros )
		{
			inherited::PixelsClock( currentMicros );

			for( int i = 0; i < PixelGroups.size(); ++i )
				PixelGroups[ i ]->PixelsClock( currentMicros );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class NeoPixelsSingleColorGroup : public NeoPixelsBasicColorGroup
	{
		typedef NeoPixelsBasicColorGroup inherited;

	protected:
		virtual void ApplyColors() override
		{
			ApplyColorsAll();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class NeoPixelsReversedProperty
	{
	protected:
		NeoPixelsRunningColorGroup &FOwner;

	public:
		bool Reversed : 1;
		bool AllPixels : 1;

	public:
		void SetReversed( bool AValue );

	public:
		NeoPixelsReversedProperty( NeoPixelsRunningColorGroup &AOwner ) :
			FOwner( AOwner ),
			Reversed( false ),
			AllPixels( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class NeoPixelsRunningColorGroup : public NeoPixelsBasicColorGroup
	{
		typedef NeoPixelsBasicColorGroup inherited;

	public:
		OpenWire::ConnectSinkPin	StepInputPin;
		OpenWire::SourcePin	ColorOutputPin;

	public:
		NeoPixelsReversedProperty	Reversed;

	public:
		void ReversePixels()
		{
			for( uint32_t i = 0; i < CountPixels / 2; ++i )
			{
				TRGBWColor AOldColor1 = FOwner.GetPixelColor( FStartPixel + ( CountPixels - i - 1 ));
				TRGBWColor AOldColor2 = FOwner.GetPixelColor( FStartPixel + i );

				FOwner.SetPixelColor( FStartPixel + i, AOldColor1 );
				FOwner.SetPixelColor( FStartPixel + ( CountPixels - i - 1 ), AOldColor2 );
			}
		}

	protected:
		void AnimatePixels()
		{
			if( Reversed.Reversed )
			{
				TRGBWColor AOldColor = FOwner.GetPixelColor( FStartPixel );
				ColorOutputPin.Notify( &AOldColor );
				for( uint32_t i = 0; i < CountPixels - 1; ++i )
				{
					AOldColor = FOwner.GetPixelColor( FStartPixel + i + 1 );
					FOwner.SetPixelColor( FStartPixel + i, AOldColor );
				}

				FOwner.SetPixelColor( FStartPixel + CountPixels - 1, FColor );
			}

			else
			{
				TRGBWColor AOldColor = FOwner.GetPixelColor( FStartPixel + CountPixels - 1 );
				ColorOutputPin.Notify( &AOldColor );
				for( int i = CountPixels - 1; i--; )
				{
					AOldColor = FOwner.GetPixelColor( FStartPixel + i );
					FOwner.SetPixelColor( FStartPixel + i + 1, AOldColor );
				}

				FOwner.SetPixelColor( FStartPixel, FColor );
			}
//			FOwner.FModified = true;
		}

		void DoReceiveStep( void *_Data )
		{
			AnimatePixels();
		}

	protected:
		virtual void PixelsClock( unsigned long currentMicros ) override
		{
			if( StepInputPin.IsConnected())
				return;

			AnimatePixels();
		}

	public:
		NeoPixelsRunningColorGroup( NeoPixelsController &AOwner ) :
			inherited( AOwner ),
			Reversed( *this )
		{
			StepInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&NeoPixelsRunningColorGroup::DoReceiveStep );
		}
	};
//---------------------------------------------------------------------------
	inline void NeoPixelsReversedProperty::SetReversed( bool AValue )
	{
		if( Reversed == AValue )
			return;

		Reversed = AValue;
		if( AllPixels )
			FOwner.ReversePixels();

	}
//---------------------------------------------------------------------------
	class NeoPixelsColorPixelGroup : public NeoPixelsBasicColorGroup
	{
		typedef NeoPixelsBasicColorGroup inherited;

	public:
		OpenWire::SinkPin	IndexInputPin;

	public:
		uint32_t InitialIndex = 0;

	protected:
		uint32_t	FIndex;
		bool		FModified;

	protected:
		void DoReceiveIndex( void *_Data )
		{
			uint32_t AIndex = *(uint32_t *)_Data;
			if( AIndex > CountPixels )
				AIndex = CountPixels;

			if( FIndex == AIndex )
				return;

			FIndex = AIndex;
			FModified = true;
		}

		virtual void PixelsClock( unsigned long currentMicros )
		{
			if( FModified )
			{
				FOwner.SetPixelColor( FStartPixel + FIndex, FColor );
//				FOwner.FModified = true;
				FModified = false;
			}
		}

		virtual void ApplyColors() 
		{
			FModified = true;
		}

	public:
		NeoPixelsColorPixelGroup( NeoPixelsController &AOwner ) :
			inherited( AOwner )
		{
			IndexInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&NeoPixelsColorPixelGroup::DoReceiveIndex );
		}
	};
//---------------------------------------------------------------------------
//#if 0
	template<int WIDTH, int HEIGHT> class NeoPixels2DGraphicsGroup : public NeoPixelsCommonGroup, public GraphicsImpl
	{
		typedef NeoPixelsCommonGroup inherited;

	public:
		TGraphicsRGBWTextSettings	Text;

	public:
		virtual int16_t width(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return WIDTH;

			return HEIGHT;
		}

		virtual int16_t height(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return WIDTH;

			return HEIGHT;
		}

	public:
		virtual void drawGrayscalePixel(int16_t x, int16_t y, TUniColor color) override
		{
			uint16_t AGray = ( color & 0xFF );
			drawPixel( x, y, 0xFF00000000 | ( color << 16 ) | ( AGray << 8 ) | AGray );
		}

		virtual void drawColorPixel(int16_t x, int16_t y, TUniColor color) override
		{
			uint64_t AAlpha = ( color & 0xFF000000 );
			drawPixel( x, y, ( AAlpha << 8 ) | ( color & 0xFFFFFF ) );
		}

		virtual void drawPixel(int16_t x, int16_t y, TUniColor color) override
		{
			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return;

//			color = 0xFFFFFF0000;
			uint8_t AAlpha = color >> 32;
			if( AAlpha == 0 )
				return;

//			Serial.print( x ); Serial.print( ":" ); Serial.print( y ); Serial.print( " = " ); Serial.println( uint32_t( color ), HEX ); 

			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = WIDTH - x - 1;
					break;

				case goDown:
					x = WIDTH - x - 1;
					y = HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = HEIGHT - y - 1;
					break;
			}  

			color &= 0xFFFFFFFF;

			if( AAlpha < 255 )
			{
				float ACoefficient = float( AAlpha ) / 255.0f;
				TUniColor AOldColor = FOwner.GetPixelColor( FStartPixel + x + y * WIDTH );

				uint8_t AWhite = (( AOldColor >> 24 ) & 0xFF ) * ( 1 - ACoefficient ) + (( color >> 24 ) & 0xFF ) * ACoefficient + 0.5;
				uint8_t ARed = (( AOldColor >> 16 ) & 0xFF ) * ( 1 - ACoefficient ) + (( color >> 16 ) & 0xFF ) * ACoefficient + 0.5;
				uint8_t AGreen = (( AOldColor >> 8 ) & 0xFF ) * ( 1 - ACoefficient ) + (( color >> 8 ) & 0xFF ) * ACoefficient + 0.5;
				uint8_t ABlue = ( AOldColor & 0xFF ) * ( 1 - ACoefficient ) + ( color & 0xFF ) * ACoefficient + 0.5;

				color = ( uint32_t( AWhite ) << 24 ) | ( uint32_t( ARed ) << 16 ) | ( uint32_t( AGreen ) << 8 ) | ABlue;
			}

//			color = 0xFFFF0000;
//			Serial.print( FStartPixel + x + y * WIDTH ); Serial.print( " = " ); Serial.println( uint32_t( color ));
			FOwner.SetPixelColor( FStartPixel + x + y * WIDTH, color );
//			FBuffer[ x + y * WIDTH ] = color;
//			FModified = true;
		}

		virtual TUniColor GetPixelColor( int16_t x, int16_t y ) override
		{
			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return 0xFF00000000;

			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = WIDTH - x - 1;
					break;

				case goDown:
					x = WIDTH - x - 1;
					y = HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = HEIGHT - y - 1;
					break;
			}  

			return TUniColor( FOwner.GetPixelColor( FStartPixel + x + y * WIDTH )) | 0xFF00000000;
//			return TUniColor( FBuffer[ x + y * WIDTH ] ) | 0xFF00000000;
		}

	public:
		virtual void StartPixels( int &AStartPixel )
		{
			inherited::StartPixels( AStartPixel );
//return;
			AStartPixel += WIDTH * HEIGHT;

			setTextSize( Text.Size );
			setTextColor( Text.Color, Text.BackgroundColor );
			setTextWrap( Text.Wrap );

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Render( false );
		}

/*
		virtual void PixelsClock( unsigned long currentMicros )
		{
			if( FModified )
			{
				for( int i = 0; i < WIDTH * HEIGHT; ++i )
				{
//					Serial.print( i ); Serial.print( " = " ); Serial.println( FBuffer[ i ], HEX ); 
					FOwner.SetPixelColor( FStartPixel + i, FBuffer[ i ] );
				}

				FModified = false;
			}
		}
*/
	public:
		NeoPixels2DGraphicsGroup( NeoPixelsController &AOwner, const unsigned char * AFont ) :
			inherited( AOwner ),
			GraphicsImpl( AFont )
		{
		}
		
	};
	//---------------------------------------------------------------------------
	NeoPixelsCommonGroup::NeoPixelsCommonGroup( NeoPixelsController &AOwner ) :
		FOwner( AOwner )
	{
		AOwner.AddPixelGroup( this );
	}
//---------------------------------------------------------------------------
}

#endif

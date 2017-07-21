////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_IS31FL3731_h
#define _MITOV_DISPLAY_IS31FL3731_h

#include <Mitov.h>
#include <Mitov_Graphics_Grayscale.h>
#include <Mitov_Graphics_Monochrome.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class DisplayIS31FL3731;
	class DisplayIS31FL3731AnimationElement;
	class DisplayIS31FL3731BasicPictureElement;
//---------------------------------------------------------------------------
	class DisplayIS31FL3731BasicElement
	{
	public:
		DisplayIS31FL3731	&FOwner;

	public:
		bool	Enabled = true;

	public:
		virtual void AssignFrames( uint8_t &AFrame ) {}
		virtual void SystemInitStart( bool &AStarted ) {}
		virtual void Refresh() {}

	public:
		DisplayIS31FL3731BasicElement( DisplayIS31FL3731 &AOwner );

	};
//---------------------------------------------------------------------------
/*
	class TArduinoLedDriverIS31FL3731Blink
	{
	public:
		bool	Enabled : 1;
		uint8_t	Period : 3;

	public:
		TArduinoLedDriverIS31FL3731Blink() :
			Enabled( false ),
			Period( 0 )
		{
		}
	};
*/
//---------------------------------------------------------------------------
	enum TArduinoLedDriverIS31FL3731AudioGainMode { igmDisabled, igmSlow, igmFast };
	enum TArduinoLedDriverIS31FL3731AudioGain { isg0dB, isg3dB, isg6dB, isg9dB, isg12dB, isg15dB, isg18dB, isg21dB };
//---------------------------------------------------------------------------
	class TArduinoLedDriverIS31FL3731AudioModulation
	{
	public:
		bool	Enabled : 1;
		TArduinoLedDriverIS31FL3731AudioGainMode	GainMode : 2;
		TArduinoLedDriverIS31FL3731AudioGain Gain : 3;

	public:
		TArduinoLedDriverIS31FL3731AudioModulation() :
			Enabled( false ),
			GainMode( igmDisabled ),
			Gain( isg0dB )
		{
		}
	};
//---------------------------------------------------------------------------
	class DisplayIS31FL3731 : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
        bool Enabled : 1;
		uint8_t	BlinkPeriod : 3;
		uint8_t	Address = 0x74;

	public:
		TArduinoLedDriverIS31FL3731AudioModulation	AudioModulation;
//		TArduinoLedDriverIS31FL3731Blink	Blink;

	public:
		Mitov::SimpleList<DisplayIS31FL3731BasicElement *>	FElements;

	public:
		TwoWire	&FWire;

	public:
		static const uint8_t	REG_CONFIG  = 0x00;
		static const uint8_t	REG_CONFIG_PICTUREMODE = 0x00;
		static const uint8_t	REG_CONFIG_AUTOPLAYMODE = 0x08;
		static const uint8_t	REG_CONFIG_AUDIOPLAYMODE = 0x10;

		static const uint8_t	REG_PICTUREFRAME = 0x01;

		static const uint8_t	REG_SHUTDOWN = 0x0A;

		static const uint8_t	COMMANDREGISTER = 0xFD;
		static const uint8_t	AUTO_PLAY_CONTROL_REGISTER1 = 0x02;
		static const uint8_t	AUTO_PLAY_CONTROL_REGISTER2 = 0x03;
		static const uint8_t	DISPLAY_OPTION_REGISTER = 0x05;
		static const uint8_t	AUDIO_SYNCHRONIZATION_REGISTER = 0x06;
		static const uint8_t	BREATH_CONTROL_REGISTER1 = 0x08;
		static const uint8_t	BREATH_CONTROL_REGISTER2 = 0x09;
		static const uint8_t	AGC_CONTROL_REGISTER = 0x0B;
		static const uint8_t	AUDIO_ADC_RATE_REGISTER = 0x0C;
		static const uint8_t	BANK_FUNCTIONREG = 0x0B;    // helpfully called 'page nine'

	public:
		static const uint8_t WIDTH = 16;
		static const uint8_t HEIGHT = 9;

	public:
		void SelectBank( uint8_t b ) 
		{
			I2C::WriteByte( Address, COMMANDREGISTER, b );
		}

		void WriteRegister8(uint8_t b, uint8_t reg, uint8_t data)
		{
			SelectBank( b );
			I2C::WriteByte( Address, reg, data );
		}

		void UpdateAssignFrames()
		{
			uint8_t AFrame = 0;
			for( int i = 0; i < FElements.size(); ++i )
				FElements[ i ]->AssignFrames( AFrame );
		}


		void ReassignFrames( DisplayIS31FL3731BasicElement *AStartElement )
		{
			int AStartIndex = FElements.IndexOf( AStartElement );

			uint8_t AFrame = 0;
			int ASize = FElements.size();
			for( int i = 0; i < ASize; ++i )
				FElements[ ( i + AStartIndex ) % ASize ]->AssignFrames( AFrame );

		}

		void UpdateAudioModulation()
		{
			uint8_t AData;

			if( AudioModulation.Enabled )
				AData = ( 0b10 | (( AudioModulation.GainMode == igmFast ) ? 0b100 : 0 ) | AudioModulation.Gain );

			else
				AData = 0;

			WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::AGC_CONTROL_REGISTER, AData );
			WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::AUDIO_SYNCHRONIZATION_REGISTER, AudioModulation.Enabled ? 1 : 0 );
		}

/*
		void UpdateBlink()
		{
//			return;
			uint8_t	AData = 0b100000 | (( Blink.Enabled ) ? 0b1000 : 0 ) | Blink.Period;
			WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::DISPLAY_OPTION_REGISTER, AData );
		}
*/
	protected:
		virtual void SystemLoopEnd() override
		{
			for( int i = 0; i < FElements.size(); ++i )
				FElements[ i ]->Refresh();

//			if( FModified )
//				if( ! RefreshInputPin.IsConnected() )
//					display();

		}

	public:
		virtual void SystemInit() override // Make it public so it can be accessed by Enabled
		{
			// shutdown
			WriteRegister8( BANK_FUNCTIONREG, REG_SHUTDOWN, 0x00 );

			if( ! Enabled )
				return;

			delay(10);

			// out of shutdown
			WriteRegister8( BANK_FUNCTIONREG, REG_SHUTDOWN, 0x01 );




//			uint8_t	AData = 0b100000 | (( Blink.Enabled ) ? 0b1000 : 0 ) | Blink.Period;
//			uint8_t	AData = 0b100000 | 0b1000;
			uint8_t	AData = 0b1000 | BlinkPeriod;
			WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::DISPLAY_OPTION_REGISTER, AData );

/*
			SelectBank( 0 );

			FWire.beginTransmission( Address );

			FWire.write( 0x12 );
			FWire.write( 2 );
			for( int i = 1; i < 16; ++i)
				FWire.write( 0 );

			FWire.endTransmission();
*/

			UpdateAssignFrames();
//			UpdateBlink();
			UpdateAudioModulation();

			bool AStarted = false;
			for( int i = 0; i < FElements.size(); ++i )
				FElements[ i ]->SystemInitStart( AStarted );
				
			for (uint8_t f=0; f<8; f++)
				for (uint8_t i=0; i<=0x11; i++)
					WriteRegister8(f, i, 0xff);     // each 8 LEDs on

//			UpdateFadeEffect();
//			display();
		}

	public:
		DisplayIS31FL3731( TwoWire &AWire ) :
			Enabled( true ),
			BlinkPeriod( 2 ),
			FWire( AWire )
		{
		}
	};
//---------------------------------------------------------------------------
	class TArduinoLedDriverIS31FL3731FadeEffect
	{
	public:
		bool	Enabled : 1;
		uint8_t	FadeIn : 3;
		uint8_t	FadeOut : 3;
		uint8_t	ExtinguishTime : 3;

	public:
		TArduinoLedDriverIS31FL3731FadeEffect() :
			Enabled( false ),
			FadeIn( 0 ),
			FadeOut( 0 ),
			ExtinguishTime( 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayIS31FL3731BasicBlink
	{
	public:
		bool	Enabled = false;

	protected:
		TwoWire	&FWire;

	public:
		virtual void PopulateReg()
		{
			if( Enabled )
			{
				for( int i = 0; i < 18; ++i)
					FWire.write( 0xFF );

			}

			else
			{
				for( int i = 0; i < 18; ++i)
					FWire.write( 0 );

			}
		}

		virtual void SetFrame()
		{
		}

	public:
		DisplayIS31FL3731BasicBlink( DisplayIS31FL3731BasicPictureElement &AOwner, void * );

	};
//---------------------------------------------------------------------------
	class DisplayIS31FL3731Blink : public DisplayIS31FL3731BasicBlink, public GraphicsImpl
	{
		typedef DisplayIS31FL3731BasicBlink inherited;

	public:
		TGraphicsMonochromeTextSettings	Text;

	protected:
		bool	FModified = true;

	public:
		uint8_t	FPicture[ DisplayIS31FL3731::WIDTH * ( DisplayIS31FL3731::HEIGHT + 7 ) / 8 ];

//	protected:
//		DisplayIS31FL3731BasicPictureElement &FOwner

	public:
		virtual void PopulateReg() override
		{
			if( Enabled )
			{
				for( int i = 0; i < 18; ++i)
					FWire.write( FPicture[ i ] );

			}

			else
			{
				for( int i = 0; i < 18; ++i)
					FWire.write( 0 );

			}
		}

		virtual void SetFrame() override
		{
			setTextSize( Text.Size );
			setTextColor( Text.Color, Text.BackgroundColor );
			setTextWrap( Text.Wrap );

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Render( false );
		}

	public:
		virtual int16_t width(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return DisplayIS31FL3731::WIDTH;

			return DisplayIS31FL3731::HEIGHT;
		}

		virtual int16_t height(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return DisplayIS31FL3731::HEIGHT;

			return DisplayIS31FL3731::WIDTH;
		}

	public:
		virtual void drawPixel(int16_t x, int16_t y, TUniColor color) override
		{
			if( color == tmcNone )
				return;

			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return;

			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = DisplayIS31FL3731::WIDTH - x - 1;
					break;

				case goDown:
					x = DisplayIS31FL3731::WIDTH - x - 1;
					y = DisplayIS31FL3731::HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = DisplayIS31FL3731::HEIGHT - y - 1;
					break;
			}

			// x is which column
			switch (color) 
			{
				case tmcWhite :		FPicture[(x/8) + y * ( DisplayIS31FL3731::WIDTH / 8 ) ] |=  (1 << (x&7)); break;
				case tmcBlack :		FPicture[(x/8) + y * ( DisplayIS31FL3731::WIDTH / 8 ) ] &= ~(1 << (x&7)); break; 
				case tmcInvert :	FPicture[(x/8) + y * ( DisplayIS31FL3731::WIDTH / 8 ) ] ^=  (1 << (x&7)); break; 
			}

			FModified = true;
		}

		virtual TUniColor GetPixelColor( int16_t x, int16_t y ) override
		{
			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return false;

			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = DisplayIS31FL3731::WIDTH - x - 1;
					break;

				case goDown:
					x = DisplayIS31FL3731::WIDTH - x - 1;
					y = DisplayIS31FL3731::HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = DisplayIS31FL3731::HEIGHT - y - 1;
					break;
			}

			return (( FPicture[ ( x / 8 ) + y * ( DisplayIS31FL3731::WIDTH / 8 ) ] & ( 1 << ( x & 7 ))) != 0 );
		}

	public:
		DisplayIS31FL3731Blink( DisplayIS31FL3731BasicPictureElement &AOwner,  const unsigned char * AFont ) :
			inherited( AOwner, nullptr ),
			GraphicsImpl( AFont )
		{
			memset( FPicture, 0, sizeof( FPicture ));
		}
	};
//---------------------------------------------------------------------------
	class DisplayIS31FL3731BasicPictureElement : public GraphicsGrayscale
	{
		typedef GraphicsGrayscale inherited;

	public:
//		uint8_t	BackgroundColor = 0;
		TGraphicsGrayscaleTextSettings	Text;

	protected:
		uint8_t	FPicture[ DisplayIS31FL3731::WIDTH * DisplayIS31FL3731::HEIGHT ];

	protected:
		bool	FModified = false;

	public:
		TwoWire	&FWire;

	public:
		DisplayIS31FL3731BasicBlink *FBlink = nullptr;

	protected:
		virtual int16_t width(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return DisplayIS31FL3731::WIDTH;

			return DisplayIS31FL3731::HEIGHT;
		}

		virtual int16_t height(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return DisplayIS31FL3731::HEIGHT;

			return DisplayIS31FL3731::WIDTH;
		}

	public:
		virtual void drawPixel(int16_t x, int16_t y, TUniColor color) override
		{
			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return;

//			Serial.println( color );
			uint8_t AAlpha = color >> 8;
			if( AAlpha == 0 )
				return;

			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = DisplayIS31FL3731::WIDTH - x - 1;
					break;

				case goDown:
					x = DisplayIS31FL3731::WIDTH - x - 1;
					y = DisplayIS31FL3731::HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = DisplayIS31FL3731::HEIGHT - y - 1;
					break;
			}  

			// charlie wing is smaller
			//if (x > 15) return;
			//if (y > 7) return;

/*
			if (x > 7) {
			x=15-x;
			y += 8;
			} else {
			y = 7-y;
			}

			_swap_int16_t(x, y);

 
			if ((x < 0) || (x >= 16)) return;
			if ((y < 0) || (y >= 9)) return;
			if (color > 255) color = 255; // PWM 8bit max
*/
//			setLEDPWM(x + y*16, color, _frame);
//			FOwner.WriteRegister8( ( FFrame ) ? 0 : 1 , 0x24 + x + y * DisplayIS31FL3731::WIDTH, color );

			color &= 0xFF;
//			Serial.println( AAlpha );

			if( AAlpha < 255 )
			{
				float ACoefficient = float( AAlpha ) / 255.0f;
				color = (color * ACoefficient + FPicture[ x + y * DisplayIS31FL3731::WIDTH ] * ( 1 - ACoefficient )) + 0.5;
			}
//				color = MitovMax( color , FPicture[ x + y * DisplayIS31FL3731::WIDTH ] );

//			Serial.println( "Color" );
//			Serial.println( color );
			FPicture[ x + y * DisplayIS31FL3731::WIDTH ] = color;
			FModified = true;
			return;
		}

		virtual TUniColor GetPixelColor( int16_t x, int16_t y ) override
		{
			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return 0xFF00;

			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = DisplayIS31FL3731::WIDTH - x - 1;
					break;

				case goDown:
					x = DisplayIS31FL3731::WIDTH - x - 1;
					y = DisplayIS31FL3731::HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = DisplayIS31FL3731::HEIGHT - y - 1;
					break;
			}  

			return FPicture[ x + y * DisplayIS31FL3731::WIDTH ] | 0xFF00;
		}

	protected:
		void RenderImage( DisplayIS31FL3731	&AOwner )
		{
			uint8_t *AData = FPicture;
			for( int y = 0; y < DisplayIS31FL3731::HEIGHT; ++y )
			{
				FWire.beginTransmission( AOwner.Address );
				FWire.write( 0x24 + y * DisplayIS31FL3731::WIDTH );
				for( int x = 0; x < DisplayIS31FL3731::WIDTH; ++x )
					FWire.write( *AData ++ );

				FWire.endTransmission();
			}

			FWire.beginTransmission( AOwner.Address );
			FWire.write( 0x12 );
			bool APopulate = ( FBlink != nullptr );
			if( APopulate )
				APopulate = FBlink->Enabled;

			if( APopulate )
				FBlink->PopulateReg();

			else
			{
				for( int i = 0; i < 18; ++i)
					FWire.write( 0 );

			}

			FWire.endTransmission();
		}

	public:
		DisplayIS31FL3731BasicPictureElement( TwoWire &AWire, const unsigned char * AFont ) :
			inherited( AFont ),
			FWire( AWire )
		{
			memset( FPicture, 0, sizeof( FPicture ));
		}

	};
//---------------------------------------------------------------------------
	class DisplayIS31FL3731FrameElement : public DisplayIS31FL3731BasicPictureElement
	{
		typedef DisplayIS31FL3731BasicPictureElement inherited;

	public:
		OpenWire::ConnectSinkPin	RefreshInputPin;

	protected:
		uint8_t	FFrame;
		DisplayIS31FL3731	&FOwner;

	public:
		void SetFrame( uint8_t	AFrame )
		{
			setTextSize( Text.Size );
			setTextColor( Text.Color, Text.BackgroundColor );
			setTextWrap( Text.Wrap );

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Render( false );

			if( FBlink )
				FBlink->SetFrame();

			FFrame = AFrame;
			Render();
		}

		void Render()
		{
//			Serial.println( "display" );
//			Serial.println( ( FFrame ) ? FFrame0 : FFrame1 );
//			FOwner.SelectBank( DisplayIS31FL3731::BANK_FUNCTIONREG );
			FOwner.SelectBank( FFrame );
			RenderImage( FOwner );

//			FOwner.WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::REG_PICTUREFRAME, ( FFrame ) ? FFrame0 : FFrame1 );
			FModified = false;
		}

		void DoRefreshReceived( void * )
		{
			if( FModified )
				Render();
		}

	public:
		DisplayIS31FL3731FrameElement( DisplayIS31FL3731AnimationElement &AOwner, const unsigned char * AFont );
	};
//---------------------------------------------------------------------------
	class TArduinoLedDriverIS31FL3731AudioControl
	{
	public:
		bool	Enabled : 1;
		uint16_t	SamplingInterval : 14;

	public:
		uint8_t GetRateValue()
		{
			uint16_t AValue = float( SamplingInterval ) / 64 + 0.5;
			return AValue;
		}

	public:
		TArduinoLedDriverIS31FL3731AudioControl() :
			Enabled( false ),
			SamplingInterval( 16383 )
		{
		}
	};
//---------------------------------------------------------------------------
	class DisplayIS31FL3731AnimationElement : public DisplayIS31FL3731BasicElement, public ClockingSupport
	{
		typedef DisplayIS31FL3731BasicElement inherited;

	public:
		TArduinoLedDriverIS31FL3731AudioControl	AudioControl;
		TArduinoLedDriverIS31FL3731FadeEffect	FadeEffect;
		uint8_t	Loops : 3;
		uint8_t	FrameDelay : 6;

	protected:
		bool    FFrameAssigned : 1;
		uint8_t	FFrameStart : 3;

	public:
		Mitov::SimpleList<DisplayIS31FL3731FrameElement *>	FFrames;

	public:
		void UpdateFadeEffect()
		{
			uint8_t	AData = ( FadeEffect.FadeOut << 4 ) | FadeEffect.FadeIn;
			FOwner.WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::BREATH_CONTROL_REGISTER1, AData );

			AData = ( Enabled ? 0b10000 : 0 ) | FadeEffect.ExtinguishTime;
			FOwner.WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::BREATH_CONTROL_REGISTER2, AData );
		}

		void UpdateLoop()
		{
			if( AudioControl.Enabled )
				FOwner.WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::AUDIO_ADC_RATE_REGISTER, AudioControl.GetRateValue() );


			uint8_t	AData = ( ( AudioControl.Enabled ) ? DisplayIS31FL3731::REG_CONFIG_AUDIOPLAYMODE : DisplayIS31FL3731::REG_CONFIG_AUTOPLAYMODE ) | FFrameStart;

			FOwner.WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::REG_CONFIG, AData );

			AData = ( Loops << 4 ) | ( FFrames.size() & 0b111 );
			FOwner.WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::AUTO_PLAY_CONTROL_REGISTER1, AData );

			FOwner.WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::AUTO_PLAY_CONTROL_REGISTER2, FrameDelay );
		}

	public:
		virtual void SystemInitStart( bool &AStarted ) override
		{
			if( !Enabled )
				return;

			for( int i = 0; i < FFrames.size(); ++ i )
				FFrames[ i ]->SetFrame( FFrameStart + i );

			if( AStarted )
				return;

			if( ClockInputPin.IsConnected() )
				return;

			AStarted = true;

			DoClockReceive( nullptr );
		}

		virtual void AssignFrames( uint8_t &AFrame ) override
		{
			if( AFrame > 7 - MitovMin<uint8_t>( 7, FFrames.size() ) )
			{
				FFrameAssigned = false;
				return;
			}

			FFrameAssigned = true;
			FFrameStart = AFrame;
			AFrame += FFrames.size();
		}

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			if( ! Enabled )
				return;

			if( ! FFrameAssigned )
				FOwner.ReassignFrames( this );

			UpdateFadeEffect();
			UpdateLoop();
			// picture mode
		}

	public:
		DisplayIS31FL3731AnimationElement( DisplayIS31FL3731 &AOwner ) :
			inherited( AOwner ),
			Loops( 0 ),
			FrameDelay( 0 ),
			FFrameAssigned( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class DisplayIS31FL3731PictureElement : public DisplayIS31FL3731BasicElement, public DisplayIS31FL3731BasicPictureElement, public ClockingSupport
	{
		typedef DisplayIS31FL3731BasicElement inherited;

	public:
		OpenWire::ConnectSinkPin	RefreshInputPin;

	public:
		TArduinoLedDriverIS31FL3731FadeEffect	FadeEffect;

	protected:
		bool    FFrameAssigned : 1;
		bool	FFrame : 1;
		uint8_t	FFrame0 : 3;
		uint8_t	FFrame1 : 3;

	public:
		void UpdateFadeEffect()
		{
			uint8_t	AData = ( FadeEffect.FadeOut << 4 ) | FadeEffect.FadeIn;
			FOwner.WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::BREATH_CONTROL_REGISTER1, AData );

			AData = ( Enabled ? 0b10000 : 0 ) | FadeEffect.ExtinguishTime;
			FOwner.WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::BREATH_CONTROL_REGISTER2, AData );
		}

	public:
		virtual void AssignFrames( uint8_t &AFrame ) override
		{
			if( AFrame > 7 - 2 )
			{
				FFrameAssigned = false;
				return;
			}

			FFrameAssigned = true;
			FFrame0 = AFrame ++;
			FFrame1 = AFrame ++;
		}

	protected:
		void display()
		{
//			Serial.println( "display" );
//			Serial.println( ( FFrame ) ? FFrame0 : FFrame1 );
//			FOwner.SelectBank( DisplayIS31FL3731::BANK_FUNCTIONREG );
			FOwner.SelectBank( ( FFrame ) ? FFrame0 : FFrame1 );
			RenderImage( FOwner );

			FOwner.WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::REG_PICTUREFRAME, ( FFrame ) ? FFrame0 : FFrame1 );
			FFrame = !FFrame;
			FModified = false;
		}

		void DoRefreshReceived( void * )
		{
			if( FModified )
				display();
		}

	public:
		virtual void SystemInitStart( bool &AStarted ) override
		{
			for( int i = 0; i < DisplayIS31FL3731::HEIGHT * DisplayIS31FL3731::WIDTH; ++i )
				FPicture[ i ] = BackgroundColor;

			if( !Enabled )
				return;

			setTextSize( Text.Size );
			setTextColor( Text.Color, Text.BackgroundColor );
			setTextWrap( Text.Wrap );

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Render( false );

			if( FBlink )
				FBlink->SetFrame();

			if( AStarted )
				return;

			if( ClockInputPin.IsConnected() )
				return;

			AStarted = true;

			DoClockReceive( nullptr );
		}

		virtual void Refresh() override
		{
			if( FModified )
				if( ! RefreshInputPin.IsConnected() )
					display();
		}

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			if( ! Enabled )
				return;

			// picture mode
			FOwner.WriteRegister8( DisplayIS31FL3731::BANK_FUNCTIONREG, DisplayIS31FL3731::REG_CONFIG, DisplayIS31FL3731::REG_CONFIG_PICTUREMODE );
			UpdateFadeEffect();
			display();
		}

	public:
		DisplayIS31FL3731PictureElement( DisplayIS31FL3731 &AOwner, const unsigned char * AFont ) :
			inherited( AOwner ),
			DisplayIS31FL3731BasicPictureElement( AOwner.FWire, AFont ),
			FFrameAssigned( false ),
			FFrame( false )
		{
			RefreshInputPin.SetCallback( MAKE_CALLBACK( DisplayIS31FL3731PictureElement::DoRefreshReceived ));
		}

	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	DisplayIS31FL3731BasicElement::DisplayIS31FL3731BasicElement( DisplayIS31FL3731 &AOwner ) :
		FOwner( AOwner )
	{
		AOwner.FElements.push_back( this );
	}
//---------------------------------------------------------------------------
	DisplayIS31FL3731FrameElement::DisplayIS31FL3731FrameElement( DisplayIS31FL3731AnimationElement &AOwner, const unsigned char * AFont ) :
		inherited( AOwner.FOwner.FWire, AFont ),
		FOwner( AOwner.FOwner )
	{
		AOwner.FFrames.push_back( this );
		RefreshInputPin.SetCallback( MAKE_CALLBACK( DisplayIS31FL3731FrameElement::DoRefreshReceived ));
	}
//---------------------------------------------------------------------------
	DisplayIS31FL3731BasicBlink::DisplayIS31FL3731BasicBlink( DisplayIS31FL3731BasicPictureElement &AOwner, void * ) :
		FWire( AOwner.FWire )
	{		
		AOwner.FBlink = this;
	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}

#endif

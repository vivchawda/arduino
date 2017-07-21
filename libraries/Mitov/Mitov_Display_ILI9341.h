////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_ILI9341_h
#define _MITOV_DISPLAY_ILI9341_h

#include <Mitov_Graphics_Color.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

#ifdef _INHERITED_ILI9341_DISPLAY_
  #define _INHERITED_ILI9341_DISPLAY_VIRTUAL_ virtual
#else
  #define _INHERITED_ILI9341_DISPLAY_VIRTUAL_
#endif

#ifdef _INHERITED_ILI9341_DISPLAY_SERIAL_
  #define _INHERITED_ILI9341_DISPLAY_SERIAL_VIRTUAL_ virtual
#else
  #define _INHERITED_ILI9341_DISPLAY_SERIAL_VIRTUAL_
#endif

#ifdef _INHERITED_ILI9341_DISPLAY_RESET_
  #define _INHERITED_ILI9341_DISPLAY_VIRTUAL_RESET_ virtual
#else
  #define _INHERITED_ILI9341_DISPLAY_VIRTUAL_RESET_
#endif

namespace Mitov
{
/*
static const uint8_t PROGMEM
  Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735::SWRESET,   ST7735::DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735::SLPOUT ,   ST7735::DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735::FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735::FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735::FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735::INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735::PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735::PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735::PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735::PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735::PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735::VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735::INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735::MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    ST7735::COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 };                 //     16-bit color
*/
namespace ILI9341Extended
{
	static const uint8_t INIT_DELAY = 0xFF;

	static const uint8_t MADCTL		= 0x36;
	static const uint8_t MADCTL_MY  = 0x80;
	static const uint8_t MADCTL_MX  = 0x40;
	static const uint8_t MADCTL_MV  = 0x20;
	static const uint8_t MADCTL_ML  = 0x10;
	static const uint8_t MADCTL_RGB = 0x00;
	static const uint8_t MADCTL_BGR = 0x08;
	static const uint8_t MADCTL_MH  = 0x04;

	static const uint8_t COLADDRSET = 0x2A;
	static const uint8_t PAGEADDRSET = 0x2B;

/*
	static const uint8_t PROGMEM // Adafruit version
		Init[] =
		{
			0x01, 0, // ILI9341_SOFTRESET

			INIT_DELAY, 50,

			0x28, 0, // ILI9341_DISPLAYOFF

			0xC0, 1, // ILI9341_POWERCONTROL1
			0x23,

			0xC1, 1, // ILI9341_POWERCONTROL2
			0x10,

			0xC5, 2, // ILI9341_VCOMCONTROL1
			0x2B,
			0x2B,

			0xC7, 1, // ILI9341_VCOMCONTROL2
			0xC0,

			MADCTL, 1, // ILI9341_MEMCONTROL
			MADCTL_MY | MADCTL_BGR,

			0x3A, 1, // ILI9341_PIXELFORMAT
			0x55,

			0xB1, 2, // ILI9341_FRAMECONTROL
			0x00,
			0x1B,

			0xB7, 1, // ILI9341_ENTRYMODE
			0x07,

			0x11, 0, // ILI9341_SLEEPOUT

			INIT_DELAY, 150,

			0x29, 0, // ILI9341_DISPLAYON

			INIT_DELAY, 250,
			INIT_DELAY, 250

//			setAddrWindow( 0, 0, WIDTH - 1, HEIGHT - 1 );
		};
*/
	static const uint8_t PROGMEM
		Init[] =
		{
//			24,		// 23 commands

			0xCF, 3,	// 1. Power Control B - http://www.newhavendisplay.com/app_notes/ILI9341.pdf
			0x00,
			0xC1,
			0x30,

			0xED, 4,	//  2. Power on sequence contro
			0x64,
			0x03, 
			0x12, 
			0x81,

			0xE8, 3,	//  3. Driver timing control A
			0x85,
			0x00, 
			0x78, 

			0xCB, 5,	//  4. Power control A 
			0x39,
			0x2C, 
			0x00, 
			0x34, 
			0x02, 

			0xF7, 1,	// 5. Pump ratio control
			0x20,

			0xEA, 2,	// 6. Driver timing control B
			0x00, 
			0x00,

			0xC0, 1,    //  7. Power control - https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf
			0x18,		//VRH[5:0] 

			0xC1, 1,    // 8. Power control 
			0x12,		//SAP[2:0];BT[3:0] 

			0xC5, 2,    // 9. VCOM control 
			0x31,
			0x3C,

			0xC7, 1,    // 10. VCOM control2 
			0x99,

			//--

			0x3A, 1,    // 11. Pixel Format Set
			0x55,

			0xB1, 2,    // 12. Frame Control
			0x00,
			0x16,

			0xB6, 2,    // 13. Display Function Control 
			0x0A,
			0xA2,

			0xF6, 2,	// 14. Interface Control 
			0x01,
			0x30,

			0xF2, 1,    // 15. 3Gamma Function Disable 
			0x00,

			0x26, 1,    // 16. Gamma curve selected 
			0x02,

			0xE0, 15,   // 17. Set Gamma 
			0x0F,
			0x1D,
			0x1A,
			0x0A,
			0x0D,
			0x07,
			0x49,
			0x66,
			0x3B,
			0x07,
			0x11,
			0x01,
			0x09,
			0x05,
			0x04,

			0xE1, 15,   // 18. Set Gamma 
			0x00,
			0x18,
			0x1D,
			0x02,
			0x0F,
			0x04,
			0x36,
			0x13,
			0x4C,
			0x07,
			0x13,
			0x0F,
			0x2E,
			0x2F,
			0x05,


			0x11, 0,    // 19. Exit Sleep 

			INIT_DELAY, 120,	// 20.

			0x29, 0,    // 21. Display on 
	
			0x2A, 4,    // 22. Set Gamma 
			0x00,
			0x00,
			0x00,
			0xEF,

			0x2B, 4,    // 23. Set Gamma 
			0x00,
			0x00,
			0x01,
			0x3F,

			0x2C		// 24. Memory Write  
		};

};
//	ILI9341 Extended command set

	class DisplayILI9341 : public OpenWire::Component, public GraphicsImpl
	{
		typedef OpenWire::Component inherited;

	public:
		static const uint16_t WIDTH = 240;
		static const uint16_t HEIGHT = 320;

	protected:
		static const uint8_t CHIP_SELECT_PIN = 0;
		static const uint8_t COMMAND_DATA_PIN = 1;
		static const uint8_t RESET_PIN = 2;
		static const uint8_t READ_PIN = 3;
		static const uint8_t WRITE_PIN = 4;
		static const uint8_t FIRST_DATA_PIN = 5;

	public:
		TGraphicsColorTextSettings	Text;

		uint16_t FPins[ 8 + 5 ];

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
				return HEIGHT;

			return WIDTH;
		}

	protected:
		_INHERITED_ILI9341_DISPLAY_VIRTUAL_ void InitPins()
		{
			for( int i = 0; i < sizeof( FPins ) / sizeof( FPins[ 0 ] ); ++i )
				pinMode( FPins[ i ], OUTPUT );

			digitalWrite(FPins[ RESET_PIN ],HIGH);
			delay(50);
			digitalWrite(FPins[ RESET_PIN ],LOW);
			delay(100);
	
			digitalWrite(FPins[ RESET_PIN ],HIGH);
			delay(50);
			digitalWrite(FPins[ CHIP_SELECT_PIN ],HIGH);
			digitalWrite(FPins[ READ_PIN ],HIGH);
			digitalWrite(FPins[ WRITE_PIN ],HIGH);
			delay(10);
		}

	protected:
		virtual void SystemInit() override
		{
			InitPins();

			int i = 0;
			while(i < sizeof( ILI9341Extended::Init )) 
			{
				uint8_t a = pgm_read_byte(& ILI9341Extended::Init[i++]);
				uint8_t ACount = pgm_read_byte(& ILI9341Extended::Init[i++]);
				if( a == ILI9341Extended::INIT_DELAY ) 
					delay( ACount );

				else
				{
					cmd( a );
					for( int j = 0; j < ACount; ++j )
						w_data( pgm_read_byte(& ILI9341Extended::Init[i++] ));

				}
			}


/*
			cmd(0x0036);    // Memory Access Control 
//#ifdef V_SCREEN	
			w_data(0x004a); 
//#endif
*/
/*
#ifdef H_SCREEN	
			w_data(0x2a); 
#endif
*/

			UpdateOrientation();
/*
			clear( Func::Color565( 255 ) );
			UpdateInverted();


*/
			ClearScreen( BackgroundColor ); // TEMPORARY REMOVED FOR TEST!

			setTextSize( Text.Size );
			setTextColor( Text.Color, Text.BackgroundColor );
			setTextWrap( Text.Wrap );

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Render( false );

		}

		virtual void UpdateOrientation()
		{
			cmd( ILI9341Extended::MADCTL);

			switch( Orientation )
			{
				case goUp: 		w_data(ILI9341Extended::MADCTL_BGR); break;
				case goDown:	w_data(ILI9341Extended::MADCTL_MX | ILI9341Extended::MADCTL_MY | ILI9341Extended::MADCTL_BGR); break;
				case goLeft: 	w_data(ILI9341Extended::MADCTL_MY | ILI9341Extended::MADCTL_MV | ILI9341Extended::MADCTL_BGR); break;
				case goRight:	w_data(ILI9341Extended::MADCTL_MX | ILI9341Extended::MADCTL_MV | ILI9341Extended::MADCTL_BGR); break;
			}

			setAddrWindow( 0, 0, width() - 1, height() - 1 ); // CS_IDLE happens here
		}

		void setAddrWindow(int x1, int y1, int x2, int y2) 
		{
			uint32_t t;

			t = x1;
			t <<= 16;
			t |= x2;
			writeRegister32( ILI9341Extended::COLADDRSET, t );  // HX8357D uses same registers!
			t = y1;
			t <<= 16;
			t |= y2;
			writeRegister32( ILI9341Extended::PAGEADDRSET, t ); // HX8357D uses same registers!
		}

		_INHERITED_ILI9341_DISPLAY_SERIAL_VIRTUAL_ void writeRegister32(uint8_t r, uint32_t d) 
		{
			cmd( r );
			w_data(d >> 24);
			w_data(d >> 16);
			w_data(d >> 8);
			w_data(d);
		}

	public:
#ifdef _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

		virtual void drawAlphaColorBitmap( int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawAlphaColorBitmap( x, y, bitmap, w, h );
				return;
			}

			// check rotation, move pixel around if necessary

			setAddrWindow(x,y,x + w - 1, y + h - 1);

			cmd( 0x2C );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint16_t AColor = Func::Color565( pgm_read_dword( bitmap++ ));
					w_data( AColor >> 8 );
					w_data( AColor );
				}

		}

#endif // _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_COLOR_BITMAP_
		virtual void drawColorBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawColorBitmap( x, y, bitmap, w, h );
				return;
			}

			setAddrWindow(x,y,x + w - 1, y + h - 1);

			cmd( 0x2C );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t ARed = pgm_read_byte( bitmap ++ );
					uint32_t AGreen = pgm_read_byte( bitmap ++ );
					uint32_t ABlue = pgm_read_byte( bitmap ++ );
					uint16_t AColor = Func::Color565( ( AGreen << 8 ) | ( ARed << 16 ) | ABlue );

					w_data( AColor >> 8 );
					w_data( AColor );
				}

		}

#endif // _GRAPHICS_DRAW_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_GRAYSCALE_BITMAP_

		virtual void drawGrayscaleBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t AAlpha ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawGrayscaleBitmap( x, y, bitmap, w, h, AAlpha );
				return;
			}

			setAddrWindow(x,y,x + w - 1, y + h - 1);

			cmd( 0x2C );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t AGray = pgm_read_byte( bitmap ++ );
					uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );

					w_data( AColor >> 8 );
					w_data( AColor );
				}

		}

#endif // _GRAPHICS_DRAW_GRAYSCALE_BITMAP_

#ifdef _GRAPHICS_DRAW_GRAYSCALE_ALPHA_BITMAP_

		virtual void drawAlphaGrayscaleBitmap( int16_t x, int16_t y, const uint16_t *_bitmap, int16_t w, int16_t h ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawAlphaGrayscaleBitmap( x, y, _bitmap, w, h );
				return;
			}

			const uint8_t *bitmap = (const uint8_t *)_bitmap;

			setAddrWindow(x,y,x + w - 1, y + h - 1);

			cmd( 0x2C );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t AGray = pgm_read_byte( bitmap );
					uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );
					bitmap += 2;

					w_data( AColor >> 8 );
					w_data( AColor );
				}

		}

#endif // _GRAPHICS_DRAW_GRAYSCALE_ALPHA_BITMAP_

#ifdef _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_

		virtual void drawBWBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, TUniColor color, TUniColor bg ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawBWBitmap( x, y, bitmap, w, h, color, bg );
				return;
			}

			int16_t byteWidth = (w + 7) / 8;

//			color = Func::Color565( color );
//			bg = Func::Color565( bg );
  
			setAddrWindow(x,y,x + w - 1, y + h - 1);

			cmd( 0x2C );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint16_t AColor;
					if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7)))
						AColor = 0xFFFF;

					else 
      					AColor = 0;

					w_data( AColor >> 8 );
					w_data( AColor );
				}

		}

#endif // _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_

#ifdef _GRAPHICS_DRAW_REPEAT_ALPHA_COLOR_BITMAP_

		virtual void drawRepeatAlphaColorBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawRepeatAlphaColorBitmap( x, y, bitmap, AListCount, w, h );
				return;
			}

			setAddrWindow(x,y, x + w - 1, y + h - 1);

			cmd( 0x2C );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( pgm_read_dword( bitmap ));
				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				bitmap += 4;
				for( int16_t j = 0; j <= ACount; ++ j )
				{
					w_data( AHi );
					w_data( ALo );
				}
			}
		}

#endif // _GRAPHICS_DRAW_REPEAT_ALPHA_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_REPEAT_COLOR_BITMAP_

		virtual void drawRepeatColorBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawRepeatColorBitmap( x, y, bitmap, AListCount, w, h );
				return;
			}

			setAddrWindow(x,y, x + w - 1, y + h - 1);

			cmd( 0x2C );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );

				uint32_t ARed = pgm_read_byte( bitmap ++ );
				uint32_t AGreen = pgm_read_byte( bitmap ++ );
				uint32_t ABlue = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( ( ARed << 16 ) | ( AGreen << 8 ) | ABlue );

				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				for( int16_t j = 0; j <= ACount; ++ j )
				{
					w_data( AHi );
					w_data( ALo );
				}
			}
		}

#endif // _GRAPHICS_DRAW_REPEAT_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_GRAYSCALE_REPEAT_BITMAP_

		virtual void drawRepeatGrayscaleBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t AAlpha )  override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawRepeatGrayscaleBitmap( x, y, bitmap, AListCount, w, h, AAlpha );
				return;
			}

			setAddrWindow(x,y, x + w - 1, y + h - 1);

			cmd( 0x2C );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );

				uint32_t AGray = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );

				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				for( int16_t j = 0; j <= ACount; ++ j )
				{
					w_data( AHi );
					w_data( ALo );
				}
			}
		}

#endif // _GRAPHICS_DRAW_GRAYSCALE_REPEAT_BITMAP_

#ifdef _GRAPHICS_DRAW_GRAYSCALE_REPEAT_ALPHA_BITMAP_

		virtual void drawAlphaGrayscaleRepeatBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h )  override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawAlphaGrayscaleRepeatBitmap( x, y, bitmap, AListCount, w, h );
				return;
			}

			setAddrWindow(x,y, x + w - 1, y + h - 1);

			cmd( 0x2C );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap );

				bitmap += 2;

				uint32_t AGray = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );

				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				for( int16_t j = 0; j <= ACount; ++ j )
				{
					w_data( AHi );
					w_data( ALo );
				}
			}
		}

#endif // _GRAPHICS_DRAW_GRAYSCALE_REPEAT_ALPHA_BITMAP_

		virtual void drawGrayscalePixel(int16_t x, int16_t y, TUniColor color) override
		{
			uint16_t AGray = ( color & 0xFF );
			drawPixel( x, y, color << 16 | ( AGray << 8 ) | AGray );
		}

		virtual void drawPixel(int16_t x, int16_t y, TUniColor color) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if((x < 0) ||(x >= AWidth) || (y < 0) || (y >= AHeight)) 
				return;

			uint8_t AAlpha = color >> 24;
			if( AAlpha == 0 )
				return;

			if( AAlpha < 255 )
			{
				float ACoefficient = float( AAlpha ) / 255.0f;
				uint8_t	r = color >> 16;
				uint8_t	g = color >> 8;
				uint8_t	b = color;

				TUniColor AOldColor = GetPixelColor( x, y );
				uint8_t	OldR = AOldColor >> 16;
				uint8_t	OldG = AOldColor >> 8;
				uint8_t	OldB = AOldColor;

//				Serial.print( OldR ); Serial.print( " " );Serial.print( OldG ); Serial.print( " " );Serial.println( OldB );

				r = float( r ) * ACoefficient + float( OldR ) * ( 1 - ACoefficient ) + 0.5;
				g = float( g ) * ACoefficient + float( OldG ) * ( 1 - ACoefficient ) + 0.5;
				b = float( b ) * ACoefficient + float( OldB ) * ( 1 - ACoefficient ) + 0.5;

				color = Func::Color565( r, g, b );
//				color = Func::Color565( 255, 255, 255 );
			}

			else
				color = Func::Color565( color );

			setAddrWindow(x,y,x+1,y+1);

			cmd( 0x2C );

			w_data( color >> 8 ); 
			w_data( color );
		}

#if ( defined(__ILI9341_SCROLL__) && defined(__PARALLEL_COMPONENT_DISPLAY_ILI9341__))
		virtual void Scroll( TGraphicsPos X, TGraphicsPos Y, TUniColor color ) override
		{
			if( X == 0 && Y == 0 )
				return;

			int AFromYIndex, AToYIndex;
			int AHeight = height() - abs( Y );
			if( Y < 0 )
			{
				AFromYIndex = -Y;
				AToYIndex = 0;
			}

			else
			{
				AFromYIndex = 0;
				AToYIndex = Y;
			}

			int AFromXIndex, AToXIndex;
			int AWidth = width() - abs( X );
			if( X < 0 )
			{
				AFromXIndex = -X;
				AToXIndex = 0;
			}

			else
			{
				AFromXIndex = 0;
				AToXIndex = X;
			}

			if( Y < 0 )
			{
				for( int ay = 0; ay < AHeight; ++ay )
				{
					if( X < 0 )
					{
						uint16_t *ABuffer = new uint16_t[ AWidth ];
						setAddrWindow( AFromXIndex, ay + AFromYIndex, AFromXIndex + AWidth - 1, ay + AFromYIndex );

						cmd( 0x2E ); // Read command

//						DDRD &= 0b00000011;
//						DDRB &= 0b11111100;

//						if( FPins[ FIRST_DATA_PIN ] != _VISUINO_MAX_PIN_NO_ )
						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
							pinMode( FPins[ i ], INPUT );

						r_data(); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = r_data();
							AResult <<= 8;
							AResult |= r_data();
							AResult <<= 8;
							AResult |= r_data();

							ABuffer[ ax ] = Func::Color565( AResult );
						}

//						DDRD |= 0b11111100;
//						DDRB |= 0b00000011;

//						if( FPins[ FIRST_DATA_PIN ] != _VISUINO_MAX_PIN_NO_ )
						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
							pinMode( FPins[ i ], OUTPUT );

						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

						cmd( 0x2C );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							w_data( AColor >> 8 ); 
							w_data( AColor );
						}

						delete [] ABuffer;

						drawFastHLine( AWidth, ay + AToYIndex, width() - AWidth, color );
					}

					else
					{
						uint16_t *ABuffer = new uint16_t[ AWidth ];

						setAddrWindow( AFromXIndex, ay + AFromYIndex, AFromXIndex + AWidth - 1, ay + AFromYIndex );

						cmd( 0x2E ); // Read command

//						if( FPins[ FIRST_DATA_PIN ] != _VISUINO_MAX_PIN_NO_ )
						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
							pinMode( FPins[ i ], INPUT );

						r_data(); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = r_data();
							AResult <<= 8;
							AResult |= r_data();
							AResult <<= 8;
							AResult |= r_data();

							ABuffer[ ax ] = Func::Color565( AResult );
						}

//						if( FPins[ FIRST_DATA_PIN ] != _VISUINO_MAX_PIN_NO_ )
						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
							pinMode( FPins[ i ], OUTPUT );

						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

						cmd( 0x2C );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							w_data( AColor >> 8 ); 
							w_data( AColor );
						}

						delete [] ABuffer;

						drawFastHLine( 0, ay + AToYIndex, X, color );
					}
				}

				fillRect( 0, AHeight, width(), height(), color );
			}

			else
			{
				for( int ay = AHeight; ay--; )
				{
					if( X < 0 )
					{
						uint16_t *ABuffer = new uint16_t[ AWidth ];

						setAddrWindow( AFromXIndex, ay + AFromYIndex, AFromXIndex + AWidth - 1, ay + AFromYIndex );

						cmd( 0x2E ); // Read command

//						if( FPins[ FIRST_DATA_PIN ] != _VISUINO_MAX_PIN_NO_ )
						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
							pinMode( FPins[ i ], INPUT );

						r_data(); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = r_data();
							AResult <<= 8;
							AResult |= r_data();
							AResult <<= 8;
							AResult |= r_data();

							ABuffer[ ax ] = Func::Color565( AResult );
						}

//						if( FPins[ FIRST_DATA_PIN ] != _VISUINO_MAX_PIN_NO_ )
						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
							pinMode( FPins[ i ], OUTPUT );

						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

						cmd( 0x2C );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							w_data( AColor >> 8 ); 
							w_data( AColor );
						}

						delete [] ABuffer;

						drawFastHLine( AWidth, ay + AToYIndex, width() - AWidth, color );
					}

					else
					{
						uint16_t *ABuffer = new uint16_t[ AWidth ];

						setAddrWindow( AFromXIndex, ay + AFromYIndex, AFromXIndex + AWidth - 1, ay + AFromYIndex );

						cmd( 0x2E ); // Read command

//						if( FPins[ FIRST_DATA_PIN ] != _VISUINO_MAX_PIN_NO_ )
						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
							pinMode( FPins[ i ], INPUT );

						r_data(); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = r_data();
							AResult <<= 8;
							AResult |= r_data();
							AResult <<= 8;
							AResult |= r_data();

							ABuffer[ ax ] = Func::Color565( AResult );
						}

//						if( FPins[ FIRST_DATA_PIN ] != _VISUINO_MAX_PIN_NO_ )
						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
							pinMode( FPins[ i ], OUTPUT );

						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

						cmd( 0x2C );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							w_data( AColor >> 8 ); 
							w_data( AColor );
						}

						delete [] ABuffer;

						drawFastHLine( 0, ay + AToYIndex, X, color );
					}
				}

				fillRect( 0, 0, width(), Y, color );
			}
		}
#endif

		virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, TUniColor color) override
		{
			// Rudimentary clipping
			uint16_t AHeight = height();
			uint16_t AWidth = width();
			if((x >= AWidth ) || (y >= AHeight )) 
				return;

			uint8_t AAlpha = color >> 24;
			if( AAlpha < 255 )
			{
				GraphicsImpl::drawFastHLine( x, y, w, color );
				return;
			}

			if((x+w-1) >= AWidth )
				w = AWidth - x;

			color = Func::Color565( color );
			setAddrWindow(x, y, x+w-1, y);

			uint8_t hi = color >> 8, lo = color;

			cmd( 0x2C );

			while (w--) 
			{
				w_data(hi);
				w_data(lo);
			}
		}

		virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, TUniColor color) override
		{
			// Rudimentary clipping
			uint16_t AHeight = height();
			uint16_t AWidth = width();
			if((x >= AWidth) || (y >= AHeight)) 
				return;

			uint8_t AAlpha = color >> 24;
			if( AAlpha == 0 )
				return;

			if( AAlpha < 255 )
			{
				GraphicsImpl::drawFastVLine( x, y, h, color );
				return;
			}

			if((y+h-1) >= AHeight) 
				h = AHeight-y;

			color = Func::Color565( color );
			setAddrWindow(x, y, x, y+h-1);

			uint8_t hi = color >> 8, lo = color;

			cmd( 0x2C );

			while (h--) 
			{
				w_data(hi);
				w_data(lo);
			}
    
		}

		virtual void fillRect( int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color ) override
		{
			// rudimentary clipping (drawChar w/big text requires this)
			int16_t AWidth = width();
			int16_t AHeight = height();

			if((x >= AWidth) || (y >= AHeight))
				return;

			uint8_t AAlpha = color >> 24;
			if( AAlpha == 0 )
				return;

			if( AAlpha < 255 )
			{
				GraphicsImpl::fillRect( x, y, w, h, color );
				return;
			}

			if((x + w - 1) >= AWidth)
				w = AWidth - x;

			if((y + h - 1) >= AHeight)
				h = AHeight - y;

			setAddrWindow(x, y, x+w-1, y+h-1);

			color = Func::Color565( color );

			uint8_t hi = color >> 8, lo = color;

			cmd( 0x2C );
			for( y=0; y < h; ++y )
				for( x=0; x < w; ++x )
				{
					w_data(hi);
					w_data(lo);
				}

		}

		virtual void ClearScreen( TUniColor color ) override
		{
			fillRect( 0, 0, width(), height(), 0xFF000000 | color );
		}

#if ( defined(__AVR__) || defined( __PARALLEL_COMPONENT_DISPLAY_ILI9341__ ))
		virtual TUniColor GetPixelColor( int16_t x, int16_t y ) override
		{
//			return 0xFFFFFFFF;
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if((x < 0) ||(x >= AWidth) || (y < 0) || (y >= AHeight)) 
				return 0x0000;

			setAddrWindow(x,y,x+1,y+1);

			cmd( 0x2E ); // Read command

			for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
				pinMode( FPins[ i ], INPUT );

			r_data(); // Fake read

			TUniColor AResult = r_data();
			AResult <<= 8;
			AResult |= r_data();
			AResult <<= 8;
			AResult |= r_data();

			for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
				pinMode( FPins[ i ], OUTPUT );

//			if( AResult )
//				Serial.println( int( AResult ), HEX );

//			return 0xFF000000 | Func::ColorFrom565( AResult ); // Not supported
			return 0xFF030303 | AResult; // Not supported
		}
#endif

	protected:
#if ( defined(__AVR__) || defined( __PARALLEL_COMPONENT_DISPLAY_ILI9341__ ))
		_INHERITED_ILI9341_DISPLAY_VIRTUAL_ void cmd( uint8_t cmd )
		{
			for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
				digitalWrite( FPins[ i ], ( cmd & ( 1 << i ) ) ? HIGH : LOW );

//			PORTC=PORTC&0Xf0|0x01;

//			PORTB=(0xfc&PORTB)|(0x03&cmd);
//			PORTD=(0x03&PORTD)|(0xfc&cmd);
//			PORTC|=0x02;
//			PORTC|=0x08;

//			digitalWrite( FPins[ READ_PIN ], HIGH );
			digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );
			digitalWrite( FPins[ WRITE_PIN ], LOW );
			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

//			for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
//				digitalWrite( FPins[ i ], ( cmd & ( 1 << i ) ) ? HIGH : LOW );

			digitalWrite( FPins[ WRITE_PIN ], HIGH );
			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
		}

		uint8_t r_data()
		{
//			digitalWrite( FPins[ WRITE_PIN ], HIGH );
			digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );
			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

			digitalWrite( FPins[ READ_PIN ], LOW );
//			digitalWrite( FPins[ READ_PIN ], LOW ); // Delay
//			digitalWrite( FPins[ READ_PIN ], LOW ); // Delay

			uint8_t AResult = 0;
//			for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
//				if( digitalRead( FPins[ i ] ) == HIGH )
//					AResult |= ( 1 << i );

			for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
				if( digitalRead( FPins[ i ] ) == HIGH )
					AResult |= ( 1 << i );

			digitalWrite( FPins[ READ_PIN ], HIGH );
			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

//			if( AResult )
//				Serial.println( AResult );

			return AResult;
		}

		_INHERITED_ILI9341_DISPLAY_VIRTUAL_ void  w_data( uint8_t data )
		{
			for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
				digitalWrite( FPins[ i ], ( data & ( 1 << i ) ) ? HIGH : LOW );

//			PORTC=PORTC&0Xf0|0x05;
//			PORTB=(0xfc&PORTB)|(0x03&data);
//			PORTD=(0x03&PORTD)|(0xfc&data);
//			PORTC|=0x02;
//			PORTC|=0x08;

			digitalWrite( FPins[ READ_PIN ], HIGH );
			digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );
			digitalWrite( FPins[ WRITE_PIN ], LOW );
			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

//			for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
//				digitalWrite( FPins[ i ], ( data & ( 1 << i ) ) ? HIGH : LOW );

			digitalWrite( FPins[ WRITE_PIN ], HIGH );
			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

		}
#else
		virtual void cmd( uint8_t cmd ) = 0;
		virtual void  w_data( uint8_t data ) = 0;
#endif
	public:
		DisplayILI9341( uint16_t AD0, uint16_t AD1, uint16_t AD2, uint16_t AD3, uint16_t AD4, uint16_t AD5, uint16_t AD6, uint16_t AD7, uint16_t ARead, uint16_t AWrite, uint16_t ACommandData, uint16_t AChipSelect, uint16_t AReset, const unsigned char * AFont ) :
			GraphicsImpl( AFont )
		{
			FPins[ FIRST_DATA_PIN + 0 ] = AD0;
			FPins[ FIRST_DATA_PIN + 1 ] = AD1;
			FPins[ FIRST_DATA_PIN + 2 ] = AD2;
			FPins[ FIRST_DATA_PIN + 3 ] = AD3;
			FPins[ FIRST_DATA_PIN + 4 ] = AD4;
			FPins[ FIRST_DATA_PIN + 5 ] = AD5;
			FPins[ FIRST_DATA_PIN + 6 ] = AD6;
			FPins[ FIRST_DATA_PIN + 7 ] = AD7;
			FPins[ READ_PIN ] = ARead;
			FPins[ WRITE_PIN ] = AWrite;
			FPins[ COMMAND_DATA_PIN ] = ACommandData;
			FPins[ CHIP_SELECT_PIN ] = AChipSelect;
			FPins[ RESET_PIN ] = AReset;
		}

		DisplayILI9341( uint16_t ACommandData, uint16_t AChipSelect, int16_t AReset, const unsigned char * AFont ) :
			GraphicsImpl( AFont )
		{
			FPins[ COMMAND_DATA_PIN ] = ACommandData;
			FPins[ CHIP_SELECT_PIN ] = AChipSelect;
			FPins[ RESET_PIN ] = AReset;
//			FPins[ FIRST_DATA_PIN ] = _VISUINO_MAX_PIN_NO_;
		}
	};
//---------------------------------------------------------------------------
#ifdef _INHERITED_ILI9341_DISPLAY_
	class DisplayILI9341OptimizedD2D9A0A4 : public DisplayILI9341
	{
		typedef DisplayILI9341 inherited;

	protected:
#ifdef __AVR__
		virtual void cmd( uint8_t cmd ) override
		{
			PORTC=PORTC&0Xf0|0x01;

			PORTB=(0xfc&PORTB)|(0x03&cmd);
			PORTD=(0x03&PORTD)|(0xfc&cmd);
			PORTC|=0x02;
			PORTC|=0x08;
		}

		virtual void  w_data( uint8_t data ) override
		{
			PORTC=PORTC&0Xf0|0x05;
			PORTB=(0xfc&PORTB)|(0x03&data);
			PORTD=(0x03&PORTD)|(0xfc&data);
			PORTC|=0x02;
			PORTC|=0x08;
		}

		inline uint8_t r_data()
		{
			PORTC=PORTC & 0b11110000 | 0b00000110;

			asm volatile(       
				"rjmp .+0" "\n\t" 
				"rjmp .+0" "\n\t" 
				"rjmp .+0" "\n\t" 
				"nop"      "\n"   
				::);

			uint8_t AData = ( PIND & 0b11111100 ) | ( PINB & 0b00000011 );

			PORTC|=0b00000001;
			PORTC|=0x08;

			return AData;
		}

		virtual TUniColor GetPixelColor( int16_t x, int16_t y ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if((x < 0) ||(x >= AWidth) || (y < 0) || (y >= AHeight)) 
				return 0x0000;

			setAddrWindow(x,y,x+1,y+1);

			cmd( 0x2E ); // Read command

			DDRD &= 0b00000011;
			DDRB &= 0b11111100;
//			for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
//				pinMode( FPins[ i ], INPUT );

			r_data(); // Fake read

			TUniColor AResult = r_data();
			AResult <<= 8;
			AResult |= r_data();
			AResult <<= 8;
			AResult |= r_data();

			DDRD |= 0b11111100;
			DDRB |= 0b00000011;
//			for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
//				pinMode( FPins[ i ], OUTPUT );

//			if( AResult )
//				Serial.println( int( AResult ), HEX );

//			return 0xFF000000 | Func::ColorFrom565( AResult ); // Not supported
			if( AResult & 0x00FC0000 )
				AResult |= 0x00030000;

			if( AResult & 0x0000FC00 )
				AResult |= 0x00000300;

			if( AResult & 0x000000FC )
				AResult |= 0x00000003;

			return 0xFF000000 | AResult;
//			return 0xFF030303 | AResult; // Not supported
		}

#ifdef __ILI9341_SCROLL__
		virtual void Scroll( TGraphicsPos X, TGraphicsPos Y, TUniColor color ) override
		{
			if( X == 0 && Y == 0 )
				return;

			int AFromYIndex, AToYIndex;
			int AHeight = height() - abs( Y );
			if( Y < 0 )
			{
				AFromYIndex = -Y;
				AToYIndex = 0;
			}

			else
			{
				AFromYIndex = 0;
				AToYIndex = Y;
			}

			int AFromXIndex, AToXIndex;
			int AWidth = width() - abs( X );
			if( X < 0 )
			{
				AFromXIndex = -X;
				AToXIndex = 0;
			}

			else
			{
				AFromXIndex = 0;
				AToXIndex = X;
			}

			if( Y < 0 )
			{
				for( int ay = 0; ay < AHeight; ++ay )
				{
					if( X < 0 )
					{
						uint16_t *ABuffer = new uint16_t[ AWidth ];
						setAddrWindow( AFromXIndex, ay + AFromYIndex, AFromXIndex + AWidth - 1, ay + AFromYIndex );

						cmd( 0x2E ); // Read command

						DDRD &= 0b00000011;
						DDRB &= 0b11111100;

//						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
//							pinMode( FPins[ i ], INPUT );

						r_data(); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = r_data();
							AResult <<= 8;
							AResult |= r_data();
							AResult <<= 8;
							AResult |= r_data();

							ABuffer[ ax ] = Func::Color565( AResult );
						}

						DDRD |= 0b11111100;
						DDRB |= 0b00000011;

//						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
//							pinMode( FPins[ i ], OUTPUT );

						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

						cmd( 0x2C );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							w_data( AColor >> 8 ); 
							w_data( AColor );
						}

						delete [] ABuffer;

//						for( int ax = 0; ax < AWidth; ++ax )
//							drawPixel( ax + AToXIndex, ay + AToYIndex, GetPixelColor( ax + AFromXIndex, ay + AFromYIndex ));

						drawFastHLine( AWidth, ay + AToYIndex, width() - AWidth, color );
//						for( int ax = AWidth; ax < width(); ++ax )
//							drawPixel( ax, ay + AToYIndex, color );
					}

					else
					{
						uint16_t *ABuffer = new uint16_t[ AWidth ];

						setAddrWindow( AFromXIndex, ay + AFromYIndex, AFromXIndex + AWidth - 1, ay + AFromYIndex );

						cmd( 0x2E ); // Read command

						DDRD &= 0b00000011;
						DDRB &= 0b11111100;

//						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
//							pinMode( FPins[ i ], INPUT );

						r_data(); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = r_data();
							AResult <<= 8;
							AResult |= r_data();
							AResult <<= 8;
							AResult |= r_data();

							ABuffer[ ax ] = Func::Color565( AResult );
						}

						DDRD |= 0b11111100;
						DDRB |= 0b00000011;

//						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
//							pinMode( FPins[ i ], OUTPUT );

						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

						cmd( 0x2C );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							w_data( AColor >> 8 ); 
							w_data( AColor );
						}

						delete [] ABuffer;

//						for( int ax = AWidth; ax--; )
//							drawPixel( ax + AToXIndex, ay + AToYIndex, GetPixelColor( ax + AFromXIndex, ay + AFromYIndex ));

						drawFastHLine( 0, ay + AToYIndex, X, color );
//						for( int ax = 0; ax < X; ++ax )
//							drawPixel( ax, ay + AToYIndex, color );
					}
				}

				fillRect( 0, AHeight, width(), height(), color );
//				for( int ay = AHeight; ay < height(); ++ay )
//					for( int ax = 0; ax < width(); ++ax )
//						drawPixel( ax, ay, color );
			}

			else
			{
				for( int ay = AHeight; ay--; )
				{
					if( X < 0 )
					{
						uint16_t *ABuffer = new uint16_t[ AWidth ];

						setAddrWindow( AFromXIndex, ay + AFromYIndex, AFromXIndex + AWidth - 1, ay + AFromYIndex );

						cmd( 0x2E ); // Read command

						DDRD &= 0b00000011;
						DDRB &= 0b11111100;

//						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
//							pinMode( FPins[ i ], INPUT );

						r_data(); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = r_data();
							AResult <<= 8;
							AResult |= r_data();
							AResult <<= 8;
							AResult |= r_data();

							ABuffer[ ax ] = Func::Color565( AResult );
						}

						DDRD |= 0b11111100;
						DDRB |= 0b00000011;

//						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
//							pinMode( FPins[ i ], OUTPUT );

						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

						cmd( 0x2C );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							w_data( AColor >> 8 ); 
							w_data( AColor );
						}

						delete [] ABuffer;

//						for( int ax = 0; ax < AWidth; ++ax )
//							drawPixel( ax + AToXIndex, ay + AToYIndex, GetPixelColor( ax + AFromXIndex, ay + AFromYIndex ));

						drawFastHLine( AWidth, ay + AToYIndex, width() - AWidth, color );
//						for( int ax = AWidth; ax < width(); ++ax )
//							drawPixel( ax, ay + AToYIndex, color );
					}

					else
					{
						uint16_t *ABuffer = new uint16_t[ AWidth ];

						setAddrWindow( AFromXIndex, ay + AFromYIndex, AFromXIndex + AWidth - 1, ay + AFromYIndex );

						cmd( 0x2E ); // Read command

						DDRD &= 0b00000011;
						DDRB &= 0b11111100;

//						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
//							pinMode( FPins[ i ], INPUT );

						r_data(); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = r_data();
							AResult <<= 8;
							AResult |= r_data();
							AResult <<= 8;
							AResult |= r_data();

							ABuffer[ ax ] = Func::Color565( AResult );
						}

						DDRD |= 0b11111100;
						DDRB |= 0b00000011;

//						for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
//							pinMode( FPins[ i ], OUTPUT );

						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

						cmd( 0x2C );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							w_data( AColor >> 8 ); 
							w_data( AColor );
						}

						delete [] ABuffer;

						drawFastHLine( 0, ay + AToYIndex, X, color );
					}
				}

				fillRect( 0, 0, width(), Y, color );
			}
		}
#endif

#endif // __AVR__
	public:
		DisplayILI9341OptimizedD2D9A0A4( const unsigned char * AFont ) :
			inherited( 8, 9, 2, 3, 4, 5, 6, 7, 14, 15, 16, 17, 18, AFont )
		{
		}

	};

#endif // _INHERITED_ILI9341_DISPLAY_
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

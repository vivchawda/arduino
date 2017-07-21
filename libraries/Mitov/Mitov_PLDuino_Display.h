////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PLDUINO_DISPLAY_h
#define _MITOV_PLDUINO_DISPLAY_h

#include <Mitov.h>
#include <Mitov_Graphics_Color.h>
#include <Mitov_Arduino_SPI.h>
#include <Mitov_Display_ILI9341.h>
#include <Mitov_PLDuino.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class ArduinoPLDuinoTouch
	{
	protected:
		static const uint32_t CAL_X			= 0x00378F66UL;
		static const uint32_t CAL_Y			= 0x03C34155UL;
		static const uint32_t CAL_S			= 0x000EF13FUL;
		static const int32_t touch_x_left	= (CAL_X >> 14) & 0x3FFF;
		static const int32_t touch_x_right	= CAL_X & 0x3FFF;
		static const int32_t touch_y_top	= (CAL_Y >> 14) & 0x3FFF;
		static const int32_t touch_y_bottom	= CAL_Y & 0x3FFF;
		static const int32_t disp_x_size	= (CAL_S >> 12) & 0x0FFF;
		static const int32_t disp_y_size	= CAL_S & 0x0FFF;
		static const int32_t touch_width	= touch_x_right - touch_x_left;
		static const int32_t touch_height	= touch_y_bottom - touch_y_top;

	public:
		OpenWire::SourcePin	XOutputPin;
		OpenWire::SourcePin	YOutputPin;
		OpenWire::SourcePin	TouchedOutputPin;

	public:
		uint16_t	Precision = 100;

	public:
		void Init()
		{
//			Serial.println( "INIT" );
			pinMode( Mitov::PLDiono::TOUCH_IRQ, INPUT); digitalWrite( Mitov::PLDiono::TOUCH_IRQ, HIGH);
			pinMode( Mitov::PLDiono::TOUCH_CS, OUTPUT); digitalWrite( Mitov::PLDiono::TOUCH_CS, HIGH);
		}

		void ReadData( GraphicsOrientation Orientation )
		{
//			Serial.println( "ReadData" );
			if( digitalRead( Mitov::PLDiono::TOUCH_IRQ ))
				return;

			unsigned long tx=0;
			unsigned long ty=0;
			int datacount=0;
	
			SPI.begin();
			SPI.setClockDivider(SPI_CLOCK_DIV32);
			digitalWrite( Mitov::PLDiono::TOUCH_CS, LOW);
			for(int i=0; datacount < Precision && i < Precision*2; i++)
			{
				unsigned long x = ctl(0x90);
				unsigned long y = ctl(0xD0);
				if (!(x>max(touch_x_left, touch_x_right) || x==0 || y>max(touch_y_top, touch_y_bottom) || y==0))
				{
					ty += x;
					tx += y;
					datacount++;
				}
			}
			digitalWrite( Mitov::PLDiono::TOUCH_CS, HIGH);
			SPI.end();
			if( !datacount ) 
				return;

			long tpx = long(tx / datacount - touch_x_left) * disp_x_size / touch_width;
			long tpy = long(ty / datacount - touch_y_top) * disp_y_size / touch_height;

//			Serial.print( tpx ); Serial.print( " x " ); Serial.println( tpy );
			uint32_t AX;
			uint32_t AY;

			switch( Orientation )
			{
				case goLeft:
					AX = tpx;
					AY = tpy;
					break;

				case goRight:
					AX = DisplayILI9341::WIDTH - 1 - tpx;
					AY = DisplayILI9341::HEIGHT - 1 - tpy;
					break;

				case goDown:
					AX = DisplayILI9341::HEIGHT - 1 - tpy;
					AY = tpx;
					break;

				default:
					AX = tpy;
					AY = DisplayILI9341::WIDTH - 1 - tpx;
					break;
/*
				case goRight: 	w_data(ILI9341Extended::MADCTL_BGR); break;
				case goLeft:	w_data(ILI9341Extended::MADCTL_MX | ILI9341Extended::MADCTL_MY | ILI9341Extended::MADCTL_BGR); break;
				case goUp: 		w_data(ILI9341Extended::MADCTL_MY | ILI9341Extended::MADCTL_MV | ILI9341Extended::MADCTL_BGR); break;
				case goDown:	w_data(ILI9341Extended::MADCTL_MX | ILI9341Extended::MADCTL_MV | ILI9341Extended::MADCTL_BGR); break;
*/
			}

			XOutputPin.Notify( &AX );
			YOutputPin.Notify( &AY );
			TouchedOutputPin.Notify( nullptr );
/*

			switch( Orientation )
			{
				case NO_ROTATION:	return Point(tpx, tpy);
				case ROTATE_90:		return Point(tpy, disp_x_size-tpx);
				case ROTATE_180:	return Point(disp_x_size-tpx, disp_y_size-tpy);
				case ROTATE_270:	return Point(disp_y_size-tpy, tpx);
			}
*/
		}

	protected:
		word ctl(byte command)
		{
			SPI.transfer(command);
			byte f1 = SPI.transfer(0x00);
			byte f2 = SPI.transfer(0x00);
			return word(f1, f2) >> 3;
		}


/*
	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
		}

	public:
		ArduinoPLDuinoTouch
*/
	};
//---------------------------------------------------------------------------
	class ArduinoPLDuinoDisplay : public DisplayILI9341
	{
		typedef DisplayILI9341 inherited;

	public:
		float Backlight = 1.0;

#ifdef _INHERITED_DISPLAY_TOUCH_
		ArduinoPLDuinoTouch	Touch;
#endif

	public:
		void UpdateBacklight()
		{
			int AValue = ( 1.0 - Backlight ) * PWMRANGE;
			analogWrite( Mitov::PLDiono::LCD_BACKLIGHT, AValue );
		}

	protected:
		inline void FastSPI( uint8_t AValue )
		{
		    SPDR = AValue;
		    while(!(SPSR & _BV(SPIF)));
		}

	protected:
		virtual void InitPins() override
		{
//			Serial.println( "RESET" );
//			pinMode( FPins[ RESET_PIN ], OUTPUT );
			pinMode( FPins[ CHIP_SELECT_PIN ], OUTPUT );
			pinMode( FPins[ COMMAND_DATA_PIN ], OUTPUT );

			pinMode(Mitov::PLDiono::LCD_PWR, OUTPUT); digitalWrite(Mitov::PLDiono::LCD_PWR, LOW);
			pinMode(Mitov::PLDiono::LCD_BACKLIGHT, OUTPUT); digitalWrite(Mitov::PLDiono::LCD_BACKLIGHT, HIGH);

			// Needed to allow reading from the Display!
			pinMode(Mitov::PLDiono::TOUCH_CS, OUTPUT); digitalWrite(Mitov::PLDiono::TOUCH_CS, HIGH); 

			delay(1500); // give LCD enough time to reset completely
			pinMode(Mitov::PLDiono::LCD_PWR, OUTPUT); digitalWrite(Mitov::PLDiono::LCD_PWR, HIGH);
	    
			delay(250);
			digitalWrite(Mitov::PLDiono::LCD_RST, LOW);
			delay(50);
			digitalWrite(Mitov::PLDiono::LCD_RST, HIGH);
			delay(50);

//			analogWrite( Mitov::PLDiono::LCD_BACKLIGHT, 250 );

//			digitalWrite(Mitov::PLDiono::LCD_BACKLIGHT, LOW);

			UpdateBacklight();

/*
			digitalWrite(FPins[ RESET_PIN ],HIGH);
			delay(50);
			digitalWrite(FPins[ RESET_PIN ],LOW);
			delay(100);
	
			digitalWrite(FPins[ RESET_PIN ],HIGH);
*/
			delay(50);
			digitalWrite(FPins[ CHIP_SELECT_PIN ],HIGH);

#ifdef _INHERITED_DISPLAY_TOUCH_
			Touch.Init();
#endif
		}

	public:
#if ( defined(__ILI9341_SCROLL__))
		virtual void Scroll( TGraphicsPos X, TGraphicsPos Y, TUniColor color ) override
		{
//			Serial.print( X ); Serial.print( " " );	Serial.println( Y );

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
/*
				cmd( 0x33 );
				w_data( 0 );
				w_data( 0 );

				w_data( ( HEIGHT + Y ) >> 8 );
				w_data( HEIGHT + Y );

				w_data( ( -Y ) >> 8 );
				w_data( -Y );

//				setAddrWindow( AFromXIndex, 0, AFromXIndex + AWidth - 1, HEIGHT + Y );
//				setAddrWindow( 0, 0, WIDTH - 1, HEIGHT - 1 );

				cmd( 0x37 );
//				w_data( ( -Y ) >> 8 );
				//w_data( -Y );
				w_data( 0 );
				w_data( 8 );
//				delay( 100 );

				return;
//				Serial.println( "SCROLL" );
*/
				for( int ay = 0; ay < AHeight; ++ay )
				{
					if( X < 0 )
					{
						uint16_t *ABuffer = new uint16_t[ AWidth ];
						setAddrWindow( AFromXIndex, ay + AFromYIndex, AFromXIndex + AWidth - 1, ay + AFromYIndex );

						SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );
						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

						FastSPI( 0x2E ); // Read command
//						cmd( 0x2E ); // Read command

						FastSPI(0x00); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = SPI.transfer(0x00);
							AResult <<= 8;
							AResult |= SPI.transfer(0x00);
							AResult <<= 8;
							AResult |= SPI.transfer(0x00);

							ABuffer[ ax ] = Func::Color565( AResult );
						}

						SPI.endTransaction();
						digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
//						DDRD |= 0b11111100;
//						DDRB |= 0b00000011;


						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

//						cmd( 0x2C );
						SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );

						FastSPI( 0x2C );

						digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							FastSPI( AColor >> 8 );
							FastSPI( AColor );
						}

						SPI.endTransaction();

						digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

						delete [] ABuffer;

						drawFastHLine( AWidth, ay + AToYIndex, width() - AWidth, color );
					}

					else
					{
//				Serial.println( "SCROLL" );
						uint16_t *ABuffer = new uint16_t[ AWidth ];

						setAddrWindow( AFromXIndex, ay + AFromYIndex, AFromXIndex + AWidth - 1, ay + AFromYIndex );

						SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );
						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

						FastSPI( 0x2E ); // Read command
//						cmd( 0x2E ); // Read command

						FastSPI(0x00); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = SPI.transfer(0x00);
							AResult <<= 8;
							AResult |= SPI.transfer(0x00);
							AResult <<= 8;
							AResult |= SPI.transfer(0x00);

							ABuffer[ ax ] = Func::Color565( AResult );
						}

						SPI.endTransaction();
						digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

//						cmd( 0x2C );
						SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );

						FastSPI( 0x2C );

						digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							FastSPI( AColor >> 8 );
							FastSPI( AColor );
						}

						SPI.endTransaction();

						digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

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

						SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );
						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

						FastSPI( 0x2E ); // Read command
//						cmd( 0x2E ); // Read command

						FastSPI(0x00); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = SPI.transfer(0x00);
							AResult <<= 8;
							AResult |= SPI.transfer(0x00);
							AResult <<= 8;
							AResult |= SPI.transfer(0x00);

							ABuffer[ ax ] = Func::Color565( AResult );
						}

						SPI.endTransaction();
						digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

//						cmd( 0x2C );

						SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );

						FastSPI( 0x2C );

						digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							FastSPI( AColor >> 8 );
							FastSPI( AColor );
						}

						SPI.endTransaction();

						digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

						delete [] ABuffer;

						drawFastHLine( AWidth, ay + AToYIndex, width() - AWidth, color );
					}

					else
					{
						uint16_t *ABuffer = new uint16_t[ AWidth ];

						setAddrWindow( AFromXIndex, ay + AFromYIndex, AFromXIndex + AWidth - 1, ay + AFromYIndex );

						SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );
						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

						FastSPI( 0x2E ); // Read command
//						cmd( 0x2E ); // Read command

						FastSPI(0x00); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = SPI.transfer(0x00);
							AResult <<= 8;
							AResult |= SPI.transfer(0x00);
							AResult <<= 8;
							AResult |= SPI.transfer(0x00);

							ABuffer[ ax ] = Func::Color565( AResult );
						}

						SPI.endTransaction();
						digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

//						cmd( 0x2C );
						SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );

						FastSPI( 0x2C );

						digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							FastSPI( AColor >> 8 );
							FastSPI( AColor );
						}

						SPI.endTransaction();

						digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

						delete [] ABuffer;

						drawFastHLine( 0, ay + AToYIndex, X, color );
					}
				}

				fillRect( 0, 0, width(), Y, color );
			}
		}
#endif
		virtual TUniColor GetPixelColor( int16_t x, int16_t y ) override
		{
//			return 0xFFFFFFFF;
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if((x < 0) ||(x >= AWidth) || (y < 0) || (y >= AHeight)) 
				return 0x0000;

			setAddrWindow(x,y,x+1,y+1);

			SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
			digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );
			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

			FastSPI( 0x2E ); // Read command
//			cmd( 0x2E ); // Read command

			FastSPI( 0x00 ); // Fake read

			TUniColor AResult = SPI.transfer(0x00);
			AResult <<= 8;
			AResult |= SPI.transfer(0x00);
			AResult <<= 8;
			AResult |= SPI.transfer(0x00);

			SPI.endTransaction();
			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

//			if( AResult )
//				Serial.println( int( AResult ), HEX );

//			return 0xFF000000 | Func::ColorFrom565( AResult ); // Not supported
			return 0xFF030303 | AResult; // Not supported
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

//			cmd( 0x2C );
			SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );
			digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );

			FastSPI( 0x2C );

			digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );

			for( y=0; y < h; ++y )
				for( x=0; x < w; ++x )
				{
					FastSPI( hi );
					FastSPI( lo );
				}

			SPI.endTransaction();

			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
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

			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );
			digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );

			SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

			FastSPI( 0x2C );

			digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );

			FastSPI( color >> 8 ); 
			FastSPI( color );

			SPI.endTransaction();

			
			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

/*
			cmd( 0x2C );

			w_data( color >> 8 ); 
			w_data( color );
*/
		}

	public:
		virtual int16_t width(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return HEIGHT;

			return WIDTH;
		}

		virtual int16_t height(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return WIDTH;

			return HEIGHT;
		}

	protected:
		virtual void UpdateOrientation() override
		{
			cmd( ILI9341Extended::MADCTL);

			switch( Orientation )
			{
				case goRight: 	w_data(ILI9341Extended::MADCTL_BGR); break;
				case goLeft:	w_data(ILI9341Extended::MADCTL_MX | ILI9341Extended::MADCTL_MY | ILI9341Extended::MADCTL_BGR); break;
				case goUp: 		w_data(ILI9341Extended::MADCTL_MY | ILI9341Extended::MADCTL_MV | ILI9341Extended::MADCTL_BGR); break;
				case goDown:	w_data(ILI9341Extended::MADCTL_MX | ILI9341Extended::MADCTL_MV | ILI9341Extended::MADCTL_BGR); break;
/*
				case goUp: 		w_data(ILI9341Extended::MADCTL_BGR); break;
				case goDown:	w_data(ILI9341Extended::MADCTL_MX | ILI9341Extended::MADCTL_MY | ILI9341Extended::MADCTL_BGR); break;
				case goLeft: 	w_data(ILI9341Extended::MADCTL_MY | ILI9341Extended::MADCTL_MV | ILI9341Extended::MADCTL_BGR); break;
				case goRight:	w_data(ILI9341Extended::MADCTL_MX | ILI9341Extended::MADCTL_MV | ILI9341Extended::MADCTL_BGR); break;
*/
			}

			setAddrWindow( 0, 0, width() - 1, height() - 1 ); // CS_IDLE happens here
		}

	protected:
/*
		uint8_t r_data()
		{
//			digitalWrite( FPins[ WRITE_PIN ], HIGH );
			digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );
//			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );


//			uint8_t AResult = 0;
//			for( int i = FIRST_DATA_PIN; i < FIRST_DATA_PIN + 8; ++i )
//				if( digitalRead( FPins[ i ] ) == HIGH )
//					AResult |= ( 1 << i );

			SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
			uint8_t AResult = SPI.transfer(0x00);
			SPI.endTransaction();

			digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );
//			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

//			if( AResult )
//				Serial.println( AResult );

			return AResult;
		}
*/
		virtual void writeRegister32(uint8_t r, uint32_t d) override 
		{
			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );
			digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );

			SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
			FastSPI( r );

			digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );

			FastSPI( d >> 24 );
			FastSPI( d >> 16 );
			FastSPI( d >> 8 );
			FastSPI( d );

			SPI.endTransaction();

			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
		}

		virtual void  w_data( uint8_t data ) override
		{
//			Serial.print( "w_data: " ); Serial.println( data, HEX );

			digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );
			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

			SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
			FastSPI( data );
			SPI.endTransaction();

			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
		}

		virtual void cmd( uint8_t cmd ) override
		{
//			Serial.print( "cmd: " ); Serial.println( cmd, HEX );
			digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );
			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

			SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
			FastSPI( cmd );
			SPI.endTransaction();

			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
		}

	protected:
/*
		virtual void SystemInit() override
		{
//			pinMode(TOUCH_IRQ, INPUT);

			pinMode(PLDiono::LCD_PWR, OUTPUT); digitalWrite(PLDiono::LCD_PWR, LOW);
			pinMode(PLDiono::LCD_BACKLIGHT, OUTPUT); digitalWrite(PLDiono::LCD_BACKLIGHT, HIGH);

//			pinMode(TOUCH_CS, OUTPUT); digitalWrite(TOUCH_CS, HIGH); 

			delay(1500); // give LCD enough time to reset completely
			pinMode(PLDiono::LCD_PWR, OUTPUT); digitalWrite(PLDiono::LCD_PWR, HIGH);
	    
			delay(250);
			digitalWrite(PLDiono::LCD_RST, LOW);
			delay(50);
			digitalWrite(PLDiono::LCD_RST, HIGH);
			delay(50);

			digitalWrite(PLDiono::LCD_BACKLIGHT, LOW);

			inherited::SystemInit();
		}
*/
#ifdef _INHERITED_DISPLAY_TOUCH_
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			Touch.ReadData( Orientation );
//			inherited::SystemLoopBegin( currentMicros );
		}
#endif
	public:
		ArduinoPLDuinoDisplay( const unsigned char * AFont ) :
			inherited( PLDiono::LCD_DC, PLDiono::LCD_CS, -1, AFont )
		{
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

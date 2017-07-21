////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_ILI9341_SPI_h
#define _MITOV_DISPLAY_ILI9341_SPI_h

#include <Mitov_Graphics_Color.h>
#include <Mitov_Arduino_SPI.h>
#include <Mitov_Display_ILI9341.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class DisplayILI9341_SPI : public DisplayILI9341
	{
		typedef DisplayILI9341 inherited;

	protected:
		BasicSPI	&FSPI;

	protected:
		virtual void InitPins() override
		{
//			Serial.println( "RESET" );
#ifdef _INHERITED_ILI9341_DISPLAY_RESET_PIN_
			pinMode( FPins[ RESET_PIN ], OUTPUT );
#endif
			pinMode( FPins[ CHIP_SELECT_PIN ], OUTPUT );
			pinMode( FPins[ COMMAND_DATA_PIN ], OUTPUT );

#ifdef _INHERITED_ILI9341_DISPLAY_RESET_PIN_
			digitalWrite(FPins[ RESET_PIN ],HIGH);
			delay(50);
			digitalWrite(FPins[ RESET_PIN ],LOW);
			delay(100);
	
			digitalWrite(FPins[ RESET_PIN ],HIGH);
#endif
			delay(50);
			digitalWrite(FPins[ CHIP_SELECT_PIN ],HIGH);
		}

	public:
#if ( defined(__ILI9341_SCROLL__))
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

						FSPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );
						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

						FSPI.transfer( 0x2E ); // Read command
//						cmd( 0x2E ); // Read command

						FSPI.transfer(0x00); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = FSPI.transfer(0x00);
							AResult <<= 8;
							AResult |= FSPI.transfer(0x00);
							AResult <<= 8;
							AResult |= FSPI.transfer(0x00);

							ABuffer[ ax ] = Func::Color565( AResult );
						}

						FSPI.endTransaction();
						digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
//						DDRD |= 0b11111100;
//						DDRB |= 0b00000011;


						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

//						cmd( 0x2C );
						FSPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );

						FSPI.transfer( 0x2C );

						digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							FSPI.transfer( AColor >> 8 );
							FSPI.transfer( AColor );
						}

						FSPI.endTransaction();

						digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

						delete [] ABuffer;

						drawFastHLine( AWidth, ay + AToYIndex, width() - AWidth, color );
					}

					else
					{
//				Serial.println( "SCROLL" );
						uint16_t *ABuffer = new uint16_t[ AWidth ];

						setAddrWindow( AFromXIndex, ay + AFromYIndex, AFromXIndex + AWidth - 1, ay + AFromYIndex );

						FSPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );
						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

						FSPI.transfer( 0x2E ); // Read command
//						cmd( 0x2E ); // Read command

						FSPI.transfer(0x00); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = FSPI.transfer(0x00);
							AResult <<= 8;
							AResult |= FSPI.transfer(0x00);
							AResult <<= 8;
							AResult |= FSPI.transfer(0x00);

							ABuffer[ ax ] = Func::Color565( AResult );
						}

						FSPI.endTransaction();
						digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

//						cmd( 0x2C );
						FSPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );

						FSPI.transfer( 0x2C );

						digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							FSPI.transfer( AColor >> 8 );
							FSPI.transfer( AColor );
						}

						FSPI.endTransaction();

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

						FSPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );
						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

						FSPI.transfer( 0x2E ); // Read command
//						cmd( 0x2E ); // Read command

						FSPI.transfer(0x00); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = FSPI.transfer(0x00);
							AResult <<= 8;
							AResult |= FSPI.transfer(0x00);
							AResult <<= 8;
							AResult |= FSPI.transfer(0x00);

							ABuffer[ ax ] = Func::Color565( AResult );
						}

						FSPI.endTransaction();
						digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

//						cmd( 0x2C );

						FSPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );

						FSPI.transfer( 0x2C );

						digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							FSPI.transfer( AColor >> 8 );
							FSPI.transfer( AColor );
						}

						FSPI.endTransaction();

						digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

						delete [] ABuffer;

						drawFastHLine( AWidth, ay + AToYIndex, width() - AWidth, color );
					}

					else
					{
						uint16_t *ABuffer = new uint16_t[ AWidth ];

						setAddrWindow( AFromXIndex, ay + AFromYIndex, AFromXIndex + AWidth - 1, ay + AFromYIndex );

						FSPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );
						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

						FSPI.transfer( 0x2E ); // Read command
//						cmd( 0x2E ); // Read command

						FSPI.transfer(0x00); // Fake read

						for( int ax = 0; ax < AWidth; ++ax )
						{
							TUniColor AResult = FSPI.transfer(0x00);
							AResult <<= 8;
							AResult |= FSPI.transfer(0x00);
							AResult <<= 8;
							AResult |= FSPI.transfer(0x00);

							ABuffer[ ax ] = Func::Color565( AResult );
						}

						FSPI.endTransaction();
						digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );

						setAddrWindow( AToXIndex, ay + AToYIndex, AToXIndex + AWidth - 1, ay + AToYIndex );

//						cmd( 0x2C );
						FSPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

						digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );
						digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );

						FSPI.transfer( 0x2C );

						digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );

						for( int ax = 0; ax < AWidth; ++ax )
						{
							uint16_t AColor = ABuffer[ ax ];
							FSPI.transfer( AColor >> 8 );
							FSPI.transfer( AColor );
						}

						FSPI.endTransaction();

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

			FSPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
			digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );
			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

			FSPI.transfer( 0x2E ); // Read command
//			cmd( 0x2E ); // Read command

			FSPI.transfer(0x00); // Fake read

			TUniColor AResult = FSPI.transfer(0x00);
			AResult <<= 8;
			AResult |= FSPI.transfer(0x00);
			AResult <<= 8;
			AResult |= FSPI.transfer(0x00);

			FSPI.endTransaction();
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

			FSPI.transfer( 0x2C );

			digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );

			for( y=0; y < h; ++y )
				for( x=0; x < w; ++x )
				{
					FSPI.transfer( hi );
					FSPI.transfer( lo );
				}

			FSPI.endTransaction();

			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
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

			FSPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
			uint8_t AResult = FSPI.transfer(0x00);
			FSPI.endTransaction();

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

			FSPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

			FSPI.transfer( r );

			digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );

			FSPI.transfer( d >> 24 );
			FSPI.transfer( d >> 16 );
			FSPI.transfer( d >> 8 );
			FSPI.transfer( d );

			FSPI.endTransaction();

			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
		}

		virtual void  w_data( uint8_t data ) override
		{
//			Serial.print( "w_data: " ); Serial.println( data, HEX );

			digitalWrite( FPins[ COMMAND_DATA_PIN ], HIGH );
			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

/*
    uint8_t backupSPCR = SPCR;
    SPCR = mySPCR;
    SPDR = c;
    while(!(SPSR & _BV(SPIF)));
    SPCR = backupSPCR;
*/
/*
    FSPI.setClockDivider(SPI_CLOCK_DIV2); // 8 MHz (full! speed!)
    FSPI.setBitOrder(MSBFIRST);
    FSPI.setDataMode(SPI_MODE0);
*/
			FSPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
			FSPI.transfer( data );
			FSPI.endTransaction();

			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
		}

		virtual void cmd( uint8_t cmd ) override
		{
//			Serial.print( "cmd: " ); Serial.println( cmd, HEX );
			digitalWrite( FPins[ COMMAND_DATA_PIN ], LOW );
			digitalWrite( FPins[ CHIP_SELECT_PIN ], LOW );

/*
    uint8_t backupSPCR = SPCR;
    SPCR = mySPCR;
    SPDR = c;
    while(!(SPSR & _BV(SPIF)));
    SPCR = backupSPCR;
*/
/*
	FSPI.setClockDivider(SPI_CLOCK_DIV2); // 8 MHz (full! speed!)
    FSPI.setBitOrder(MSBFIRST);
    FSPI.setDataMode(SPI_MODE0);
*/
			FSPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
			FSPI.transfer( cmd );
			FSPI.endTransaction();

			digitalWrite( FPins[ CHIP_SELECT_PIN ], HIGH );
		}

	public:
		DisplayILI9341_SPI( BasicSPI &ASPI, uint16_t AChipSelect, uint16_t ADataCommand, uint16_t AReset, const unsigned char * AFont ) :
			inherited( ADataCommand, AChipSelect, AReset, AFont ),
			FSPI( ASPI )
		{
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

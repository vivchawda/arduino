////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_SSD1306_h
#define _MITOV_DISPLAY_SSD1306_h

#include <Mitov.h>
#include <Mitov_Graphics_Monochrome.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class TArduinoDisplaySSD1306PreChargePeriod
	{
	public:
		uint8_t	Phase1 : 4;
		uint8_t	Phase2 : 4;

	public:
		TArduinoDisplaySSD1306PreChargePeriod() :
			Phase1( 1 ),
			Phase2( 15 )
		{
		}
	};
//---------------------------------------------------------------------------
	enum ArduinoDisplayOLEDType { odtSSD1306, odtSH1106 };
//---------------------------------------------------------------------------
	template<ArduinoDisplayOLEDType T_PARAM> class DisplaySSD1306 : public OpenWire::Component, public GraphicsImpl
	{
		typedef OpenWire::Component inherited;

	protected:
		static const uint8_t SSD1306_SETCONTRAST = 0x81;
		static const uint8_t SSD1306_DISPLAYALLON_RESUME = 0xA4;
		static const uint8_t SSD1306_DISPLAYALLON = 0xA5;
		static const uint8_t SSD1306_NORMALDISPLAY = 0xA6;
		static const uint8_t SSD1306_INVERTDISPLAY = 0xA7;
		static const uint8_t SSD1306_DISPLAYOFF = 0xAE;
		static const uint8_t SSD1306_DISPLAYON = 0xAF;

		static const uint8_t SSD1306_SETDISPLAYOFFSET = 0xD3;
		static const uint8_t SSD1306_SETCOMPINS = 0xDA;

		static const uint8_t SSD1306_SETVCOMDETECT = 0xDB;

		static const uint8_t SSD1306_SETDISPLAYCLOCKDIV = 0xD5;
		static const uint8_t SSD1306_SETPRECHARGE = 0xD9;

		static const uint8_t SSD1306_SETMULTIPLEX = 0xA8;

		static const uint8_t SSD1306_SETLOWCOLUMN = 0x00;
		static const uint8_t SSD1306_SETHIGHCOLUMN = 0x10;

		static const uint8_t SSD1306_SETSTARTLINE = 0x40;

		static const uint8_t SSD1306_MEMORYMODE = 0x20;
		static const uint8_t SSD1306_COLUMNADDR = 0x21;
		static const uint8_t SSD1306_PAGEADDR = 0x22;

		static const uint8_t SSD1306_COMSCANINC = 0xC0;
		static const uint8_t SSD1306_COMSCANDEC = 0xC8;

		static const uint8_t SSD1306_SEGREMAP = 0xA0;

		static const uint8_t SSD1306_CHARGEPUMP = 0x8D;

		static const uint8_t SSD1306_EXTERNALVCC = 0x1;
		static const uint8_t SSD1306_SWITCHCAPVCC = 0x2;

	public:
#ifdef _MITOV_SSD1306_RESET_PIN_
		OpenWire::SourcePin			ResetOutputPin;
#endif

#ifdef _MITOV_SSD1306_REFRESH_PIN_
		OpenWire::ConnectSinkPin	RefreshInputPin;
#endif

	public:
		float	Contrast = 0.812;

		TGraphicsMonochromeTextSettings	Text;

		TArduinoDisplaySSD1306PreChargePeriod	PreChargePeriod;

		bool	UseChargePump : 1;

	protected:
		bool	FModified : 1;
		uint8_t *buffer;
		int16_t	FWidth;
		int16_t	FHeight;

#ifdef _MITOV_DISPLAY_SSD1306_I2C
		TwoWire &FWire;
#endif
	public:
		void UpdateContrast()
		{
			SendCommand( SSD1306_SETCONTRAST );
			SendCommand( Contrast * 255 + 0.5 );
		}

	public:
		virtual int16_t width(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return FWidth;

			return FHeight;
		}

		virtual int16_t height(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return FHeight;

			return FWidth;
		}

	public:
		virtual void drawPixel(int16_t x, int16_t y, TUniColor color) override
		{
			if( color == tmcNone )
				return;

			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return;

			// check rotation, move pixel around if necessary
			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = FWidth - x - 1;
					break;

				case goDown:
					x = FWidth - x - 1;
					y = FHeight - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = FHeight - y - 1;
					break;
				}  

			// x is which column
			switch (color) 
			{
				case tmcWhite :		buffer[x+ (y/8) * FWidth ] |=  (1 << (y&7)); break;
				case tmcBlack :		buffer[x+ (y/8) * FWidth ] &= ~(1 << (y&7)); break; 
				case tmcInvert :	buffer[x+ (y/8) * FWidth ] ^=  (1 << (y&7)); break; 
			}

			FModified = true;
    	}

		virtual void ClearScreen( TUniColor color ) override
		{
			switch( color )
			{
			case tmcBlack :
				memset(buffer, 0, ( FWidth * FHeight / 8));
				break;

			case tmcWhite :
				memset(buffer, 255, ( FWidth * FHeight / 8));
				break;

			case tmcInvert :
				for( int i = 0; i < FWidth * FHeight / 8; ++i )
					buffer[ i ] = ~buffer[ i ];

				break;

			}
		}

		virtual TUniColor GetPixelColor( int16_t x, int16_t y ) override
		{
			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return false;

			// check rotation, move pixel around if necessary
			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = FWidth - x - 1;
					break;

				case goDown:
					x = FWidth - x - 1;
					y = FHeight - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = FHeight - y - 1;
					break;
				}  

			return (( buffer[ x + ( y / 8 ) * FWidth ] & ( 1 << ( y & 7 ))) != 0 );
		}

		void drawFastHLine( Graphics *&AGraphics, int16_t x, int16_t y, int16_t w, TUniColor color) 
		{
			bool bSwap = false;
			switch( Orientation ) 
			{ 
//				case goUp:
					// 0 degree rotation, do nothing
//					break;
				case goRight:
					// 90 degree rotation, swap x & y for rotation, then invert x
					bSwap = true;
					swap(x, y);
					x = FWidth - x - 1;
					break;

				case goDown:
					// 180 degree rotation, invert x and y - then shift y around for height.
					x = FWidth - x - 1;
					y = FHeight - y - 1;
					x -= (w-1);
					break;

				case goLeft:
					// 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
					bSwap = true;
					swap(x, y);
					y = FHeight - y - 1;
					y -= (w-1);
					break;
			}

			if( bSwap ) 
				drawFastVLineInternal(x, y, w, color);

			else
				drawFastHLineInternal(x, y, w, color);
		}

		virtual void Scroll( TGraphicsPos X, TGraphicsPos Y, TUniColor color ) override
		{
			if( X == 0 && Y == 0 )
				return;

			// check rotation, move pixel around if necessary
			switch ( Orientation ) 
			{
				case goRight:
					swap(X, Y);
					X = -X;
					break;

				case goDown:
					X = -X;
					Y = -Y;
					break;

				case goLeft:
					swap(X, Y);
					Y = -Y;
					break;
				}  		

			uint8_t	AOffset = abs( Y ) % 8;
			int32_t	AYBytes = Y / 8;
			AYBytes *= FWidth;

/*
			if( ! Y )
			{
				uint8_t *ABuffer = buffer;
				if( X > 0 )
					for( int ayi = 0; ayi < FHeight / 8; ++ ayi, ABuffer += FWidth )
					{
						memmove( ABuffer + X, ABuffer, FWidth - X );
						switch( color )
						{
							case tmcBlack:
								memset( ABuffer, 0, X ); break;

							case tmcWhite:
								memset( ABuffer, 255, X ); break;

							case tmcInvert:
								for( int axi = 0; axi < X; ++ axi )
									ABuffer[ axi ] = ~ABuffer[ axi ];
						}
					}

				else // Y = 0 && X < 0
					for( int ayi = 0; ayi < FHeight / 8; ++ ayi, ABuffer += FWidth )
					{
						memmove( ABuffer, ABuffer - X, FWidth + X );
						switch( color )
						{
							case tmcBlack:
								memset( ABuffer + FWidth + X, 0, -X ); break;

							case tmcWhite:
								memset( ABuffer + FWidth + X, 255, -X ); break;

							case tmcInvert:
								for( int axi = FWidth + X; axi < FWidth; ++ axi )
									ABuffer[ axi ] = ~ABuffer[ axi ];
						}
					}

			}

			else // Y <> 0
*/
			{
				if( Y > 0 )
				{
					if( X == 0 )
					{
						if( AYBytes )
							memmove( buffer + AYBytes, buffer, ( FHeight / 8 ) * FWidth - AYBytes );

					}

					else if( X > 0 )
					{
						uint8_t *ABuffer = buffer + FWidth * (( ( FHeight - Y + 7 ) / 8 ) - 1 );
						for( int ayi = 0; ayi < ( FHeight - Y + 7 ) / 8; ++ ayi, ABuffer -= FWidth )
						{
							memmove( ABuffer + AYBytes + X, ABuffer, FWidth - X );
							switch( color )
							{
								case tmcBlack:
									memset( ABuffer + AYBytes, 0, X ); break;

								case tmcWhite:
									memset( ABuffer + AYBytes, 255, X ); break;

								case tmcInvert:
									for( int axi = 0; axi < X; ++ axi )
										ABuffer[ axi + AYBytes ] = ~ABuffer[ axi + AYBytes ];
							}
						}
					}

					else // Y > 0 && X < 0
					{ 
						uint8_t *ABuffer = buffer + FWidth * (( ( FHeight - Y + 7 ) / 8 ) - 1 );
						for( int ayi = 0; ayi < ( FHeight - Y + 7 ) / 8; ++ ayi, ABuffer -= FWidth )
						{
							memmove( ABuffer + AYBytes, ABuffer - X, FWidth + X );
							switch( color )
							{
								case tmcBlack:
									memset( ABuffer + AYBytes + FWidth + X, 0, -X ); break;

								case tmcWhite:
									memset( ABuffer + AYBytes + FWidth + X, 255, -X ); break;

								case tmcInvert:
									for( int axi = FWidth + X; axi < FWidth; ++ axi )
										ABuffer[ axi + AYBytes ] = ~ABuffer[ axi + AYBytes ];
							}
						}
					}

					uint8_t * AToPtr = buffer + FWidth * FHeight / 8 - 1;
					uint8_t *AFromPtr = AToPtr - FWidth;
					for( int ayi = 0; ayi < ( FHeight - Y + 7 ) / 8 - 1; ++ ayi )
						for( int axi = 0; axi < FWidth; ++ axi, --AFromPtr, --AToPtr )
							*AToPtr = *AToPtr << AOffset | ( *AFromPtr >> ( 8 - AOffset ));

					if( color != tmcNone )
					{
						uint8_t	AMask;
						if( color == tmcBlack )
							AMask = 0xFF << AOffset;

						else
							AMask = 0xFF >> ( 8 - AOffset );

						AToPtr = buffer + AYBytes;
						for( int axi = 0; axi < FWidth; ++ axi, ++AToPtr )
						{
							switch( color )
							{
								case tmcBlack:
									*AToPtr <<= AOffset;
									*AToPtr &= AMask; 
									break;

								case tmcWhite:
									*AToPtr <<= AOffset;
									*AToPtr |= AMask; 
									break;

								case tmcInvert:
									*AToPtr <<= AOffset | ( *AToPtr & ( ~AMask ));
									*AToPtr ^= AMask; 
									break;
							}
						}
					}
				}

				else // Y < 0
				{
					if( X == 0 )
					{
						if( AYBytes )
							memmove( buffer, buffer - AYBytes, ( FHeight / 8 ) * FWidth + AYBytes );

					}

					else if( X > 0 )
					{
						uint8_t *ABuffer = buffer;
						for( int ayi = 0; ayi < ( FHeight + Y + 7 ) / 8; ++ ayi, ABuffer += FWidth )
						{
							memmove( ABuffer + X, ABuffer - AYBytes, FWidth - X );
							switch( color )
							{
								case tmcBlack:
									memset( ABuffer, 0, X ); break;

								case tmcWhite:
									memset( ABuffer, 255, X ); break;

								case tmcInvert:
									for( int axi = 0; axi < X; ++ axi )
										ABuffer[ axi ] = ~ABuffer[ axi ];
							}
						}
					}
					else // Y < 0 && X < 0
					{
						uint8_t *ABuffer = buffer;
						for( int ayi = 0; ayi < ( FHeight + Y + 7 ) / 8; ++ ayi, ABuffer += FWidth )
						{
							memmove( ABuffer, ABuffer - AYBytes - X, FWidth + X );
							switch( color )
							{
								case tmcBlack:
									memset( ABuffer + FWidth + X, 0, -X ); break;

								case tmcWhite:
									memset( ABuffer + FWidth + X, 255, -X ); break;

								case tmcInvert:
									for( int axi = FWidth + X; axi < FWidth; ++ axi )
										ABuffer[ axi ] = ~ABuffer[ axi ];
							}
						}
					}

					if( AOffset )
					{
						uint8_t * AToPtr = buffer;
						uint8_t *AFromPtr = AToPtr + FWidth;
						for( int ayi = 0; ayi < ( FHeight + Y + 7 ) / 8 - 1; ++ ayi )
							for( int axi = 0; axi < FWidth; ++ axi, ++AFromPtr, ++AToPtr )
								*AToPtr = *AToPtr >> AOffset | ( *AFromPtr << ( 8 - AOffset ));

						if( color != tmcNone )
						{
							uint8_t	AMask;
							if( color == tmcBlack )
								AMask = 0xFF >> AOffset;

							else
								AMask = 0xFF << ( 8 - AOffset );

							AToPtr = buffer + FWidth * FHeight / 8 + AYBytes - FWidth;
							for( int axi = 0; axi < FWidth; ++ axi, ++AToPtr )
							{
								switch( color )
								{
									case tmcBlack:
										*AToPtr >>= AOffset;
										*AToPtr &= AMask; 
										break;

									case tmcWhite:
										*AToPtr >>= AOffset;
										*AToPtr |= AMask; 
										break;

									case tmcInvert:
										*AToPtr >>= AOffset | ( *AToPtr & ( ~AMask ));
										*AToPtr ^= AMask; 
										break;
								}
							}
						}
					}
				}

				if( AYBytes )
				{
					if( Y > 0 )
					{
						switch( color )
						{
							case tmcBlack:
								memset( buffer, 0, AYBytes ); break;

							case tmcWhite:
								memset( buffer, 255, AYBytes ); break;

							case tmcInvert:
								for( int axi = 0; axi < FWidth; ++ axi )
									buffer[ axi ] = ~buffer[ axi ];
						}
					}

					else // Y < 0
					{
						switch( color )
						{
							case tmcBlack:
								memset( buffer + FHeight * FWidth / 8 + AYBytes, 0, - AYBytes ); break;

							case tmcWhite:
								memset( buffer + FHeight * FWidth / 8 + AYBytes, 255, - AYBytes ); break;

							case tmcInvert:
								for( int axi = 0; axi < FWidth; ++ axi )
									buffer[ axi + FHeight * FWidth / 8 + AYBytes ] = ~buffer[ axi + FHeight * FWidth / 8 + AYBytes ];
						}
					}
				}
			}
		}

	protected:
/*
		void ScrollBufferUp( uint8_t ANumLines )
		{
			memmove( buffer, buffer + ANumLines * FWidth / 8, ( FHeight - ANumLines ) * FWidth / 8 );
			memset( buffer + ( FHeight - ANumLines ) * FWidth / 8, 0, ANumLines * FWidth / 8 );
		}
*/
		void drawFastHLineInternal(int16_t x, int16_t y, int16_t w, TUniColor color) 
		{
			if( color == tmcNone )
				return;

			// Do bounds/limit checks
			if(y < 0 || y >= FHeight) 
				return;

			// make sure we don't try to draw below 0
			if(x < 0) 
			{ 
				w += x;
				x = 0;
			}

			// make sure we don't go off the edge of the display
			if( (x + w) > FWidth) 
				w = (FWidth - x);

			// if our width is now negative, punt
			if(w <= 0) 
				return;

			// set up the pointer for  movement through the buffer
			register uint8_t *pBuf = buffer;
			// adjust the buffer pointer for the current row
			pBuf += ((y/8) * FWidth);
			// and offset x columns in
			pBuf += x;

			register uint8_t mask = 1 << (y&7);

			switch (color) 
			{
				case tmcWhite:         while(w--) { *pBuf++ |= mask; }; break;
				case tmcBlack: mask = ~mask;   while(w--) { *pBuf++ &= mask; }; break;
				case tmcInvert:         while(w--) { *pBuf++ ^= mask; }; break;
			}
		}

		void drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, TUniColor color) 
		{
			if( color == tmcNone )
				return;

			// do nothing if we're off the left or right side of the screen
			if(x < 0 || x >= FWidth) 
				return;

			// make sure we don't try to draw below 0
			if(__y < 0) 
			{ 
				// __y is negative, this will subtract enough from __h to account for __y being 0
				__h += __y;
				__y = 0;
			} 

			// make sure we don't go past the height of the display
			if( (__y + __h) > FHeight)
				__h = (FHeight - __y);
			
			// if our height is now negative, punt 
			if(__h <= 0)
				return;			

			// this display doesn't need ints for coordinates, use local byte registers for faster juggling
			register uint8_t y = __y;
			register uint8_t h = __h;


			// set up the pointer for fast movement through the buffer
			register uint8_t *pBuf = buffer;
			// adjust the buffer pointer for the current row
			pBuf += ((y/8) * FWidth);
			// and offset x columns in
			pBuf += x;

			// do the first partial byte, if necessary - this requires some masking
			register uint8_t mod = (y&7);
			if(mod) 
			{
				// mask off the high n bits we want to set 
				mod = 8-mod;

				// note - lookup table results in a nearly 10% performance improvement in fill* functions
				// register uint8_t mask = ~(0xFF >> (mod));
				static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
				register uint8_t mask = premask[mod];

				// adjust the mask if we're not going to reach the end of this byte
				if( h < mod)
					mask &= (0XFF >> (mod-h));

				switch (color) 
				{
					case tmcWhite :  *pBuf |=  mask;  break;
					case tmcBlack :  *pBuf &= ~mask;  break;
					case tmcInvert : *pBuf ^=  mask;  break;
				}
  
				// fast exit if we're done here!
				if(h<mod) { return; }

				h -= mod;

				pBuf += FWidth;
			}


			// write solid bytes while we can - effectively doing 8 rows at a time
			if(h >= 8) 
			{ 
				if (color == tmcInvert)  
				{          // separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
					do  
					{
						*pBuf=~(*pBuf);

						// adjust the buffer forward 8 rows worth of data
						pBuf += FWidth;

						// adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
						h -= 8;
					} while(h >= 8);
				}

				else 
				{
					// store a local value to work with 
					register uint8_t val = (color == tmcWhite) ? 255 : 0;

					do  
					{
						// write our value in
						*pBuf = val;

						// adjust the buffer forward 8 rows worth of data
						pBuf += FWidth;

						// adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
						h -= 8;
					} while(h >= 8);
				}
			}

			// now do the final partial byte, if necessary
			if(h) 
			{
				mod = h & 7;
				// this time we want to mask the low bits of the byte, vs the high bits we did above
				// register uint8_t mask = (1 << mod) - 1;
				// note - lookup table results in a nearly 10% performance improvement in fill* functions
				static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
				register uint8_t mask = postmask[mod];
				switch (color) 
				{
					case tmcWhite:   *pBuf |=  mask;  break;
					case tmcBlack:   *pBuf &= ~mask;  break;
					case tmcInvert:  *pBuf ^=  mask;  break;
				}
			}
		}

#ifdef _MITOV_DISPLAY_SSD1306_I2C
	public:
		uint8_t	Address = 0x3C;

	protected:
		virtual void SendCommand( uint8_t ACommand ) //override
		{
//			Serial.println( ACommand, HEX );
			FWire.beginTransmission( Address );
			FWire.write( 0x00 ); // Co = 0, D/C = 0
			FWire.write( ACommand );
			FWire.endTransmission();
		}

		virtual void SendBuffer() //override
		{
			// I2C
			for( uint16_t i=0; i < ( FWidth * FHeight /8); ) 
			{
				// send a bunch of data in one xmission
				FWire.beginTransmission( Address );
				FWire.write( 0x40 );
				FWire.write( buffer + i, 16 );
				i += 16;
				FWire.endTransmission();
			}
		}

#else // _MITOV_DISPLAY_SSD1306_I2C
		virtual void SendCommand(uint8_t ACommand ) = 0;
		virtual void SendBuffer() = 0;
#endif // _MITOV_DISPLAY_SSD1306_I2C
/*
	public:
		uint8_t	Address = 0x3C;

	protected:
		virtual void SendCommand( uint8_t ACommand ) //override
		{
//			Serial.println( ACommand, HEX );
			FWire.beginTransmission( Address );
			FWire.write( 0x00 ); // Co = 0, D/C = 0
			FWire.write( ACommand );
			FWire.endTransmission();
		}

		virtual void SendBuffer() //override
		{
			// I2C
			for( uint16_t i=0; i < ( FWidth * FHeight /8); ) 
			{
				// send a bunch of data in one xmission
				FWire.beginTransmission( Address );
				FWire.write( 0x40 );
				FWire.write( buffer + i, 16 );
				i += 16;
				FWire.endTransmission();
			}
		}
*/

		void SendCommands( uint8_t ACommands[], int ASize ) 
		{
			for( int i = 0; i < ASize; ++ i )
				SendCommand( ACommands[ i ] );
		}

public:
		void display()
		{
			SendCommand(SSD1306_COLUMNADDR);
			SendCommand(0);   // Column start address (0 = reset)
			SendCommand( FWidth - 1 ); // Column end address (127 = reset)

			SendCommand(SSD1306_PAGEADDR);
			SendCommand(0); // Page start address (0 = reset)
			if( FHeight == 64 )
				SendCommand(7); // Page end address

			if( FHeight == 32 )
				SendCommand(3); // Page end address

			if( FHeight == 16 )
				SendCommand(1); // Page end address

			// save I2C bitrate
		#ifdef TWBR
			uint8_t twbrbackup = TWBR;
			TWBR = 12; // upgrade to 400KHz!
		#endif

			//Serial.println(TWBR, DEC);
			//Serial.println(TWSR & 0x3, DEC);

			SendBuffer();

		#ifdef TWBR
			TWBR = twbrbackup;
		#endif
			FModified = false;
		}

	protected:
		virtual void SystemLoopEnd() override
		{
#ifdef _MITOV_SSD1306_REFRESH_PIN_
			if( FModified )
				if( ! RefreshInputPin.IsConnected() )
#endif
					display();

		}

		virtual void SystemInit() override
		{
//			Serial.println( "Test1" );
#ifdef __SAM3X8E__
			// Force 400 KHz I2C, rawr! (Uses pins 20, 21 for SDA, SCL)
			TWI1->TWI_CWGR = 0;
			TWI1->TWI_CWGR = ((VARIANT_MCK / (2 * 400000)) - 4) * 0x101;
#endif

#ifdef _MITOV_SSD1306_RESET_PIN_
			if( ResetOutputPin.IsConnected() )
			{
				ResetOutputPin.SendValue( true );
				// VDD (3.3V) goes high at start, lets just chill for a ms
				delay(1);
				// bring reset low
				ResetOutputPin.SendValue( false );
				// wait 10ms
				delay(10);
				// bring out of reset
				ResetOutputPin.SendValue( true );
				// turn on VCC (9V?)
			}
#endif

			uint8_t InitCommands1[] =
			{
				SSD1306_DISPLAYOFF,                    // 0xAE
				SSD1306_SETDISPLAYCLOCKDIV,            // 0xD5
				0x80,                                  // the suggested ratio 0x80
				SSD1306_SETMULTIPLEX,                  // 0xA8
				0x3F,
				SSD1306_SETDISPLAYOFFSET,              // 0xD3
				0x0,                                   // no offset
				SSD1306_SETSTARTLINE | 0x0,            // line #0
				SSD1306_CHARGEPUMP                     // 0x8D
			};

			// Init sequence for 128x64 OLED module
			SendCommands( ARRAY_PARAM( InitCommands1 ) );
			SendCommand( UseChargePump ? 0x10 : 0x14 );

			uint8_t InitCommands2[] =
			{
				SSD1306_MEMORYMODE,                    // 0x20
				0x00,                                  // 0x0 act like ks0108
				SSD1306_SEGREMAP | 0x1,
				SSD1306_COMSCANDEC,
				SSD1306_SETCOMPINS,                    // 0xDA
				0x12
//					SSD1306_SETCONTRAST                    // 0x81
			};

			SendCommands( ARRAY_PARAM( InitCommands2 ) );
			UpdateContrast();

			SendCommand(SSD1306_SETPRECHARGE);                  // 0xd9
			SendCommand( ( uint8_t( PreChargePeriod.Phase2 ) << 4 ) | PreChargePeriod.Phase1 );

			uint8_t InitCommands3[] =
			{
				SSD1306_SETVCOMDETECT,                 // 0xDB
				0x40,
				SSD1306_DISPLAYALLON_RESUME,           // 0xA4
				SSD1306_NORMALDISPLAY,                 // 0xA6
				SSD1306_DISPLAYON
			};

			SendCommands( ARRAY_PARAM( InitCommands3 ) );

			setTextSize( Text.Size );
			setTextColor( Text.Color, Text.BackgroundColor );
			setTextWrap( Text.Wrap );
			

//			FDisplay.begin( SSD1306_SWITCHCAPVCC, Address, false );
//			FValue = InitialValue;
//			inherited::SystemInit();

//			drawPixel( 20, 20, WHITE );

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Render( false );

			display();
		}

#ifdef _MITOV_SSD1306_REFRESH_PIN_
		void DoRefreshReceived( void * )
		{
			display();
		}
#endif

	public:
		DisplaySSD1306( int16_t AWidth, int16_t AHeight, const unsigned char * AFont ) :
			GraphicsImpl( AFont ),
			UseChargePump( false ),
			FModified( false ),
			FWidth( AWidth ),
			FHeight( AHeight )
#ifdef _MITOV_DISPLAY_SSD1306_I2C
			, FWire( Wire )
#endif
		{
			buffer = new uint8_t[ AWidth * AHeight / 8 ];
			ClearScreen( BackgroundColor );
#ifdef _MITOV_SSD1306_REFRESH_PIN_
			RefreshInputPin.SetCallback( MAKE_CALLBACK( DisplaySSD1306::DoRefreshReceived ));
#endif
//				memset(buffer, 0, ( AWidth * AHeight / 8));
		}

#ifdef _MITOV_DISPLAY_SSD1306_I2C
		DisplaySSD1306( TwoWire &AWire, int16_t AWidth, int16_t AHeight, const unsigned char * AFont ) :
			GraphicsImpl( AFont ),
			UseChargePump( false ),
			FModified( false ),
			FWidth( AWidth ),
			FHeight( AHeight ), 
			FWire( AWire )
		{
			buffer = new uint8_t[ AWidth * AHeight / 8 ];
			ClearScreen( BackgroundColor );
#ifdef _MITOV_SSD1306_REFRESH_PIN_
			RefreshInputPin.SetCallback( MAKE_CALLBACK( DisplaySSD1306::DoRefreshReceived ));
#endif
//				memset(buffer, 0, ( AWidth * AHeight / 8));
		}
#endif

	};
//---------------------------------------------------------------------------
#ifdef _MITOV_DISPLAY_SSD1306_I2C
	template<> void DisplaySSD1306<odtSH1106>::SendBuffer()
	{
		byte height=64;
		byte width=132; 
		height >>= 3;
		width >>= 3;
		// I2C
		byte m_row = 0;
		byte m_col = 2;
		int p = 0;
		for ( byte i = 0; i < height; i++)
		{
			// send a bunch of data in one xmission
			SendCommand(0xB0 + i + m_row);//set page address
			SendCommand(m_col & 0xf);//set lower column address
			SendCommand(0x10 | (m_col >> 4));//set higher column address

			for( byte j = 0; j < 8; j++)
			{
				FWire.beginTransmission( Address );
				FWire.write( 0x40 );
				FWire.write( buffer + p, width );
				p += width;
				FWire.endTransmission();
			}
		}
	}
#endif // _MITOV_DISPLAY_SSD1306_I2C
//---------------------------------------------------------------------------
	template<> void DisplaySSD1306<odtSH1106>::display()
	{
		SendCommand(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
		SendCommand(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
		SendCommand(SSD1306_SETSTARTLINE | 0x0); // line #0

		// save I2C bitrate
	#ifdef TWBR
		uint8_t twbrbackup = TWBR;
		TWBR = 12; // upgrade to 400KHz!
	#endif

		SendBuffer();

	#ifdef TWBR
		TWBR = twbrbackup;
	#endif
		FModified = false;
	}
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

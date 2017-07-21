////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_NOKIA_h
#define _MITOV_DISPLAY_NOKIA_h

#include <Mitov.h>
#include <Mitov_Graphics_Monochrome.h>

#define MitovNokiaPartialUpdate

namespace Mitov
{
//---------------------------------------------------------------------------
	class DisplayNokia5110 : public Basic_SPI, public GraphicsImpl
	{
		typedef Basic_SPI inherited;

	public:
		OpenWire::SourcePin			DataCommandOutputPin;

#ifdef _MITOV_NOKIA_5110_RESET_PIN_
		OpenWire::SourcePin			ResetOutputPin;
#endif

#ifdef _MITOV_NOKIA_5110_REFRESH_PIN_
		OpenWire::ConnectSinkPin	RefreshInputPin;
#endif

	protected:
		static	const uint8_t WIDTH = 84;
		static	const uint8_t HEIGHT = 48;

		static	const uint8_t PCD8544_POWERDOWN = 0x04;
		static	const uint8_t PCD8544_ENTRYMODE = 0x02;
		static	const uint8_t PCD8544_EXTENDEDINSTRUCTION = 0x01;

		static	const uint8_t PCD8544_DISPLAYBLANK = 0x0;
		static	const uint8_t PCD8544_DISPLAYNORMAL = 0x4;
		static	const uint8_t PCD8544_DISPLAYALLON = 0x1;
		static	const uint8_t PCD8544_DISPLAYINVERTED = 0x5;

		// H = 0
		static	const uint8_t PCD8544_FUNCTIONSET = 0x20;
		static	const uint8_t PCD8544_DISPLAYCONTROL = 0x08;
		static	const uint8_t PCD8544_SETYADDR = 0x40;
		static	const uint8_t PCD8544_SETXADDR = 0x80;

		// H = 1
		static	const uint8_t PCD8544_SETTEMP = 0x04;
		static	const uint8_t PCD8544_SETBIAS = 0x10;
		static	const uint8_t PCD8544_SETVOP = 0x80;

	public:
		TGraphicsMonochromeTextSettings	Text;
		float	Contrast = 0.39;
		uint8_t BiasSystem : 3; // = 4;
		bool	Inverted : 1;
		bool	PoweredUp : 1;

	protected:
		bool	FModified : 1;

#ifdef MitovNokiaPartialUpdate
		uint8_t xUpdateMin : 7;
		uint8_t xUpdateMax : 7;
		uint8_t yUpdateMin : 6;
		uint8_t yUpdateMax : 6;
#endif

		uint8_t buffer[ HEIGHT * WIDTH / 8 ];

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

			// x is which column
			switch (color) 
			{
				case tmcWhite :		buffer[x+ (y/8) * WIDTH ] |=  (1 << (y&7)); break;
				case tmcBlack :		buffer[x+ (y/8) * WIDTH ] &= ~(1 << (y&7)); break; 
				case tmcInvert :	buffer[x+ (y/8) * WIDTH ] ^=  (1 << (y&7)); break; 
			}

			FModified = true;
			updateBoundingBox(x,y,x,y);
    	}

		virtual void ClearScreen( TUniColor color ) override
		{
			switch( color )
			{
			case tmcBlack :
				memset(buffer, 0, sizeof( buffer ));
				break;

			case tmcWhite :
				memset(buffer, 255, sizeof( buffer ));
				break;

			case tmcInvert :
				for( int i = 0; i < sizeof( buffer ); ++i )
					buffer[ i ] = ~buffer[ i ];

				break;

			}

			updateBoundingBox(0,0,WIDTH - 1,HEIGHT - 1);
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

			return (( buffer[ x + ( y / 8 ) * WIDTH ] & ( 1 << ( y & 7 ))) != 0 );
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
					x = WIDTH - x - 1;
					break;

				case goDown:
					// 180 degree rotation, invert x and y - then shift y around for height.
					x = WIDTH - x - 1;
					y = HEIGHT - y - 1;
					x -= (w-1);
					break;

				case goLeft:
					// 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
					bSwap = true;
					swap(x, y);
					y = HEIGHT - y - 1;
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
			AYBytes *= WIDTH;

/*
			if( ! Y )
			{
				uint8_t *ABuffer = buffer;
				if( X > 0 )
					for( int ayi = 0; ayi < HEIGHT / 8; ++ ayi, ABuffer += WIDTH )
					{
						memmove( ABuffer + X, ABuffer, WIDTH - X );
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
					for( int ayi = 0; ayi < HEIGHT / 8; ++ ayi, ABuffer += WIDTH )
					{
						memmove( ABuffer, ABuffer - X, WIDTH + X );
						switch( color )
						{
							case tmcBlack:
								memset( ABuffer + WIDTH + X, 0, -X ); break;

							case tmcWhite:
								memset( ABuffer + WIDTH + X, 255, -X ); break;

							case tmcInvert:
								for( int axi = WIDTH + X; axi < WIDTH; ++ axi )
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
							memmove( buffer + AYBytes, buffer, ( HEIGHT / 8 ) * WIDTH - AYBytes );

					}

					else if( X > 0 )
					{
						uint8_t *ABuffer = buffer + WIDTH * (( ( HEIGHT - Y + 7 ) / 8 ) - 1 );
						for( int ayi = 0; ayi < ( HEIGHT - Y + 7 ) / 8; ++ ayi, ABuffer -= WIDTH )
						{
							memmove( ABuffer + AYBytes + X, ABuffer, WIDTH - X );
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
						uint8_t *ABuffer = buffer + WIDTH * (( ( HEIGHT - Y + 7 ) / 8 ) - 1 );
						for( int ayi = 0; ayi < ( HEIGHT - Y + 7 ) / 8; ++ ayi, ABuffer -= WIDTH )
						{
							memmove( ABuffer + AYBytes, ABuffer - X, WIDTH + X );
							switch( color )
							{
								case tmcBlack:
									memset( ABuffer + AYBytes + WIDTH + X, 0, -X ); break;

								case tmcWhite:
									memset( ABuffer + AYBytes + WIDTH + X, 255, -X ); break;

								case tmcInvert:
									for( int axi = WIDTH + X; axi < WIDTH; ++ axi )
										ABuffer[ axi + AYBytes ] = ~ABuffer[ axi + AYBytes ];
							}
						}
					}

					uint8_t * AToPtr = buffer + WIDTH * HEIGHT / 8 - 1;
					uint8_t *AFromPtr = AToPtr - WIDTH;
					for( int ayi = 0; ayi < ( HEIGHT - Y + 7 ) / 8 - 1; ++ ayi )
						for( int axi = 0; axi < WIDTH; ++ axi, --AFromPtr, --AToPtr )
							*AToPtr = *AToPtr << AOffset | ( *AFromPtr >> ( 8 - AOffset ));

					if( color != tmcNone )
					{
						uint8_t	AMask;
						if( color == tmcBlack )
							AMask = 0xFF << AOffset;

						else
							AMask = 0xFF >> ( 8 - AOffset );

						AToPtr = buffer + AYBytes;
						for( int axi = 0; axi < WIDTH; ++ axi, ++AToPtr )
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
							memmove( buffer, buffer - AYBytes, ( HEIGHT / 8 ) * WIDTH + AYBytes );

					}

					else if( X > 0 )
					{
						uint8_t *ABuffer = buffer;
						for( int ayi = 0; ayi < ( HEIGHT + Y + 7 ) / 8; ++ ayi, ABuffer += WIDTH )
						{
							memmove( ABuffer + X, ABuffer - AYBytes, WIDTH - X );
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
						for( int ayi = 0; ayi < ( HEIGHT + Y + 7 ) / 8; ++ ayi, ABuffer += WIDTH )
						{
							memmove( ABuffer, ABuffer - AYBytes - X, WIDTH + X );
							switch( color )
							{
								case tmcBlack:
									memset( ABuffer + WIDTH + X, 0, -X ); break;

								case tmcWhite:
									memset( ABuffer + WIDTH + X, 255, -X ); break;

								case tmcInvert:
									for( int axi = WIDTH + X; axi < WIDTH; ++ axi )
										ABuffer[ axi ] = ~ABuffer[ axi ];
							}
						}
					}

					if( AOffset )
					{
						uint8_t * AToPtr = buffer;
						uint8_t *AFromPtr = AToPtr + WIDTH;
						for( int ayi = 0; ayi < ( HEIGHT + Y + 7 ) / 8 - 1; ++ ayi )
							for( int axi = 0; axi < WIDTH; ++ axi, ++AFromPtr, ++AToPtr )
								*AToPtr = *AToPtr >> AOffset | ( *AFromPtr << ( 8 - AOffset ));

						if( color != tmcNone )
						{
							uint8_t	AMask;
							if( color == tmcBlack )
								AMask = 0xFF >> AOffset;

							else
								AMask = 0xFF << ( 8 - AOffset );

							AToPtr = buffer + WIDTH * HEIGHT / 8 + AYBytes - WIDTH;
							for( int axi = 0; axi < WIDTH; ++ axi, ++AToPtr )
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
								for( int axi = 0; axi < WIDTH; ++ axi )
									buffer[ axi ] = ~buffer[ axi ];
						}
					}

					else // Y < 0
					{
						switch( color )
						{
							case tmcBlack:
								memset( buffer + HEIGHT * WIDTH / 8 + AYBytes, 0, - AYBytes ); break;

							case tmcWhite:
								memset( buffer + HEIGHT * WIDTH / 8 + AYBytes, 255, - AYBytes ); break;

							case tmcInvert:
								for( int axi = 0; axi < WIDTH; ++ axi )
									buffer[ axi + HEIGHT * WIDTH / 8 + AYBytes ] = ~buffer[ axi + HEIGHT * WIDTH / 8 + AYBytes ];
						}
					}
				}
			}

			updateBoundingBox(0,0,WIDTH - 1,HEIGHT - 1);
		}

	protected:
/*
		void ScrollBufferUp( uint8_t ANumLines )
		{
			memmove( buffer, buffer + ANumLines * WIDTH / 8, ( HEIGHT - ANumLines ) * WIDTH / 8 );
			memset( buffer + ( HEIGHT - ANumLines ) * WIDTH / 8, 0, ANumLines * WIDTH / 8 );
		}
*/
		void drawFastHLineInternal(int16_t x, int16_t y, int16_t w, TUniColor color) 
		{
			if( color == tmcNone )
				return;

			// Do bounds/limit checks
			if(y < 0 || y >= HEIGHT) 
				return;

			// make sure we don't try to draw below 0
			if(x < 0) 
			{ 
				w += x;
				x = 0;
			}

			// make sure we don't go off the edge of the display
			if( (x + w) > WIDTH) 
				w = (WIDTH - x);

			// if our width is now negative, punt
			if(w <= 0) 
				return;

			// set up the pointer for  movement through the buffer
			register uint8_t *pBuf = buffer;
			// adjust the buffer pointer for the current row
			pBuf += ((y/8) * WIDTH);
			// and offset x columns in
			pBuf += x;

			register uint8_t mask = 1 << (y&7);

			switch (color) 
			{
				case tmcWhite:         while(w--) { *pBuf++ |= mask; }; break;
				case tmcBlack: mask = ~mask;   while(w--) { *pBuf++ &= mask; }; break;
				case tmcInvert:         while(w--) { *pBuf++ ^= mask; }; break;
			}

			updateBoundingBox( x, y, x + w - 1, y );
		}

		void drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, TUniColor color) 
		{
			if( color == tmcNone )
				return;

			// do nothing if we're off the left or right side of the screen
			if(x < 0 || x >= WIDTH) 
				return;

			// make sure we don't try to draw below 0
			if(__y < 0) 
			{ 
				// __y is negative, this will subtract enough from __h to account for __y being 0
				__h += __y;
				__y = 0;
			} 

			// make sure we don't go past the height of the display
			if( (__y + __h) > HEIGHT)
				__h = (HEIGHT - __y);
			
			// if our height is now negative, punt 
			if(__h <= 0)
				return;			

			// this display doesn't need ints for coordinates, use local byte registers for faster juggling
			register uint8_t y = __y;
			register uint8_t h = __h;


			// set up the pointer for fast movement through the buffer
			register uint8_t *pBuf = buffer;
			// adjust the buffer pointer for the current row
			pBuf += ((y/8) * WIDTH);
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

				pBuf += WIDTH;
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
						pBuf += WIDTH;

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
						pBuf += WIDTH;

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

			updateBoundingBox( x, y, x, y + h - 1 );
		}

		virtual void SendCommand(uint8_t ACommand )
		{
			DataCommandOutputPin.SendValue<bool>( false );
			ChipSelectOutputPin.SendValue<bool>( false );
//			Serial.println( ACommand, HEX );
			FSPI.transfer( ACommand );
			ChipSelectOutputPin.SendValue<bool>( true );
		}

		virtual void SendBuffer()
		{
//			Serial.println( xUpdateMin );
//			Serial.println( xUpdateMax );
			uint8_t col, maxcol, p;
  
			for( p = 0; p < 6; p++) 
			{
#ifdef MitovNokiaPartialUpdate
				// check if this page is part of update
				if ( yUpdateMin >= ((p+1)*8) )
					continue;   // nope, skip it!
			
				if (yUpdateMax < p*8)
					break;
			
#endif

				SendCommand(PCD8544_SETYADDR | p);


#ifdef MitovNokiaPartialUpdate
				col = xUpdateMin;
				maxcol = xUpdateMax;
#else
			// start at the beginning of the row
				col = 0;
				maxcol = WIDTH-1;
#endif
				SendCommand(PCD8544_SETXADDR | col);

				DataCommandOutputPin.SendValue<bool>( true );
				ChipSelectOutputPin.SendValue<bool>( false );

				for(; col <= maxcol; col++)
					FSPI.transfer( buffer[(WIDTH*p)+col]);
			
				ChipSelectOutputPin.SendValue<bool>( true );
			}

			SendCommand(PCD8544_SETYADDR );  // no idea why this is necessary but it is to finish the last byte?
#ifdef MitovNokiaPartialUpdate
			xUpdateMin = WIDTH - 1;
			xUpdateMax = 0;
			yUpdateMin = HEIGHT-1;
			yUpdateMax = 0;
#endif		
		}

		void updateBoundingBox(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax) 
		{
#ifdef MitovNokiaPartialUpdate
			if (xmin < xUpdateMin) 
				xUpdateMin = xmin;

			if (xmax > xUpdateMax) 
			{
				xUpdateMax = xmax;
//				if( xUpdateMax > WIDTH - 1 )
//					xUpdateMax = WIDTH - 1;

			}

			if (ymin < yUpdateMin) 
				yUpdateMin = ymin;

			if (ymax > yUpdateMax) 
				yUpdateMax = ymax;
#endif
		}

/*
	public:
		uint8_t	Address = 0x3C;

	protected:
		virtual void SendCommand( uint8_t ACommand ) //override
		{
//			Serial.println( ACommand, HEX );
			Wire.beginTransmission( Address );
			Wire.write( 0x00 ); // Co = 0, D/C = 0
			Wire.write( ACommand );
			Wire.endTransmission();
		}

		virtual void SendBuffer() //override
		{
			// I2C
			for( uint16_t i=0; i < ( WIDTH * HEIGHT /8); ) 
			{
				// send a bunch of data in one xmission
				Wire.beginTransmission( Address );
				Wire.write( 0x40 );
				Wire.write( buffer + i, 16 );
				i += 16;
				Wire.endTransmission();
			}
		}
*/

/*
		void SendCommands( uint8_t ACommands[], int ASize ) 
		{
			for( int i = 0; i < ASize; ++ i )
				SendCommand( ACommands[ i ] );
		}
*/
	public:
		void display()
		{
//			Serial.println( "display()" );
			SendBuffer();

			FModified = false;
		}

		void UpdateContrast()
		{
			// get into the EXTENDED mode!
			SendCommand(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );

			uint8_t AContrast = ( Contrast * 0x7F ) + 0.5;

			SendCommand( PCD8544_SETVOP | AContrast); // Experimentally determined

			// normal mode
			if( PoweredUp )
				SendCommand(PCD8544_FUNCTIONSET );

			else
				SendCommand(PCD8544_FUNCTIONSET | PCD8544_POWERDOWN );
		}

		void UpdateInverted()
		{
			if( Inverted )
				SendCommand(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYINVERTED);

			else
				// Set display to Normal
				SendCommand(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);
		}

		void UpdatePoweredUp()
		{
			if( PoweredUp )
			{
				SendCommand(PCD8544_FUNCTIONSET );
				display();
			}

			else
				SendCommand(PCD8544_FUNCTIONSET | PCD8544_POWERDOWN );
		}

		void UpdateBiasSystem()
		{
			// get into the EXTENDED mode!
			SendCommand(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );

			SendCommand(PCD8544_SETBIAS | BiasSystem );

			// normal mode
			if( PoweredUp )
				SendCommand(PCD8544_FUNCTIONSET );

			else
				SendCommand(PCD8544_FUNCTIONSET | PCD8544_POWERDOWN );
		}

	protected:
		virtual void SystemLoopEnd() override
		{
#ifdef _MITOV_NOKIA_5110_REFRESH_PIN_
			if( FModified )
				if( ! RefreshInputPin.IsConnected() )
#endif
					display();

		}

		virtual void SystemInit() override
		{
#ifdef _MITOV_NOKIA_5110_RESET_PIN_
			if( ResetOutputPin.IsConnected() )
			{
				ResetOutputPin.SendValue<bool>( false );
				delay(500);
				ResetOutputPin.SendValue<bool>( true );
			}
#endif

			// get into the EXTENDED mode!
			SendCommand(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );

			// LCD bias select (4 is optimal?)
			SendCommand(PCD8544_SETBIAS | BiasSystem );

			uint8_t AContrast = ( Contrast * 0x7F ) + 0.5;

//			Serial.println( AContrast );

			SendCommand( PCD8544_SETVOP | AContrast); // Experimentally determined

			// normal mode
			if( PoweredUp )
				SendCommand(PCD8544_FUNCTIONSET );

			else
				SendCommand(PCD8544_FUNCTIONSET | PCD8544_POWERDOWN );
//			SendCommand(PCD8544_FUNCTIONSET);

			UpdateInverted();


//			Serial.println( "Test1" );
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

#ifdef _MITOV_NOKIA_5110_REFRESH_PIN_
		void DoRefreshReceived( void * )
		{
			display();
		}
#endif

	public:
		DisplayNokia5110( BasicSPI &ASPI, const unsigned char * AFont ) :
			inherited( ASPI ),
			GraphicsImpl( AFont ),
			BiasSystem( 4 ),
			Inverted( false ),
			PoweredUp( true ),
			FModified( false )
#ifdef MitovNokiaPartialUpdate
			,
			xUpdateMin( 0 ),
			xUpdateMax( WIDTH - 1 ),
			yUpdateMin( 0 ),
			yUpdateMax( HEIGHT - 1 )
#endif
		{
			ClearScreen( BackgroundColor );
#ifdef _MITOV_NOKIA_5110_REFRESH_PIN_
			RefreshInputPin.SetCallback( MAKE_CALLBACK( DisplayNokia5110::DoRefreshReceived ));
#endif
		}

	};
//---------------------------------------------------------------------------
}

#endif

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GRAPHICS_MONOCHROME_h
#define _MITOV_GRAPHICS_MONOCHROME_h

#include <Mitov.h>
#include <Mitov_Graphics.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class GraphicsMonochromeElementFillScreen : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		TArduinoMonochromeColor	Color = tmcBlack;

	public:
		virtual void Render( bool AForce ) override
		{
			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			FOwner.GetGraphics()->ClearScreen( Color );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsMonochromeElementDrawRectangle : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	FillColor : 2;
		int32_t		X = 0;
		int32_t		Y = 0;
		uint32_t	Width = 40;
		uint32_t	Height = 20;

	public:
		virtual void Render( bool AForce ) override
		{
			if( FillColor == tmcNone )
				if( Color == tmcNone )
					return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			if( FillColor != tmcNone )
				AGraphics->fillRect( AParentX + X, AParentY + Y, Width, Height, FillColor );

			if( Color != tmcNone )
				AGraphics->drawRect( AParentX + X, AParentY + Y, Width, Height, Color );
		}

	public:
		GraphicsMonochromeElementDrawRectangle( GraphicsIntf &AOwner ) :
			inherited( AOwner ),
			Color( tmcWhite ),
			FillColor( tmcNone )
		{
		}

	};
//---------------------------------------------------------------------------
	class GraphicsMonochromeElementDrawRoundRectangle : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	FillColor : 2;
		int32_t		X = 0;
		int32_t		Y = 0;
		uint32_t	Width = 40;
		uint32_t	Height = 20;
		TArduinoGraphicsCornerSize	CornerSize;

	public:
		virtual void Render( bool AForce ) override
		{
			if( FillColor == tmcNone )
				if( Color == tmcNone )
					return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			if( FillColor != tmcNone )
				AGraphics->fillRoundRect( AParentX + X, AParentY + Y, Width, Height, CornerSize.X, CornerSize.Y, FillColor );

			if( Color != tmcNone )
				AGraphics->drawRoundRect( AParentX + X, AParentY + Y, Width, Height, CornerSize.X, CornerSize.Y, Color );
		}

	public:
		GraphicsMonochromeElementDrawRoundRectangle( GraphicsIntf &AOwner ) :
			inherited( AOwner ),
			Color( tmcWhite ),
			FillColor( tmcNone )
		{
		}

	};
//---------------------------------------------------------------------------
	class GraphicsMonochromeElementDrawEllipse : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	FillColor : 2;
		bool		Centered	: 1;
		int32_t		X = 0;
		int32_t		Y = 0;
		uint32_t	Width = 40;
		uint32_t	Height = 20;

	public:
		virtual void Render( bool AForce ) override
		{
			if( FillColor == tmcNone )
				if( Color == tmcNone )
					return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			TGraphicsPos	AX = X;
			TGraphicsPos	AY = Y;

			if( Centered )
			{
				AX -= Width / 2;
				AY -= Height / 2;
			}

			if( FillColor != tmcNone )
				AGraphics->fillEllipseWH( AParentX + AX, AParentY + AY, Width, Height, FillColor );

			if( Color != tmcNone )
				AGraphics->drawEllipseWH( AParentX + AX, AParentY + AY, Width, Height, Color );
		}

	public:
		GraphicsMonochromeElementDrawEllipse( GraphicsIntf &AOwner ) :
			inherited( AOwner ),
			Color( tmcWhite ),
			FillColor( tmcNone ),
			Centered( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class GraphicsMonochromeElementDrawPixel : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		TArduinoMonochromeColor	Color = tmcWhite;
		int32_t		X = 0;
		int32_t		Y = 0;

	public:
		virtual void Render( bool AForce ) override
		{
			if( Color == tmcNone )
				return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			FOwner.GetGraphics()->drawPixel( AParentX + X, AParentY + Y, Color );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsMonochromeElementDrawLine : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		TArduinoMonochromeColor	Color = tmcWhite;
		int32_t		X = 0;
		int32_t		Y = 0;
		int32_t		Width = 40;
		int32_t		Height = 20;

	public:
		virtual void Render( bool AForce ) override
		{
			if( Color == tmcNone )
				return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			FOwner.GetGraphics()->drawLine( AParentX + X, AParentY + Y, AParentX + X + Width, AParentY + Y + Height, Color );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
#ifdef _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_

	class GraphicsMonochromeElementDrawBitmap : public Mitov::GraphicsElementClocked
	{
		typedef Mitov::GraphicsElementClocked inherited;

    public:
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	FillColor : 2;
		int32_t	X = 0;
		int32_t	Y = 0;
		uint8_t	Width;
		uint8_t	Height;

		const unsigned char *_Bytes;

	public:
		virtual void Render( bool AForce ) override
		{
			if(( FillColor == tmcNone ) && ( Color == tmcNone ))
				return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			FOwner.GetGraphics()->drawBWBitmap( AParentX + X, AParentY + Y, _Bytes, Width, Height, Color, FillColor );
		}

	public:
		GraphicsMonochromeElementDrawBitmap( GraphicsIntf &AOwner ) :
			inherited( AOwner ),
			Color( tmcWhite ),
			FillColor( tmcNone )
		{
		}

	};

#endif // _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_
//---------------------------------------------------------------------------
	class GraphicsElementDrawTransparencyBitmap : public Mitov::GraphicsElementClocked
	{
		typedef Mitov::GraphicsElementClocked inherited;

    public:
		int32_t	X = 0;
		int32_t	Y = 0;
		uint8_t	Width;
		uint8_t	Height;

		const unsigned char *_Bytes;

	public:
		virtual void Render( bool AForce ) override
		{
			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			FOwner.GetGraphics()->drawTransparencyBitmap( AParentX + X, AParentY + Y, _Bytes, Width, Height );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsMonochromeElementDrawText : public GraphicsElementClocked, public GraphicsTextIntf
	{
		typedef GraphicsElementClocked inherited;

	public:
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	FillColor : 2;
		uint8_t	Size : 4;
		bool	Wrap : 1;
		int32_t	X = 0;
		int32_t	Y = 0;

		String	Text;

	public:
		virtual void Render( bool AForce ) override
		{
			if(( FillColor == tmcNone ) && ( Color == tmcNone ))
				return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			int16_t ACursorY; // = AGraphics->getCursorY();
			int16_t ACursorX; // = AGraphics->getCursorX();

			AGraphics->getCursor( ACursorX, ACursorY );
			TUniColor AOldColor;
			TUniColor AOldBackgroundColor;

			uint8_t	AOldSize = AGraphics->getTextSize();
			bool AOldWrap = AGraphics->getTextWrap();

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Enter( AGraphics );

			AGraphics->setTextSize( Size );
			AGraphics->setTextWrap( Wrap );			

			AGraphics->getTextColor( AOldColor, AOldBackgroundColor );

			AGraphics->setTextColor( Color, FillColor );

			AGraphics->setCursor( X, Y );
			AGraphics->print( Text );

			AGraphics->setTextColor( AOldColor, AOldBackgroundColor );

			AGraphics->setCursor( ACursorX, ACursorY );

			AGraphics->setTextWrap( AOldWrap );
			AGraphics->setTextSize( AOldSize );

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Leave( AGraphics );

		}

	public:
		GraphicsMonochromeElementDrawText( GraphicsIntf &AOwner ) :
			inherited( AOwner ),
			Color( tmcWhite ),
			FillColor( tmcNone ),
			Size( 1 ),
			Wrap( true )
		{
		}

	};
//---------------------------------------------------------------------------
	class GraphicsMonochromeElementTextField : public GraphicsElementClocked, public GraphicsTextIntf
	{
		typedef GraphicsElementClocked inherited;

	public:
		OpenWire::ValueSinkPin<String>	InputPin;

	public:
		int32_t	X = 0;
		int32_t	Y = 0;
		uint16_t Width = 6;
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	FillColor : 2;
		TArduinoTextHorizontalAlign	HorizontalAlign : 2;
		bool AutoSize : 1;
		bool Wrap : 1;

	protected:
		uint8_t	FOldSize = 0;

	public:
		virtual void Render( bool AForce ) override
		{
			if(( FillColor == tmcNone ) && ( Color == tmcNone ))
				return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			int16_t ACursorY; // = AGraphics->getCursorY();
			int16_t ACursorX; // = AGraphics->getCursorX();

			AGraphics->getCursor( ACursorX, ACursorY );

			TUniColor AOldColor;
			TUniColor AOldBackgroundColor;

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Enter( AGraphics );

			bool AOldWrap = AGraphics->getTextWrap();

			AGraphics->setTextWrap( Wrap );			

			AGraphics->getTextColor( AOldColor, AOldBackgroundColor );

			AGraphics->setTextColor( Color, FillColor );

			AGraphics->setCursor( X, Y );
			String AText = InputPin.Value;
			if( AutoSize )
			{
				uint8_t ANewSize = AText.length();
				while( AText.length() < FOldSize )
					AText += " ";				

				FOldSize = ANewSize;
			}

			else
			{
				while( AText.length() < Width )
				{
					switch( HorizontalAlign )
					{
						case thaLeft :
							AText += " ";
							break;

						case thaRight :
							AText = " " + AText;
							break;

						case thaCenter :
							AText = " " + AText + " ";
							break;
					}
				}

				AText = AText.substring( 0, Width );
			}

			AGraphics->print( AText );

			AGraphics->setTextColor( AOldColor, AOldBackgroundColor );
			AGraphics->setCursor( ACursorX, ACursorY );
			AGraphics->setTextWrap( AOldWrap );			

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Leave( AGraphics );

		}

	protected:
		void DoReceiveData( void *_Data )
		{
			if( ! inherited::ClockInputPin.IsConnected() )
				Render( true );
		}

	public:
		GraphicsMonochromeElementTextField( GraphicsIntf &AOwner ) :
			inherited( AOwner ),
			Color( tmcWhite ),
			FillColor( tmcBlack ),
			HorizontalAlign( thaLeft ),
			AutoSize( true ),
			Wrap( true )
		{
			InputPin.SetCallback( MAKE_CALLBACK( GraphicsMonochromeElementTextField::DoReceiveData ));
		}

		GraphicsMonochromeElementTextField( GraphicsIntf &AOwner, String AInitialValue ) :
			inherited( AOwner ),
			Color( tmcWhite ),
			FillColor( tmcBlack ),
			HorizontalAlign( thaLeft ),
			AutoSize( true ),
			Wrap( true )
		{
			InputPin.Value = AInitialValue;
			InputPin.SetCallback( MAKE_CALLBACK( GraphicsMonochromeElementTextField::DoReceiveData ));
		}

	};
//---------------------------------------------------------------------------
	class GraphicsMonochromeElementDrawLines : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		TArduinoMonochromeColor	Color = tmcWhite;
		int32_t	X = 0;
		int32_t	Y = 0;

	public:
		GraphicsPoint	*Points;
		uint32_t	_PointsCount = 0;

	public:
		virtual void Render( bool AForce ) override
		{
			if( _PointsCount == 0 )
				return;

			if( Color == tmcNone )
				return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			int32_t	AX1 = X;
			int32_t	AY1 = Y;

			for( uint32_t i = 0; i < _PointsCount; ++i )
			{
				int32_t	AX2 = Points[ i ].X + X;
				int32_t	AY2 = Points[ i ].Y + Y;

				AGraphics->drawLine( AParentX + AX1, AParentY + AY1, AParentX + AX2, AParentY + AY2, Color );

				AX1 = AX2;
				AY1 = AY2;
			}
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsMonochromeElementDrawPolygon : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	FillColor : 2;
		int32_t	X = 0;
		int32_t	Y = 0;

	public:
		GraphicsPoint	*Points;
		uint32_t	_PointsCount = 0;

	public:
		virtual void Render( bool AForce ) override
		{
			if( _PointsCount == 0 )
				return;

			if(( FillColor == tmcNone ) && ( Color == tmcNone ))
				return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			int32_t	AStartX = AParentX + X;
			int32_t	AStartY = AParentY + Y;

			if( FillColor != tmcNone )
			{
				int32_t	AMinX = AStartX;
				int32_t	AMinY = AStartY;
				int32_t	AMaxX = AStartX;
				int32_t	AMaxY = AStartY;

				for( uint32_t i = 0; i < _PointsCount; ++i )
				{
					int32_t	AX2 = Points[ i ].X + AStartX;
					int32_t	AY2 = Points[ i ].Y + AStartY;

					if( AX2 < AMinX )
						AMinX = AX2;

					if( AX2 > AMaxX )
						AMaxX = AX2;

					if( AY2 < AMinY )
						AMinY = AY2;

					if( AY2 > AMaxY )
						AMaxY = AY2;

				}

				if( AMinX < 0 )
				 AMinX = 0;

				if( AMinY < 0 )
				 AMinY = 0;

				int32_t	AWidth = AGraphics->width();
				int32_t	AHeight = AGraphics->height();

				if( AMaxX > AWidth )
				 AMaxX = AWidth;

				if( AMaxY > AHeight )
				 AMaxY = AHeight;

				if( AMinX > AMaxX )
					return;

				if( AMinY > AMaxY )
					return;

				int32_t *nodeX = new int32_t[ _PointsCount ];
				for( int pixelY = AMinY; pixelY < AMaxY; ++ pixelY )
				{
					//  Build a list of nodes.
					int nodes = 0;
					int j = _PointsCount - 1;
					for( uint32_t i = 0; i < _PointsCount; i++ )
					{
						if ( (( Points[ i ].Y + AStartY ) < pixelY && ( Points[ j ].Y + AStartY ) >= pixelY ) || (( Points[ j ].Y + AStartY ) < pixelY && ( Points[ i ].Y + AStartY ) >= pixelY ))
							nodeX[nodes ++] = ( Points[i].X + AStartX ) + float( pixelY - ( Points[i].Y + AStartY ) ) / ( Points[j].Y - Points[i].Y ) * float( (Points[j].X - Points[i].X ) ) + 0.5;

						j = i;
					}

					//  Sort the nodes, via a simple “Bubble” sort.
					int i = 0;
					while( i < nodes - 1 )
					{
						if( nodeX[ i ] > nodeX[ i + 1 ] )
						{
							int32_t swap = nodeX[ i ];
							nodeX[ i ] = nodeX[ i + 1 ];
							nodeX[ i + 1 ] = swap;
							if( i )
								i--;
						}

						else
							i++;
						
					}

					//  Fill the pixels between node pairs.
					for( int i = 0; i < nodes; i += 2 ) 
					{
						int32_t ALeft = nodeX[ i ];
						int32_t ARight = nodeX[ i + 1 ];

						if( ALeft >= AWidth )
							break;

						if( ARight > 0 ) 
						{
							if( ALeft < 0 ) 
								ALeft = 0;

							if( ARight > AWidth )
								ARight = AWidth;

							AGraphics->drawFastHLine( ALeft, pixelY, ARight - ALeft, FillColor );

	//					for (pixelX=nodeX[i]; pixelX<nodeX[i+1]; pixelX++) fillPixel(pixelX,pixelY); 
						}
					}

				}

			}

			if( Color != tmcNone )
			{
				int32_t	AX1 = X;
				int32_t	AY1 = Y;

				for( uint32_t i = 1; i < _PointsCount; ++i )
				{
					int32_t	AX2 = Points[ i ].X + X;
					int32_t	AY2 = Points[ i ].Y + Y;

					AGraphics->drawLine( AParentX + AX1, AParentY + AY1, AParentX + AX2, AParentY + AY2, Color );

					AX1 = AX2;
					AY1 = AY2;
				}

				AGraphics->drawLine( AParentX + AX1, AParentY + AY1, AParentX + X, AParentY + Y, Color );
			}
		}

	public:
		GraphicsMonochromeElementDrawPolygon( GraphicsIntf &AOwner ) :
			inherited( AOwner ),
			Color( tmcWhite ),
			FillColor( tmcNone )
		{
		}

	};
//---------------------------------------------------------------------------
	class GraphicsMonochromeElementCheckPixel : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t X = 0;
		uint32_t Y = 0;

	public:
		virtual void Render( bool AForce ) override
		{
			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			Graphics *AGraphics = FOwner.GetGraphics();

			TUniColor AValue = AGraphics->GetPixelColor( X, Y );
			OutputPin.SendValue<bool>( AValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsTextElementFont : public GraphicsTextElementBasic
	{
		typedef GraphicsTextElementBasic inherited;

	protected:
		const unsigned char *FFont;

	protected:
		const unsigned char *FOldFont;

	public:
		virtual void Enter( Graphics *AGraphics )
		{
			FOldFont = AGraphics->getFont();
			AGraphics->setFont( FFont );
		}

		virtual void Leave( Graphics *AGraphics ) 
		{
			AGraphics->setFont( FOldFont );
		}

	public:
		GraphicsTextElementFont( GraphicsTextIntf &AOwner, const unsigned char *AFont ) :
			inherited( AOwner ),
			FFont( AFont )
		{
		}

	};
//---------------------------------------------------------------------------
	class GraphicsMonochromeElementScroll : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		TArduinoMonochromeColor	Color = tmcBlack;
		int32_t	X = 0;
		int32_t	Y = 0;

	public:
		virtual void Render( bool AForce ) override
		{
			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			Graphics *AGraphics = FOwner.GetGraphics();

			AGraphics->Scroll( X, Y, Color );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class TGraphicsMonochromeTextSettings
	{
	public:
		uint8_t	Size : 4;
		bool	Wrap : 1;
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	BackgroundColor : 2;

	public:
		TGraphicsMonochromeTextSettings() :
			Size( 1 ),
			Wrap( true ),
			Color( tmcWhite ),
			BackgroundColor( tmcNone )
		{
		}
	};
//---------------------------------------------------------------------------
}

#endif

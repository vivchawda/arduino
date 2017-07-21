////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_VALUE_ARRAY_h
#define _MITOV_VALUE_ARRAY_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T> class ValueArray : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::TypedSourcePin<T>	OutputPin;
		OpenWire::SinkPin			IndexInputPin;

	public:
		uint32_t InitialIndex = 0;

	public:
		T *_Values;
		uint32_t _MaxIndex;

	public:
		operator T *() { return _Values; }

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			if( InitialIndex > _MaxIndex )
				InitialIndex = _MaxIndex;

			OutputPin.SetValue( _Values[ InitialIndex ], true );
		}

		virtual void DoClockReceive( void *_Data ) override
		{
			OutputPin.SetValue( _Values[ InitialIndex ], false );
		}

	protected:
		void DoReceiveIndex( void *_Data )
		{
			uint32_t AIndex = *(uint32_t *)_Data;
			if( AIndex > _MaxIndex )
				AIndex = _MaxIndex;

			if( InitialIndex == AIndex )
				return;

			InitialIndex = AIndex;

			if( ! ClockInputPin.IsConnected() )
				OutputPin.SetValue( _Values[ InitialIndex ], false );
		}

	public:
		ValueArray()
		{
			IndexInputPin.SetCallback( MAKE_CALLBACK( ValueArray::DoReceiveIndex ));
		}
	};
//---------------------------------------------------------------------------
	template<> class ValueArray<bool> : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::TypedSourcePin<bool>	OutputPin;
		OpenWire::SinkPin				IndexInputPin;

	public:
		uint32_t InitialIndex = 0;

	public:
		uint8_t *_Values;
		uint32_t _MaxIndex;

	public:
		class _ByteAccess
		{
		protected:
			ValueArray<bool>	*FOwner;
			int					FIndex;

		public:
			void operator = ( bool AValue ) 
			{ 
				if( AValue  )
					FOwner->_Values[ FIndex / 8 ] |= 1 << ( FIndex % 8 );

				else
					FOwner->_Values[ FIndex / 8 ] &= ~( 1 << ( FIndex % 8 ));

			}

		public:
			_ByteAccess( ValueArray<bool> *AOwner, int AIndex ) :
				FOwner( AOwner ),
				FIndex( AIndex )
			{
			}
		};

		_ByteAccess operator [](int AIndex) { return _ByteAccess( this, AIndex ); }

	protected:
		virtual void SystemStart() override
		{
			inherited::SystemStart();
			if( InitialIndex > _MaxIndex )
				InitialIndex = _MaxIndex;

			OutputPin.SetValue( _Values[ InitialIndex / 8 ] & ( 1 << ( InitialIndex % 8 )), true );
		}

		virtual void DoClockReceive( void *_Data ) override
		{
			OutputPin.SetValue( _Values[ InitialIndex / 8 ] & ( 1 << ( InitialIndex % 8 )), false );
		}

	protected:
		void DoReceiveIndex( void *_Data )
		{
			uint32_t AIndex = *(uint32_t *)_Data;
			if( AIndex > _MaxIndex )
				AIndex = _MaxIndex;

			if( InitialIndex == AIndex )
				return;

			InitialIndex = AIndex;

			if( ! ClockInputPin.IsConnected() )
				OutputPin.SetValue( _Values[ InitialIndex / 8 ] & ( 1 << ( InitialIndex % 8 )), false );
		}

	public:
		ValueArray()
		{
			IndexInputPin.SetCallback( MAKE_CALLBACK( ValueArray::DoReceiveIndex ));
		}
	};
//---------------------------------------------------------------------------
	template<typename CONST_T, typename T> class ValueConstArray : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::TypedSourcePin<T>	OutputPin;
		OpenWire::SinkPin			IndexInputPin;

	public:
		uint32_t InitialIndex = 0;

	public:
		const /*PROGMEM*/ CONST_T *_Values;
		uint32_t _MaxIndex;

	protected:
		virtual void SetValue( bool AChangeOnly ) = 0;

	protected:
		virtual void SystemStart() override
		{
			inherited::SystemStart();
			if( InitialIndex > _MaxIndex )
				InitialIndex = _MaxIndex;
			
			SetValue( true );
		}

		virtual void DoClockReceive( void *_Data ) override
		{
			SetValue( false );
		}

	protected:
		void DoReceiveIndex( void *_Data )
		{
			uint32_t AIndex = *(uint32_t *)_Data;
			if( AIndex > _MaxIndex )
				AIndex = _MaxIndex;

			if( InitialIndex == AIndex )
				return;

			InitialIndex = AIndex;

			if( ! ClockInputPin.IsConnected() )
				SetValue( false );
		}

	public:
		ValueConstArray()
		{
			IndexInputPin.SetCallback( MAKE_CALLBACK( ValueConstArray::DoReceiveIndex ));
		}
	};
//---------------------------------------------------------------------------
	class ValueConstFloatArray : public ValueConstArray<float, float>
	{
	protected:
		virtual void SetValue( bool AChangeOnly )
		{
			OutputPin.SetValue( pgm_read_float( _Values + InitialIndex ), AChangeOnly );
		}

	};
//---------------------------------------------------------------------------
	class ValueConstStringArray : public ValueConstArray<const char *, String>
	{
	protected:
		virtual void SetValue( bool AChangeOnly )
		{
			char *APtr = (char *)pgm_read_dword( _Values + InitialIndex );
			String AText = APtr;
//			memcpy_P( APtr, _Values + InitialIndex, sizeof( APtr ))
/*
			int Alength = strlen( APtr );
			Serial.println( Alength );
			char *AStr = new char[ Alength + 1 ];
			memcpy( AStr, APtr, Alength + 1 );
//			Serial.println( _Values[ InitialIndex ] );
			String AText = AStr;
			delete [] AStr;
*/
			OutputPin.SetValue( AText, AChangeOnly );
		}

	};
	//---------------------------------------------------------------------------
	class ValueConstBoolArray : public ValueConstArray<uint8_t, bool>
	{
	protected:
		virtual void SetValue( bool AChangeOnly )
		{
			OutputPin.SetValue( pgm_read_byte( _Values + ( InitialIndex / 8 )) & ( 1 << ( InitialIndex % 8 )), AChangeOnly );
		}

	};
//---------------------------------------------------------------------------
	class ValueConstIntegerArray : public ValueConstArray<int32_t, int32_t>
	{
	protected:
		virtual void SetValue( bool AChangeOnly )
		{
			uint32_t AValue = pgm_read_dword( _Values + InitialIndex );
			OutputPin.SetValue( *(int32_t *)&AValue, AChangeOnly );
		}

	};
//---------------------------------------------------------------------------
	class ValueConstUnsignedArray : public ValueConstArray<uint32_t, uint32_t>
	{
	protected:
		virtual void SetValue( bool AChangeOnly )
		{
			OutputPin.SetValue( pgm_read_dword( _Values + InitialIndex ), AChangeOnly );
		}

	};
//---------------------------------------------------------------------------
	class ValueConstColorArray : public ValueConstArray<uint32_t, Mitov::TColor>
	{
	protected:
		virtual void SetValue( bool AChangeOnly )
		{
			uint32_t AValue = pgm_read_dword( _Values + InitialIndex );
			OutputPin.SetValue( *(Mitov::TColor *)&AValue, AChangeOnly );
		}

	};
//---------------------------------------------------------------------------
	class ValueRGBWConstColorArray : public ValueConstArray<uint32_t, Mitov::TRGBWColor>
	{
	protected:
		virtual void SetValue( bool AChangeOnly )
		{
			uint32_t AValue = pgm_read_dword( _Values + InitialIndex );
			OutputPin.SetValue( *(Mitov::TRGBWColor *)&AValue, AChangeOnly );
		}

	};
//---------------------------------------------------------------------------
/*
	class ValueConstDateTimeArray : public ValueConstArray<Mitov::TDateTime, Mitov::TDateTime>
	{
	protected:
		virtual void SetValue( bool AChangeOnly )
		{
			uint32_t ADate = pgm_read_dword( _Values + InitialIndex );
			uint32_t ATime = pgm_read_dword( ((byte *)_Values ) + InitialIndex * sizeof(TDateTime) + sizeof(uint32_t) );
			OutputPin.SetValue( TDateTime( ADate, ATime ), AChangeOnly );
		}

	};
*/
//---------------------------------------------------------------------------
}

#endif

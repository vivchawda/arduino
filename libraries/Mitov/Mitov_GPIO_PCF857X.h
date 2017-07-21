////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GPIO_PCF857X_h
#define _MITOV_GPIO_PCF857X_h

#include <Mitov.h>
#include <Mitov_Basic_GPIO.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class GPIO_PCF8574 : public Mitov::BasicGPIO<OpenWire::Component>
	{
		typedef Mitov::BasicGPIO<OpenWire::Component> inherited;

	public:
		uint8_t	Address = 0x38;

	protected:
		uint8_t	FDirectionBits = 0xFF;
		uint8_t	FReadBits = 0;

		TwoWire &FWire;

	protected:
		void UpdateAll()
		{
			FWire.beginTransmission( Address );
			FWire.write( FDirectionBits );
			FWire.endTransmission();
		}

	public:
		virtual void PerformRead() override
		{
			if( ! FDirectionBits )
				return;

			FWire.requestFrom( Address, (uint8_t)1 );
			FReadBits = FWire.read();

			for( int i = 0; i < FChannels.size(); ++i )
				FChannels[ i ]->UpdateInput();

		}

	public:
		bool GetBitValue( uint8_t AIndex )
		{
			return( ( FReadBits & ( ((uint8_t)1 ) << AIndex )) != 0 );
		}

	public:
		void SetChannelValue( int AIndex, bool AValue )
		{
			if( AValue )
				FDirectionBits |= ( (uint8_t)1 ) << AIndex;

			else
				FDirectionBits &= ~( ((uint8_t)1 ) << AIndex );
		}

	protected:
		virtual void SystemStart()
		{
			inherited::SystemStart();

			UpdateAll();

			for( int i = 0; i < FChannels.size(); ++i )
				FChannels[ i ]->SendOutput();
		}

		virtual void SystemLoopUpdateHardware()
		{
			UpdateAll();
			inherited::SystemLoopUpdateHardware();
		}

	public:
		GPIO_PCF8574( TwoWire &AWire ) :
			FWire( AWire )
		{
		}

	};
//---------------------------------------------------------------------------
	class GPIO_PCF8575 : public Mitov::BasicGPIO<OpenWire::Component>
	{
		typedef Mitov::BasicGPIO<OpenWire::Component> inherited;

	public:
		uint8_t	Address = 0x38;

	protected:
		uint16_t	FDirectionBits = 0xFF;
		uint16_t	FReadBits = 0;

		TwoWire &FWire;

	protected:
		void UpdateAll()
		{
			FWire.beginTransmission( Address );
			FWire.write( FDirectionBits );
			FWire.endTransmission();
		}

	public:
		virtual void PerformRead() override
		{
			if( ! FDirectionBits )
				return;

			FWire.requestFrom( Address, (uint8_t)2 );
			FReadBits = FWire.read();

			for( int i = 0; i < 16; ++i )
				FChannels[ i ]->UpdateInput();

		}

	public:
		bool GetBitValue( uint8_t AIndex )
		{
			return( ( FReadBits & ( ((uint16_t)1 ) << AIndex )) != 0 );
		}

	public:
		void SetChannelValue( int AIndex, bool AValue )
		{
			if( AValue )
				FDirectionBits |= ( (uint16_t)1 ) << AIndex;

			else
				FDirectionBits &= ~( ((uint16_t)1 ) << AIndex );
		}

	protected:
		virtual void SystemStart()
		{
			inherited::SystemStart();

			UpdateAll();

			for( int i = 0; i < 16; ++i )
				FChannels[ i ]->SendOutput();
		}

		virtual void SystemLoopUpdateHardware()
		{
			UpdateAll();
			inherited::SystemLoopUpdateHardware();
		}

	public:
		GPIO_PCF8575( TwoWire &AWire ) :
			FWire( AWire )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class GPIO_PCF857X_Channel : public OwnedBasicGPIOChannel<T_OWNER>
	{
		typedef OwnedBasicGPIOChannel<T_OWNER> inherited;
		
	public:
		virtual void UpdateInput()
		{			
			inherited::UpdateOutput( inherited::FOwner.GetBitValue( inherited::FIndex ));
		}

	protected:
        virtual void PinDirectionsInit()
        {
			if( inherited::FIsOutput )
				inherited::FOwner.SetChannelValue( inherited::FIndex, inherited::FValue );

			else
				inherited::FOwner.SetChannelValue( inherited::FIndex, true );

        }

		virtual void DoDataReceive( void * _Data ) override
		{
			bool AValue = *(bool *)_Data;
			if( inherited::FValue == AValue )
				return;

			inherited::FValue = AValue;
			if( inherited::FIsOutput )
				inherited::FOwner.SetChannelValue( inherited::FIndex, AValue );
		}

	public:
		GPIO_PCF857X_Channel( T_OWNER &AOwner, int AIndex, bool AInitialValue, bool AIsOutput, bool AIsPullUp, bool AIsCombinedInOut ) :
			inherited( AOwner, AIndex, AInitialValue, AIsOutput, AIsPullUp, AIsCombinedInOut )
		{
			PinDirectionsInit();
		}

	};
//---------------------------------------------------------------------------
}

#endif

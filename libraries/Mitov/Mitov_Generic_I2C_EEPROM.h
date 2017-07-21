////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GENERIC_I2C_EEPROM_h
#define _MITOV_GENERIC_I2C_EEPROM_h

#include <Mitov.h>
#include <Mitov_Memory.h>
#include <Mitov_Basic_I2C.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<uint8_t T_ADDRESS> class ArduinoGenericEEPROM_I2C : public OpenWire::Component, public Mitov::MemoryIntf
	{
		typedef	OpenWire::Component	inherited;
		
	public:
		virtual bool ReadData( uint32_t AAddress, uint32_t ASize, byte *AData ) override
		{
			if( ASize == 0 )
				return true;

			I2C::ReadBytes_16bitAddress( T_ADDRESS, true, AAddress, ASize, AData );
			AData += ASize;
			AAddress += ASize;

			return true;
		}

		virtual bool WriteData( uint32_t AAddress, uint32_t ASize, byte *AData ) override
		{
	        for( int i = 0; i < ASize; ++i )
				EEPROWrite( AAddress++, *AData++ );

			return true;
		}

	protected:
		inline void EEPROWrite( uint32_t eeaddress, byte data ) 
		{
			Wire.beginTransmission( T_ADDRESS );
			Wire.write((uint16_t)(eeaddress >> 8));   // MSB
			Wire.write((uint16_t)(eeaddress & 0xFF)); // LSB
			Wire.write(data);
			Wire.endTransmission();
 
			delay(5);
		}

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();

			uint32_t AAddress = 0;
			byte AOffset = 0;
			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->PopulateAddress( AAddress, AOffset );

		}

		virtual void SystemLoopUpdateHardware() override
		{
			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->ProcessWrite( this );

//			inherited::SystemLoopEnd();
		}

	};
//---------------------------------------------------------------------------
}

#endif

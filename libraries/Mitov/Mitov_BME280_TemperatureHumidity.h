////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BME280_TEMPERATUREHUMIDITY_h
#define _MITOV_BME280_TEMPERATUREHUMIDITY_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif


namespace Mitov
{
//---------------------------------------------------------------------------
	class BME280_TemperatureHumidityBasic : public OpenWire::Component, public Mitov::ClockingSupport
	{
		typedef	OpenWire::Component	inherited;

	public:
		OpenWire::SourcePin				PressureOutputPin;
		OpenWire::SourcePin				TemperatureOutputPin;
		OpenWire::SourcePin				HumidityOutputPin;
		OpenWire::TypedSourcePin<bool>	ErrorOutputPin;

	public:
		bool	Enabled : 1;
		bool	InFahrenheit : 1;
		bool	Address	: 1; // In base class to save space
		bool	SPI3Wire : 1; // In base class to save space

	public:
		uint8_t	OversamplingPressure : 3;
		uint8_t	OversamplingTemperature : 3;
		uint8_t	OversamplingHumidity : 3;
		uint8_t	IIRFilterCoefficient : 3;
		uint8_t	InactiveDuration : 3;

	protected:
      static const byte BME280_REGISTER_DIG_T1             = 0x88;
      static const byte BME280_REGISTER_DIG_T2             = 0x8A;
      static const byte BME280_REGISTER_DIG_T3             = 0x8C;

      static const byte BME280_REGISTER_DIG_P1             = 0x8E;
      static const byte BME280_REGISTER_DIG_P2             = 0x90;
      static const byte BME280_REGISTER_DIG_P3             = 0x92;
      static const byte BME280_REGISTER_DIG_P4             = 0x94;
      static const byte BME280_REGISTER_DIG_P5             = 0x96;
      static const byte BME280_REGISTER_DIG_P6             = 0x98;
      static const byte BME280_REGISTER_DIG_P7             = 0x9A;
      static const byte BME280_REGISTER_DIG_P8             = 0x9C;
      static const byte BME280_REGISTER_DIG_P9             = 0x9E;

      static const byte BME280_REGISTER_DIG_H1             = 0xA1;
      static const byte BME280_REGISTER_DIG_H2             = 0xE1;
      static const byte BME280_REGISTER_DIG_H3             = 0xE3;
      static const byte BME280_REGISTER_DIG_H4             = 0xE4;
      static const byte BME280_REGISTER_DIG_H5             = 0xE5;
      static const byte BME280_REGISTER_DIG_H6             = 0xE7;

      static const byte BME280_REGISTER_CHIPID             = 0xD0;
      static const byte BME280_REGISTER_VERSION            = 0xD1;
      static const byte BME280_REGISTER_SOFTRESET          = 0xE0;

      static const byte BME280_REGISTER_CAL26              = 0xE1;  // R calibration stored in 0xE1-0xF0

      static const byte BME280_REGISTER_CONTROLHUMID       = 0xF2;
      static const byte BME280_REGISTER_CONTROL            = 0xF4;
      static const byte BME280_REGISTER_CONFIG             = 0xF5;
      static const byte BME280_REGISTER_PRESSUREDATA       = 0xF7;
      static const byte BME280_REGISTER_TEMPDATA           = 0xFA;
      static const byte BME280_REGISTER_HUMIDDATA          = 0xFD;

	protected:
		struct
		{
		  uint16_t dig_T1;
		  int16_t  dig_T2;
		  int16_t  dig_T3;

		  uint16_t dig_P1;
		  int16_t  dig_P2;
		  int16_t  dig_P3;
		  int16_t  dig_P4;
		  int16_t  dig_P5;
		  int16_t  dig_P6;
		  int16_t  dig_P7;
		  int16_t  dig_P8;
		  int16_t  dig_P9;

		  uint8_t  dig_H1;
		  int16_t  dig_H2;
		  uint8_t  dig_H3;
		  int16_t  dig_H4;
		  int16_t  dig_H5;
		  int8_t   dig_H6;
		} CalibrationData;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			if( ! Enabled )
				return;

			int32_t adc_T = read24(BME280_REGISTER_TEMPDATA);
//			Serial.println( adc_T );
			adc_T >>= 4;

			int32_t var1  = ((((adc_T>>3) - ((int32_t)CalibrationData.dig_T1 <<1))) *
				((int32_t)CalibrationData.dig_T2)) >> 11;

			int32_t var2  = (((((adc_T>>4) - ((int32_t)CalibrationData.dig_T1)) *
					((adc_T>>4) - ((int32_t)CalibrationData.dig_T1))) >> 12) *
				((int32_t)CalibrationData.dig_T3)) >> 14;

			int32_t t_fine = var1 + var2;

			if( TemperatureOutputPin.IsConnected() )
			{
				float T = (t_fine * 5 + 128) >> 8;
				T /= 100;

				if( InFahrenheit )
					T = T * ( 9.0/5.0 ) + 32.0;

				TemperatureOutputPin.Notify( &T );
			}

			if( PressureOutputPin.IsConnected() )
			{
				int32_t adc_P = read24(BME280_REGISTER_PRESSUREDATA);
				adc_P >>= 4;

				int64_t var1 = ((int64_t)t_fine) - 128000;
				int64_t var2 = var1 * var1 * (int64_t)CalibrationData.dig_P6;
				var2 = var2 + ((var1*(int64_t)CalibrationData.dig_P5)<<17);
				var2 = var2 + (((int64_t)CalibrationData.dig_P4)<<35);
				var1 = ((var1 * var1 * (int64_t)CalibrationData.dig_P3)>>8) +
				((var1 * (int64_t)CalibrationData.dig_P2)<<12);
				var1 = (((((int64_t)1)<<47)+var1))*((int64_t)CalibrationData.dig_P1)>>33;

				float AResult;
				if (var1 == 0)
					AResult = 0;  // avoid exception caused by division by zero

				else
				{
					int64_t p = 1048576 - adc_P;
					p = (((p<<31) - var2)*3125) / var1;
					int64_t var1 = (((int64_t)CalibrationData.dig_P9) * (p>>13) * (p>>13)) >> 25;
					int64_t var2 = (((int64_t)CalibrationData.dig_P8) * p) >> 19;

					p = ((p + var1 + var2) >> 8) + (((int64_t)CalibrationData.dig_P7)<<4);
					AResult = (float)p/256;			
				}

				PressureOutputPin.Notify( &AResult );
			}

			if( HumidityOutputPin.IsConnected() )
			{
				int32_t adc_H = read16(BME280_REGISTER_HUMIDDATA);

				int32_t v_x1_u32r;

				v_x1_u32r = (t_fine - ((int32_t)76800));

				v_x1_u32r = (((((adc_H << 14) - (((int32_t)CalibrationData.dig_H4) << 20) -
						(((int32_t)CalibrationData.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
						(((((((v_x1_u32r * ((int32_t)CalibrationData.dig_H6)) >> 10) *
						(((v_x1_u32r * ((int32_t)CalibrationData.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
						((int32_t)2097152)) * ((int32_t)CalibrationData.dig_H2) + 8192) >> 14));

				v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
								((int32_t)CalibrationData.dig_H1)) >> 4));

				v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
				v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
				float h = (v_x1_u32r>>12);
				h /= 1024.0;

				HumidityOutputPin.Notify( &h );
			}
		}

	protected:
		virtual uint8_t read8(uint8_t reg) = 0;
		virtual uint16_t read16(uint8_t reg) = 0;
		virtual uint16_t read16_LE(uint8_t reg) = 0;

		inline int16_t readS16_LE(byte reg) 
		{
			return (int16_t)read16_LE(reg);
		}

		virtual uint32_t read24(uint8_t reg) = 0;
		virtual void write8(uint8_t reg, uint8_t value) = 0;

	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			CalibrationData.dig_T1 = read16_LE(BME280_REGISTER_DIG_T1);
			CalibrationData.dig_T2 = readS16_LE(BME280_REGISTER_DIG_T2);
			CalibrationData.dig_T3 = readS16_LE(BME280_REGISTER_DIG_T3);

			CalibrationData.dig_P1 = read16_LE(BME280_REGISTER_DIG_P1);
			CalibrationData.dig_P2 = readS16_LE(BME280_REGISTER_DIG_P2);
			CalibrationData.dig_P3 = readS16_LE(BME280_REGISTER_DIG_P3);
			CalibrationData.dig_P4 = readS16_LE(BME280_REGISTER_DIG_P4);
			CalibrationData.dig_P5 = readS16_LE(BME280_REGISTER_DIG_P5);
			CalibrationData.dig_P6 = readS16_LE(BME280_REGISTER_DIG_P6);
			CalibrationData.dig_P7 = readS16_LE(BME280_REGISTER_DIG_P7);
			CalibrationData.dig_P8 = readS16_LE(BME280_REGISTER_DIG_P8);
			CalibrationData.dig_P9 = readS16_LE(BME280_REGISTER_DIG_P9);

			CalibrationData.dig_H1 = read8(BME280_REGISTER_DIG_H1);
			CalibrationData.dig_H2 = readS16_LE(BME280_REGISTER_DIG_H2);
			CalibrationData.dig_H3 = read8(BME280_REGISTER_DIG_H3);
			CalibrationData.dig_H4 = (read8(BME280_REGISTER_DIG_H4) << 4) | (read8(BME280_REGISTER_DIG_H4+1) & 0xF);
			CalibrationData.dig_H5 = (read8(BME280_REGISTER_DIG_H5+1) << 4) | (read8(BME280_REGISTER_DIG_H5) >> 4);
			CalibrationData.dig_H6 = (int8_t)read8(BME280_REGISTER_DIG_H6);		

			write8(BME280_REGISTER_CONTROLHUMID, OversamplingHumidity );

			uint8_t AValue = ( ( InactiveDuration & 0b111 ) << 5 ) | (( IIRFilterCoefficient & 0b111 ) << 2 ) | ( SPI3Wire & 1 );
			write8(BME280_REGISTER_CONFIG, AValue );

			AValue = ( ( OversamplingTemperature & 0b111 ) << 5 ) | (( OversamplingPressure & 0b111 ) << 2 ) | 0b11;
//			Serial.println( AValue );
//			AValue = 0xB7; // 16x ovesampling, normal mode
			write8(BME280_REGISTER_CONTROL, AValue );
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{

			if( Enabled )
				if( ! ClockInputPin.IsConnected() )
					DoClockReceive( nullptr );

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		BME280_TemperatureHumidityBasic() :
			Enabled( true ),
			InFahrenheit( false ),
			Address( true ),
			SPI3Wire( false ),
			OversamplingPressure( 1 ),
			OversamplingTemperature( 1 ),
			OversamplingHumidity( 1 ),
			IIRFilterCoefficient( 0 ),
			InactiveDuration( 0 )
		{
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BEDAC_h
#define _MITOV_BEDAC_h

#include <Mitov.h>

#ifdef VISUINO_ARDUINO_LEONARDO

#define BEDAC_DIGITAL_14 A0
#define BEDAC_DIGITAL_15 A1
#define BEDAC_DIGITAL_16 A2
#define BEDAC_DIGITAL_17 A3
#define BEDAC_DIGITAL_18 A4
#define BEDAC_DIGITAL_19 A5

#else // VISUINO_ARDUINO_LEONARDO

#define BEDAC_DIGITAL_14 14
#define BEDAC_DIGITAL_15 15
#define BEDAC_DIGITAL_16 16
#define BEDAC_DIGITAL_17 17
#define BEDAC_DIGITAL_18 18
#define BEDAC_DIGITAL_19 19

#endif // VISUINO_ARDUINO_LEONARDO


#endif

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2015 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SINE_GENERATOR_h
#define _MITOV_SINE_GENERATOR_h

#include <Mitov.h>

namespace Mitov
{
	class SineGenerator : public Mitov::CommonSource // Mitov::CommonGenerator
	{
		typedef Mitov::CommonSource inherited;

	protected:
		float	FValue;
		float	FPhase;
		unsigned long FLastTime;
//		float   FTime;
//		float   FLastTime;
		
	public:
        bool    Enabled;
		float	Frequency;
		float	Amplitude;
		float	Offset;
		float	Phase;

	protected:
/*
		virtual void Clock()
		{
			FValue = sin( FTime * 2 * PI ) * Amplitude + Offset;

			FTime += Frequency / Rate;

			while( FTime > 1 )
				FTime -= 1;

//			FValue = !FValue;
			OpenWire::ValueData<float> AData( FValue );
			OutputPin.Notify( &AData );
		}
*/
		virtual void SystemStart()
		{
			FPhase = Phase;

			inherited::SystemStart();
		}

		virtual void SystemClock( unsigned long currentMicros )
		{
            if( Enabled )
            {
/*
			    float APeriod = 1000000 / Frequency;

			    float ATime = ((float)( currentMicros % ((unsigned long)APeriod) )) / APeriod;

			    FValue = sin( ( ATime * 2 * PI ) + Phase ) * Amplitude + Offset;
*/
			    float APeriod = 1000000 / Frequency;

				float ATime = ( currentMicros - FLastTime );
			    ATime /= APeriod;
				float PhaseShift = ATime * 2 * PI;
				FPhase += PhaseShift;
				FPhase = fmod( FPhase, 2 * PI );

			    FValue = sin( FPhase ) * Amplitude + Offset;
/*
				float PhaseShift = ( currentMicros - FLastTime ) * 2 * PI;
				FPhase += PhaseShift;
				FPhase = fmod( FPhase, 2 * PI );
			    FValue = sin( FPhase ) * Amplitude + Offset;
*/
			}

			FLastTime = currentMicros;
			OutputPin.Notify( &FValue );
		}

	public:
		SineGenerator() :
			FValue( 0 ),
			FPhase( 0 ),
//			FTime( 0 ),
			FLastTime( 0 ),
            Enabled( true ),
			Frequency( 1 ),
			Amplitude( 1 ),
			Offset( 0 )
		{
//			Rate = 1000;
		}

	};
}

#endif

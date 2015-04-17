////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2015 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_LED_BAR_DISPLAY_h
#define _MITOV_LED_BAR_DISPLAY_h

#include <Mitov.h>
#include <Mitov_BasicDisplay.h>

namespace Mitov
{
	class LEDBarDisplay : public Mitov::BasicDisplay<int>
	{
		typedef	Mitov::BasicDisplay<int>	inherited;

	public:
		Mitov::SimpleList<OpenWire::SourcePin>	BarsOutputPins;

	protected:
		virtual void UpdateDisplay()
		{
			int AValue = (Enabled) ? FValue : 0;
			for( int i = 0; i < BarsOutputPins.size(); ++i )
			{
				bool ABitValue = ( AValue > 0 );
				BarsOutputPins[ i ].Notify( &ABitValue );
				AValue >>= 1;
			}
		}

	};
//---------------------------------------------------------------------------
}

#endif

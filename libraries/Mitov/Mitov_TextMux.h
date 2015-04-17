////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2015 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TEXT_MUX_h
#define _MITOV_TEXT_MUX_h

#include <Mitov_BasicMux.h>

namespace Mitov
{
	class TextMux : public BasicCommonMux<char *>
	{
		typedef	BasicCommonMux<char *>	inherited;

	protected:
		virtual char * CalculateOutput()
		{
			return (char *)inherited::InputPins[ FChannel ].Value.c_str();
		}
	};
//---------------------------------------------------------------------------
}

#endif

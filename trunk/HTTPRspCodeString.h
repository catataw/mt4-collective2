//+------------------------------------------------------------------+
//| Copyright 2010 OpenThinking Systems, LLC
//|
//|  Licensed under the Apache License, Version 2.0 (the "License");
//|  you may not use this file except in compliance with the License.
//|  You may obtain a copy of the License at
//|
//|               http://www.apache.org/licenses/LICENSE-2.0
//|
//|  Unless required by applicable law or agreed to in writing,
//|  software distributed under the License is distributed on an
//|  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//|  KIND, either express or implied. See the License for the
//|  specific language governing permissions and limitations under
//|  the License.
//+------------------------------------------------------------------+
#pragma once

#include "AbstractCodeInfo.h"
#include <string>
#include <map>

class HTTPRspCodeString: public AbstractCodeInfo
{
protected:
	std::map<unsigned int, std::string> m_StatusCodeStrings;
public:
	HTTPRspCodeString(void);
	virtual std::string getInfo( unsigned int pResponseCode );
};
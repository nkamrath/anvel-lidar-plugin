//////////////////////////////////////////////////////////////////////
// Copyright 2017 Quantum Signal, LLC
//
// Permission is hereby granted, free of charge, to any person 
// obtaining a copy of this software and associated documentation 
// files (the "Software"), to deal in the Software without 
// restriction, including without limitation the rights to use, 
// copy, modify, merge, publish, distribute, sublicense, and/or 
// sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be 
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS 
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.
//////////////////////////////////////////////////////////////////////

#include "GLICSensorPlugin.h"
#include "GLICSensor.h"

//... Other includes here...

using namespace Anvel;
using namespace Anvel::Plugins;

//////////////////////////////////////////////////////////////////////////
//         Plugin DLL Code    
//////////////////////////////////////////////////////////////////////////

static const char* kPluginName = "GLICSensorPlugin";

//////////////////////////////////////////////////////////////////////////

GLICSensorPlugin::GLICSensorPlugin()
{
	//... Internal Initialization here
}

//////////////////////////////////////////////////////////////////////////

GLICSensorPlugin::~GLICSensorPlugin()
{
	//...Internal Shutdown here
}

//////////////////////////////////////////////////////////////////////////

void GLICSensorPlugin::Initialize()
{
	m_pSensorFactory = new GLICSensorFactory();
}

//////////////////////////////////////////////////////////////////////////

void GLICSensorPlugin::Shutdown()
{
	//Do plugin shutdown work here
	delete m_pSensorFactory;
}

//////////////////////////////////////////////////////////////////////////

String GLICSensorPlugin::GetName() const
{
	return String(kPluginName);
}

//////////////////////////////////////////////////////////////////////////

//The implementation of our commonly used plugin functions
ANVEL_PLUGIN_COMMON_DEF( GLICSensorPlugin );

//////////////////////////////////////////////////////////////////////////
//
// Plugin Export Code
//
//////////////////////////////////////////////////////////////////////////

GLICSensorPlugin* pPlugin;

extern "C" SENSOR_API void RegisterPlugin(Anvel::Plugins::PluginHost* host) 
{
	pPlugin = new GLICSensorPlugin();
	host->RegisterPlugin( pPlugin );
}

extern "C" SENSOR_API void UnregisterPlugin(Anvel::Plugins::PluginHost* host) 
{
	host->UnregisterPlugin(kPluginName, pPlugin);
	delete pPlugin;
}

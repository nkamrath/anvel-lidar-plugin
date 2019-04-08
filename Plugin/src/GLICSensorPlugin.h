
#ifndef _GLIC_SENSOR_PLUGIN_H_
#define _GLIC_SENSOR_PLUGIN_H_

#include "Core/Plugin.h"
#include "Simulation/Sensor.h"
#include "Core/PluginVersioning.h"

#if defined(_LINUX)
	#define SENSOR_API 
#else
	#ifdef ANVEL_SAMPLE_PLUGIN_EXPORT
		#define SENSOR_API __declspec(dllexport)
	#else
		#define SENSOR_API __declspec(dllimport)
	#endif
#endif

namespace Anvel
{
	namespace Plugins
	{
		///Sample Plugin
		class GLICSensorPlugin: public Plugins::Plugin
		{
			public:
				GLICSensorPlugin();
				~GLICSensorPlugin();

			public: //[Plugin interface]
				//Called to initialize anything that this plugin needs after it has been loaded
				void Initialize() override;

				//Called by plugin host when this plugin should be shutdown
				void Shutdown() override;

				//Get the name of this plugin
				String GetName() const override;

				//Our common declarations that can be used with most plugins
				ANVEL_PLUGIN_COMMON_DECL;
			private:
				//... Store internal plugin data here
				ISensorFactory* m_pSensorFactory = nullptr;
		};
	}
}

#endif

#ifndef GLIC_SENSOR_H_
#define GLIC_SENSOR_H_

#include "GLICSensorPlugin.h"
#include "Simulation/Sensor.h"
#include "Simulation/GPSSensor.h"
#include "Simulation/DefaultSensors.h"
#include "Simulation/IMUSensor.h"
#include "Simulation/LidarSensor.h"
#include "UDPSocket.h"

namespace Anvel
{
	extern const SENSOR_API SensorType kSensorTypeGLICSensor;

	///Sample specific configuration parameters
	class GLICSensorStaticAssetParams : public SensorStaticAssetParams
	{
		public:
		uint32 m_intData;
		uint32 m_gpsSensorID;
	};

	class GLICSensorFactory;
	class GPSSensor;


	class SENSOR_API GLICSensor : public Sensor
	{
		friend class GLICSensorFactory;

		public:
			virtual ~GLICSensor();

			virtual void Update(TimeValue dt);

		protected:
			GLICSensor(ID specificId, SensorStaticAssetParams& params, DynamicAssetParams& dynamicParams);

		protected:
			uint32 m_sampleIntData;

			std::shared_ptr<GPSSensor> m_gpsSensor;
			std::shared_ptr<IMUSensor> m_imuSensor;
			std::shared_ptr<LidarSensor> m_lidarSensor;
			uint32 m_lastLidarIndex = 0;

			double m_timeSinceLastGPS = 0;
			double m_timeSinceLastIMU = 0;


			bool m_firstRun = true;
			
			UDPSocket m_udpSocket;
	};

	class SENSOR_API GLICSensorFactory : public ISensorFactory, public IPropertyProvider
	{
		public:
			GLICSensorFactory();
			~GLICSensorFactory();

		public:
			virtual Sensor* CreateSensor(SensorStaticAssetParams& params, DynamicAssetParams& dynamicParams);
			virtual StaticAssetParamsPtr ParseXmlParams(const tinyxml2::XMLElement* pXmlElement);

			virtual ObjectPropertySet GetProperties(ID objID);
			virtual IdVector GetIdsOfType(const ID dataType) const;

		private:
			Sensor* CreateGLICSensor(SensorStaticAssetParams& params, DynamicAssetParams& dynamicParams);
			void RegisterProperties();
			PropertyGroupInstance GetSensorProperties(GLICSensor& sensor);
			SensorID GetNextSensorID();

			ID m_sensorIDCount;
	};
}

#endif
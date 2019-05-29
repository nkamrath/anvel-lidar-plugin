

#include "GLICSensor.h"

#include "ode/ode.h"
#include "Core/PropertyManager.h"
#include "Simulation/World/WorldManager.h"
#include "Simulation/World/Terrain.h"
#include "Simulation/World/Environment.h"
#include "Simulation/Sensor.h"
#include "Simulation/Sensors/SensorManager.h"
#include "Simulation/Physics/ODEPhysics.h"
#include "Simulation/DefaultSensors.h"

namespace Anvel
{
	const DataTypeGUID kSampleSensorGUID = { 0xfd24ef34, 0xc4d2, 0x4364,{ 0x83, 0x93, 0x53, 0xd7, 0x6f, 0x2d, 0xa7, 0xb5 } };

	namespace Types
	{
		SENSOR_API ID GLICSensor = 0;
	}

	const SENSOR_API SensorType kSensorTypeGLICSensor = "GLIC Sensor";

	GLICSensor::GLICSensor(ID specificId, SensorStaticAssetParams& params, DynamicAssetParams& dynamicParams)
		: Sensor(specificId, params, dynamicParams)
	{
		//Do initialization here

		//cast to our specific type of asset params, and grab data 
		const GLICSensorStaticAssetParams& GLICParams = static_cast<const GLICSensorStaticAssetParams&>(params);
		m_sampleIntData = GLICParams.m_intData;

		std::vector<Sensor*> allSensors = Anvel::SensorManager::GetSingleton().GetAllSensors();

		for(int i = 0; i < allSensors.size(); i++)
		{
			if(allSensors[i])
			{
				if(allSensors[i]->GetSensorType() == kSensorTypeGPS)
				{
					m_gpsSensor = std::shared_ptr<GPSSensor>(static_cast<GPSSensor*>(allSensors[i]));
				}
				else if(allSensors[i]->GetSensorType() == kSensorTypeIMU)
				{
					m_imuSensor = std::shared_ptr<IMUSensor>(static_cast<IMUSensor*>(allSensors[i]));
				}
				else if(allSensors[i]->GetSensorType() == kSensorTypeLidar)
				{
					m_lidarSensor = std::shared_ptr<LidarSensor>(static_cast<LidarSensor*>(allSensors[i]));
				}
			}
		}

		m_udpSocket = UDPSocket("239.255.76.67", 7667);
	}

	//////////////////////////////////////////////////////////////////////////

	GLICSensor::~GLICSensor()
	{
		//Any sensor resources should be shut down here
	}

	void GLICSensor::Update(TimeValue dt)
	{
		//check to see if it is time to write an update to this sensor
		m_sampleTimeLeft -= dt;
		m_timeSinceLastGPS += dt;
		m_timeSinceLastIMU += dt;

		//if we still have time left, skip writing an update
		if (m_sampleTimeLeft > 0.0)
		{
			return;
		}
		else 
		{
			m_sampleTimeLeft += m_sampleStep;
		}
		
		char buff[256];

		//get gps data
		
		GpsData gpsData;
		Vector3 accelData;
		Vector3 rotData;
		Vector3 lidarPosition;
		std::vector<LidarBufferEntry> lidarData;
		uint32 latestPointCloudIndex = 0;

		//std::shared_ptr<Sensor> tmp = gpsSensor.lock();
		// if(m_gpsSensor != nullptr and m_timeSinceLastGPS > 1.0)
		// {
		// 	m_timeSinceLastGPS -= 1.0;
		// 	gpsData = m_gpsSensor->GetData();
		// 	sprintf(buff, "GPS Data: %f, %f", gpsData.gpsPosition.x, gpsData.gpsPosition.y);
		// 	double temp_data[2] = { gpsData.gpsPosition.x, gpsData.gpsPosition.y };
		// 	m_udpSocket.Send(SensorPacketType::GPS, 0, temp_data, sizeof(temp_data));
		// 	//LogMessage(buff, kLogMsgPlugin);
		// }

		// if(m_imuSensor != nullptr and m_timeSinceLastIMU > 0.1)
		// {
		// 	m_timeSinceLastIMU -= 0.1;
		// 	accelData = m_imuSensor->GetAcceleration();
		// 	rotData = m_imuSensor->GetRotationRates();
		// 	double temp_data[6] = { accelData.x, accelData.y, accelData.z, rotData.x, rotData.y, rotData.z };
		// 	m_udpSocket.Send(SensorPacketType::IMU, 0, temp_data, sizeof(temp_data));
		// 	//sprintf(buff, "IMU Data: %f, %f, %f", accelData.x, accelData.y, accelData.z);
		// 	//LogMessage(buff, kLogMsgPlugin);
		// }

		if(m_lidarSensor != nullptr)
		{
			SensorDataLock<PointVector> temp = m_lidarSensor->GetPointCloud();
			std::vector<Point>* pointCloud = temp.GetData();
			uint32_t pc_size = pointCloud->size()-1;

			lidarPosition = m_lidarSensor->GetWorldObject()->GetPosition();

			if(pointCloud->size() > 0)
			{
				if(m_firstRun)
				{
					(*pointCloud)[0].x = 0.1;
					(*pointCloud)[0].y = 0.2;
					(*pointCloud)[0].z = 0.3;

					(*pointCloud)[pc_size].x = 0.1;
					(*pointCloud)[pc_size].y = 0.2;
					(*pointCloud)[pc_size].z = 0.3;
					m_firstRun = false;
				}
				else if((*pointCloud)[0].x != 0.1 && (*pointCloud)[0].y != 0.2 && (*pointCloud)[0].z != 0.3 &&
					(*pointCloud)[pc_size].x != 0.1 && (*pointCloud)[pc_size].y != 0.2 && (*pointCloud)[pc_size].z != 0.3)
				{
					//segment the packet by 800 point fragments
					uint32_t pc_frag_size = 800;
					for(int i = 0; i < pointCloud->size()/pc_frag_size; i++)
					{
						//m_udpSocket.Send(SensorPacketType::LIDAR, 0, pointCloud->data(), pointCloud->size()*sizeof(Point));
						m_udpSocket.Send(SensorPacketType::LIDAR, lidarPosition, 0, &pointCloud->data()[i*pc_frag_size], pc_frag_size*sizeof(Point));
					}
					//don't forget remainder if any
					uint32_t remainder = pointCloud->size()%pc_frag_size;
					if(remainder > 0)
					{
						m_udpSocket.Send(SensorPacketType::LIDAR, lidarPosition, 0, &pointCloud->data()[pc_size-remainder], remainder*sizeof(Point));
					}
					
					sprintf(buff, "lidar buff size: %d", pointCloud->size());
					LogMessage(buff, kLogMsgPlugin);
					(*pointCloud)[0].x = 0.1;
					(*pointCloud)[0].y = 0.2;
					(*pointCloud)[0].z = 0.3;

					(*pointCloud)[pc_size].x = 0.1;
					(*pointCloud)[pc_size].y = 0.2;
					(*pointCloud)[pc_size].z = 0.3;
				}
			}
		}
	}

	GLICSensorFactory::GLICSensorFactory()
		: m_sensorIDCount(0)
	{
		DataTypeManager& dataTypeMgr = DataTypeManager::GetSingleton();

		DataTypeDescription typeDesc("GLICSensor", "GLIC Sensor", "GLIC Sensor Plugin");
		typeDesc.typeIsA.push_back(Types::Sensor);

		Types::GLICSensor = dataTypeMgr.RegisterDataType(typeDesc, kSampleSensorGUID);

		RegisterProperties();

		Anvel::SensorManager::GetSingleton().RegisterSensorFactory(kSensorTypeGLICSensor, this);

		GLICSensorStaticAssetParams* pAssetParams = new GLICSensorStaticAssetParams();
		pAssetParams->m_assetPreview = "GenericSensorIcon.png";
		pAssetParams->m_category = "Sensors";
		pAssetParams->m_isPlaceable = true;
		pAssetParams->m_assetName = kSensorTypeGLICSensor;
		pAssetParams->m_intData = 13;
		pAssetParams->m_gpsSensorID = 0;
		pAssetParams->m_worldObjectAssetName = "GenericSensor";
		pAssetParams->m_sampleRate = 100;
		pAssetParams->m_sensorType = kSensorTypeGLICSensor;
		auto defHash = CalculateAssetDefinitionHash(*pAssetParams);
		AssetManager::GetSingleton().RegisterAsset(StaticAssetParamsPtr(pAssetParams), Types::Sensor, defHash);
	}

	GLICSensorFactory::~GLICSensorFactory()
	{
		SensorManager::GetSingleton().UnregisterSensorFactory(this);
	}

	//////////////////////////////////////////////////////////////////////////

	SensorID GLICSensorFactory::GetNextSensorID()
	{
		ID sensorID = MakeID(Types::GLICSensor, m_sensorIDCount);
		m_sensorIDCount++;
		return sensorID;
	}

	//////////////////////////////////////////////////////////////////////////

	PropertyGroupInstance GLICSensorFactory::GetSensorProperties(GLICSensor& sensor)
	{
		PropertyGroupInstance properties;
		properties.push_back(Property(sensor.m_sampleIntData));

		return properties;
	}

	//////////////////////////////////////////////////////////////////////////

	IdVector GLICSensorFactory::GetIdsOfType(const ID /*dataType*/) const
	{
		//Since vehicles and other objects own sensors, and not this factory itself,
		//we are not responsible for keeping track of the IDs we create.
		IdVector output;
		return output;
	}

	void GLICSensorFactory::RegisterProperties()
	{
		PropertyManager& propMgr = PropertyManager::GetSingleton();

		//Sample properties
		propMgr.RegisterPropertyProvider(Types::GLICSensor, this);
		propMgr.RegisterProperty(Types::GLICSensor, "IntData", "Sample Int Data", false);
		propMgr.RegisterProperty(Types::GLICSensor, "GPSSensorID", "GPS Sensor ID", false);
	}

	//////////////////////////////////////////////////////////////////////////

	ObjectPropertySet GLICSensorFactory::GetProperties(ID objID)
	{
		uint32 dataType = GetDataType(objID);

		ObjectPropertySet output;

		auto pBaseSensor = SensorManager::GetConstSingleton().GetSensor(objID);

		if (pBaseSensor.expired())
		{
			ANVEL_ERROR();
			return output;
		}

		//Verify that the ID is of the correct type
		if (dataType == Types::GLICSensor)
		{
			auto shared = pBaseSensor.lock();
			auto* pSensor = static_cast<GLICSensor*>(shared.get());
			if (pSensor != nullptr)
			{
				//We also need to manually include our base sensor ID for the 
				//property system, adding it to the "isA" list.
				output.properties = GetSensorProperties(*pSensor);
				output.isAIds.push_back(pSensor->GetBaseSensorID());
			}
		}

		return output;
	}

	StaticAssetParamsPtr GLICSensorFactory::ParseXmlParams(const tinyxml2::XMLElement* pXmlParams)
	{
		SensorStaticAssetParams* pSensorParams = NULL;

		String sensorType = XmlUtils::GetStringAttribute(pXmlParams, "sensorType");

		if (sensorType == kSensorTypeGLICSensor)
		{
			GLICSensorStaticAssetParams* pParams = new GLICSensorStaticAssetParams();
			pSensorParams = pParams;

			//Do sensor specific XML parsing here
			pParams->m_intData = XmlUtils::GetUint32Attribute(pXmlParams, "intData", 0);
			pParams->m_gpsSensorID = XmlUtils::GetUint32Attribute(pXmlParams, "gpsSensorID", 0);
		}
		else
		{
			//Unhandled sensor type! All cases should be handled.
			ANVEL_ERROR();
			return StaticAssetParamsPtr();
		}

		//the following is necessary for all sensors
		pSensorParams->m_worldObjectAssetName = XmlUtils::GetStringAttribute(pXmlParams, "objectAsset");
		pSensorParams->m_sensorType = sensorType;
		pSensorParams->m_sampleRate = XmlUtils::GetDoubleAttribute(pXmlParams, "sampleRate");
		pSensorParams->m_assetName = XmlUtils::GetStringAttribute(pXmlParams, "name");
		pSensorParams->m_assetPreview = "";
		pSensorParams->m_category = "Sensors";
		pSensorParams->m_isPlaceable = true;

		return StaticAssetParamsPtr(pSensorParams);
	}

	//////////////////////////////////////////////////////////////////////////

	Sensor* GLICSensorFactory::CreateGLICSensor(SensorStaticAssetParams& params, DynamicAssetParams& dynamicParams)
	{
		GLICSensor* pSensor = new GLICSensor(GetNextSensorID(), params, dynamicParams);
		return pSensor;
	}

	//////////////////////////////////////////////////////////////////////////

	Sensor* GLICSensorFactory::CreateSensor(SensorStaticAssetParams& params, DynamicAssetParams& dynamicParams)
	{
		Sensor* pSensor = NULL;
		if (params.m_sensorType == kSensorTypeGLICSensor)
		{
			pSensor = CreateGLICSensor(params, dynamicParams);
		}
		else
		{
			LogMessage("Unhandled Sensor type attempting creation from GLICSensorFactory.", kLogMsgError);
		}

		return pSensor;
	}
}
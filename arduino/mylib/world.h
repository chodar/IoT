
#include <OneWire.h>
#include <DallasTemperature.h>

#include <DHTesp.h> // https://github.com/beegee-tokyo/DHTesp

class World
{
  public:
    virtual float getTemperature() = 0;
    virtual float getHumidity() = 0;
    virtual float getSmoke() = 0;

	void printState()
	{
		Serial.print("Temperature is: " + String(getTemperature()) 
			+ ", humidity: " + String(getHumidity())
			+ "% smoke: " + String(getSmoke()));
	}

	const float NoResult = -1;
};

class WorldNoResult : public World
{
  public:
    virtual float getTemperature()
    {
		return NoResult;
	};
    
    virtual float getHumidity()
    {
		return NoResult;
	};
    
    virtual float getSmoke()
    {
		return NoResult;
	};
};

class WorldDallas: public World
{
public:
	WorldDallas(int pin) : oneWire(pin), sensors(&oneWire)
	{
		sensors.begin(); 
	}

	virtual float getTemperature()
	{
		Serial.print("Requesting temperatures...");
		sensors.requestTemperatures(); // Send the command to get temperatures
		Serial.println("DONE");

		Serial.print("Temperature is: ");

		float temperature = sensors.getTempCByIndex(0);
		
		Serial.println(String(temperature)); 
		
		return temperature;
	};

	virtual float getHumidity()
	{
		return NoResult;
	};

	virtual float getSmoke()
	{
		return NoResult;
	};

private:
	// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
	OneWire oneWire;
	DallasTemperature sensors;


};



class WorldDht22 : public World
{
public:
	WorldDht22(int dhtPin)
	{
		dht.setup(dhtPin, DHTesp::DHT22);
		Serial.println("DHT initiated");
	}

	virtual float getTemperature()
	{
		float value = dht.getTemperature();
		// Check if any reads failed and exit early (to try again).
		if (dht.getStatus() != 0) {
			Serial.println("DHT11 error status: " + String(dht.getStatusString()));
			return NoResult;
		}

		return value;
	};

	virtual float getHumidity()
	{
		float value = dht.getHumidity();
		// Check if any reads failed and exit early (to try again).
		if (dht.getStatus() != 0) {
			Serial.println("DHT11 error status: " + String(dht.getStatusString()));
			return NoResult;
		}

		return value;
	};

	virtual float getSmoke()
	{
		return NoResult;
	};

private:
	DHTesp dht;
};

/*


String getSmoke()
{
//String result = String(analogRead(A0));
String result = "0.0";
Serial.println("Smoke level " + result);

return result;
}


*/
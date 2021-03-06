//
// C++ Implementation by Ajay Bhaga
//

#ifndef EANN_SIMPLE_SENSOR_H
#define EANN_SIMPLE_SENSOR_H

#include <string>
#include <stdlib.h>     /* abs */
#include "util/math_helper.h"
#include "util/random_d.h"
#include "ai/agent.h"
#include <cyclone.h>

// Class representing a sensor reading the distance to the nearest obstacle in a specified direction.
class Sensor {
public:
    Sensor(Agent *agent);
    ~Sensor();

    void start();
    void update();
    void hide();
    void show();

    const cyclone::Vector3 &getDirection() const;
    Agent *getAgent() const;
    const cyclone::Vector3 &getOffset() const;
    void setOffset(const cyclone::Vector3 &offset);
    void setDirection(const cyclone::Vector3 &direction);
    const cyclone::Vector3 &getTarget() const;
    const cyclone::Vector3 &getCenter() const;
    void setCenter(const cyclone::Vector3 &center);
    cyclone::CollisionSphere getCollisionSphere(const cyclone::Vector3 &position,
                                                        const cyclone::Vector3 &extents);

    bool isHit() const;
    void setHit(bool hit);
    long getLastHit() const;
    void setLastHit(long lastHit);

    // The current sensor readings in percent of maximum distance.
    float output;

private:
    bool visibility;

    const long AGENT_HIT_TIMEOUT = 3000;
    const float MAX_DIST = 10.0f;
    const float MIN_DIST = 0.01f;

    // Offset from center of agent
    cyclone::Vector3 offset;
    // Sensor center
    cyclone::Vector3 center;
    // Sensor target
    cyclone::Vector3 target;

    Agent *agent;
    // Sensor direction
    cyclone::Vector3 direction;

    cyclone::Vector3 halfSize;

    bool hit = false;
    long lastHit = 0;

};

#endif //EANN_SIMPLE_SENSOR_H
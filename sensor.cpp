//
// C++ Implementation by Ajay Bhaga
//

#include "sensor.h"
#include "evolution_manager.h"

// TODO: Add sensor types (multiple types can diversify the mutation process).

Sensor::Sensor(Agent *agent) {
    this->agent = agent;
}

Sensor::~Sensor() {

}

void Sensor::start() {
    show();
}

void Sensor::update() {
    // Update stored position derived from agent position
    this->center = agent->getPosition() + this->offset;

    std::vector<Agent*> agents = EvolutionManager::getInstance()->getAgents();
    std::vector<AgentController*> controllers = EvolutionManager::getInstance()->getAgentControllers();

    // Calculate hit distance
    float hitDistance = 0.0;

    cyclone::CollisionSphere sphere = cyclone::CollisionSphere();
    sphere.body = new cyclone::RigidBody();
    sphere.body->setPosition(this->center);
//    sphere.radius = (this->target-this->center).magnitude();
    sphere.radius = 0.1f;

    cyclone::CollisionSphere sphereB = cyclone::CollisionSphere();
    sphereB.body = new cyclone::RigidBody();
    for (int i = 0; i < agents.size(); i++) {
        sphereB.body->setPosition(agents[i]->getPosition());
        sphereB.radius = 0.01f;

        // Check if name matches, skip this one
        if (strcmp(agents[i]->getName(), this->getAgent()->getName()) == 0) {
            // Found you, skip
            continue;
        } else {
            if (cyclone::IntersectionTests::sphereAndSphere(sphere, sphereB)) {
                // Hit another agent
                hitDistance = (sphere.body->getPosition() - sphereB.body->getPosition()).magnitude();
                agents[i]->setHit(true);
                agents[i]->setLastHit(std::chrono::high_resolution_clock::now().time_since_epoch().count());


                std::cout << "Collision between " << agents[i]->getName() << " and " << this->getAgent()->getName() << std::endl;

            }
        }

        // Check for hit timeout
        if (agents[i]->getLastHit() > 0) {
            if (agents[i]->getLastHit()-std::chrono::high_resolution_clock::now().time_since_epoch().count() > AGENT_HIT_TIMEOUT) {
                agents[i]->setHit(false);
            }
        }
    }


    /*
    // Send raycast into direction of sensor
    cyclone::CollisionPlane plane = cyclone::CollisionPlane();
    plane.direction = cyclone::Vector3(0.0, 1.0f, 0.0);
    plane.offset = -100.0;


    if (cyclone::IntersectionTests::sphereAndHalfSpace(sphere, plane)) {
        hitDistance = sphere.radius;
    }*/

    if (hitDistance < MIN_DIST) {
        hitDistance = MIN_DIST;
    }

    // Transform to percent of max distance
    this->output = hitDistance;

    // Set position of sensor target
    this->target = this->center + (this->direction * hitDistance);
}

// Hides the visual representation of the sensor
void Sensor::hide() {
    visibility = false;
}

// Shows the visual representation of the sensor
void Sensor::show() {
    visibility = true;
}

Agent *Sensor::getAgent() const {
    return agent;
}

const cyclone::Vector3 &Sensor::getTarget() const {
    return target;
}

const cyclone::Vector3 &Sensor::getDirection() const {
    return direction;
}

const cyclone::Vector3 &Sensor::getOffset() const {
    return offset;
}

void Sensor::setOffset(const cyclone::Vector3 &offset) {
    Sensor::offset = offset;
}

void Sensor::setDirection(const cyclone::Vector3 &direction) {
    Sensor::direction = direction;
    //this->direction.normalize();
}

const cyclone::Vector3 &Sensor::getCenter() const {
    return center;
}

void Sensor::setCenter(const cyclone::Vector3 &center) {
    Sensor::center = center;
}

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

/**
 * We use a sphere to collide bone on bone to allow some limited
 * interpenetration.
 */
cyclone::CollisionSphere Sensor::getCollisionSphere(const cyclone::Vector3 &position,
                                                    const cyclone::Vector3 &extents)
{
    cyclone::CollisionSphere sphere;
    sphere.body = new cyclone::RigidBody();
    sphere.body->setPosition(position);
    sphere.body->setOrientation(cyclone::Quaternion());
    sphere.body->setVelocity(cyclone::Vector3());
    sphere.body->setRotation(cyclone::Vector3());
    halfSize = extents;

    cyclone::real mass = halfSize.x * halfSize.y * halfSize.z * 8.0f;
    sphere.body->setMass(mass);

    cyclone::Matrix3 tensor;
    tensor.setBlockInertiaTensor(halfSize, mass);
    sphere.body->setInertiaTensor(tensor);

    sphere.body->setLinearDamping(0.95f);
    sphere.body->setAngularDamping(0.8f);
    sphere.body->clearAccumulators();
    sphere.body->setAcceleration(cyclone::Vector3::GRAVITY);

    sphere.body->setCanSleep(false);
    sphere.body->setAwake();

    sphere.body->calculateDerivedData();
    sphere.calculateInternals();

    sphere.radius = halfSize.x;
    sphere.offset = cyclone::Matrix4();
    if (halfSize.y < sphere.radius) sphere.radius = halfSize.y;
    if (halfSize.z < sphere.radius) sphere.radius = halfSize.z;
    sphere.calculateInternals();
    return sphere;
}

void Sensor::update() {
    // Update stored position derived from agent position
    this->center = agent->getPosition() + this->offset;

    std::vector<Agent*> agents = EvolutionManager::getInstance()->getAgents();
    std::vector<AgentController*> controllers = EvolutionManager::getInstance()->getAgentControllers();

    // Calculate hit distance
    float hitDistance = 0.0;

//    sphere.radius = (this->target-this->center).magnitude();
    cyclone::CollisionSphere sphere = this->getCollisionSphere(this->center, cyclone::Vector3(1.0, 1.0, 1.0));

    for (int i = 0; i < agents.size(); i++) {
        cyclone::CollisionSphere sphereB = this->getCollisionSphere(agents[i]->getPosition(), cyclone::Vector3(1.0, 1.0, 1.0));

        // Check if name matches, skip this one
        if (strcmp(agents[i]->getName(), this->getAgent()->getName()) == 0) {
            // Found you, skip
            continue;
        } else {
            if (cyclone::IntersectionTests::sphereAndSphere(sphere, sphereB)) {
                // Hit another agent
                hitDistance = (sphere.body->getPosition() - sphereB.body->getPosition()).magnitude();

                long hitTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
                this->setHit(true);
                this->setLastHit(hitTime);
                agents[i]->setHit(true);
                agents[i]->setLastHit(hitTime);

                std::cout << "Collision between " << agents[i]->getName() << " and " << this->getAgent()->getName() << " at distance " << hitDistance << std::endl;
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

    // Set position of sensor target (adjusted based on hit)
//    this->target = this->center + (this->direction * hitDistance);
    // Set position of sensor target (fixed)
    this->target = this->center + (this->direction * 1.0f);

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
    this->direction.normalize();
}

const cyclone::Vector3 &Sensor::getCenter() const {
    return center;
}

void Sensor::setCenter(const cyclone::Vector3 &center) {
    Sensor::center = center;
}

bool Sensor::isHit() const {
    return hit;
}

void Sensor::setHit(bool hit) {
    this->hit = hit;
}

long Sensor::getLastHit() const {
    return lastHit;
}

void Sensor::setLastHit(long lastHit) {
    this->lastHit = lastHit;
}

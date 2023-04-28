#include "hcp/RobotRenderer.hpp"

#include "Logger.hpp"
#include "Shaders.hpp"

#include "hcp/Resources.hpp"

#include <iostream>
#include <nlohmann/json.hpp>
#include <glm/gtc/matrix_transform.hpp>

static HCPLogger logger("RobotRenderer");

// Renderer state
static bool hasAdviseInit = false;
static bool hasInit = false;
static bool hasLoadedResources = false;

// Robot state
static float robotX = 0.0f;
static float robotY = 0.0f;
static float robotSwivel = 0.0f;
static float robotClaw = 0.0f;

struct RobotPart
{
    RobotPart* parent = nullptr;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    HCPMeshPtr mesh;
    std::vector<RobotPart*> children;

    void draw(glm::mat4 modelView = glm::mat4(1.0f))
    {
        glm::mat4 model = modelView;
        model = glm::translate(model, position);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));

        hcps::setModelViewMatrix(model);
        hcps::POS_COLOR();
        mesh->render();

        for(auto child : children)
        {
            child->draw(model);
        }
    }

    void addChild(RobotPart* child)
    {
        child->parent = this;
        children.push_back(child);
    }
};

// Robot parts
std::unordered_map<std::string, RobotPart*> robotParts;
RobotPart* robotBase = nullptr;
RobotPart* robotArm = nullptr;

// Resources
static nlohmann::json robotSpecs;

// Returns true if the renderer has not been initialized or resources have not been loaded
static bool i_adviseInit(const char* module);

// Loads the robot specs from the JSON file
static void i_loadRobotSpecs();

// Loads meshes from the robot specs
static void i_loadMeshes();

// Loads parts from the robot specs
static void i_loadParts();

namespace HCPRobotRenderer
{
    void init()
    {
        if(hasInit) return;
        logger.infof("Initializing RobotRenderer");

        i_loadRobotSpecs();
        i_loadMeshes();
        i_loadParts();

        hasInit = true;
    }

    void terminate()
    {
        logger.infof("Terminating RobotRenderer");

        // Free meshes
        for(auto& mesh : robotSpecs["meshes"])
        {
            std::string name = mesh["name"].get<std::string>();
            name = "robot:" + name;

            hcpr::removeMesh(name.c_str());
        }

        // Free parts
        robotParts.clear();
    }

    void loadResources()
    {
        logger.infof("Loading RobotRenderer resources");
    }

    void drawAll()
    {
        if(i_adviseInit("drawAll")) return;

        if(robotBase) robotBase->draw(hcps::getModelViewMatrix());
    }

    void drawArm()
    {
        if(i_adviseInit("drawArm")) return;

        if(robotArm) robotArm->draw(hcps::getModelViewMatrix());
    }

    void setX(float x)
    {
        robotX = x;
    }

    void setY(float y)
    {
        robotY = y;
    }

    void setSwivel(float swivel)
    {
        robotSwivel = swivel;
    }

    void setClaw(float claw)
    {
        robotClaw = claw;
    }

    float getX()
    {
        return robotX;
    }

    float getY()
    {
        return robotY;
    }

    float getSwivel()
    {
        return robotSwivel;
    }

    float getClaw()
    {
        return robotClaw;
    }
}

static bool i_adviseInit(const char* module)
{
    if(!hasInit || !hasLoadedResources)
    {
        if(!hasAdviseInit)
        {
            logger.submodule(module).errorf("Cannot perform operation, RobotRenderer has not been initialized or resources have not been loaded");
            hasAdviseInit = true;
        }
        return true;
    }

    return false;
}

static void i_loadRobotSpecs()
{
    logger.infof("Loading robot specs");

    size_t jsonDataLen;
    const char* jsonData = (const char*) hcpr::loadResource(HCP_RESOURCE_PATH(robot, robot.json), HCP_RESOURCE(robot, specs), &jsonDataLen);

    if(!jsonData)
    {
        logger.errorf("Failed to load robot specs");
        return;
    }

    robotSpecs = nlohmann::json::parse(jsonData, jsonData + jsonDataLen);

    hcpr::unloadResource(HCP_RESOURCE(robot, specs));

    hasInit = true;
}

static void i_loadMeshes()
{
    logger.infof("Loading robot meshes");
    HCPVertexFormat vtxFmt;
    vtxFmt.size = 2;
    vtxFmt[0].data = HCPVF_ATTRB_USAGE_POS
                   | HCPVF_ATTRB_TYPE_FLOAT
                   | HCPVF_ATTRB_SIZE(3)
                     | HCPVF_ATTRB_NORMALIZED_FALSE;
    vtxFmt[1].data = HCPVF_ATTRB_USAGE_COLOR
                   | HCPVF_ATTRB_TYPE_FLOAT
                   | HCPVF_ATTRB_SIZE(4)
                   | HCPVF_ATTRB_NORMALIZED_FALSE;

    for(auto& mesh : robotSpecs["meshes"])
    {
        std::string name = mesh["name"].get<std::string>();
        std::string file = mesh["file"].get<std::string>();
        name = "robot:" + name;
        file = "res/robot/" + file;

        HCPMeshPtr mesh = hcpm::loadMeshes(file.c_str())[0];
        mesh->makeRenderable(vtxFmt);

        hcpr::addMesh(mesh, name.c_str());
    }

    hasLoadedResources = true;
}

static void i_loadParts()
{
    logger.infof("Loading robot parts");

    // Load all parts initially
    for(auto& part : robotSpecs["parts"])
    {
        std::string name = part["name"].get<std::string>();
        std::string mesh = part["mesh"].get<std::string>();
        mesh = "robot:" + mesh;

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        position = glm::vec3(part["position"][0].get<float>(), part["position"][1].get<float>(), part["position"][2].get<float>());
        rotation = glm::vec3(part["rotation"][0].get<float>(), part["rotation"][1].get<float>(), part["rotation"][2].get<float>());
        scale = glm::vec3(part["scale"][0].get<float>(), part["scale"][1].get<float>(), part["scale"][2].get<float>());

        RobotPart* robotPart = new RobotPart();
        robotPart->mesh = hcpr::getMesh(mesh.c_str());
        robotPart->position = position;
        robotPart->rotation = rotation;
        robotPart->scale = scale;

        robotParts.insert({name, robotPart});
    }

    // Link all parts
    for(auto& part : robotSpecs["parts"])
    {
        if(part["parent"].is_null())
            continue;

        std::string name = part["name"].get<std::string>();
        std::string parentName = part["parent"].get<std::string>();

        RobotPart* child = robotParts[name];
        RobotPart* parent = robotParts[parentName];
        parent->addChild(child);
    }

    // Get base and arm
    robotBase = robotParts["base"];
    robotArm = robotParts["arm"];
}
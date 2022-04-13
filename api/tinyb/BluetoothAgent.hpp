#pragma once
#include "BluetoothObject.hpp"
#include "BluetoothManager.hpp"
#include <vector>


#define AGENT_PATH "/org/bluez/agent"
#define AGENT_INTERFACE "org.bluez.Agent1"

/* Forward declaration of types */
struct _Object;
typedef struct _Object Object;
struct _AgentManager1;
typedef struct _AgentManager1 AgentManager1;
using namespace tinyb;
class BluetoothAgent: public BluetoothObject
{
friend class tinyb::BluetoothManager;

public:
    BluetoothAgent(const BluetoothAgent &object);

    void BluetoothAgentRegister();

    void BluetoothAgentUnregister();


    static std::string java_class() {
        return std::string(JAVA_PACKAGE "/BluetoothAgent");
    }
    static BluetoothType class_type() { return BluetoothType::AGENT; }

    virtual std::string get_java_class() const;
    virtual std::string get_class_name() const;
    virtual std::string get_object_path() const;
    virtual BluetoothType get_bluetooth_type() const;

    virtual BluetoothAgent *clone() const;

    ~BluetoothAgent();

protected:
    BluetoothAgent(AgentManager1 *object);

    static std::unique_ptr<BluetoothAgent> make(Object *object,
        BluetoothType type = BluetoothType::AGENT,
        std::string *name = nullptr,
        std::string *identifier = nullptr,
        BluetoothObject *parent = nullptr);

private:
    AgentManager1 *object;
    unsigned int agent_id_  = 0;

};
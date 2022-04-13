#include "generated-code.h"
#include "tinyb_utils.hpp"
#include "BluetoothNotificationHandler.hpp"
#include "BluetoothAdapter.hpp"
#include "BluetoothDevice.hpp"
#include "BluetoothManager.hpp"
#include "BluetoothException.hpp"
#include "BluetoothAgent.hpp"

//#include <gio/gdbusobjectmanagerclient.h>

using namespace tinyb;
static const gchar *_bt_agent_introspect_xml = "<node name=\"/org/bluez/agent\">\n\t<interface name=\"org.bluez.Agent1\">\n\t\t<method name=\"Release\">\n\t\t</method>\n\t\t<method name=\"RequestPinCode\">\n\t\t\t<arg name=\"device\" direction=\"in\" type=\"o\"/>\n\t\t\t<arg name=\"pincode\" direction=\"out\" type=\"s\"/>\n\t\t</method>\n\t\t<method name=\"DisplayPinCode\">\n\t\t\t<arg name=\"device\" direction=\"in\" type=\"o\"/>\n\t\t\t<arg name=\"pincode\" direction=\"in\" type=\"s\"/>\n\t\t</method>\n\t\t<method name=\"RequestPasskey\">\n\t\t\t<arg name=\"device\" direction=\"in\" type=\"o\"/>\n\t\t\t<arg name=\"passkey\" direction=\"out\" type=\"u\"/>\n\t\t</method>\n\t\t<method name=\"DisplayPasskey\">\n\t\t\t<arg name=\"device\" direction=\"in\" type=\"o\"/>\n\t\t\t<arg name=\"passkey\" direction=\"in\" type=\"u\"/>\n\t\t\t<arg name=\"entered\" direction=\"in\" type=\"q\"/>\n\t\t</method>\n\t\t<method name=\"RequestConfirmation\">\n\t\t\t<arg name=\"device\" direction=\"in\" type=\"o\"/>\n\t\t\t<arg name=\"passkey\" direction=\"in\" type=\"u\"/>\n\t\t</method>\n\t\t<method name=\"RequestAuthorization\">\n\t\t\t<arg name=\"device\" direction=\"in\" type=\"o\"/>\n\t\t</method>\n\t\t<method name=\"AuthorizeService\">\n\t\t\t<arg name=\"device\" direction=\"in\" type=\"o\"/>\n\t\t\t<arg name=\"uuid\" direction=\"in\" type=\"s\"/>\n\t\t</method>\n\t\t<method name=\"Cancel\">\n\t\t</method>\n\t</interface>\n</node>\n";

static void _bt_agent_method_call_func(GDBusConnection *connection, const gchar *sender, const gchar *object_path, 
const gchar *interface_name, const gchar *method_name, GVariant *parameters, GDBusMethodInvocation *invocation, gpointer user_data)
{
    auto agent = static_cast<BluetoothAgent*>(user_data);

    printf("agent method name : %s\n", method_name);;

    if(g_strcmp0(method_name, "AuthorizeService") == 0){

    }else if (g_strcmp0(method_name, "Cancel") == 0){

    }else if (g_strcmp0(method_name, "DisplayPasskey") == 0){

    }else if (g_strcmp0(method_name, "DisplayPinCode") == 0){

    }else if (g_strcmp0(method_name, "Release") == 0){

    }else if (g_strcmp0(method_name, "RequestAuthorization") == 0){

    }else if (g_strcmp0(method_name, "RequestConfirmation") == 0){

    }else if (g_strcmp0(method_name, "RequestPasskey") == 0){

    }else if (g_strcmp0(method_name, "RequestPinCode") == 0){

    }
}

BluetoothAgent::BluetoothAgent(const BluetoothAgent &object){
    BluetoothAgent(object.object);
}

BluetoothAgent::BluetoothAgent(AgentManager1 *object){
    this->object = object;
    g_object_ref(object);
}



void BluetoothAgent::BluetoothAgentRegister(){
    GDBusInterfaceVTable bt_agent_table;

    memset(&bt_agent_table, 0x0, sizeof(bt_agent_table));
    bt_agent_table.method_call = _bt_agent_method_call_func;
    if(this->agent_id_ == 0){
        GDBusNodeInfo *bt_agent_node_info = g_dbus_node_info_new_for_xml(_bt_agent_introspect_xml, NULL);
        GDBusInterfaceInfo *bt_agent_interface_info = g_dbus_node_info_lookup_interface(bt_agent_node_info, AGENT_INTERFACE);
        this->agent_id_ = g_dbus_connection_register_object(g_dbus_object_manager_client_get_connection(G_DBUS_OBJECT_MANAGER_CLIENT(gdbus_manager)), AGENT_PATH, bt_agent_interface_info, &bt_agent_table, this, NULL, NULL);
        g_dbus_node_info_unref(bt_agent_node_info);
    }

    if(this->agent_id_ > 0){
        /*register agent*/
        GError *error = NULL;
        agent_manager1_call_register_agent_sync(object, AGENT_PATH,"DisplayYesNo", NULL, &error);
        handle_error(error);
    }
}

void BluetoothAgent::BluetoothAgentUnregister(){
    if(this->agent_id_ > 0){
        /*unregister agent*/

        /*destory agent*/
    }
}

BluetoothAgent::~BluetoothAgent(){
    BluetoothAgentUnregister();
    g_object_unref(object);
}

std::unique_ptr<BluetoothAgent> BluetoothAgent::make(Object *object,
        BluetoothType type, std::string *name, std::string *identifier,
        BluetoothObject *parent)
{
    AgentManager1 *agent;
    if((type == BluetoothType::NONE || type == BluetoothType::AGENT) &&
        (agent = object_get_agent_manager1(object)) != NULL) {

        std::unique_ptr<BluetoothAgent> p(new BluetoothAgent(agent));
        g_object_unref(agent);

        return p;
    }

    return std::unique_ptr<BluetoothAgent>();

}

std::string BluetoothAgent::get_class_name() const
{
    return std::string("BluetoothAgent");
}

std::string BluetoothAgent::get_java_class() const
{
    return std::string(JAVA_PACKAGE "/BluetoothAgent");
}

std::string BluetoothAgent::get_object_path() const
{
    return std::string(g_dbus_proxy_get_object_path(G_DBUS_PROXY(object)));
}

BluetoothType BluetoothAgent::get_bluetooth_type() const
{
    return BluetoothType::AGENT;
}

BluetoothAgent *BluetoothAgent::clone() const
{
    return new BluetoothAgent(object);
}
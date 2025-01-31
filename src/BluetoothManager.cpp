/*
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "generated-code.h"
#include "BluetoothManager.hpp"
#include "BluetoothAdapter.hpp"
#include "BluetoothDevice.hpp"
#include "BluetoothGattService.hpp"
#include "BluetoothGattCharacteristic.hpp"
#include "BluetoothGattDescriptor.hpp"
#include "BluetoothEvent.hpp"
#include "BluetoothException.hpp"
#include "version.h"

#include <cassert>

using namespace tinyb;

class tinyb::BluetoothEventManager {
public:

    static void _on_interface_handle(GDBusObject *object,
        GDBusInterface *interface, gpointer user_data, BluetoothEventType evt_type){
        GDBusInterfaceInfo *info = g_dbus_interface_get_info(interface);
        BluetoothType type = BluetoothType::NONE;
        BluetoothManager *manager = BluetoothManager::get_bluetooth_manager();

        /* Unknown interface, ignore */
        if (info == NULL)
            return;

        if(IS_GATT_SERVICE1_PROXY(interface)) {
            type = BluetoothType::GATT_SERVICE;
            BluetoothGattService obj(GATT_SERVICE1(interface));
            auto uuid = obj.get_uuid();
            auto parent = obj.get_device();
            manager->handle_event(type, nullptr, &uuid, &parent, obj, evt_type);
        }
        else if(IS_GATT_CHARACTERISTIC1_PROXY(interface)) {
            type = BluetoothType::GATT_CHARACTERISTIC;
            BluetoothGattCharacteristic obj(GATT_CHARACTERISTIC1(interface));
            auto uuid = obj.get_uuid();
            auto parent = obj.get_service();
            manager->handle_event(type, nullptr, &uuid, &parent, obj,evt_type);
        }
        else if(IS_GATT_DESCRIPTOR1_PROXY(interface)) {
            type = BluetoothType::GATT_DESCRIPTOR;
            BluetoothGattDescriptor obj(GATT_DESCRIPTOR1(interface));
            auto uuid = obj.get_uuid();
            auto parent = obj.get_characteristic();
            manager->handle_event(type, nullptr, &uuid, &parent, obj,evt_type);
        }
        else if(IS_DEVICE1_PROXY(interface)) {
            type = BluetoothType::DEVICE;
            BluetoothDevice obj(DEVICE1(interface));
            auto name = obj.get_name();
            auto uuid = obj.get_address();
            auto parent = obj.get_adapter();
            manager->handle_event(type, &name, &uuid, &parent, obj,evt_type);
        }
        else if(IS_ADAPTER1_PROXY(interface)) {
            type = BluetoothType::ADAPTER;
            BluetoothAdapter obj(ADAPTER1(interface));
            auto name = obj.get_name();
            auto uuid = obj.get_address();
            manager->handle_event(type, &name, &uuid, nullptr, obj, evt_type);
        }
    }

    static void on_interface_added (GDBusObject *object,
        GDBusInterface *interface, gpointer user_data) {
            _on_interface_handle(object, interface, user_data,BluetoothEventType::ADDED);
    }

    static void on_object_added (GDBusObjectManager *manager,
        GDBusObject *object, gpointer user_data) {
        GList *l, *interfaces = g_dbus_object_get_interfaces(object);

        for(l = interfaces; l != NULL; l = l->next){
            on_interface_added(object, (GDBusInterface *)l->data, user_data);
        }
            
        g_list_free_full(interfaces, g_object_unref);
    }


    static void on_interface_removed (GDBusObject *object,
        GDBusInterface *interface, gpointer user_data){
        _on_interface_handle(object, interface, user_data,BluetoothEventType::REMOVED);
    }


    static void on_object_removed (GDBusObjectManager *manager,
        GDBusObject *object, gpointer user_data){
        GList *l, *interfaces = g_dbus_object_get_interfaces(object);

        for(l = interfaces; l != NULL; l = l->next){
            on_interface_removed(object, (GDBusInterface *)l->data, user_data);
        }
            
        g_list_free_full(interfaces, g_object_unref);
    }
};

GDBusObjectManager *gdbus_manager = NULL;
GThread *manager_thread = NULL;

std::string BluetoothManager::get_class_name() const
{
    return std::string("BluetoothManager");
}

std::string BluetoothManager::get_java_class() const
{
    return std::string(JAVA_PACKAGE "/BluetoothManager");
}

std::string BluetoothManager::get_object_path() const
{
    return std::string("/");
}

BluetoothType BluetoothManager::get_bluetooth_type() const
{
   return BluetoothType::NONE;
}

std::string BluetoothManager::get_api_version() {
    return std::string(gVERSION_API);
}

std::string BluetoothManager::get_library_version() {
    return std::string(gVERSION_SHORT);
}

std::unique_ptr<BluetoothObject> BluetoothManager::get_object(
    BluetoothType type, std::string *name, std::string *identifier,
    BluetoothObject *parent)
{
    auto list = get_objects(type, name, identifier, parent);
    if (list.empty())
        return std::unique_ptr<BluetoothObject>();
    return std::move(list.front());
}

std::vector<std::unique_ptr<BluetoothObject>> BluetoothManager::get_objects(
    BluetoothType type, std::string *name, std::string *identifier,
    BluetoothObject *parent)
{
    std::vector<std::unique_ptr<BluetoothObject>> vector;
    GList *l, *objects = g_dbus_object_manager_get_objects(gdbus_manager);

    for (l = objects; l != NULL; l = l->next) {
        Object *object = OBJECT(l->data);

        auto p_service = BluetoothGattService::make(object, type, name, identifier, parent);
        if (p_service != nullptr)
            vector.push_back(std::move(p_service));

        auto p_characteristic = BluetoothGattCharacteristic::make(object, type, name, identifier, parent);
        if (p_characteristic != nullptr)
            vector.push_back(std::move(p_characteristic));

        auto p_descriptor = BluetoothGattDescriptor::make(object, type, name, identifier, parent);
        if (p_descriptor != nullptr)
            vector.push_back(std::move(p_descriptor));

        auto p_device = BluetoothDevice::make(object, type, name, identifier, parent);
        if (p_device != nullptr)
            vector.push_back(std::move(p_device));

        auto p_adapter = BluetoothAdapter::make(object, type, name, identifier, parent);
        if (p_adapter != nullptr)
            vector.push_back(std::move(p_adapter));

        auto p_agent = BluetoothAgent::make(object, type, name, identifier, parent);
        if (p_agent != nullptr)
            vector.push_back(std::move(p_agent));
    }
    g_list_free_full(objects, g_object_unref);
    return vector;
}

std::unique_ptr<BluetoothObject> BluetoothManager::find(BluetoothType type,
    std::string *name, std::string* identifier, BluetoothObject *parent,
    std::chrono::milliseconds timeout)
{
    std::shared_ptr<BluetoothEvent> event(new BluetoothEvent(type, name,
        identifier, parent));
    add_event(event);

    auto object = get_object(type, name, identifier, parent);

    if (object == nullptr) {
        event->wait(timeout);
        object = std::unique_ptr<BluetoothObject>(event->get_result());
    }

    event->cancel();
    return object;
}

std::weak_ptr<BluetoothEvent> BluetoothManager::find(BluetoothType type,
    std::string *name, std::string* identifier, BluetoothObject *parent,
    BluetoothCallback cb, bool execute_once,
    std::chrono::milliseconds timeout)
{
    std::shared_ptr<BluetoothEvent> event(new BluetoothEvent(type, name,
        identifier, parent));
    add_event(event);
    return std::weak_ptr<BluetoothEvent>(event);
}

std::weak_ptr<BluetoothEvent> BluetoothManager::listen(BluetoothType type, std::string *name,
    std::string* identifier, BluetoothObject *parent, BluetoothCallback cb,
    void* data, bool execute_once,
    std::chrono::milliseconds timeout)
{

    std::shared_ptr<BluetoothEvent> event(new BluetoothEvent(type, name,
        identifier, parent,execute_once,cb,data));
    add_event(event);
    return std::weak_ptr<BluetoothEvent>(event);
}


void BluetoothManager::handle_event(BluetoothType type, std::string *name,
    std::string *identifier, BluetoothObject *parent, BluetoothObject &object, BluetoothEventType evt_type)
{
    for (auto it = event_list.begin();
        it != event_list.end();) {
        if ((*it)->get_type() != BluetoothType::NONE && ((*it)->get_type()) != type) {
            ++it;
            continue; /* this event does not match */
        }
        if ((*it)->get_name() != NULL)
            if (name == NULL || *((*it)->get_name()) != *name) {
                ++it;
                continue; /* this event does not match */
            }
        if ((*it)->get_identifier() != NULL)
            if (identifier == NULL || *((*it)->get_identifier()) != *identifier) {
                ++it;
                continue; /* this event does not match */
            }
        if ((*it)->get_parent() != NULL)
            if (parent == NULL || *((*it)->get_parent()) != *parent) {
                ++it;
                continue; /* this event does not match */
            }
        /* The event matches, execute and see if it needs to reexecute */

        if ((*it)->execute_callback(object,evt_type))
            it = event_list.erase(it);
        else
            ++it;
    }
}

static gpointer init_manager_thread(void *data)
{
    GMainLoop *loop;
    GDBusObjectManager *gdbus_manager = (GDBusObjectManager *) data;

    loop = g_main_loop_new(NULL, FALSE);
    
    g_signal_connect(gdbus_manager,
    "interface-added",
        G_CALLBACK(BluetoothEventManager::on_interface_added),
        NULL);

    g_signal_connect(gdbus_manager,
        "object-added",
         G_CALLBACK(BluetoothEventManager::on_object_added),
         NULL);

    g_signal_connect(gdbus_manager,
        "interface-removed",
         G_CALLBACK(BluetoothEventManager::on_interface_removed),
         NULL);

    g_signal_connect(gdbus_manager,
        "object-removed",
         G_CALLBACK(BluetoothEventManager::on_object_removed),
         NULL);
    g_main_loop_run(loop);
    return NULL;
}

BluetoothManager::BluetoothManager() : event_list()
{
    GError *error = NULL;
    GList *objects, *l;


    gdbus_manager = object_manager_client_new_for_bus_sync(
            G_BUS_TYPE_SYSTEM,
            G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE,
            "org.bluez",
            "/",
            NULL, /* GCancellable */
            &error);

    if (gdbus_manager == nullptr) {
        std::string error_str("Error getting object manager client: ");
        error_str += error->message;
        g_error_free(error);
        throw std::runtime_error(error_str);
    }

    manager_thread = g_thread_new(NULL, init_manager_thread, gdbus_manager);

    objects = g_dbus_object_manager_get_objects(gdbus_manager);

    default_adapter = nullptr;
    for (l = objects; l != NULL; l = l->next) {
        Object *object = (Object *) l->data;
        Adapter1 *adapter = object_get_adapter1(object);
        if (adapter != NULL) {
            default_adapter = std::unique_ptr<BluetoothAdapter>(new BluetoothAdapter(adapter));
            g_object_unref(adapter);
            break;
        }
    }
    g_list_free_full(objects, g_object_unref);

    if (default_adapter == nullptr) {
        std::cout << "No adapter installed or not recognized by system" << std::endl;
        //throw BluetoothException("No adapter installed or not recognized by system");
    }
}

BluetoothManager *BluetoothManager::get_bluetooth_manager()
{
    static BluetoothManager bluetooth_manager;
    return &bluetooth_manager;
}

BluetoothManager::BluetoothManager(const BluetoothManager &)
{
    /* Should not be called */
}

BluetoothManager::~BluetoothManager()
{
    /* Should not be called */
}

std::vector<std::unique_ptr<BluetoothAdapter>> BluetoothManager::get_adapters()
{
    std::vector<std::unique_ptr<BluetoothAdapter>> vector;
    GList *l, *objects = g_dbus_object_manager_get_objects(gdbus_manager);

    for (l = objects; l != NULL; l = l->next) {
        Object *object = OBJECT(l->data);

        auto p = BluetoothAdapter::make(object);
        if (p != nullptr)
            vector.push_back(std::move(p));
    }
    g_list_free_full(objects, g_object_unref);

    return vector;
}

std::vector<std::unique_ptr<BluetoothDevice>> BluetoothManager::get_devices()
{
    std::vector<std::unique_ptr<BluetoothDevice>> vector;
    GList *l, *objects = g_dbus_object_manager_get_objects(gdbus_manager);

    for (l = objects; l != NULL; l = l->next) {
        Object *object = OBJECT(l->data);

        auto p = BluetoothDevice::make(object);
        if (p != nullptr)
            vector.push_back(std::move(p));
    }
    g_list_free_full(objects, g_object_unref);

    return vector;
}

std::unique_ptr<BluetoothAgent> BluetoothManager::get_agentmanager(){
    std::unique_ptr<BluetoothAgent> agent;
    GList *l, *objects = g_dbus_object_manager_get_objects(gdbus_manager);

    for (l = objects; l != NULL; l = l->next) {
        Object *object = OBJECT(l->data);

        auto p = BluetoothAgent::make(object);
        if (p != nullptr)
            agent = std::move(p);
    }
    g_list_free_full(objects, g_object_unref);
    return agent;
}

std::vector<std::unique_ptr<BluetoothGattService>> BluetoothManager::get_services()
{
    std::vector<std::unique_ptr<BluetoothGattService>> vector;
    GList *l, *objects = g_dbus_object_manager_get_objects(gdbus_manager);

    for (l = objects; l != NULL; l = l->next) {
        Object *object = OBJECT(l->data);

        auto p = BluetoothGattService::make(object);
        if (p != nullptr)
            vector.push_back(std::move(p));
    }
    g_list_free_full(objects, g_object_unref);

    return vector;
}

bool BluetoothManager::set_default_adapter(BluetoothAdapter &adapter)
{
    default_adapter = std::unique_ptr<BluetoothAdapter>(adapter.clone());
    return true;
}

bool BluetoothManager::init_custom_agent(){

    if(custom_agent != nullptr){
        return false;
    }
    custom_agent = std::move(get_agentmanager());
    return true;
}

bool BluetoothManager::set_default_adapter( std::unique_ptr<BluetoothAdapter> &adapter){
    if(adapter != nullptr){
        default_adapter = std::move(adapter);
    }else if(default_adapter != nullptr){
        default_adapter.reset(nullptr);
    }
    return true;
}

std::unique_ptr<BluetoothAdapter> BluetoothManager::get_default_adapter()
{
    if (default_adapter != nullptr){
        return std::unique_ptr<BluetoothAdapter>(default_adapter->clone());
    }else{
        return nullptr;
    }
}

bool BluetoothManager::register_custom_agent(){
    if (custom_agent != nullptr){
        custom_agent->BluetoothAgentRegister();
    }
    return false;
}

bool BluetoothManager::set_powered(bool value){
    if (default_adapter != nullptr){
        default_adapter->set_powered(value);
        return true;
    }
    else{
        return false;
    }
}

bool BluetoothManager::start_discovery()
{
    if (default_adapter != nullptr)
        return default_adapter->start_discovery();
    else
        return false;
}

bool BluetoothManager::stop_discovery()
{
    if (default_adapter != NULL)
        return default_adapter->stop_discovery();
    else
        return false;
}

bool BluetoothManager::get_discovering()
{
    if (default_adapter != NULL)
        return default_adapter->get_discovering();
    else
        return false;
}

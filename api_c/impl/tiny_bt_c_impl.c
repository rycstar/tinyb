

#include <tinyb.hpp>

#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
#include <csignal>

#include "tiny_bt_c.h"


using namespace tinyb;
using namespace std;

static void listen_adapter_callback(BluetoothObject &object, BluetoothEventType evt_type, void *data)
{
    manager = BluetoothManager::get_bluetooth_manager();
    BluetoothAdapter *adapter = static_cast<BluetoothAdapter *>(&object);

    std::unique_ptr<BluetoothAdapter> def_adapter = manager->get_default_adapter();
    if(evt_type == BluetoothEventType::ADDED){
        /*if default adapter is null, set it to default adapter*/
        if(def_adapter == nullptr){
            manager->set_default_adapter(adapter);
        }
    }else{
        /*remove default adapter*/
        if(def_adapter != nullptr && adapter->get_address() == def_adapter->get_address()){
            manager->set_default_adapter(nullptr);
        }
    }
}


void listen_device_callback(BluetoothObject &object, BluetoothEventType evt_type, void *data)
{
    BluetoothDevice *device = static_cast<BluetoothDevice *>(&object);
    std::cout << "Class = " << device->get_class_name() << " ";
    std::cout << "Address = " << device->get_address() << " ";
    std::cout << "Alias = " << device->get_alias() << " ";
    std::cout << "Connected = " << device->get_connected() << " ";
    if(evt_type == BluetoothEventType::ADDED){
        std::cout << "Action = " << "Added" << std::endl;
    }else{
        std::cout << "Action = " << "Removed" << std::endl;
    }
    std::cout << std::endl;
}


/*
* init tiny bluetooth manager
* 
*/
int tiny_bt_client_init(void){
    BluetoothManager *manager = nullptr;
    try {
        manager = BluetoothManager::get_bluetooth_manager();
    } catch(const std::runtime_error& e) {
        std::cerr << "Error while initializing libtinyb: " << e.what() << std::endl;
        return -1;
    }

    manager->listen(BluetoothType::ADAPTER,nullptr,nullptr,nullptr, listen_adapter_callback,nullptr,false, std::chrono::milliseconds::zero());

    manager->listen(BluetoothType::DEVICE,nullptr,nullptr,nullptr, listen_device_callback,nullptr,false, std::chrono::milliseconds::zero());
    return 0;
}

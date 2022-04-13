#include <tinyb.hpp>

#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
#include <csignal>

#include "cli/cli.h"
#include "cli/clifilesession.h"

using namespace std;
using namespace cli;
using namespace tinyb;

static void listen_adapter_callback(BluetoothObject &object, BluetoothEventType evt_type, void *data)
{
    BluetoothManager *manager = BluetoothManager::get_bluetooth_manager();
    BluetoothAdapter *adapter = dynamic_cast<BluetoothAdapter *>(&object);

    std::unique_ptr<BluetoothAdapter> def_adapter = manager->get_default_adapter();
    if(evt_type == BluetoothEventType::ADDED){
        /*if default adapter is null, set it to default adapter*/
        if(def_adapter == nullptr){
            manager->set_default_adapter(*adapter);
        }
    }else{
        /*remove default adapter*/
        if(def_adapter != nullptr && adapter->get_address() == def_adapter->get_address()){
            std::unique_ptr<BluetoothAdapter> null_adapter;
            manager->set_default_adapter(null_adapter);
        }
    }
}


void listen_device_callback(BluetoothObject &object, BluetoothEventType evt_type, void *data)
{
    BluetoothDevice *device = dynamic_cast<BluetoothDevice *>(&object);
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

    manager->init_custom_agent();

    manager->register_custom_agent();

    manager->listen(BluetoothType::ADAPTER,nullptr,nullptr,nullptr, listen_adapter_callback,nullptr,false, std::chrono::milliseconds::zero());

    manager->listen(BluetoothType::DEVICE,nullptr,nullptr,nullptr, listen_device_callback,nullptr,false, std::chrono::milliseconds::zero());

    return 0;
}

/***********************************************************************cli related functions*************************************************************************************/

static bool string2bool (const std::string & v)
{
    return !v.empty () &&
        (strcasecmp (v.c_str (), "true") == 0 ||
         atoi (v.c_str ()) != 0);
}


static void _show_adapters(std::ostream &out){
    BluetoothManager *manager = BluetoothManager::get_bluetooth_manager();

    std::unique_ptr<BluetoothAdapter> def_adapter = manager->get_default_adapter();

    if(def_adapter == nullptr){
        out << "no default adapter selected\n";
        return;
    }
    out << "Address:" << def_adapter->get_address() <<"\n";
    out << "Name:" << def_adapter->get_name() <<"\n";
    out << "Alias:" << def_adapter->get_alias() <<"\n";
    out << "Powered:" << def_adapter->get_powered() <<"\n";
    out << "Discoverable:" << def_adapter->get_discoverable() <<"\n";
    out << "Pairable:" << def_adapter->get_pairable() <<"\n";
    out << "----------------------------------------\n";

}

static void _set_adapter_prop(std::ostream &out, const std::string &prop, const std::string &val){ 

    BluetoothManager *manager = BluetoothManager::get_bluetooth_manager();

    std::unique_ptr<BluetoothAdapter> def_adapter = manager->get_default_adapter();

    if(def_adapter == nullptr){
        out << "no default adapter selected\n";
        return;
    }

    if(prop == std::string("alias")){
        def_adapter->set_alias(val);
    }
    else if (prop == std::string("powered")){
        def_adapter->set_powered(string2bool(val));
        /*we should wait a sec for adapter init*/
    }
    else if (prop == std::string("discoverable")){
        def_adapter->set_discoverable(string2bool(val));
    }
    else if (prop == std::string("pairable")){
        def_adapter->set_pairable(string2bool(val));
    }else{
        out << "unsupported property:" << prop <<"\n";
    }
}

static void _start_discovery(std::ostream &out){
    BluetoothManager *manager = BluetoothManager::get_bluetooth_manager();
    if(manager->get_default_adapter() == nullptr){
        out << "no default adapter selected\n";
        return;
    }
    manager->start_discovery();
}

static void _stop_discovery(std::ostream &out){
    BluetoothManager *manager = BluetoothManager::get_bluetooth_manager();
    if(manager->get_default_adapter() == nullptr){
        out << "no default adapter selected\n";
        return;
    }
    manager->stop_discovery();
}

static void _print_device(std::ostream &out, std::unique_ptr<BluetoothDevice> & device){
    out << "[" << device->get_address() <<"]" << "\t" << device->get_alias() << "\n";
}

static void _list_devices(std::ostream &out){
    BluetoothManager *manager = BluetoothManager::get_bluetooth_manager();

    std::vector<std::unique_ptr<BluetoothDevice>> devices = manager->get_devices();
    for(unsigned int i = 0; i< devices.size(); i++){
        _print_device(out, devices[i]);
    }
}

static void _pair_devices(std::ostream &out, const std::string & address){
    BluetoothManager *manager = BluetoothManager::get_bluetooth_manager();
    std::string mac_addr(address);
    std::unique_ptr<BluetoothObject> obj =  manager->get_object(BluetoothType::DEVICE,nullptr, &mac_addr, nullptr);

    if(obj == nullptr) return;

    BluetoothDevice *t = dynamic_cast<BluetoothDevice *>(obj.release());

    std::unique_ptr<BluetoothDevice> device(t);

    device->pair();
}

static void _connect_devices(std::ostream &out, const std::string & address){
    BluetoothManager *manager = BluetoothManager::get_bluetooth_manager();
    std::string mac_addr(address);
    std::unique_ptr<BluetoothObject> obj =  manager->get_object(BluetoothType::DEVICE,nullptr, &mac_addr, nullptr);

    if(obj == nullptr) return;

    BluetoothDevice *t = dynamic_cast<BluetoothDevice *>(obj.release());

    std::unique_ptr<BluetoothDevice> device(t);

    device->connect();
}


int main(int argc, char *argv[]){

    /*
    * init tiny bluetooth
    */
    if(tiny_bt_client_init() < 0){
        return -1;
    }

    // setup cli
    cli::SetColor();

    auto rootMenu = make_unique< Menu >( "bt_cli" );

    auto subMenu = make_unique< Menu >( "adapter" );

    subMenu -> Insert(
            "show",
            [](std::ostream& out){ _show_adapters(out); },
            "show property of default adapter" );

    subMenu -> Insert(
            "set",
            [](std::ostream& out, const std::string &prop, const std::string &val){ _set_adapter_prop(out, prop,val); },
            "set property of default adapter" );
    subMenu -> Insert(
            "start_discovery",
            [](std::ostream& out){ _start_discovery(out); },
            "start to discovery devices" );
    subMenu -> Insert(
            "stop_discovery",
            [](std::ostream& out){ _stop_discovery(out); },
            "stop discovery" );

    rootMenu -> Insert( std::move(subMenu) );

    subMenu = make_unique< Menu >( "device" );

    subMenu -> Insert(
            "list",
            [](std::ostream& out){ _list_devices(out); },
            "list all the devices" );
    
    subMenu -> Insert(
            "pair",
            [](std::ostream& out, const std::string & address){ _pair_devices(out, address); },
            "pair the device" );

    subMenu -> Insert(
            "connect",
            [](std::ostream& out, const std::string & address){ _connect_devices(out, address); },
            "connect to the device" );

    rootMenu -> Insert( std::move(subMenu) );

    Cli cli( std::move(rootMenu) );
    // global exit action
    cli.ExitAction( [](auto& out){ out << "Goodbye.\n"; } );

    CliFileSession input(cli);
    input.Start();

    return 0;
}
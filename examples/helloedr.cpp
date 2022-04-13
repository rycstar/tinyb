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

#include <tinyb.hpp>

#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
#include <csignal>

using namespace tinyb;

/** Converts a raw temperature read from the sensor to a Celsius value.
 * @param[in] raw_temp The temperature read from the sensor (two bytes)
 * @return The Celsius value of the temperature
 */
static float celsius_temp(uint16_t raw_temp)
{
    const float SCALE_LSB = 0.03125;
    return ((float)(raw_temp >> 2)) * SCALE_LSB;
}


std::atomic<bool> running(true);

void signal_handler(int signum)
{
    if (signum == SIGINT) {
        running = false;
    }
}

void print_devices(BluetoothManager * manager){
    std::cout << "Discovered devices: " << std::endl;
    /* Get the list of devices */
    auto list = manager->get_devices();

    for (auto it = list.begin(); it != list.end(); ++it) {

        std::cout << "Class = " << (*it)->get_class_name() << " ";
        std::cout << "Address = " << (*it)->get_address() << " ";
        std::cout << "Alias = " << (*it)->get_alias() << " ";
        std::cout << "Connected = " << (*it)->get_connected() << " ";
        std::cout << std::endl;

    }
    /* If not, wait and try again */
    std::this_thread::sleep_for(std::chrono::seconds(4));
    std::cout << std::endl;
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


int main(int argc, char **argv)
{
    BluetoothManager *manager = nullptr;
    try {
        manager = BluetoothManager::get_bluetooth_manager();
    } catch(const std::runtime_error& e) {
        std::cerr << "Error while initializing libtinyb: " << e.what() << std::endl;
        exit(1);
    }

    std::unique_ptr<BluetoothAdapter> def_adapter =  manager->get_default_adapter();

    if(def_adapter == nullptr){
        def_adapter = manager->find<BluetoothAdapter>(nullptr, nullptr, nullptr, std::chrono::milliseconds::zero());
    }

    std::cout << "get default adapter :" << def_adapter.get() <<std::endl;

    manager->listen(BluetoothType::DEVICE,nullptr,nullptr,def_adapter.get(), listen_device_callback,nullptr,false, std::chrono::milliseconds::zero());

    //manager->set_default_adapter(def_adapter);

    def_adapter->set_powered(true);
    /*sleep 2s to wait the result*/

    std::this_thread::sleep_for(std::chrono::seconds(2));

    def_adapter->set_alias(std::string("terry.rong"));

    def_adapter->set_discoverable(true);
    
    std::cout << "set set_discoverable done!" <<std::endl;

    //manager->start_discovery();

    /*sleep 2s to wait the result*/
    std::this_thread::sleep_for(std::chrono::seconds(1000));

    for (int i = 0; i < 3; ++i) {
        //print_devices(manager);
    }


    /* Stop the discovery (the device was found or number of tries ran out */
    //manager->stop_discovery();

    def_adapter->set_powered(false);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}

#ifndef __TINY_BT_C_H__
#define __TINY_BT_C_H__
#ifdef __cplusplus
extern "C"{
#endif

typedef void(* tiny_bt_evt_cb)(const char *, const char * ,void* priv);
typedef void(* tiny_bt_prop_cb)(const char *, const char * ,void* priv);

typedef enum {
    E_ADAPTER_NAME = 0, 
    E_ADAPTER_ALIAS,
    E_ADAPTER_CLASS,
    E_ADAPTER_POWERED,
    E_ADAPTER_DISCOVERABLE,
    E_ADAPTER_DISCOVERABLE_TIMEOUT,
    E_ADAPTER_PAIRABLE,
    E_ADAPTER_PAIRABLE_TIMEOUT,
    E_ADAPTER_INVALID,
}eAdapterProps;
#define ADAPTER_PROP_NUM E_ADAPTER_INVALID

typedef enum {
    E_DEVICE_NAME = 0, 
    E_DEVICE_ALIAS, 
    E_DEVICE_ICON, 
    E_DEVICE_CLASS, 
    E_DEVICE_APPEARANCE, 
    E_DEVICE_PAIRED, 
    E_DEVICE_CONNECTED, 
    E_DEVICE_TRUSTED, 
    E_DEVICE_BLOCKED, 
    E_DEVICE_ADAPTER,
    E_DEVICE_LEGACY_PAIRING,
    E_DEVICE_RSSI,
    E_DEVICE_INVALID
}eDevicesProps;
#define DEVICE_PROP_NUM E_DEVICE_INVALID
/*
* init tiny bluetooth manager
* 
*/
int tiny_bt_client_init(void);

/*
* register event listener for interface and objects
* for example : adapter insert/removed, device connected/disconnected
*/
int register_tiny_bt_event_handler(tiny_bt_evt_cb handler);


/*
* register property listener for adapter and device
*/
int register_tiny_bt_prop_handler(tiny_bt_prop_cb handler);


/*
* discovery Start/Stop
*/

void tiny_bt_client_discovery(int start);

/*
* get adapter prop
*/


/*
* get device prop
*/

/*
* set adapter prop
*/

#ifdef __cplusplus
}
#endif
#endif
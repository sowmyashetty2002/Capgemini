#include <stdio.h>
#include <string.h>
#include "blecommon.h"
#include "blestack.h"
#include "bleconfig.h"
#include "datatypes.h"
#include "log.h"
#include "gatt.h"
#include "gap.h"
#include "list.h"

#define MAX_DEV 20
BOOL op_started = FALSE;
U8 idx;

typedef struct{
	BDAddr address;
	U8 short_name[10];
	U8 complete_name[20];
	U16 appearance;
	U16 conn_handle;
}RemDevInfo;


RemDevInfo devices[MAX_DEV];

/*void set_scan_response()
{
	U8 data_len = 0;
	U8 data[MAX_ADV_SCAN_RESP_DATA_LENGTH];
	data[] = "sowm";
	if((GAP_SetAdvScanRespData(TRUE, data, data_len, gap_cb)) != BLE_STATUS_PENDING)
	{
		printf("failed to set advertisement data\n");
	}
	else
	{
		printf("setting advertisement data\n");
	}

}
*/


void gap_cb(GAPEvent *evt)
{
	switch(evt->event_code)
	{
		case GAP_ADV_ENABLED_EVT:
			if(evt->status == GAP_EVENT_SUCCESS)
			{
				printf("device entered into advertising mode\n");
			}
			break;
		case GAP_ADV_DISABLED_EVT:
			if(evt->status == GAP_EVENT_SUCCESS)
			{
				printf("advertisement disabled\n");
			}
			break;
		case GAP_ADV_DATA_SET_EVT:
			if(evt->status == GAP_EVENT_SUCCESS)
			{
				printf("advertisement data set successfully\n");
			}
			break;
		default:
			printf("GAP Event type %02x not handled\n",evt->event_code);
			break;
	}
}








void set_scan_response()
{
	//U8 scan_resp_data[] = {
	//	's','o','w',
//	};

//	U8 scan_resp_length = sizeof(scan_resp_data);


	typedef struct{

		U8 complete_name[10];
		U8 sho_rt_name[10];

	}name;

	name scan_resp;
	scan_resp.complete_name[10] = "sowmya";
	scan_resp.sho_rt_name[10] = "shetty";

	U8 scan_resp_length = sizeof(scan_resp);


	if((GAP_SetAdvScanRespData(TRUE, &scan_resp, scan_resp_length, gap_cb)) != BLE_STATUS_PENDING)
	{
		printf("failed to set advertisement data\n");
	}
	else
	{
		printf("setting advertisement data\n");
	}

}


void set_scan_resp_data()
{
	BleStatus st;
	U8 data[10] = AD_TYPE_COMPLETE_NAME;
	strcpy((char*)data,"sowmya");
	U8 data_len = strlen("sowmya");

	if((GAP_SetAdvScanRespData(TRUE, data, data_len, gap_cb)) != BLE_STATUS_PENDING)
	{
		printf("failed to set advertisement data\n");
	}
	else
	{
		printf("setting advertisement data\n");
	}


}


void handle_start_adv()
{

	printf("inside advertisemnt.........................................\n");
	GAPAdvParam adv_param;

	adv_param.chan_map = 0x07;

	adv_param.adv_type = GAP_ADV_IND;
	adv_param.remote_addr.addr_type = 0x00;

	adv_param.adv_interval.min = 0x00a1;
	adv_param.adv_interval.max = 0x0800;


	if((GAP_StartAdv(&adv_param,gap_cb)) != BLE_STATUS_PENDING)
	{
		printf("failed to start Advertisements\n");
	}
	else
	{
		printf("enabling advertisement\n");
	}
}




void global_cb(GAPEvent *evt)
{
	int i;
	switch(evt->event_code)
	{
		case GAP_LE_CONN_COMP_EVT:
			if(evt->status  != GAP_EVENT_SUCCESS)
			{
				printf("LE connection event is failed\n");
			}
			else
			{
				printf("\n connection established with the device : ");
				for(i = 0; i < 6; i++)
				{
					printf("%02x",evt->param.conn_info.addr.addr[i]);
					if(i != 5)
					{
						printf(":");
					}
				}
			}


		case GAP_DISCON_COMP_EVT:
			printf("\nDisconnection status = %02x, reason = %02x\n", evt->status,evt->param.discon_info.reason);
			if(evt->status != GAP_EVENT_SUCCESS)
			{
				printf("discon failed\n");
				return;
			}
		
		case GAP_ENCRYPT_CHG_EVT:

		case GAP_ENCRYPT_KEY_REFRESH_COMP_EVT:
			if(evt->status != BLE_STATUS_SUCCESS)
			{
				printf("failed to encrypt the link\n");
			}
			else
			{
				printf("successfully encrypted the link\n");
			}

		case GAP_PAIR_COMP_EVT:
			if(evt->status == BLE_STATUS_SUCCESS)
			{
				printf("successfully paired with the device:");
				for(idx = 0; idx <6; idx++)
				{
					printf("%02x ",evt->param.addr.addr[idx]);
				}
			}
			else if(evt->status == BLE_STATUS_FAILED)
			{
				printf("pairing failed with reason %02x for device:",evt->param.pair_fail.reason);
				for(idx = 0; idx <6; idx++)
				{
					printf("%02x ",evt->param.pair_fail.addr.addr[idx]);
				}
			}
			else if(evt->status == BLE_STATUS_TIMEOUT)
			{
				printf("pairing failed due to timeout with the device:");
				for(idx = 0; idx <6; idx++)
				{
					printf("%02x ", evt->param.pair_fail.addr.addr[idx]);
				}
			}
			else
			{
				printf("PAIR : unknown status\n");
				for(idx = 0; idx <6; idx++)
				{
					printf("%02x ",evt->param.addr.addr[idx]);
				}
			}
			printf("\n");
			break;

		default:
			break;
	}
}

int main()
{
	int n=5;
	int idx;
	BleStatus status;
	BleStatus gap_status;
	TransUSB usb;
	HCITransType type = TYPE_USB;

	//type = TYPE_USB;
	usb.bus_num = 001;
	usb.dev_id = 005;
	
	DBG_LogEnable(TRUE);

	if(type == 0)
	{
		status = BLE_StackInit(type, &usb);
	}
	if(status == BLE_STATUS_SUCCESS)
	{
		for(idx = 0;idx < MAX_DEV; idx++)
		{
			devices[idx].conn_handle = 0xFFFF;
		}
	}
	else
	{
		printf("failed to initailise ble stack\n");
		return -1;
	}





/*	const char* name = "Sow";
	U8 nameLength = strlen(name);

	BleStatus st = GAP_SetLocalName((U8*)name, nameLength);

	if(st == BLE_STATUS_SUCCESS)
	{
		printf("name set successfully\n");
	}
	else
	{
		printf("not");
	}

*/
	sleep(5);


	status = GAP_RegisterGlobalCallback(global_cb);
	if(status != BLE_STATUS_SUCCESS)
	{
		printf("register global cb failed\n");
	}
	sleep(5);


//
//	 set_scan_response();



	printf ("Starting advertisement\n");	
	handle_start_adv();
	while(1)
	{
		//gap_cb();
	//	gap_status = GAP_RegisterGlobalCallback(gap_cb);
	
	}

}

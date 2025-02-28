/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <stdio.h>
#include <nrf_modem.h>
#include <modem/nrf_modem_lib.h>
#include <modem/lte_lc.h>
#include <net/azure_iot_hub.h>
#include <net/azure_fota.h>
#include <zephyr/dfu/mcuboot.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/random/rand32.h>

static K_SEM_DEFINE(network_connected_sem, 0, 1);
static K_SEM_DEFINE(cloud_connected_sem, 0, 1);
static bool network_connected;
static struct k_work_delayable reboot_work;

BUILD_ASSERT(!IS_ENABLED(CONFIG_LTE_AUTO_INIT_AND_CONNECT),
			 "This sample does not support auto init and connect");

static void azure_event_handler(struct azure_iot_hub_evt *const evt)
{
	switch (evt->type) {
	case AZURE_IOT_HUB_EVT_CONNECTING:
		printk("AZURE_IOT_HUB_EVT_CONNECTING\n");
		break;
	case AZURE_IOT_HUB_EVT_CONNECTED:
		printk("AZURE_IOT_HUB_EVT_CONNECTED\n");
		break;
	case AZURE_IOT_HUB_EVT_CONNECTION_FAILED:
		printk("AZURE_IOT_HUB_EVT_CONNECTION_FAILED\n");
		printk("Error code received from IoT Hub: %d\n",
		       evt->data.err);
		break;
	case AZURE_IOT_HUB_EVT_DISCONNECTED:
		printk("AZURE_IOT_HUB_EVT_DISCONNECTED\n");
		break;
	case AZURE_IOT_HUB_EVT_READY:
		printk("AZURE_IOT_HUB_EVT_READY\n");
		k_sem_give(&cloud_connected_sem);
		break;
	case AZURE_IOT_HUB_EVT_DATA_RECEIVED:
		printk("AZURE_IOT_HUB_EVT_DATA_RECEIVED\n");
		break;
	case AZURE_IOT_HUB_EVT_TWIN_RECEIVED:
		printk("AZURE_IOT_HUB_EVT_TWIN_RECEIVED\n");
		break;
	case AZURE_IOT_HUB_EVT_TWIN_DESIRED_RECEIVED:
		printk("AZURE_IOT_HUB_EVT_TWIN_DESIRED_RECEIVED\n");
		printf("Desired device property: %.*s\n",
		       evt->data.msg.payload.size,
		       evt->data.msg.payload.ptr);
		break;
	case AZURE_IOT_HUB_EVT_DIRECT_METHOD:
		printk("AZURE_IOT_HUB_EVT_DIRECT_METHOD\n");
		printk("Method name: %s\n", evt->data.method.name.ptr);
		printf("Payload: %.*s\n", evt->data.method.payload.size,
		       evt->data.method.payload.ptr);
		break;
	case AZURE_IOT_HUB_EVT_TWIN_RESULT_SUCCESS:
		printk("AZURE_IOT_HUB_EVT_TWIN_RESULT_SUCCESS, ID: %s\n",
		       evt->data.result.request_id.ptr);
		break;
	case AZURE_IOT_HUB_EVT_TWIN_RESULT_FAIL:
		printk("AZURE_IOT_HUB_EVT_TWIN_RESULT_FAIL, ID %s, status %d\n",
		       evt->data.result.request_id.ptr, evt->data.result.status);
		break;
	case AZURE_IOT_HUB_EVT_PUBACK:
		printk("AZURE_IOT_HUB_EVT_PUBACK\n");
		break;
	case AZURE_IOT_HUB_EVT_FOTA_START:
		printk("AZURE_IOT_HUB_EVT_FOTA_START\n");
		break;
	case AZURE_IOT_HUB_EVT_FOTA_DONE:
		printk("AZURE_IOT_HUB_EVT_FOTA_DONE\n");
		printk("The device will reboot in 5 seconds to apply update\n");
		k_work_schedule(&reboot_work, K_SECONDS(5));
		break;
	case AZURE_IOT_HUB_EVT_FOTA_ERASE_PENDING:
		printk("AZURE_IOT_HUB_EVT_FOTA_ERASE_PENDING\n");
		break;
	case AZURE_IOT_HUB_EVT_FOTA_ERASE_DONE:
		printk("AZURE_IOT_HUB_EVT_FOTA_ERASE_DONE\n");
		break;
	case AZURE_IOT_HUB_EVT_ERROR:
		printk("AZURE_IOT_HUB_EVT_ERROR\n");
		break;
	default:
		printk("Unknown Azure IoT Hub event type: %d\n", evt->type);
		break;
	}
}

static void reboot_work_fn(struct k_work *work)
{
	ARG_UNUSED(work);

	sys_reboot(SYS_REBOOT_COLD);
}

static void lte_handler(const struct lte_lc_evt *const evt)
{
	switch (evt->type) {
	case LTE_LC_EVT_NW_REG_STATUS:
		if ((evt->nw_reg_status != LTE_LC_NW_REG_REGISTERED_HOME) &&
		    (evt->nw_reg_status != LTE_LC_NW_REG_REGISTERED_ROAMING)) {
			if (!network_connected) {
				break;
			}

			network_connected = false;

			printk("LTE network is disconnected.\n");
			printk("Subsequent sending data may block or fail.\n");
			break;
		}

		printk("Network registration status: %s\n",
		       evt->nw_reg_status == LTE_LC_NW_REG_REGISTERED_HOME ?
		       "Connected - home network" : "Connected - roaming");
		k_sem_give(&network_connected_sem);
		network_connected = true;
		break;
	case LTE_LC_EVT_PSM_UPDATE:
		printk("PSM parameter update: TAU: %d, Active time: %d\n",
		       evt->psm_cfg.tau, evt->psm_cfg.active_time);
		break;
	case LTE_LC_EVT_EDRX_UPDATE: {
		char log_buf[60];
		ssize_t len;

		len = snprintf(log_buf, sizeof(log_buf),
			       "eDRX parameter update: eDRX: %f, PTW: %f",
			       evt->edrx_cfg.edrx, evt->edrx_cfg.ptw);
		if (len > 0) {
			printk("%s\n", log_buf);
		}
		break;
	}
	case LTE_LC_EVT_RRC_UPDATE:
		printk("RRC mode: %s\n",
		       evt->rrc_mode == LTE_LC_RRC_MODE_CONNECTED ?
		       "Connected" : "Idle");
		break;
	case LTE_LC_EVT_CELL_UPDATE:
		printk("LTE cell changed: Cell ID: %d, Tracking area: %d\n",
		       evt->cell.id, evt->cell.tac);
		break;
	default:
		break;
	}
}

static void modem_configure(void)
{
	int err;

	err = lte_lc_init();
	if (err) {
		printk("Failed initializing the link controller, error: %d\n", err);
		return;
	}

	/* Explicitly disable PSM so that don't go to sleep after the initial connection
	 * establishment. Needed to be able to pick up new FOTA jobs more quickly.
	 */
	err = lte_lc_psm_req(false);
	if (err) {
		printk("Failed disabling PSM, error: %d\n", err);
		return;
	}

	err = lte_lc_connect_async(lte_handler);
	if (err) {
		printk("Modem could not be configured, error: %d\n", err);
		return;
	}
}

void main(void)
{
	int err;

	printk("Azure FOTA sample started\n");
	printk("This may take a while if a modem firmware update is pending\n");

	err = nrf_modem_lib_get_init_ret();
	switch (err) {
	case MODEM_DFU_RESULT_OK:
		printk("Modem firmware update successful!\n");
		printk("Modem will run the new firmware after reboot\n");
		k_thread_suspend(k_current_get());
		break;
	case MODEM_DFU_RESULT_UUID_ERROR:
	case MODEM_DFU_RESULT_AUTH_ERROR:
		printk("Modem firmware update failed\n");
		printk("Modem will run non-updated firmware on reboot.\n");
		break;
	case MODEM_DFU_RESULT_HARDWARE_ERROR:
	case MODEM_DFU_RESULT_INTERNAL_ERROR:
		printk("Modem firmware update failed\n");
		printk("Fatal error.\n");
		break;
	case -1:
		printk("Could not initialize modem library.\n");
		printk("Fatal error.\n");
		return;
	default:
		break;
	}

	k_work_init_delayable(&reboot_work, reboot_work_fn);

	err = azure_iot_hub_init(azure_event_handler);
	if (err) {
		printk("Azure IoT Hub could not be initialized, error: %d\n",
		       err);
		return;
	}

	printk("Connecting to LTE network\n");
	modem_configure();
	k_sem_take(&network_connected_sem, K_FOREVER);

	err = azure_iot_hub_connect(NULL);
	if (err < 0) {
		printk("azure_iot_hub_connect failed: %d\n", err);
		return;
	}

	k_sem_take(&cloud_connected_sem, K_FOREVER);

	/* All initializations and cloud connection were successful, now mark
	 * image as working so that we will not revert upon reboot.
	 */
	boot_write_img_confirmed();
}

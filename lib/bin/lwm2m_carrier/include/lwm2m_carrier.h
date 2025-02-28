/*
 * Copyright (c) 2019-2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file lwm2m_carrier.h
 *
 * @defgroup lwm2m_carrier_api LwM2M carrier library API
 * @{
 */

#ifndef LWM2M_CARRIER_H__
#define LWM2M_CARRIER_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LwM2M carrier library events.
 * @{
 */
/** Carrier library initialized. */
#define LWM2M_CARRIER_EVENT_INIT	  1
/** Request connect to the LTE network. */
#define LWM2M_CARRIER_EVENT_LTE_LINK_UP	  2
/**
 * Request disconnect from the LTE network.
 * The link must be offline until a subsequent LWM2M_CARRIER_EVENT_LTE_LINK_UP event.
 */
#define LWM2M_CARRIER_EVENT_LTE_LINK_DOWN 3
/** Request power off LTE network. */
#define LWM2M_CARRIER_EVENT_LTE_POWER_OFF 4
/** LwM2M carrier bootstrapped. */
#define LWM2M_CARRIER_EVENT_BOOTSTRAPPED  6
/** LwM2M carrier registered. */
#define LWM2M_CARRIER_EVENT_REGISTERED	  7
/** LwM2M carrier operation is deferred. */
#define LWM2M_CARRIER_EVENT_DEFERRED	  8
/** Firmware update started. */
#define LWM2M_CARRIER_EVENT_FOTA_START	  9
/** Application will reboot. */
#define LWM2M_CARRIER_EVENT_REBOOT	  10
/** Modem domain event received. */
#define LWM2M_CARRIER_EVENT_MODEM_DOMAIN  12
/** Data received through the App Data Container object */
#define LWM2M_CARRIER_EVENT_APP_DATA      13
/** An error occurred. */
#define LWM2M_CARRIER_EVENT_ERROR	  20
/** @} */

/**
 * @name LwM2M carrier library firmware update types.
 * @{
 */
/** Receiving a modem firmware delta patch. */
#define LWM2M_CARRIER_FOTA_START_MODEM_DELTA 0
/** Receiving application firmware. */
#define LWM2M_CARRIER_FOTA_START_APPLICATION 2
/** @} */

/**
 * @brief LwM2M carrier library firmware update event structure.
 */
typedef struct {
	/** Firmware type to be received. */
	uint32_t type;
	/** URI from where the firmware will be downloaded. Set to NULL if no URI will be used. */
	const char *uri;
} lwm2m_carrier_event_fota_start_t;

/**
 * @name LwM2M carrier library modem domain event types.
 * @{
 */
/** Mobile Equipment (ME) is overheated and therefore the modem is deactivated. */
#define LWM2M_CARRIER_MODEM_EVENT_ME_OVERHEATED  0
/** Mobile Equipment (ME) battery voltage is low and therefore the modem is deactivated. */
#define LWM2M_CARRIER_MODEM_EVENT_ME_BATTERY_LOW 1
/** Modem has detected a reset loop and will restrict Attach attempts for the next 30 minutes. */
#define LWM2M_CARRIER_MODEM_EVENT_RESET_LOOP	 2
/** @} */

/**
 * @brief LwM2M carrier library modem domain event type value.
 */
typedef uint32_t lwm2m_carrier_event_modem_domain_t;

/**
 * @brief LwM2M carrier library app data event structure.
 */
typedef struct {
	/** Buffer containing received application data. */
	const uint8_t *buffer;
	/** Number of bytes received. */
	size_t buffer_len;
} lwm2m_carrier_event_app_data_t;

/**
 * @name LwM2M carrier library event deferred reasons.
 * @{
 */
/** No reason given. */
#define LWM2M_CARRIER_DEFERRED_NO_REASON	   0
/** Failed to activate PDN. */
#define LWM2M_CARRIER_DEFERRED_PDN_ACTIVATE	   1
/** No route to bootstrap server. */
#define LWM2M_CARRIER_DEFERRED_BOOTSTRAP_NO_ROUTE  2
/** Failed to connect to bootstrap server. */
#define LWM2M_CARRIER_DEFERRED_BOOTSTRAP_CONNECT   3
/** Bootstrap sequence not completed. */
#define LWM2M_CARRIER_DEFERRED_BOOTSTRAP_SEQUENCE  4
/** No route to server. */
#define LWM2M_CARRIER_DEFERRED_SERVER_NO_ROUTE	   5
/** Failed to connect to server. */
#define LWM2M_CARRIER_DEFERRED_SERVER_CONNECT	   6
/** Server registration sequence not completed. */
#define LWM2M_CARRIER_DEFERRED_SERVER_REGISTRATION 7
/** Server in maintenance mode. */
#define LWM2M_CARRIER_DEFERRED_SERVICE_UNAVAILABLE 8
/** Waiting for SIM MSISDN. */
#define LWM2M_CARRIER_DEFERRED_SIM_MSISDN	   9
/** @} */

/**
 * @brief LwM2M carrier library deferred event structure.
 */
typedef struct {
	/** Deferred event reason. */
	uint32_t reason;
	/** Time before operation is resumed (seconds). */
	int32_t timeout;
} lwm2m_carrier_event_deferred_t;

/**
 * @name LwM2M carrier library event error types.
 * @{
 */
/** No error. */
#define LWM2M_CARRIER_ERROR_NO_ERROR		0
/** Failed to connect to the LTE network. */
#define LWM2M_CARRIER_ERROR_LTE_LINK_UP_FAIL	1
/** Failed to disconnect from the LTE network. */
#define LWM2M_CARRIER_ERROR_LTE_LINK_DOWN_FAIL	2
/** LwM2M carrier bootstrap failed. */
#define LWM2M_CARRIER_ERROR_BOOTSTRAP		3
/** Update package rejected. */
#define LWM2M_CARRIER_ERROR_FOTA_PKG		4
/** Protocol error. */
#define LWM2M_CARRIER_ERROR_FOTA_PROTO		5
/** Connection error. */
#define LWM2M_CARRIER_ERROR_FOTA_CONN		6
/** Connection lost. */
#define LWM2M_CARRIER_ERROR_FOTA_CONN_LOST	7
/** Update failed. */
#define LWM2M_CARRIER_ERROR_FOTA_FAIL		8
/** Illegal object configuration detected. */
#define LWM2M_CARRIER_ERROR_CONFIGURATION	9
/** LwM2M carrier init failed. */
#define LWM2M_CARRIER_ERROR_INIT		10
/** Internal error detected. */
#define LWM2M_CARRIER_ERROR_INTERNAL		11
/** @} */

/**
 * @brief LwM2M carrier library error event structure.
 */
typedef struct {
	/** Error event type. */
	uint32_t type;
	/** Error event value. */
	int32_t value;
} lwm2m_carrier_event_error_t;

/**
 * @brief LwM2M carrier library event structure.
 */
typedef struct {
	/** Event type. */
	uint32_t type;
	/** Pointer to event data, according to event type. */
	union {
		/** @c LWM2M_CARRIER_EVENT_FOTA_START */
		lwm2m_carrier_event_fota_start_t *fota_start;
		/** @c LWM2M_CARRIER_EVENT_MODEM_DOMAIN */
		lwm2m_carrier_event_modem_domain_t *modem_domain;
		/** @c LWM2M_CARRIER_EVENT_APP_DATA */
		lwm2m_carrier_event_app_data_t *app_data;
		/** @c LWM2M_CARRIER_EVENT_DEFERRED */
		lwm2m_carrier_event_deferred_t *deferred;
		/** @c LWM2M_CARRIER_EVENT_ERROR */
		lwm2m_carrier_event_error_t *error;
		/** For all other event types, it will be set to NULL. */
	} data;
} lwm2m_carrier_event_t;

/**
 * @name LwM2M device power sources types.
 * @{
 */
#define LWM2M_CARRIER_POWER_SOURCE_DC		    0
#define LWM2M_CARRIER_POWER_SOURCE_INTERNAL_BATTERY 1
#define LWM2M_CARRIER_POWER_SOURCE_EXTERNAL_BATTERY 2
#define LWM2M_CARRIER_POWER_SOURCE_ETHERNET	    4
#define LWM2M_CARRIER_POWER_SOURCE_USB		    5
#define LWM2M_CARRIER_POWER_SOURCE_AC		    6
#define LWM2M_CARRIER_POWER_SOURCE_SOLAR	    7
/** @} */

/**
 * @name LwM2M device error codes.
 * @{
 */
#define LWM2M_CARRIER_ERROR_CODE_NO_ERROR		 0
#define LWM2M_CARRIER_ERROR_CODE_LOW_CHARGE		 1
#define LWM2M_CARRIER_ERROR_CODE_EXTERNAL_SUPPLY_OFF	 2
#define LWM2M_CARRIER_ERROR_CODE_GPS_FAILURE		 3
#define LWM2M_CARRIER_ERROR_CODE_LOW_SIGNAL		 4
#define LWM2M_CARRIER_ERROR_CODE_OUT_OF_MEMORY		 5
#define LWM2M_CARRIER_ERROR_CODE_SMS_FAILURE		 6
#define LWM2M_CARRIER_ERROR_CODE_IP_CONNECTIVITY_FAILURE 7
#define LWM2M_CARRIER_ERROR_CODE_PERIPHERAL_MALFUNCTION	 8
/** @} */

/**
 * @name LwM2M device battery status.
 * @{
 * @note These values are only valid for the LwM2M Device INTERNAL_BATTERY if present.
 */
#define LWM2M_CARRIER_BATTERY_STATUS_NORMAL	     0
#define LWM2M_CARRIER_BATTERY_STATUS_CHARGING	     1
#define LWM2M_CARRIER_BATTERY_STATUS_CHARGE_COMPLETE 2
#define LWM2M_CARRIER_BATTERY_STATUS_DAMAGED	     3
#define LWM2M_CARRIER_BATTERY_STATUS_LOW_BATTERY     4
#define LWM2M_CARRIER_BATTERY_STATUS_NOT_INSTALLED   5
#define LWM2M_CARRIER_BATTERY_STATUS_UNKNOWN	     6
/** @} */

/**
 * @name LwM2M portfolio identity types.
 * @{
 */
#define LWM2M_CARRIER_IDENTITY_ID	    0
#define LWM2M_CARRIER_IDENTITY_MANUFACTURER 1
#define LWM2M_CARRIER_IDENTITY_MODEL	    2
#define LWM2M_CARRIER_IDENTITY_SW_VERSION   3
/** @} */

/**
 * @brief Structure holding LwM2M carrier library initialization parameters.
 */
typedef struct {
	/** Connect to certification servers if true, connect to production servers otherwise. */
	bool certification_mode;
	/** Disable bootstrap from Smartcard mode when this is enabled by the carrier. */
	bool disable_bootstrap_from_smartcard;
	/** Denotes whether @c server_uri is an LwM2M Bootstrap-Server or an LwM2M Server. */
	bool is_bootstrap_server;
	/** Optional URI of the custom server. Null-terminated string of max 255 characters. */
	const char *server_uri;
	/** Default server lifetime (in seconds). Used only for an LwM2M Server. */
	int32_t server_lifetime;
	/** Default DTLS session idle timeout (in seconds). */
	int32_t session_idle_timeout;
	/** Optional Pre-Shared Key. Null-terminated string of max 64 hexadecimal digits. */
	const char *psk;
	/** Optional custom APN. Null-terminated string of max 63 characters. */
	const char *apn;
	/** Optional LwM2M device manufacturer name. Null-terminated string of max 32 characters. */
	const char *manufacturer;
	/** Optional LwM2M device model number. Null-terminated string of max 32 characters. */
	const char *model_number;
	/** Optional LwM2M device type. Null-terminated string of max 32 characters. */
	const char *device_type;
	/** Optional LwM2M device hardware version. Null-terminated string of max 32 characters. */
	const char *hardware_version;
	/** Optional LwM2M device software version. Null-terminated string of max 32 characters. */
	const char *software_version;
	/**
	 * LG U+ Service Code registered for this device. This field is only required for devices
	 * certifying with LG U+. Null-terminated string of at most 5 characters.
	 */
	const char *service_code;
} lwm2m_carrier_config_t;

/**
 * @brief Initialize the LwM2M carrier library.
 *
 * @param[in] config Configuration parameters for the library. Optional.
 *
 * @note The library does not copy the contents of pointers in the config parameters. The
 *       application has to make sure that the provided parameters are valid throughout the
 *       application lifetime (i.e. placed in static memory or in flash).
 *
 * @note This function will block until a SIM card is initialized by the modem. The library cannot
 *       proceed if, for example, no SIM is inserted, or a PIN code must be entered.
 *
 * @note The first time this function is called after a modem firmware update (FOTA), it may block
 *       for several seconds in order to complete the procedure.
 *
 * @retval  0      If initialization was successful.
 * @retval -EINVAL If the configuration parameters are invalid. See @c lwm2m_carrier_config_t
 * @retval -EACCES If initialization failed due to an error in the OS integration layer.
 * @retval -ENOMEM If initialization failed due to insufficient OS resources.
 * @retval -EIO    If initialization failed due to a modem or AT command error.
 * @retval -EFAULT If initialization failed due to an internal error.
 */
int lwm2m_carrier_init(const lwm2m_carrier_config_t *config);

/**
 * @brief LwM2M carrier library main function.
 *
 * Intended to run on a separate thread. The function will exit only on non-recoverable errors.
 */
void lwm2m_carrier_run(void);

/**
 * @brief Function to read all time parameters.
 *
 * @details Input NULL for the parameters to ignore.
 *
 * @param[out] utc_time   Pointer to time since Epoch in seconds.
 * @param[out] utc_offset Pointer to UTC offset in minutes.
 * @param[out] tz         Pointer to null-terminated timezone string pointer.
 */
void lwm2m_carrier_time_read(int32_t *utc_time, int *utc_offset, const char **tz);

/**
 * @brief Function to read current UTC time
 *
 * @note This function can be implemented by the application, if custom time management is needed.
 *
 * @return  Current UTC time since Epoch in seconds.
 */
int32_t lwm2m_carrier_utc_time_read(void);

/**
 * @brief Function to read offset to UTC time.
 *
 * @note This function can be implemented by the application, if custom time management is needed.
 *
 * @return  UTC offset in minutes.
 */
int lwm2m_carrier_utc_offset_read(void);

/**
 * @brief Function to read timezone
 *
 * @note This function can be implemented by the application, if custom time management is needed.
 *
 * @return  Null-terminated timezone string pointer, IANA Timezone (TZ) database format.
 */
char *lwm2m_carrier_timezone_read(void);

/**
 * @brief Function to write current UTC time (LwM2M server write operation)
 *
 * @note This function can be implemented by the application, if custom time management is needed.
 *
 * @param[in] time Time since Epoch in seconds.
 *
 * @retval  0      If success.
 * @retval -EINVAL If value of time is less than 0.
 */
int lwm2m_carrier_utc_time_write(int32_t time);

/**
 * @brief Function to write UTC offset (LwM2M server write operation)
 *
 * @note This function can be implemented by the application, if custom time management is needed.
 *
 * @param[in] offset UTC offset in minutes.
 *
 * @retval  0      If success.
 */
int lwm2m_carrier_utc_offset_write(int offset);

/**
 * @brief Function to write timezone (LwM2M server write operation).
 *
 * @note This function can be implemented by the application, if custom time management is needed.
 *
 * @param[in] tz Null-terminated timezone string pointer.
 *
 * @retval 0      If success.
 */
int lwm2m_carrier_timezone_write(const char *tz);

/**
 * @brief LwM2M carrier library event handler.
 *
 * This function will be called by the LwM2M carrier library whenever some event significant for the
 * application occurs.
 *
 * @note This function has to be implemented by the application.
 *
 * @param[in] event LwM2M carrier event that occurred.
 *
 * @return  In case of @c LWM2M_CARRIER_EVENT_REBOOT events if non-zero is returned, the LwM2M
 *          carrier library will not reboot the device. The application should reboot at the
 *          earliest convenient time.
 */
int lwm2m_carrier_event_handler(const lwm2m_carrier_event_t *event);

/**
 * @brief Set the available power sources supported and used by the LwM2M device.
 *
 * @note It is necessary to call this function before any other device power source related
 *       functions listed in this file, as any updates of voltage/current measurements performed on
 *       power sources that have not been reported will be discarded.
 * @note Upon consecutive calls of this function, the corresponding current and voltage measurements
 *       will be reset to 0. Similarly, the battery status will be set to UNKNOWN and the battery
 *       level to 0%.
 *
 * @param[in]  power_sources      Array of available device power sources.
 * @param[in]  power_source_count Number of power sources currently used by the device.
 *
 * @retval  0      If the available power sources have been set successfully.
 * @retval -E2BIG  If the reported number of power sources is bigger than the maximum supported.
 * @retval -EINVAL If one or more of the power sources are not supported.
 */
int lwm2m_carrier_avail_power_sources_set(const uint8_t *power_sources, uint8_t power_source_count);

/**
 * @brief Set or update the latest voltage measurements made on one of the available device power
 *        sources.
 *
 * @note The voltage measurement needs to be specified in millivolts (mV) and is to be assigned to
 *       one of the available power sources.
 *
 * @param[in]  power_source Power source to which the measurement corresponds.
 * @param[in]  value        Voltage measurement expressed in mV.
 *
 * @retval  0      If the voltage measurements have been updated successfully.
 * @retval -EINVAL If the power source is not supported.
 * @retval -ENODEV If the power source is not listed as an available power source.
 */
int lwm2m_carrier_power_source_voltage_set(uint8_t power_source, int32_t value);

/**
 * @brief Set or update the latest current measurements made on one of the available device power
 *        sources.
 *
 * @note The current measurement needs to be specified in milliamperes (mA) and is to be assigned
 *       to one of the available power sources.
 *
 * @param[in]  power_source Power source to which the measurement corresponds.
 * @param[in]  value        Current measurement expressed in mA.
 *
 * @retval  0      If the current measurements have been updated successfully.
 * @retval -EINVAL If the power source is not supported.
 * @retval -ENODEV If the power source is not listed as an available power source.
 */
int lwm2m_carrier_power_source_current_set(uint8_t power_source, int32_t value);

/**
 * @brief Set or update the latest battery level (internal battery).
 *
 * @note The battery level is to be specified as a percentage, hence values outside the range 0-100
 *       will be ignored.
 *
 * @note The value is only valid for the Device internal battery if present.
 *
 * @param[in]  battery_level Internal battery level percentage.
 *
 * @retval  0      If the battery level has been updated successfully.
 * @retval -EINVAL If the specified battery level lies outside the 0-100% range.
 * @retval -ENODEV If internal battery is not listed as an available power source.
 */
int lwm2m_carrier_battery_level_set(uint8_t battery_level);

/**
 * @brief Set or update the latest battery status (internal battery).
 *
 * @note The value is only valid for the Device internal battery.
 *
 * @param[in]  battery_status Internal battery status to be reported.
 *
 * @retval  0      If the battery status has been updated successfully.
 * @retval -EINVAL If the battery status is not supported.
 * @retval -ENODEV If internal battery is not listed as an available power source.
 */
int lwm2m_carrier_battery_status_set(int32_t battery_status);

/**
 * @brief Update the device object instance error code by adding an individual error.
 *
 * @note Upon initialization of the device object instance, the error  code is specified as 0,
 *       indicating no error. The error code is to be updated whenever a new error occurs.
 * @note If the reported error is NO_ERROR, all existing error codes will be reset.
 * @note If the reported error is already present, the error code will remain unchanged.
 *
 * @param[in]  error Individual error to be added.
 *
 * @retval  0      If the error code has been added successfully.
 * @retval -EINVAL If the error code is not supported.
 */
int lwm2m_carrier_error_code_add(int32_t error);

/**
 * @brief Update the device object instance error code by removing an individual error.
 *
 * @note Upon initialization of the device object instance, the error code is specified as 0,
 *       indicating no error. The error code is to be updated whenever an error is no longer
 *       present. When all the errors are removed, the error code is specified as 0, hence
 *       indicating no error again.
 *
 * @param[in]  error Individual error code to be removed.
 *
 * @retval  0      If the error has been removed successfully.
 * @retval -EINVAL If the error code is not supported.
 * @retval -ENOENT If the error to be removed is not present on the error code list.
 */
int lwm2m_carrier_error_code_remove(int32_t error);

/**
 * @brief Set the total amount of storage space to store data and software in the LwM2M Device.
 *
 * @note The value is expressed in kilobytes (kB).
 *
 * @param[in]  memory_total Total amount of storage space in kilobytes.
 *
 * @retval  0      If the total amount of storage space has been set successfully.
 * @retval -EINVAL If the reported value is bigger than INT32_MAX.
 */
int lwm2m_carrier_memory_total_set(uint32_t memory_total);

/**
 * @brief Read the estimated current available amount of storage space to store data and software
 *        in the LwM2M Device.
 *
 * @note This function must be implemented by the application in order to support the reporting of
 *       memory free, otherwise the returned value will be 0.
 *
 * @return  Available amount of storage space expressed in kB.
 */
int lwm2m_carrier_memory_free_read(void);

/**
 * @brief Read the Identity field of a given Portfolio object instance.
 *
 * @note If the provided buffer is NULL, the function will perform a dry run to determine the
 *       required buffer size (including the null terminator).
 *
 * @param[in]     instance_id   Portfolio object instance identifier.
 * @param[in]     identity_type Type of Identity field to be read.
 * @param[inout]  buffer        Buffer where the null-terminated response will be stored.
 * @param[inout]  buffer_len    Length of the provided buffer. Will return the number of bytes of
 *                              the full response.
 *
 * @retval  0      If the operation was successful.
 * @retval -ENOENT If the instance does not exist.
 * @retval -EINVAL If the provided buffer length reference is NULL or the identity type is invalid.
 * @retval -ENOMEM If the provided buffer is too small.
 */
int lwm2m_carrier_identity_read(uint16_t instance_id, uint16_t identity_type, char *buffer,
				uint16_t *buffer_len);

/**
 * @brief Set the corresponding Identity field of a Portfolio object instance to a given value.
 *
 * @param[in]  instance_id   Portfolio object instance identifier.
 * @param[in]  identity_type Type of Identity field to be written.
 * @param[in]  value         Null terminated string to be written into the Identity field.
 *
 * @retval  0      If the Identity field has been updated successfully.
 * @retval -EPERM  If the specified object instance ID corresponds to the Primary Host identity.
 * @retval -EINVAL If the input argument is a NULL pointer or an empty string, or the identity type
 *                 is invalid.
 * @retval -ENOENT If the instance does not exist.
 * @retval -E2BIG  If the input string is too long.
 * @retval -ENOMEM If it was not possible to allocate memory storage to hold the string.
 */
int lwm2m_carrier_identity_write(uint16_t instance_id, uint16_t identity_type, const char *value);

/**
 * @brief Create a new instance of the Portfolio object.
 *
 * @param[in]  instance_id    The identifier to be used for the new instance.
 *
 * @retval  0      If the instance has been created successfully.
 * @retval -ENOMEM If it was not possible to create the instance because the maximum number of
 *                 supported object instances has already been reached.
 * @retval -EINVAL If the provided instance identifier is already in use.
 */
int lwm2m_carrier_portfolio_instance_create(uint16_t instance_id);

/**
 * @brief Set or update the latest location information provided by GPS.
 *
 * @param[in]  latitude    Latitude in degrees. Must be between -90.0 and 90.0.
 * @param[in]  longitude   Longitude in degrees. Must be between -180.0 and 180.0.
 * @param[in]  altitude    Altitude is meters over sea level.
 * @param[in]  timestamp   Unix timestamp of the current GPS measurement. Must not be older than
 *                         the previous timestamp passed to this function.
 * @param[in]  uncertainty Positioning uncertainty given as a radius in meters.
 *
 * @retval  0      If the location data has been updated successfully.
 * @retval -EINVAL If at least one input argument is incorrect.
 */
int lwm2m_carrier_location_set(double latitude, double longitude, float altitude,
			       uint32_t timestamp, float uncertainty);

/**
 * @brief Set or update the latest velocity information provided by GPS.
 *
 * @note Optional float arguments shall be set to NAN if they are not available to the user.
 *
 * @param[in]  heading       Horizontal direction of movement in degrees clockwise from North.
 *                           Valid range is 0 to 359.
 * @param[in]  speed_h       Horizontal speed in meters per second. Must be non-negative.
 * @param[in]  speed_v       Optional. Vertical speed in meters per second. Positive value indicates
 *                           upward motion. Negative value indicates downward motion.
 * @param[in]  uncertainty_h Optional. Horizontal uncertainty speed, i.e. maximal deviation from
 *                           the true speed, given in meters per second. Must be non-negative.
 * @param[in]  uncertainty_v Optional. Vertical uncertainty speed in meters per second.
 *                           Must be non-negative.
 *
 * @retval  0      If the velocity data has been updated successfully.
 * @retval -EINVAL If at least one input argument is incorrect.
 * @retval -ENOMEM If it was not possible to allocate memory storage to hold the velocity data.
 */
int lwm2m_carrier_velocity_set(int heading, float speed_h, float speed_v, float uncertainty_h,
			       float uncertainty_v);

/**
 * @brief Schedule application data to be sent using the App Data Container object.
 *
 * @details This function sets the "UL data" (uplink) resource of this object to the desired value,
 *          which can then be read by, or reported to, the LwM2M server.
 *
 * @note The App Data Container object will not be initialized for every carrier.
 *
 * @param[in]  buffer     Buffer containing the application data to be sent.
 * @param[in]  buffer_len Number of bytes in the buffer.
 *
 * @retval  0      If the resource has been set successfully.
 * @retval -ENOENT If the object is not yet initialized.
 * @retval -EINVAL If the buffer is NULL.
 * @retval -ENOMEM If there is not enough memory to copy the buffer contents to the resource model.
 */
int lwm2m_carrier_app_data_send(const uint8_t *buffer, size_t buffer_len);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* LWM2M_CARRIER_H__ */

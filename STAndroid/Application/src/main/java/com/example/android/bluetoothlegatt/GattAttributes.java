/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.android.bluetoothlegatt;

import android.text.TextUtils;

import java.util.HashMap;
import java.util.UUID;

/**
 * Gatt attributes store
 */
public class GattAttributes {
    private static HashMap<String, String> attributes = new HashMap();
    private static int[] ACC_SERVICE_UUID = { 0x02,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b };
    private static int[] ACC_UUID = { 0x34,0x0a,0x1b,0x80, 0xcf,0x4b, 0x11,0xe1, 0xac,0x36, 0x00,0x02,0xa5,0xd5,0xc5,0x1b };
    private static int[] ACC_TEMP_UUID = { 0xa3,0x2e,0x55,0x20, 0xe4,0x77, 0x11,0xe2, 0xa9,0xe3, 0x00,0x02,0xa5,0xd5,0xc5,0x1b };

    /************************** RELICS OF A BYGONE ERA ***************************************/
    public static String HEART_RATE_MEASUREMENT = "00002a37-0000-1000-8000-00805f9b34fb";
    public static String CLIENT_CHARACTERISTIC_CONFIG = "00002902-0000-1000-8000-00805f9b34fb";

    static {
        final String ACC_SERVICE_UUID_STRING = convertUUIDToString(ACC_SERVICE_UUID);
        final String ACC_UUID_STRING = convertUUIDToString(ACC_UUID);
        final String ACC_TEMP_UUID_STRING = convertUUIDToString(ACC_TEMP_UUID);

        attributes.put(ACC_SERVICE_UUID_STRING, "Acceleration Service");
        attributes.put(ACC_UUID_STRING, "Acceleration Characteristic");
        attributes.put(ACC_TEMP_UUID_STRING, "Temperature Characteristic");

        /************************** RELICS OF A BYGONE ERA ***************************************/
        // Sample Services.
        attributes.put("0000180d-0000-1000-8000-00805f9b34fb", "Heart Rate Service");
        attributes.put("0000180a-0000-1000-8000-00805f9b34fb", "Device Information Service");
        // Sample Characteristics.
        attributes.put(HEART_RATE_MEASUREMENT, "Heart Rate Measurement");
        attributes.put("00002a29-0000-1000-8000-00805f9b34fb", "Manufacturer Name String");

    }

    private static String convertUUIDToString(int[] components) {
        String[] hex_strings = new String[16];
        for (int i = 0; i < 16; i++) {
            // Bit of bit fiddling to make sure "2" is printed as "002"
            hex_strings[i] = Integer.toHexString(0x100 | components[i]).substring(1);
        }
        String hyphened_uuid = TextUtils.join("", hex_strings).replaceAll(
                "(\\w{8})(\\w{4})(\\w{4})(\\w{4})(\\w{12})",
                "$1-$2-$3-$4-$5");

        return UUID.fromString(hyphened_uuid).toString();
    }

    public static String lookup(String uuid, String defaultName) {
        String name = attributes.get(uuid);
        return name == null ? defaultName : name;
    }
}

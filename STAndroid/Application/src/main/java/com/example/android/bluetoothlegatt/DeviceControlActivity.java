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

import android.app.Activity;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.PowerManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TableLayout;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * For a given BLE device, this Activity provides the user interface to connect, display data,
 * and display GATT services and characteristics supported by the device.  The Activity
 * communicates with {@code BluetoothLeService}, which in turn interacts with the
 * Bluetooth LE API.
 */
public class DeviceControlActivity extends Activity {
    String accData = "0";
    String tempData = "0";
    String turnOn = "0";

    private PowerManager mPowerManager;
    private PowerManager.WakeLock mWakeLock;
    BluetoothGattCharacteristic ledCharacteristic = null;
    BluetoothGattCharacteristic accCharacteristic = null;
    BluetoothGattCharacteristic tempCharacteristic = null;
    Handler handlerAcc = new Handler();
    Handler handlerTemp = new Handler();
    Runnable runnableAcc = new Runnable() {
        public void run() {
            getBluetoothDataForAcc();
        }
    };
    Runnable runnableTemp = new Runnable() {
        public void run() {
            getBluetoothDataForTemp();
        }
    };
    private final static String TAG = DeviceControlActivity.class.getSimpleName();

    public static final String EXTRAS_DEVICE_NAME = "DEVICE_NAME";
    public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";


    // LED Options UI
    // UI elements
    private Switch enableLED;
    private RadioGroup radioButtons;
    private RadioButton allOn;
    private RadioButton clockwise;
    private SeekBar seekBar;
    private TextView brightness_text;
    private TableLayout table;

    private TextView mConnectionState;
    private TextView mTemp;
    private TextView mPitch;
    private TextView mRoll;
    private String mDeviceName;
    private String mDeviceAddress;
    private BluetoothLeService mBluetoothLeService;

    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics =
            new ArrayList<ArrayList<BluetoothGattCharacteristic>>();
    private boolean mConnected = false;
    private BluetoothGattCharacteristic mNotifyCharacteristic;

    private final String LIST_NAME = "NAME";
    private final String LIST_UUID = "UUID";

    // Code to manage Service lifecycle.
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
            // Automatically connects to the device upon successful start-up initialization.
            mBluetoothLeService.connect(mDeviceAddress);
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService = null;
        }
    };

    // Handles various events fired by the Service.
    // ACTION_GATT_CONNECTED: connected to a GATT server.
    // ACTION_GATT_DISCONNECTED: disconnected from a GATT server.
    // ACTION_GATT_SERVICES_DISCOVERED: discovered GATT services.
    // ACTION_DATA_AVAILABLE: received data from the device.  This can be a result of read
    //                        or notification operations.
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                mConnected = true;
                updateConnectionState(R.string.connected);
                invalidateOptionsMenu();
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                mConnected = false;
                updateConnectionState(R.string.disconnected);
                invalidateOptionsMenu();
                clearUI();
            } else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                // Show all the supported services and characteristics on the user interface.
                displayGattServices(mBluetoothLeService.getSupportedGattServices());
            } else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                accData = intent.getStringExtra(BluetoothLeService.ACC_DATA);
                tempData = intent.getStringExtra(BluetoothLeService.TEMP_DATA);
                turnOn = intent.getStringExtra(BluetoothLeService.TURN_ON);

                if (turnOn != null && !turnOn.equals("0")) {
                    intent.putExtra(BluetoothLeService.TURN_ON, "0");
                    turnOnScreen();
                }
                if (accData != null && !accData.equals("0")) {
                    intent.putExtra(BluetoothLeService.ACC_DATA, "0");
                    displayData(accData);
                }
                if (tempData != null && !tempData.equals("0")) {
                    intent.putExtra(BluetoothLeService.TEMP_DATA, "0");
                    displayData(tempData);
                }

            }
        }
    };

    public void turnOnScreen(){
        // turn on screen
        Log.v("ProximityActivity", "ON!");
        mWakeLock = mPowerManager.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK | PowerManager.ACQUIRE_CAUSES_WAKEUP, "tag");
        mWakeLock.acquire();
        mWakeLock.release();
    }

    private void clearUI() {
        setVisibility(View.INVISIBLE);
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.gatt_services_characteristics);

        mPowerManager = (PowerManager) this.getSystemService(Context.POWER_SERVICE);

        final Intent intent = getIntent();
        mDeviceName = intent.getStringExtra(EXTRAS_DEVICE_NAME);
        mDeviceAddress = intent.getStringExtra(EXTRAS_DEVICE_ADDRESS);

        // Sets up UI references.
        ((TextView) findViewById(R.id.device_address)).setText(mDeviceAddress);
        mConnectionState = (TextView) findViewById(R.id.connection_state);
        mTemp = (TextView) findViewById(R.id.temp_val);
        mPitch = (TextView) findViewById(R.id.pitch_val);
        mRoll = (TextView) findViewById(R.id.roll_val);

        enableLED = (Switch) findViewById(R.id.enable_led);
        table = (TableLayout) findViewById(R.id.table);
        radioButtons = (RadioGroup) findViewById(R.id.radioGroup);
        allOn = (RadioButton) findViewById(R.id.allOnRadio);
        clockwise = (RadioButton) findViewById(R.id.rotateClockRadio);
        seekBar = (SeekBar) findViewById(R.id.seekBar);
        // Set max value to make Yusaira happy
        seekBar.setMax(10);
        brightness_text = (TextView) findViewById(R.id.brightness_string);

        clearUI();

        // Set all off initially
        for(int i = 0; i < radioButtons.getChildCount(); i++){
            ((RadioButton)radioButtons.getChildAt(i)).setEnabled(false);
        }
        seekBar.setEnabled(false);

        final byte [] possibleValues = {0x0, 0x1, 0x2, 0x3};
        /**
         * 00 -> off
         * 01 -> clockwise
         * 10 -> anti
         * 11 -> all on
         */

        // Handle change in radio buttons
        radioButtons.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener()
        {
            public void onCheckedChanged(RadioGroup rGroup, int checkedId)
            {
                // This will get the radiobutton that has changed in its check state
                RadioButton checkedRadioButton = (RadioButton)rGroup.findViewById(checkedId);
                // This puts the value (true/false) into the variable
                boolean isChecked = checkedRadioButton.isChecked();
                // If the radiobutton that has changed in check state is now checked...
                if (isChecked)
                {
                    if (checkedRadioButton.equals(allOn)) {
                        seekBar.setEnabled(true);
                        // get value from seekbar
                        byte [] toSend = {possibleValues[3], (byte)seekBar.getProgress()};
                        ledCharacteristic.setValue(toSend);
                    } else {
                        seekBar.setEnabled(false);
                        if (checkedRadioButton.equals(clockwise)) {
                            byte [] toSend = {possibleValues[1], 0x0};
                            ledCharacteristic.setValue(toSend);
                        } else {
                            byte [] toSend = {possibleValues[2], 0x0};
                            ledCharacteristic.setValue(toSend);
                        }
                    }
                }
                mBluetoothLeService.writeCharacteristic(ledCharacteristic);
            }

        });

        // Handle change in LED button
        enableLED.setOnCheckedChangeListener(new Switch.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                // do something, the isChecked will be
                // true if the switch is in the On position
                for(int i = 0; i < radioButtons.getChildCount(); i++){
                    ((RadioButton)radioButtons.getChildAt(i)).setEnabled(isChecked);
                }
                if (findViewById(radioButtons.getCheckedRadioButtonId()).equals(allOn) && isChecked) {
                    seekBar.setEnabled(true);
                    // get value from seekbar
                    byte [] toSend = {possibleValues[3], (byte)seekBar.getProgress()};
                    ledCharacteristic.setValue(toSend);
                } else {
                    seekBar.setEnabled(false);
                    byte [] toSend = {possibleValues[0], 0x0};
                    ledCharacteristic.setValue(toSend);
                }
                // we can send data here
                mBluetoothLeService.writeCharacteristic(ledCharacteristic);
            }
        });


        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                RadioButton checkedRadioButton = (RadioButton)radioButtons
                        .findViewById(radioButtons.getCheckedRadioButtonId());
                if (fromUser && checkedRadioButton.equals(allOn)) {
                    byte [] toSend = {possibleValues[3], (byte)seekBar.getProgress()};
                    ledCharacteristic.setValue(toSend);
                }
                // we can send data here
                mBluetoothLeService.writeCharacteristic(ledCharacteristic);
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}

            public void onStopTrackingTouch(SeekBar seekBar) {}
        } );

        getActionBar().setTitle(mDeviceName);
        getActionBar().setDisplayHomeAsUpEnabled(true);
        Intent gattServiceIntent = new Intent(this, BluetoothLeService.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);

        runnableAcc.run();
        runnableTemp.run();
    }

    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        if (mBluetoothLeService != null) {
            final boolean result = mBluetoothLeService.connect(mDeviceAddress);
            Log.d(TAG, "Connect request result=" + result);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mGattUpdateReceiver);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unbindService(mServiceConnection);
        mBluetoothLeService = null;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.gatt_services, menu);
        if (mConnected) {
            menu.findItem(R.id.menu_connect).setVisible(false);
            menu.findItem(R.id.menu_disconnect).setVisible(true);
        } else {
            menu.findItem(R.id.menu_connect).setVisible(true);
            menu.findItem(R.id.menu_disconnect).setVisible(false);
        }
        return true;

    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch(item.getItemId()) {
            case R.id.menu_connect:
                mBluetoothLeService.connect(mDeviceAddress);
                return true;
            case R.id.menu_disconnect:
                mBluetoothLeService.disconnect();
                return true;
            case android.R.id.home:
                onBackPressed();
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private void updateConnectionState(final int resourceId) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {

                mConnectionState.setText(resourceId);

                // UI
                if (resourceId == R.string.connected) {
                    setVisibility(View.VISIBLE);
                }
                else {
                    setVisibility(View.INVISIBLE);
                }

            }
        });
    }

    private void setVisibility(final int visibility) {
        enableLED.setVisibility(visibility);
        table.setVisibility(visibility);
        radioButtons.setVisibility(visibility);
        seekBar.setVisibility(visibility);
        brightness_text.setVisibility(visibility);
    }

    private void displayData(String data) {
        if (data != null) {
            // If accelerometer data
            String[] accData = data.split(",");
            if (accData.length > 1) {
                mRoll.setText(accData[0]);
                mPitch.setText(accData[1]);
            }
            // else
            else mTemp.setText(data);
        }
    }

    // Demonstrates how to iterate through the supported GATT Services/Characteristics.
    // In this sample, we populate the data structure that is bound to the ExpandableListView
    // on the UI.
    private void displayGattServices(List<BluetoothGattService> gattServices) {

        if (gattServices == null) return;
        String uuid = null;
        String unknownServiceString = getResources().getString(R.string.unknown_service);
        String unknownCharaString = getResources().getString(R.string.unknown_characteristic);
        ArrayList<HashMap<String, String>> gattServiceData = new ArrayList<HashMap<String, String>>();
        ArrayList<ArrayList<HashMap<String, String>>> gattCharacteristicData
                = new ArrayList<ArrayList<HashMap<String, String>>>();
        mGattCharacteristics = new ArrayList<ArrayList<BluetoothGattCharacteristic>>();

        // Loops through available GATT Services.
        for (BluetoothGattService gattService : gattServices) {
            HashMap<String, String> currentServiceData = new HashMap<String, String>();
            uuid = gattService.getUuid().toString();
            String serviceString = GattAttributes.lookup(uuid, unknownServiceString);
            // If unknown then don't store
            if (serviceString.contains(unknownServiceString)) {
                continue;
            }
            currentServiceData.put(LIST_NAME, serviceString);
            currentServiceData.put(LIST_UUID, uuid);
            gattServiceData.add(currentServiceData);

            ArrayList<HashMap<String, String>> gattCharacteristicGroupData =
                    new ArrayList<HashMap<String, String>>();
            List<BluetoothGattCharacteristic> gattCharacteristics =
                    gattService.getCharacteristics();
            ArrayList<BluetoothGattCharacteristic> charas =
                    new ArrayList<BluetoothGattCharacteristic>();

            // Loops through available Characteristics.
            for (BluetoothGattCharacteristic gattCharacteristic : gattCharacteristics) {
                HashMap<String, String> currentCharaData = new HashMap<String, String>();
                uuid = gattCharacteristic.getUuid().toString();
                String charString = GattAttributes.lookup(uuid, unknownCharaString);
                // If unknown then don't store
                if (charString.contains(unknownCharaString)) {
                    continue;
                }
                if (charString.contains("LED")) {
                    ledCharacteristic = gattCharacteristic;
                    continue;
                }
                else if (charString.contains("Acceleration")) {
                    accCharacteristic = gattCharacteristic;
                    continue;
                }
                else if (charString.contains("Temperature")) {
                    tempCharacteristic = gattCharacteristic;
                    continue;
                } else if (charString.contains("Double Tap")) {
                    final int charaProp = gattCharacteristic.getProperties();
                    if ((charaProp | BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
                        mBluetoothLeService.setNotificationForDoubleTap(gattCharacteristic);

                    }
                    continue;
                }

                charas.add(gattCharacteristic);
                currentCharaData.put(
                        LIST_NAME, charString);
                currentCharaData.put(LIST_UUID, uuid);
                gattCharacteristicGroupData.add(currentCharaData);
            }
            mGattCharacteristics.add(charas);
            gattCharacteristicData.add(gattCharacteristicGroupData);
        }
    }

    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }

    public void getBluetoothDataForAcc() {
        if (accCharacteristic != null && checkIfBluetoothServiceExists() && checkIfConnected()) {
            final int charaProp = accCharacteristic.getProperties();
            if ((charaProp | BluetoothGattCharacteristic.PROPERTY_READ) > 0) {
                // If there is an active notification on a characteristic, clear
                // it first so it doesn't update the data field on the user interface.
                if (mNotifyCharacteristic != null) {
                    mBluetoothLeService.setCharacteristicNotification(
                            mNotifyCharacteristic, false);
                    mNotifyCharacteristic = null;
                }
                mBluetoothLeService.readCharacteristic(accCharacteristic);
            }
            if ((charaProp | BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
                mNotifyCharacteristic = accCharacteristic;
                mBluetoothLeService.setCharacteristicNotification(
                        accCharacteristic, true);
            }
        }
        handlerAcc.postDelayed(runnableAcc, 1000);
    }

    public void getBluetoothDataForTemp() {
        if (tempCharacteristic != null && checkIfBluetoothServiceExists() && checkIfConnected()) {
            final int charaProp = tempCharacteristic.getProperties();
            if ((charaProp | BluetoothGattCharacteristic.PROPERTY_READ) > 0) {
                // If there is an active notification on a characteristic, clear
                // it first so it doesn't update the data field on the user interface.
                if (mNotifyCharacteristic != null) {
                    mBluetoothLeService.setCharacteristicNotification(
                            mNotifyCharacteristic, false);
                    mNotifyCharacteristic = null;
                }
                mBluetoothLeService.readCharacteristic(tempCharacteristic);
            }
            if ((charaProp | BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
                mNotifyCharacteristic = tempCharacteristic;
                mBluetoothLeService.setCharacteristicNotification(
                        tempCharacteristic, true);
            }
        }

        handlerTemp.postDelayed(runnableTemp, 1050);
    }

    private boolean checkIfBluetoothServiceExists() {
        return mBluetoothLeService != null;
    }
    private boolean checkIfConnected() {
        return mBluetoothLeService.mConnectionState == mBluetoothLeService.STATE_CONNECTED;
    }
}



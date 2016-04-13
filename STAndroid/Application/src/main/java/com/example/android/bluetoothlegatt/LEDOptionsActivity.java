package com.example.android.bluetoothlegatt;

import android.app.Activity;
import android.bluetooth.BluetoothGattCharacteristic;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.provider.MediaStore;
import android.support.v4.app.ShareCompat;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.util.Log;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.SimpleExpandableListAdapter;
import android.widget.Switch;
import android.widget.TextView;

import java.util.ArrayList;

public class LEDOptionsActivity extends Activity {

    // UI elements
    private Switch enableLED;
    private RadioGroup radioButtons;
    private RadioButton allOn;
    private SeekBar seekBar;
    private TextView mConnectionState;
    private TextView brightness_text;

    public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";


    // BT LE
    private BluetoothLeService mBluetoothLeService;
    private ArrayList<BluetoothGattCharacteristic> mGattCharacteristics =
            new ArrayList<BluetoothGattCharacteristic>();
    private String mDeviceAddress;
    private boolean mConnected = false;


    // Code to manage Service lifecycle.
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                Log.e("LED Options", "Unable to initialize Bluetooth");
                finish();
            }
            // Automatically connects to the device upon successful start-up initialization.
            System.out.println("Trying to connect to device! " + mDeviceAddress);
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
            System.out.println("DEBUG: IN led options: WE RECEIVED SOMETHING FROM BT:  " + action);

            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                mConnected = true;
                updateConnectionState(R.string.connected);
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                System.out.println("DEBUG: DEBUG: DEBUG: DEBUG: Disconnected! " + action);
                mConnected = false;
                updateConnectionState(R.string.disconnected);
                clearUI();
            } else {
                System.out.println("DEBUG: DEBUG: DEBUG: DEBUG: ELSE! " + action);
            }

//            else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
//                // Show all the supported services and characteristics on the user interface.
//                displayGattServices(mBluetoothLeService.getSupportedGattServices());
//            } else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
//                displayData(intent.getStringExtra(BluetoothLeService.EXTRA_DATA));
//            }
        }
    };

    private void clearUI() {
        // Hide all
        enableLED.setVisibility(View.INVISIBLE);
        radioButtons.setVisibility(View.INVISIBLE);
        seekBar.setVisibility(View.INVISIBLE);
        brightness_text.setVisibility(View.INVISIBLE);
    }

    private void updateConnectionState(final int resourceId) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mConnectionState.setText(resourceId);

                // UI
                enableLED.setVisibility(View.VISIBLE);
                radioButtons.setVisibility(View.VISIBLE);
                seekBar.setVisibility(View.VISIBLE);
                brightness_text.setVisibility(View.VISIBLE);
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        if (mBluetoothLeService != null) {
            final boolean result = mBluetoothLeService.connect(mDeviceAddress);
            Log.d("LED Options", "Connect request result=" + result);
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

    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ledoptions);

        // UI
        mConnectionState = (TextView) findViewById(R.id.led_connection_state);
        enableLED = (Switch) findViewById(R.id.enable_led);
        radioButtons = (RadioGroup) findViewById(R.id.radioGroup);
        allOn = (RadioButton) findViewById(R.id.allOnRadio);
        seekBar = (SeekBar) findViewById(R.id.seekBar);
        brightness_text = (TextView) findViewById(R.id.brightness_string);

        final Intent intent = getIntent();
//        mDeviceName = intent.getStringExtra(EXTRAS_DEVICE_NAME);
        mDeviceAddress = intent.getStringExtra(EXTRAS_DEVICE_ADDRESS);
        System.out.println("mdeviceaddress is " + mDeviceAddress);

//        clearUI();

        // Set all off initially
        for(int i = 0; i < radioButtons.getChildCount(); i++){
            ((RadioButton)radioButtons.getChildAt(i)).setEnabled(false);
        }
        seekBar.setEnabled(false);

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
                    } else seekBar.setEnabled(false);
                }
            }
        });

        enableLED.setOnCheckedChangeListener(new Switch.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                // do something, the isChecked will be
                // true if the switch is in the On position
                for(int i = 0; i < radioButtons.getChildCount(); i++){
                    ((RadioButton)radioButtons.getChildAt(i)).setEnabled(isChecked);
                }
                if (findViewById(radioButtons.getCheckedRadioButtonId()).equals(allOn) && isChecked) {
                    seekBar.setEnabled(true);
                } else seekBar.setEnabled(false);
            }
        });

        Intent gattServiceIntent = new Intent(this, BluetoothLeService.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
        startService(gattServiceIntent);

    }

}

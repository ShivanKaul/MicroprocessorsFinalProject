package com.example.android.bluetoothlegatt;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.ShareCompat;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;

public class LEDOptionsActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ledoptions);
    }

    /**
     * Called when the activity has detected the user's presses the back key.
     */
    @Override
    public void onBackPressed() {
        Intent intent = new Intent(this, DeviceScanActivity.class);
        finish();
        startActivity(intent);
    }
}

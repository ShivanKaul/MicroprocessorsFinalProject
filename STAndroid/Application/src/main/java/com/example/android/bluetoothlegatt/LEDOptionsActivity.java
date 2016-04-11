package com.example.android.bluetoothlegatt;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.provider.MediaStore;
import android.support.v4.app.ShareCompat;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.Switch;

public class LEDOptionsActivity extends Activity {

    private Switch enableLED;
    private RadioGroup radioButtons;
    private RadioButton allOn;
//    private RadioButton allOn;
    private SeekBar seekBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ledoptions);

        enableLED = (Switch) findViewById(R.id.enable_led);
        radioButtons = (RadioGroup) findViewById(R.id.radioGroup);
        allOn = (RadioButton) findViewById(R.id.allOnRadio);
        seekBar = (SeekBar) findViewById(R.id.seekBar);

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

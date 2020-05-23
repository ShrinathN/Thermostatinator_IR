package com.shrinath.tinyaccon;

import androidx.appcompat.app.AppCompatActivity;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class SettingsActivity extends AppCompatActivity {


    private final String TAG = "Settings";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ip_selector_dialog_layout);

        final EditText dialog_edittext_ip = findViewById(R.id.dialog_edittext_ip);
        final EditText dialog_edittext_port = findViewById(R.id.dialog_edittext_port);
        Button button_set_ip = findViewById(R.id.button_set_ip);

        final SharedPreferences.Editor editor = getApplicationContext().getSharedPreferences("ADDRESS",MODE_PRIVATE).edit();

        button_set_ip.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                editor.putString("IP", dialog_edittext_ip.getText().toString());
                editor.putString("PORT", dialog_edittext_port.getText().toString());

                Log.d(TAG, "onClick: IP ->" + dialog_edittext_ip.getText().toString());
                Log.d(TAG, "onClick: PORT ->" + dialog_edittext_port.getText().toString());

                editor.commit();
                editor.apply();
            }
        });
    }
}

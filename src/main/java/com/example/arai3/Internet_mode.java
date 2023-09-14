package com.example.arai3;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.Toast;

import com.google.zxing.integration.android.IntentIntegrator;
import com.google.zxing.integration.android.IntentResult;


public class Internet_mode extends AppCompatActivity {
    EditText etcode;
    ImageButton qrbtn;
    Button showbtn;
    String evcode;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_internet_mode);

        etcode = (EditText) findViewById(R.id.etEV_code);
        qrbtn = (ImageButton) findViewById(R.id.qrBtn);
        showbtn = (Button) findViewById(R.id.code_sub_btn);

        etcode.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View view, boolean b) {
                etcode.setBackgroundResource(R.drawable.crectangle_b_gradient);
            }
        });


        showbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                evcode = etcode.getText().toString();

//                Intent intent = new Intent(Internet_mode.this, Result_data.class);
//                intent.putExtra("KEY", evcode);
//                startActivity(intent);

                if(evcode.equals("")){
                    Toast.makeText(getBaseContext(), "Cancelled", Toast.LENGTH_SHORT).show();
                }else if(evcode.equals("1787864")){
                    Intent intent = new Intent(Internet_mode.this, Result_data.class);
                    startActivity(intent);
                }else{
                    Toast.makeText(getBaseContext(), evcode+" is Invalid!!", Toast.LENGTH_SHORT).show();
                }
            }
        });



        qrbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                IntentIntegrator intentIntegrator = new IntentIntegrator(Internet_mode.this);
                intentIntegrator.setPrompt("");
                intentIntegrator.setBeepEnabled(false);
                intentIntegrator.setOrientationLocked(false);
                intentIntegrator.initiateScan();
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        IntentResult intentResult = IntentIntegrator.parseActivityResult(requestCode, resultCode, data);
        // if the intentResult is null then
        // toast a message as "cancelled"
        if (intentResult != null) {
            if (intentResult.getContents() == null) {
                Toast.makeText(getBaseContext(), "Cancelled", Toast.LENGTH_SHORT).show();
            } else if(intentResult.getContents().equals("1787864")){
                // if the intentResult is not null we'll set
                // the content and format of scan message
                Intent intent = new Intent(Internet_mode.this, Result_data.class);
                startActivity(intent);
            }else{
                Toast.makeText(getBaseContext(), intentResult.getContents()+" is Invalid!!", Toast.LENGTH_SHORT).show();
            }
        } else {
            super.onActivityResult(requestCode, resultCode, data);
        }
    }

}
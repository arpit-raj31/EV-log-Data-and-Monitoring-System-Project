package com.example.arai3;

import static java.lang.Integer.parseInt;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.text.MessageFormat;

public class Result_data extends AppCompatActivity {

    TextView tvCharge, tvSpeed, tvMTemp, tvBTemp, tvTempered, tvVolt, tvCurrent;
    Button btn_graph, btn_loc;
    RequestQueue mQueue;
    ProgressBar socPB;
    ImageView warnFire, warn, ovspeed, agbrk, theft, replNeed;
    int pb, bT=1;
    float spd, nspd = 0, curnt = 0.0f;
    String entryId, current, voltage, speed, location, batteryTemp, motorTemp, tempered, charging="100";
//    private Handler mHandler = new Handler();
    private Handler mHandler = new Handler();

    public static final String Json_URL = "https://api.thingspeak.com/channels/1802515/feeds.json?results=2";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_result_data);

        tvBTemp = (TextView) findViewById(R.id.tvBTemp);
        tvMTemp = (TextView) findViewById(R.id.tvMTemp);
        tvCharge = (TextView) findViewById(R.id.tvCharge);
        tvTempered = (TextView) findViewById(R.id.tvTempered);
        tvVolt = (TextView) findViewById(R.id.tvVolt);
        tvCurrent = (TextView) findViewById(R.id.tvCurrent);
        tvSpeed = (TextView) findViewById(R.id.tvSpeed);

        socPB = (ProgressBar) findViewById(R.id.SOCpb);
        warnFire = (ImageView) findViewById(R.id.fireWarn);
        warn = (ImageView) findViewById(R.id.warnWarn);
        ovspeed = (ImageView) findViewById(R.id.ovSpeed);
        agbrk = (ImageView) findViewById(R.id.agbrake);
        theft = (ImageView) findViewById(R.id.theftAl);
        replNeed = (ImageView) findViewById(R.id.replNeed);

        btn_graph = (Button) findViewById(R.id.btn_graph);
        btn_loc = (Button) findViewById(R.id.btn_loc);




        mQueue = Volley.newRequestQueue(this);

        jsonParse();
        mFetchRunnable.run();

        btn_graph.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(getApplicationContext(), Web_Graph.class);
                startActivity(intent);
            }
        });

        btn_loc.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(Intent.ACTION_VIEW);
//                intent.setData(Uri.parse("geo:"));
                intent.setData(Uri.parse("geo:18.5238376,73.8125817"));
                Intent chooser = Intent.createChooser(intent, "Launch Maps");
                startActivity(chooser);
            }
        });
    }

    private Runnable mFetchRunnable = new Runnable() {
        @Override
        public void run() {
            jsonParse();
//            Toast.makeText(Result_data.this, "Data Fetched", Toast.LENGTH_SHORT).show();
            nspd = spd;
            mHandler.postDelayed(this, 1000);

        }
    };

//    ----------------------------------------------------------------------------------------------
    public void jsonParse() {
//        String urlfeed = "https://api.thingspeak.com/channels/1787864/feeds.json?results=2";

        JsonObjectRequest request = new JsonObjectRequest(Request.Method.GET, Json_URL, null, new Response.Listener<JSONObject>() {
            @Override
            public void onResponse(JSONObject response) {
                try {
                    JSONArray jsonArray = response.getJSONArray("feeds");

                    current = jsonArray.getJSONObject(jsonArray.length()-1).getString("field1");
                    voltage = jsonArray.getJSONObject(jsonArray.length()-1).getString("field2");
                    speed = jsonArray.getJSONObject(jsonArray.length()-1).getString("field3");
                    location = jsonArray.getJSONObject(jsonArray.length()-1).getString("field4");
                    batteryTemp = jsonArray.getJSONObject(jsonArray.length()-1).getString("field5");
                    motorTemp = jsonArray.getJSONObject(jsonArray.length()-1).getString("field6");
                    tempered = jsonArray.getJSONObject(jsonArray.length()-1).getString("field7");
                    charging = jsonArray.getJSONObject(jsonArray.length()-1).getString("field8");


                    tvSpeed.setText(String.format("%sKm/h", speed));
                    tvVolt.setText(String.format("%sV", voltage));
                    tvCurrent.setText(String.format("%sA", current));
                    tvBTemp.setText(String.format("%s°C", batteryTemp));
                    tvMTemp.setText(String.format("%s°C", motorTemp));
                    tvCharge.setText(MessageFormat.format("{0}%", charging));

                    pb = (int) Float.parseFloat(charging);
                    socPB.setProgress(pb);

                    spd = (int) Float.parseFloat(speed);
                    curnt = Float.parseFloat(current);

                    if(Build.VERSION.SDK_INT>=Build.VERSION_CODES.O){
                        NotificationChannel channel = new NotificationChannel("Alert! Thermal Runaway may occur.", "Alert1 Thermal Runaway may occur!", NotificationManager.IMPORTANCE_DEFAULT);
                        NotificationManager manager = getSystemService(NotificationManager.class);
                        manager.createNotificationChannel(channel);
                    }

                    bT = (int) Float.parseFloat(batteryTemp);

                    if(curnt<1 && spd>2.2f){
                        NotificationCompat.Builder nbuilder = new NotificationCompat.Builder(getApplicationContext(), "Alert! Thermal Runaway may occur.");
                        nbuilder.setContentTitle("Alert!");
                        nbuilder.setContentText("Alert! Theft Alert!!!");
                        nbuilder.setSmallIcon(R.drawable.ic_launcher_foreground);
                        nbuilder.setAutoCancel(true);

                        NotificationManagerCompat managerCompat = NotificationManagerCompat.from(getApplicationContext());
                        managerCompat.notify(1, nbuilder.build());

                        theft.setColorFilter(getResources().getColor(R.color.Red));
                    }else{
                        theft.setColorFilter(getResources().getColor(R.color.Bright_Navy_Blue));
                    }

                    if(bT>60){
                        warnFire.setAlpha(1.0f);
                        warn.setAlpha(0.0f);
                        NotificationCompat.Builder nbuilder = new NotificationCompat.Builder(getApplicationContext(), "Alert! Thermal Runaway may occur.");
                        nbuilder.setContentTitle("Alert!");
                        nbuilder.setContentText("Alert1 Thermal Runaway may occur!");
                        nbuilder.setSmallIcon(R.drawable.ic_launcher_foreground);
                        nbuilder.setAutoCancel(true);

                        NotificationManagerCompat managerCompat = NotificationManagerCompat.from(getApplicationContext());
                        managerCompat.notify(1, nbuilder.build());
                    }else{
                        warnFire.setAlpha(0.0f);
                        warn.setAlpha(1.0f);
                    }
                    if(spd < 1.1f && nspd>6.2f){
                        NotificationCompat.Builder nbuilder = new NotificationCompat.Builder(getApplicationContext(), "Alert! Thermal Runaway may occur.");
                        nbuilder.setContentTitle("Alert!");
                        nbuilder.setContentText("Alert! Aggressive Braking");
                        nbuilder.setSmallIcon(R.drawable.ic_launcher_foreground);
                        nbuilder.setAutoCancel(true);

                        NotificationManagerCompat managerCompat = NotificationManagerCompat.from(getApplicationContext());
                        managerCompat.notify(1, nbuilder.build());

                        agbrk.setColorFilter(getResources().getColor(R.color.Red));
                    }else{
                        agbrk.setColorFilter(getResources().getColor(R.color.Bright_Navy_Blue));
                    }

                    if(spd>5.5f){
                        NotificationCompat.Builder nbuilder = new NotificationCompat.Builder(getApplicationContext(), "Alert! Thermal Runaway may occur.");
                        nbuilder.setContentTitle("Alert!");
                        nbuilder.setContentText("Alert! Speed is High!");
                        nbuilder.setSmallIcon(R.drawable.ic_launcher_foreground);
                        nbuilder.setAutoCancel(true);

                        NotificationManagerCompat managerCompat = NotificationManagerCompat.from(getApplicationContext());
                        managerCompat.notify(1, nbuilder.build());

                        ovspeed.setColorFilter(getResources().getColor(R.color.Red));
                    }else{
                        ovspeed.setColorFilter(getResources().getColor(R.color.Bright_Navy_Blue));
                    }



                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                error.printStackTrace();
            }
        });

        mQueue.add(request);
    }
}

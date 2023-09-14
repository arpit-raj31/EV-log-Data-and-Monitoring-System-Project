package com.example.arai3;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class Web_Graph extends AppCompatActivity {
    WebView  WCurrent, WVolt, WSpeed, WBTemp, WMTemp, WCharge;

    @SuppressLint("SetJavaScriptEnabled")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_web_graph);

        WCurrent = (WebView) findViewById(R.id.webCurrent);
        WVolt = (WebView) findViewById(R.id.webVoltage);
        WSpeed = (WebView) findViewById(R.id.webSpeed);
        WBTemp = (WebView) findViewById(R.id.webBTemp);
        WMTemp = (WebView) findViewById(R.id.webMTemp);
        WCharge = (WebView) findViewById(R.id.webCharging);

        WCurrent.setWebViewClient(new WebViewClient());
        WebSettings wCurrentSet= WCurrent.getSettings();
        wCurrentSet.setJavaScriptEnabled(true);
        wCurrentSet.setJavaScriptCanOpenWindowsAutomatically(true);
        wCurrentSet.setDisplayZoomControls(true);
        WCurrent.loadUrl("https://thingspeak.com/channels/1802515/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15");

        WVolt.setWebViewClient(new WebViewClient());
        WebSettings wVoltSet= WVolt.getSettings();
        wVoltSet.setJavaScriptEnabled(true);
        wVoltSet.setJavaScriptCanOpenWindowsAutomatically(true);
        wVoltSet.setDisplayZoomControls(true);
        WVolt.loadUrl("https://thingspeak.com/channels/1802515/charts/2?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15");

        WSpeed.setWebViewClient(new WebViewClient());
        WebSettings wSpeedSet= WSpeed.getSettings();
        wSpeedSet.setJavaScriptEnabled(true);
        wSpeedSet.setJavaScriptCanOpenWindowsAutomatically(true);
        wSpeedSet.setDisplayZoomControls(true);
        WSpeed.loadUrl("https://thingspeak.com/channels/1802515/charts/3?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15");

        WBTemp.setWebViewClient(new WebViewClient());
        WebSettings wBTempSet= WBTemp.getSettings();
        wBTempSet.setJavaScriptEnabled(true);
        wBTempSet.setJavaScriptCanOpenWindowsAutomatically(true);
        wBTempSet.setDisplayZoomControls(true);
        WBTemp.loadUrl("https://thingspeak.com/channels/1802515/charts/5?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15");

        WMTemp.setWebViewClient(new WebViewClient());
        WebSettings wMTempSet= WMTemp.getSettings();
        wMTempSet.setJavaScriptEnabled(true);
        wMTempSet.setJavaScriptCanOpenWindowsAutomatically(true);
        wMTempSet.setDisplayZoomControls(true);
        WMTemp.loadUrl("https://thingspeak.com/channels/1802515/charts/6?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15");

        WCharge.setWebViewClient(new WebViewClient());
        WebSettings wChargeSet= WCharge.getSettings();
        wChargeSet.setJavaScriptEnabled(true);
        wChargeSet.setJavaScriptCanOpenWindowsAutomatically(true);
        wChargeSet.setDisplayZoomControls(true);
        WCharge.loadUrl("https://thingspeak.com/channels/1802515/charts/8?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15");

    }
}
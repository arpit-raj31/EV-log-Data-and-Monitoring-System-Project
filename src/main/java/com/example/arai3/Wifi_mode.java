package com.example.arai3;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class Wifi_mode extends AppCompatActivity {
    WebView webview;

//    private Handler mHandler = new Handler();

    @SuppressLint("SetJavaScriptEnabled")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_wifi_mode);
        webview =(WebView) findViewById(R.id.webviewW);
        webview.setWebViewClient(new WebViewClient());


        WebSettings webSettings= webview.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setJavaScriptCanOpenWindowsAutomatically(true);
        webview.loadUrl("http://192.168.4.1/");
//        mFetchRunnable.run();

        webview.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                webview.loadUrl("http://192.168.4.1/");
            }
        });
    }

    @Override
    public void onBackPressed() {
        if(webview.canGoBack()){
            webview.goBack();
        }
        else {
            super.onBackPressed();
        }
    }

/*
    private Runnable mFetchRunnable = new Runnable() {
        @Override
        public void run() {
            webview.loadUrl("http://192.168.4.1/");
//            Toast.makeText(Result_data.this, "Data Fetched", Toast.LENGTH_SHORT).show();
            mHandler.postDelayed(this, 1000);
        }
    };

 */
}
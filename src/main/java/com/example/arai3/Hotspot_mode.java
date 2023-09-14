package com.example.arai3;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class Hotspot_mode extends AppCompatActivity {
    WebView webview;

//    private Handler mHandler = new Handler();

    @SuppressLint("SetJavaScriptEnabled")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_hotspot_mode);

        webview =(WebView) findViewById(R.id.webviewH);
        webview.setWebViewClient(new WebViewClient());


        WebSettings webSettings= webview.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setJavaScriptCanOpenWindowsAutomatically(true);


        webview.loadUrl("http://192.168.43.83/");
//        mFetchRunnable.run();
    }
/*
    private Runnable mFetchRunnable = new Runnable() {
        @Override
        public void run() {
            webview.loadUrl("http://192.168.43.83/");
//            Toast.makeText(Result_data.this, "Data Fetched", Toast.LENGTH_SHORT).show();
            mHandler.postDelayed(this, 1000);
        }
    };
*/
    @Override
    public void onBackPressed() {
        if(webview.canGoBack()){
            webview.goBack();
        }
        else {
            super.onBackPressed();
        }
    }
}
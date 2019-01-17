package com.example.schmi.iotproject;

import android.graphics.Color;
import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;


import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;



public class MainActivity extends AppCompatActivity {
    TextView t1, t2, indicator;
    Button b1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        t1 = (TextView) findViewById(R.id.t1);
        t2 = (TextView) findViewById(R.id.t2);
        b1 = (Button) findViewById(R.id.b1);
        indicator = (TextView) findViewById(R.id.indicator);

        indicator.setText("");
        indicator.setBackgroundColor(Color.parseColor("#00ff00"));
        t2.setText("");


        b1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new getDataFromThingSpeak().execute();
            }
        });
    }



    class getDataFromThingSpeak extends AsyncTask<Void, Void, String> {

        protected void onPreExecute() {
            t2.setText("Getting data from ThingSpeak\nPlease Wait...");
        }

        protected String doInBackground(Void... voids) {
            try {
                URL url = new URL("https://api.thingspeak.com/channels/673886/feeds.json?api_key=RO4HNU8HPCYBXXVD&results=2");
                HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();

                BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(urlConnection.getInputStream()));
                StringBuilder stringBuilder = new StringBuilder();
                String line;

                while ((line = bufferedReader.readLine()) != null) {
                    stringBuilder.append(line).append("\n");
                }

                bufferedReader.close();
                urlConnection.disconnect();

                return stringBuilder.toString();

            } catch (Exception e) {
                Log.e("ERROR", e.getMessage(), e);
                return null;
            }
        }

        protected void onPostExecute(String response) {

            if (response == null) {
                Toast.makeText(MainActivity.this, "There was an error", Toast.LENGTH_SHORT).show();
                return;
            }

            try {

                JSONObject json = new JSONObject(response);
                JSONArray feeds = json.getJSONArray("feeds");
                JSONObject lastVals = feeds.getJSONObject(feeds.length() - 1);

                String val = lastVals.getString("field4");

                t1.setText("Val: " + val);
                t2.setText("Value has been received");

                checkVal(val);

            } catch (JSONException e) {
                t2.setText("Something went wrong");
                e.printStackTrace();
            }
        }
    }

    private void checkVal(String val){

        if(val.equals("Illigal parking")){
            indicator.setText("Illigal parkings detected");
            indicator.setBackgroundColor(Color.parseColor("#ff0000"));
        } else {
            indicator.setText("No illigal parkings detected");
            indicator.setBackgroundColor(Color.parseColor("#00ff00"));

        }

    }

}

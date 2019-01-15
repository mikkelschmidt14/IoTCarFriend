package com.example.schmi.iotproject;

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
    TextView t1, t2;
    Button b1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        t1 = (TextView) findViewById(R.id.textView2);
        t2 = (TextView) findViewById(R.id.textView3);
        b1 = (Button) findViewById(R.id.button);
        t2.setText("");


        b1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    new FetchThingspeakTask().execute();
                } catch (Exception e) {
                    Log.e("ERROR", e.getMessage(), e);
                }
            }
        });
    }

    class FetchThingspeakTask extends AsyncTask<Void, Void, String> {
        protected void onPreExecute() {
            t2.setText("Fetching Data from Server.Please Wait...");
        }

        protected String doInBackground(Void... urls) {
            try {
                URL url = new URL("https://api.thingspeak.com/channels/673886/feeds.json?api_key=RO4HNU8HPCYBXXVD&results=2");
                HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();

                try {
                    BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(urlConnection.getInputStream()));
                    StringBuilder stringBuilder = new StringBuilder();
                    String line;
                    while ((line = bufferedReader.readLine()) != null) {
                        stringBuilder.append(line).append("\n");
                    }
                    bufferedReader.close();
                    System.out.println("Received string: " + stringBuilder.toString());
                    return stringBuilder.toString();

                } finally {
                    urlConnection.disconnect();
                }

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
                JSONObject json = (JSONObject) new JSONTokener(response).nextValue();
                JSONArray feeds = json.getJSONArray("feeds");
                JSONObject lastVals = feeds.getJSONObject(feeds.length() - 1);

                int val = lastVals.getInt("field3");


                t1.setText("Last value was " + val);
                t2.setText("Value has been received");

            } catch (JSONException e) {
                t2.setText("Something went wrong");
                e.printStackTrace();
            }
        }
    }
}

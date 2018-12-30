package com.ncsu.bme.acelaso;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;


import com.opencsv.CSVReader;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Path;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;



public class CsvSelector extends Activity {

    public static String filename = null;
    public static String NEW_FILES;
    private final static String TAG = CsvSelector.class.getSimpleName();


    private ListView csv_list;


    public ArrayList<String> GetFiles(String DirectoryPath) {
        ArrayList<String> MyFiles = new ArrayList<String>();
        File f = new File(DirectoryPath);

        f.mkdirs();
        File[] files = f.listFiles();
        if (files.length == 0) {
            Log.d(TAG, "No files found");
            return null;
        }
        else {
            for (int i = 0; i < files.length; i++) {
                Log.d(TAG, files.length + "Files Found");
                if (files[i].toString().contains("csv")) {
                    MyFiles.add(files[i].getName());
                }
            }
        }

        Collections.sort(MyFiles);
        Collections.reverse(MyFiles);

        return MyFiles;
    }

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_csv_selector);
    }

    public void onClick(View v) {

        if (v.getId() == R.id.csvLists) {


            TextView fileselect=(TextView)findViewById(R.id.selectAfile);
            fileselect.setVisibility(View.VISIBLE);
            ArrayList<String> FilesInFolder = GetFiles(Environment.getExternalStorageDirectory().toString() + "/");
            csv_list = (ListView) findViewById(R.id.ListOfFiles);

            csv_list.setAdapter(new ArrayAdapter<String>(this,
                    android.R.layout.simple_list_item_1, FilesInFolder));


            csv_list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                public void onItemClick(AdapterView<?> parent, View v, int position, long id) {
                    filename = csv_list.getItemAtPosition(position).toString();
                    Log.d(TAG, "TEST" + " " + filename);


                    try {
                        File route= Environment.getExternalStorageDirectory();
                        File file=new File(route,filename);
                        CSVReader reader=new CSVReader(new FileReader(file));
                        String [] nextLine;
                        while ((nextLine = reader.readNext()) != null) {
                            if (nextLine != null) {
                                //Verifying the read data here
                                //System.out.println(Arrays.toString(nextLine));
                                String[] data = Arrays.toString(nextLine).split(",");

                                try {
                                    Thread.sleep(250);
                                } catch (InterruptedException e) {
                                    e.printStackTrace();
                                }

                                if (data[1].equals(" 1.5") && data[2].equals(" 23.285") && data[3].equals(" 84.751")) {
                                    Log.d(TAG, "Alert Indicator");
                                    startActivity(new Intent(CsvSelector.this, AlertPage.class));
                                }
                                else{
                                    continue;
                                }
                            }
                        }
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();

                    }
                    catch (IOException ex){
                        ex.printStackTrace();
                    }

                }
            });

        }
    }
}
package com.ncsu.bme.acelaso;

import java.sql.Timestamp;

/**
 * Created by JEET on 7/23/2015.
 */
public class DatabaseHelper {

    private float  _hr, _temp, _conductance;
    private Timestamp _timeStamp;
    //private Uri _imageURI;
    private int _id;

    public DatabaseHelper (/*int id,*/ Timestamp time, float heart_rate, float temp, float cond) {
        /*_id = id;*/
        _timeStamp = time;
        _hr = heart_rate;
        _temp = temp;
        _conductance=cond;

    }

    public int getId() { return _id; }

    public Timestamp get_timeStamp() {
        return _timeStamp;
    }

    public float get_hr() {
        return _hr;
    }

    public float get_temp() {
        return _temp;
    }

    public float get_conductance() {
        return _conductance;
    }

 }

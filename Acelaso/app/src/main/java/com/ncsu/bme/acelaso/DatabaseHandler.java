package com.ncsu.bme.acelaso;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.List;
/**
 * Created by JEET on 7/19/2015.
 */
public class DatabaseHandler extends SQLiteOpenHelper {
    private static final int DATABASE_VERSION = 1;
    private static final String DATABASE_NAME = "ACELASO-DB.db",

    ACELASO_DATABASE_TABLE = "ACELASO_DATA",
            KEY_ID = "id",
            TIMESTAMP = "time",
            KEY_HR = "heart_rate",
            KEY_TEMP = "temp",
            KEY_SKIN_CONDUCT = "cond";

    public DatabaseHandler(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        String CREATE_TABLE_TEMP = "CREATE TABLE IF NOT EXISTS " + ACELASO_DATABASE_TABLE + "("
                + //KEY_ID  + " INTEGER PRIMARY KEY AUTOINCREMENT ," +
                TIMESTAMP + " TIMESTAMP )"
                + KEY_HR + "INTEGER" + DBConstants.KEY_TEMP + " INTEGER " + KEY_SKIN_CONDUCT + "INTEGER";
        db.execSQL(CREATE_TABLE_TEMP);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        // Drop older table if existed, all data will be gone!!!
        db.execSQL("DROP TABLE IF EXISTS " + ACELASO_DATABASE_TABLE);
        // Create tables again
        onCreate(db);
    }

    public void createDatabase(DatabaseHelper dataTable) {              //Pass them through the createDatabase class
        SQLiteDatabase db = getWritableDatabase();

        ContentValues values = new ContentValues();

        // values.put(KEY_ID, dataTable.getId());
        /*values.put(TIMESTAMP, dataTable.get_timeStamp());*/
        values.put(KEY_HR, dataTable.get_hr());
        values.put(KEY_TEMP, dataTable.get_temp());
        values.put(KEY_SKIN_CONDUCT, dataTable.get_conductance());

        db.insert(ACELASO_DATABASE_TABLE, null, values);
        db.close();
    }

    public DatabaseHelper getData(int time) {           //Read data
        SQLiteDatabase db = getReadableDatabase();

        Cursor cursor = db.query(ACELASO_DATABASE_TABLE, new String[] {TIMESTAMP, KEY_HR,KEY_TEMP,KEY_SKIN_CONDUCT },
                TIMESTAMP + "=?", new String[] { String.valueOf(TIMESTAMP) }, null, null, null, null);

        if (cursor != null)
            cursor.moveToFirst();

        DatabaseHelper dataTab =new DatabaseHelper(Timestamp.valueOf(cursor.getString(0)), cursor.getInt(1),
                cursor.getInt(2), cursor.getInt(3)/*,cursor.getInt(4)*/);
        db.close();
        cursor.close();
        return dataTab;
    }
    public List<DatabaseHelper> getAllData() {      //all data can be listed
        List<DatabaseHelper> DataTab = new ArrayList<DatabaseHelper>();

        SQLiteDatabase db = getWritableDatabase();
        Cursor cursor = db.rawQuery("SELECT * FROM " + ACELASO_DATABASE_TABLE, null);

        if (cursor.moveToFirst()) {
            do {
                DataTab.add(new DatabaseHelper(Timestamp.valueOf(cursor.getString(0)), cursor.getInt(1),
                        cursor.getInt(2), cursor.getInt(3)/*, cursor.getInt(4))*/));
            }
            while (cursor.moveToNext());
        }
        cursor.close();
        db.close();
        return DataTab;
    }

}
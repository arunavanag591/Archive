package com.ncsu.bme.acelaso;

import android.database.Cursor;
import android.os.Environment;
import android.support.v7.app.ActionBarActivity;
import android.content.Intent;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.database.sqlite.SQLiteDatabase;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.util.SparseArray;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.opencsv.CSVWriter;


import java.io.File;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;
import java.util.UUID;


public class BleGattActivity extends ActionBarActivity implements BluetoothAdapter.LeScanCallback{
    private static final String TAG = "ACELASO";

    private static final String DEVICE_NAME = "ACELASO";

    private BluetoothAdapter mBluetoothAdapter;
    private SparseArray<BluetoothDevice> mDevices;

    private BluetoothGatt mConnectedGatt;

    private TextView mDieTemp, mObjTemp, mGSR, mHR;

    private ProgressDialog mProgress;


    /*RFDUINO Service*/
    private static final UUID RFDUINO_SERVICE = BluetoothHelper.sixteenBitUuid(0x2220);
    private static final UUID RFDUINO_RECEIVE = BluetoothHelper.sixteenBitUuid(0x2221);
    private static final UUID RFDUINO_SEND = BluetoothHelper.sixteenBitUuid(0x2222);
    private static final UUID RFDUINO_DISCONNECT = BluetoothHelper.sixteenBitUuid(0x2223);
    private static final UUID RFDUINO_CLIENT_CONFIGURATION = BluetoothHelper.sixteenBitUuid(0x2902);

    private static int rcv_active = 0;

    List<DatabaseHelper> DataTable = new ArrayList<DatabaseHelper>();
    ListView DataTableList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        supportRequestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
        setContentView(R.layout.activity_ble_gatt);
        setSupportProgressBarIndeterminate(true);



        mObjTemp = (TextView) findViewById(R.id.TemperatureValue);
         mGSR = (TextView) findViewById(R.id.SkinCondValue);
        mHR = (TextView) findViewById(R.id.HeartRateValue);

        /*
         * Bluetooth in Android 4.3 is accessed via the BluetoothManager, rather than
         * the old static BluetoothAdapter.getInstance()
         */
        BluetoothManager manager = (BluetoothManager) getSystemService(BLUETOOTH_SERVICE);
        mBluetoothAdapter = manager.getAdapter();

        mDevices = new SparseArray<BluetoothDevice>();

        /*
          A progress dialog will be needed while the connection process is
          taking place
         */
        mProgress = new ProgressDialog(this);
        mProgress.setIndeterminate(true);
        mProgress.setCancelable(false);
        clearDisplayValues();
    }

    @Override
    protected void onResume() {
        super.onResume();
        /*
            We need to enforce that Bluetooth is first enabled, and take the
             user to settings to enable it if they have not done so.
         */
        if (mBluetoothAdapter == null || !mBluetoothAdapter.isEnabled()) {
            //Bluetooth is disabled
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivity(enableBtIntent);
            finish();
            return;
        }

        /*
         * Check for Bluetooth LE Support.  In production, our manifest entry will keep this
         * from installing on these devices, but this will allow test devices or other
         * sideloads to report whether or not the feature exists.
         */
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, "No LE Support.", Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        clearDisplayValues();
    }

    @Override
    protected void onPause() {
        super.onPause();
        //Make sure dialog is hidden
        mProgress.dismiss();
        //Cancel any scans in progress
        mHandler.removeCallbacks(mStopRunnable);
        mHandler.removeCallbacks(mStartRunnable);
        mBluetoothAdapter.stopLeScan(this);
    }

    @Override
    protected void onStop() {
        super.onStop();
        //Disconnect from any active tag connection
        if (mConnectedGatt != null) {
            mConnectedGatt.disconnect();
            mConnectedGatt = null;
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Add the "scan" option to the menu
        getMenuInflater().inflate(R.menu.menu_ble_gatt, menu);
        //Add any device elements we've discovered to the overflow menu
        for (int i=0; i < mDevices.size(); i++) {
            BluetoothDevice device = mDevices.valueAt(i);
            menu.add(0, mDevices.keyAt(i), 0, device.getName());
        }

        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_scan:
                mDevices.clear();
                startScan();
                return true;
            default:
                //Obtain the discovered device to connect with
                BluetoothDevice device = mDevices.get(item.getItemId());
                Log.i(TAG, "Connecting to "+device.getName());
                /*
                 * Make a connection with the device using the special LE-specific
                 * connectGatt() method, passing in a callback for GATT events
                 */
                mConnectedGatt = device.connectGatt(this, false, mGattCallback);
                //Display progress UI
                mHandler.sendMessage(Message.obtain(null, MSG_PROGRESS, "Connecting to "+device.getName()+"..."));
                return super.onOptionsItemSelected(item);
        }
    }

    private void clearDisplayValues() {

        mObjTemp.setText("---");
        /*mDieTemp.setText("---");*/
        mGSR.setText("---");
        mHR.setText("---");


    }


    private Runnable mStopRunnable = new Runnable() {
        @Override
        public void run() {
            stopScan();
        }
    };
    private Runnable mStartRunnable = new Runnable() {
        @Override
        public void run() {
            startScan();
        }
    };

    private void startScan() {
        mBluetoothAdapter.startLeScan(this);
        setSupportProgressBarIndeterminateVisibility(true);
        mHandler.postDelayed(mStopRunnable, 2500);
    }

    private void stopScan() {
        mBluetoothAdapter.stopLeScan(this);
        setSupportProgressBarIndeterminateVisibility(false);
    }

    /* BluetoothAdapter.LeScanCallback */

    @Override
    public void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord) {
        Log.i(TAG, "New LE Device: " + device.getName() + " @ " + rssi);
        /*
         * We are looking for Acelaso devices only, so validate the name
         * that each device reports before adding it to our collection
         */
        if (DEVICE_NAME.equals(device.getName())) {
            mDevices.put(device.hashCode(), device);
            //Update the overflow menu
            invalidateOptionsMenu();
        }
    }

    /*
     * In this callback, we've created a bit of a state machine to enforce that only
     * one characteristic be read or written at a time until all of our sensors
     * are enabled and we are registered to get notifications.
     */
    private BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {

        /* State Machine Tracking */
        private int mState = 0;

        private void reset() { mState = 0; }

        private void advance() { mState++; }

        /*
         * Send an enable command to each sensor by writing a configuration
         * characteristic.  This is specific to the ACELASO to keep power
         * low by disabling sensors you aren't using.
         */
        private void enableNextSensor(BluetoothGatt gatt) {
            BluetoothGattCharacteristic characteristic;
            switch (mState) {
                case 0:
                    Log.d(TAG, "Enabling RFduino");
                    characteristic = gatt.getService(RFDUINO_SERVICE)
                            .getCharacteristic(RFDUINO_SEND);
                    characteristic.setValue (new byte[] {0x01});
                    break;
                default:
                    mHandler.sendEmptyMessage(MSG_DISMISS);
                    Log.i(TAG, "All Sensors Enabled");
                    return;
            }

            gatt.writeCharacteristic(characteristic);
        }

        /*
         * Read the data characteristic's value for each sensor explicitly
         */
        private void readNextSensor(BluetoothGatt gatt) {
            BluetoothGattCharacteristic characteristic;
            switch (mState) {
                case 0:
                    Log.d(TAG, "Reading RFduino");
                    characteristic = gatt.getService(RFDUINO_SERVICE)
                            .getCharacteristic(RFDUINO_RECEIVE);
                    break;
                default:
                    mHandler.sendEmptyMessage(MSG_DISMISS);
                    Log.i(TAG, "All Sensors Enabled");
                    return;
            }

            gatt.readCharacteristic(characteristic);
        }

        /*
         * Enable notification of changes on the data characteristic for each sensor
         * by writing the ENABLE_NOTIFICATION_VALUE flag to that characteristic's
         * configuration descriptor.
         */
        private void setNotifyNextSensor(BluetoothGatt gatt) {
            BluetoothGattCharacteristic characteristic;
            switch (mState) {
                case 0:
                    Log.d(TAG, "Set notify rfduino");
                    characteristic = gatt.getService(RFDUINO_SERVICE)
                            .getCharacteristic(RFDUINO_RECEIVE);
                    break;
                default:
                    mHandler.sendEmptyMessage(MSG_DISMISS);
                    Log.i(TAG, "All Sensors Enabled");
                    return;
            }

            //Enable local notifications
           /* gatt.setCharacteristicNotification(characteristic, true);*/
            //Enabled remote notifications
            /*BluetoothGattDescriptor desc = characteristic.getDescriptor(CONFIG_DESCRIPTOR);
            desc.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);*/

            if (characteristic != null) {
                BluetoothGattDescriptor receiveConfigDescriptor =
                        characteristic.getDescriptor(RFDUINO_CLIENT_CONFIGURATION);
                if (receiveConfigDescriptor != null) {
                    gatt.setCharacteristicNotification(characteristic, true);
                    receiveConfigDescriptor.setValue(
                            BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
                    gatt.writeDescriptor(receiveConfigDescriptor);
                } else {
                    Log.e(TAG, "RFduino receive config descriptor not found!");
                }

            } else {
                Log.e(TAG, "RFduino receive characteristic not found!");
            }
        }

        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            Log.d(TAG, "Connection State Change: "+status+" -> "+connectionState(newState));
            if (status == BluetoothGatt.GATT_SUCCESS && newState == BluetoothProfile.STATE_CONNECTED) {
                /*
                 * Once successfully connected, we must next discover all the services on the
                 * device before we can read and write their characteristics.
                 */
                gatt.discoverServices();
                mHandler.sendMessage(Message.obtain(null, MSG_PROGRESS, "Discovering Services..."));
            } else if (status == BluetoothGatt.GATT_SUCCESS && newState == BluetoothProfile.STATE_DISCONNECTED) {
                /*
                 * If at any point we disconnect, send a message to clear the values
                 * out of the UI
                 */
                mHandler.sendEmptyMessage(MSG_CLEAR);
            } else if (status != BluetoothGatt.GATT_SUCCESS) {
                /*
                 * If there is a failure at any stage, simply disconnect
                 */
                gatt.disconnect();
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            Log.d(TAG, "Services Discovered: "+status);
            mHandler.sendMessage(Message.obtain(null, MSG_PROGRESS, "Enabling Sensors..."));
            /*
             * With services discovered, we are going to reset our state machine and start
             * working through the sensors we need to enable
             */
            reset();
            enableNextSensor(gatt);
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            //For each read, pass the data up to the UI thread to update the display

           /* if (RFDUINO_RECEIVE.equals(characteristic.getUuid())) {
                Log.i(TAG, "RFDUINO Send Characterstic");
                mHandler.sendMessage(Message.obtain(null, MSG_RFDUINO, characteristic));
            }
            */

            //After reading the initial value, next we enable notifications
            setNotifyNextSensor(gatt);
        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            //After writing the enable flag, next we read the initial value
            readNextSensor(gatt);
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {

            if (RFDUINO_RECEIVE.equals(characteristic.getUuid())) {
                mHandler.sendMessage(Message.obtain(null, MSG_RFDUINO, characteristic));
            }
        }

        @Override
        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            //Once notifications are enabled, we move to the next sensor and start over with enable
            advance();
            enableNextSensor(gatt);
        }

        @Override
        public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
            Log.d(TAG, "Remote RSSI: "+rssi);
        }

        private String connectionState(int status) {
            switch (status) {
                case BluetoothProfile.STATE_CONNECTED:
                    return "Connected";
                case BluetoothProfile.STATE_DISCONNECTED:
                    return "Disconnected";
                case BluetoothProfile.STATE_CONNECTING:
                    return "Connecting";
                case BluetoothProfile.STATE_DISCONNECTING:
                    return "Disconnecting";
                default:
                    return String.valueOf(status);
            }
        }
    };

    /*
     * We have a Handler to process event results on the main thread
     */

    private static final int MSG_RFDUINO = 104;
    private static final int MSG_PROGRESS = 201;
    private static final int MSG_DISMISS = 202;
    private static final int MSG_CLEAR = 301;
    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {


            BluetoothGattCharacteristic characteristic;

            Calendar calendar = Calendar.getInstance();
            java.util.Date now = calendar.getTime();
            java.sql.Timestamp currentTimestamp = new java.sql.Timestamp(now.getTime());
            switch (msg.what) {

                case MSG_RFDUINO:
                    characteristic = (BluetoothGattCharacteristic) msg.obj;
                    if (characteristic.getValue() == null) {
                        Log.w(TAG, "Error obtaining rfduino value");
                        return;
                    }
                    if (rcv_active == 0) {
                        checkRFduinoValues(characteristic);
                        break;
                    }
                    else if (rcv_active == 1) {
                        updateObjTempValues(characteristic);
                        break;
                    }

                    else if (rcv_active == 3) {
                        updateGSRValues(characteristic);
                        break;
                    }
                    else if (rcv_active == 4) {
                        updateHRValues(characteristic);
                        break;
                    }

                case MSG_PROGRESS:/*
                    mProgress.setMessage((String) msg.obj);
                    if (!mProgress.isShowing()) {
                        mProgress.show();
                    }
                    break;*/
                case MSG_DISMISS:
                    mProgress.hide();
                    break;
                case MSG_CLEAR:
                    clearDisplayValues();
                    break;
            }
        }
    };


    private void checkRFduinoValues(BluetoothGattCharacteristic characteristic) {
        int ID_byte = characteristic.getIntValue(characteristic.FORMAT_UINT8,0);
        if (ID_byte == 0x05 ) {
            Log.w(TAG, "IDByte: 5");
            rcv_active = 1;
        }
       /* if (ID_byte == 0x06 ) {
            Log.w(TAG, "IDByte: 6");
            rcv_active = 2;
        }*/
        if (ID_byte == 0x07 ) {
            Log.w(TAG, "IDByte: 7");
            rcv_active = 3;
        }
        if (ID_byte == 0x08) {
            Log.w(TAG, "IDByte: 8");
            rcv_active = 4;
        }

    }

    private void updateObjTempValues(BluetoothGattCharacteristic characteristic) {
        float object_temp = TagAcelaso.extractObjectTempValues(characteristic,rcv_active);
        Log.i(TAG,String.format("%f", object_temp));
        mObjTemp.setText(String.format("%.2f", object_temp));
        //DatabaseHelper dbHelper = new DatabaseHelper(timenow, object_temp, 0, 0  );
        rcv_active=0;
    }


    private void updateGSRValues(BluetoothGattCharacteristic characteristic) {
        float gsr_value = TagAcelaso.extractGSRValues(characteristic,rcv_active);
        Log.i(TAG,String.format("%f", gsr_value));
        mGSR.setText(String.format("%.2f", gsr_value));
        //DatabaseHelper dbHelper = new DatabaseHelper(timenow, 0, 0, gsr_value  );
        rcv_active=0;
    }
    private void updateHRValues(BluetoothGattCharacteristic characteristic) {
        float hr_value = TagAcelaso.extractHRValues(characteristic,rcv_active);
        Log.i(TAG,String.format("%f", hr_value));
        mHR.setText(String.format("%.2f", hr_value));
        //DatabaseHelper dbHelper = new DatabaseHelper(timenow, 0, 0, gsr_value  );
        rcv_active=0;
    }

    private void populateList() {
        ArrayAdapter<DatabaseHelper> adapter = new DataTableAdapter();
        DataTableList.setAdapter(adapter);
    }

    private class DataTableAdapter extends ArrayAdapter<DatabaseHelper> {
        public DataTableAdapter() {
            super (BleGattActivity.this, R.layout.data_list, DataTable);
        }

        @Override
        public View getView(int position, View view, ViewGroup parent) {
            if (view == null)
                view = getLayoutInflater().inflate(R.layout.data_list, parent, false);

            DatabaseHelper currentData = DataTable.get(position);


            TextView phone = (TextView) view.findViewById(R.id.HeartRate);
            phone.setText(String.valueOf(currentData.get_hr()));
            TextView name = (TextView) view.findViewById(R.id.SkinTemperature);
            name.setText(String.valueOf(currentData.get_temp()));
            TextView email = (TextView) view.findViewById(R.id.SkinConductance);
            email.setText(String.valueOf(currentData.get_conductance()));

            return view;
        }
    }

    public void exportDatatoCSV(DatabaseHandler saveAllData){ //export data to .csv file
        File dbFile= getDatabasePath("ACELASO-DB.sqlite");
        DatabaseHandler dbHelper = new DatabaseHandler(getApplicationContext());
        File exportDir = new File(Environment.getExternalStorageDirectory(), "");
        if (!exportDir.exists())
        {
            exportDir.mkdirs();
        }

        File file = new File(exportDir, "acelasodatafile.csv");
        try
        {
            file.createNewFile();
            CSVWriter csvWrite = new CSVWriter(new FileWriter(file));
            SQLiteDatabase db = dbHelper.getReadableDatabase();
            Cursor curCSV = db.rawQuery("SELECT * FROM" + DBConstants.DATABASE_NAME,null);
            csvWrite.writeNext(curCSV.getColumnNames());
            while(curCSV.moveToNext())
            {
                //Which column you want to export-->Treating the whole data as string values
                String arrStr[] ={curCSV.getString(0),curCSV.getString(1), curCSV.getString(2),curCSV.getString(3)};
                csvWrite.writeNext(arrStr);
            }
            csvWrite.close();
            curCSV.close();
        }
        catch(Exception sqlEx)
        {
            Log.e("BleGattActivity", sqlEx.getMessage(), sqlEx);
        }
    }

    public void onClick(View view) {
        if (view.getId() == R.id.PlotData) {
            startActivity(new Intent(BleGattActivity.this, PlotPage.class));
        } else if (view.getId() == R.id.CheckData) {

            startActivity((new Intent(BleGattActivity.this, CsvSelector.class)));

        }
    }

}


package com.ncsu.bme.acelaso;

import android.bluetooth.BluetoothGattCharacteristic;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
/**
 * Created by JEET on 8/13/2015.
 */
public class TagAcelaso {private static final String TAG = "ACELASO";
    public static ByteBuffer OurByteBuffer, ObjBuffer, DieBuffer, GSRBuffer, HRBuffer;

    public static float extractRFduinoValues (BluetoothGattCharacteristic c) {
        byte[]receive_bytes = c.getValue();
        if ((receive_bytes[0]!=(0x05)) | (receive_bytes[0]!=(0x06)) | (receive_bytes[0]!=(0x07)) |(receive_bytes[0]!=(0x08))) {
            OurByteBuffer = ByteBuffer.wrap(receive_bytes);
            float rawRFduino1 = ByteBuffer.wrap(receive_bytes).order(ByteOrder.LITTLE_ENDIAN).getFloat();
            return rawRFduino1;
        }
        else return 0;

    }

    public static float extractObjectTempValues (BluetoothGattCharacteristic c, int rcv_active) {
        byte[]receive_bytes = c.getValue();
        Log.i(TAG, "Obj Temp_Recieve:" + receive_bytes);
        ObjBuffer = ByteBuffer.wrap(receive_bytes).order(ByteOrder.LITTLE_ENDIAN);
        if (ObjBuffer.remaining() >= 4  && rcv_active == 1) {
            float objectTemp = ObjBuffer.getFloat();
            /*float objectTemp = ByteBuffer.wrap(receive_bytes).order(ByteOrder.LITTLE_ENDIAN).getFloat();*/
            return objectTemp;
        }
        else return 0;
    }



    public static float extractGSRValues (BluetoothGattCharacteristic c, int rcv_active) {
        byte[]receive_bytes = c.getValue();
        Log.i(TAG, "GSR Receive:" + receive_bytes);
        GSRBuffer = ByteBuffer.wrap(receive_bytes).order(ByteOrder.LITTLE_ENDIAN);
        if (GSRBuffer.remaining() >= 4  && rcv_active == 3) {
            float gsr = GSRBuffer.getFloat();

            return gsr;
        }
        else return 0;
    }

    public static float extractHRValues (BluetoothGattCharacteristic c, int rcv_active) {
        byte[]receive_bytes = c.getValue();
        Log.i(TAG, "HR_Receive:" + receive_bytes);
        HRBuffer = ByteBuffer.wrap(receive_bytes).order(ByteOrder.LITTLE_ENDIAN);
        if (HRBuffer.remaining() >= 4 && rcv_active == 4) {
            float hr = HRBuffer.getFloat();

            return hr;
        }
        else return 0;
    }


    public static double extractHumAmbientTemperature(BluetoothGattCharacteristic c) {
        int rawT = shortSignedAtOffset(c, 0);

        return -46.85 + 175.72/65536 *(double)rawT;
    }

    public static double extractHumidity(BluetoothGattCharacteristic c) {
        int a = shortUnsignedAtOffset(c, 2);
        // bits [1..0] are status bits and need to be cleared
        a = a - (a % 4);

        return ((-6f) + 125f * (a / 65535f));
    }

    public static int[] extractCalibrationCoefficients(BluetoothGattCharacteristic c) {
        int[] coefficients = new int[8];

        coefficients[0] = shortUnsignedAtOffset(c, 0);
        coefficients[1] = shortUnsignedAtOffset(c, 2);
        coefficients[2] = shortUnsignedAtOffset(c, 4);
        coefficients[3] = shortUnsignedAtOffset(c, 6);
        coefficients[4] = shortSignedAtOffset(c, 8);
        coefficients[5] = shortSignedAtOffset(c, 10);
        coefficients[6] = shortSignedAtOffset(c, 12);
        coefficients[7] = shortSignedAtOffset(c, 14);

        return coefficients;
    }

    public static double extractBarTemperature(BluetoothGattCharacteristic characteristic, final int[] c) {
        // c holds the calibration coefficients

        int t_r;	// Temperature raw value from sensor
        double t_a; 	// Temperature actual value in unit centi degrees celsius

        t_r = shortSignedAtOffset(characteristic, 0);

        t_a = (100 * (c[0] * t_r / Math.pow(2,8) + c[1] * Math.pow(2,6))) / Math.pow(2,16);

        return t_a / 100;
    }

    public static double extractBarometer(BluetoothGattCharacteristic characteristic, final int[] c) {
        // c holds the calibration coefficients

        int t_r;	// Temperature raw value from sensor
        int p_r;	// Pressure raw value from sensor
        double S;	// Interim value in calculation
        double O;	// Interim value in calculation
        double p_a; 	// Pressure actual value in unit Pascal.

        t_r = shortSignedAtOffset(characteristic, 0);
        p_r = shortUnsignedAtOffset(characteristic, 2);


        S = c[2] + c[3] * t_r / Math.pow(2,17) + ((c[4] * t_r / Math.pow(2,15)) * t_r) / Math.pow(2,19);
        O = c[5] * Math.pow(2,14) + c[6] * t_r / Math.pow(2,3) + ((c[7] * t_r / Math.pow(2,15)) * t_r) / Math.pow(2,4);
        p_a = (S * p_r + O) / Math.pow(2,14);

        //Convert pascal to in. Hg
        double p_hg = p_a * 0.000296;

        return p_hg;
    }


    private static Integer shortSignedAtOffset(BluetoothGattCharacteristic c, int offset) {
        Integer lowerByte = c.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, offset);
        Integer upperByte = c.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, offset + 1); // Note: interpret MSB as signed.

        return (upperByte << 8) + lowerByte;
    }

    private static Integer shortUnsignedAtOffset(BluetoothGattCharacteristic c, int offset) {
        Integer lowerByte = c.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, offset);
        Integer upperByte = c.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, offset + 1); // Note: interpret MSB as unsigned.

        return (upperByte << 8) + lowerByte;
    }
}


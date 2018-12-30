package com.ncsu.bme.acelaso;

import java.util.UUID;

/**
 * Created by Michael on 4/21/2015.
 */
public class BluetoothHelper {
    public static String shorUuidFormat = "0000%04X-0000-1000-8000-00805F9B34FB";

    public static UUID sixteenBitUuid(long shortUuid) {
        assert shortUuid >= 0 && shortUuid <= 0xFFFF;
        return UUID.fromString(String.format(shorUuidFormat, shortUuid & 0xFFFF));
    }


}

package com.ncsu.bme.acelaso;

import com.jjoe64.graphview.GraphViewDataInterface;

/**
 * Created by JEET on 8/9/2015.
 */
public class GraphViewScale implements GraphViewDataInterface {


    private double x, y;

    public GraphViewScale(double x, double y) {
        this.x = x;
        this.y = y;
    }

    @Override
    public double getX() {
        return this.x;
    }

    @Override
    public double getY() {
        return this.y;
    }
}

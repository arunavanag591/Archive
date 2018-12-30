package com.ncsu.bme.acelaso;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;


public class PlotPage extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_plot_page);
      //  ImageView imageToDisplay = (ImageView) findViewById(R.id.);
    }

    public void onClick(View v){
       if (v.getId() == R.id.HRButton) {
        startActivity(new Intent(PlotPage.this, HeartPlot.class));
       }
        else if(v.getId()==R.id.TempButton)
       {
           startActivity(new Intent(PlotPage.this,TempPlot.class));
       }
        else if(v.getId()==R.id.SCButton)
       {
           startActivity(new Intent(PlotPage.this,GSRPlot.class));
       }
    }


}

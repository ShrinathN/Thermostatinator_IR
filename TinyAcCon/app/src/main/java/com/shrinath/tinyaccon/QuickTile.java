package com.shrinath.tinyaccon;

import android.app.Dialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.service.quicksettings.Tile;
import android.service.quicksettings.TileService;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import androidx.annotation.UiThread;

public class QuickTile extends TileService {

    private final String TAG = "QsTile";

    @Override
    public void onTileAdded() {

        Tile tile = getQsTile();
        tile.setState(Tile.STATE_INACTIVE);

        tile.updateTile();
        super.onTileAdded();
    }

    @Override
    public void onClick() {
        SharedPreferences sharedPreferences = getApplicationContext().getSharedPreferences("ADDRESS", MODE_PRIVATE);
        String ip = sharedPreferences.getString("IP", "127.0.0.1");
        int port = Integer.parseInt(sharedPreferences.getString("PORT", "8080"));
        Tile tile = getQsTile();
        int tile_state = tile.getState();

        Log.d(TAG, "onClick: IP -> " + ip);
        Log.d(TAG, "onClick: PORT -> " + port);

        if(tile_state == Tile.STATE_ACTIVE) {
            new UdpPacketSenderClass(0, ip,port);
            tile.setLabel("AC Off");
            tile.setState(Tile.STATE_INACTIVE);
        } else if(tile_state == Tile.STATE_INACTIVE) {
            new UdpPacketSenderClass(1, ip, port);
            tile.setLabel("AC On");
            tile.setState(Tile.STATE_ACTIVE);
        }
        tile.updateTile();
        super.onClick();
    }




}

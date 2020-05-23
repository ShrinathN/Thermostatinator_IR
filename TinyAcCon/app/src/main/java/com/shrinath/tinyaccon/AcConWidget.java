package com.shrinath.tinyaccon;

import android.app.PendingIntent;
import android.appwidget.AppWidgetManager;
import android.appwidget.AppWidgetProvider;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.IpSecManager;
import android.util.Log;
import android.widget.Button;
import android.widget.RemoteViews;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketAddress;

/**
 * Implementation of App Widget functionality.
 */
public class AcConWidget extends AppWidgetProvider {

    private static final String TAG = "Widget";

    static void updateAppWidget(Context context, AppWidgetManager appWidgetManager,
                                int appWidgetId) {

        // Construct the RemoteViews object
        RemoteViews views = new RemoteViews(context.getPackageName(), R.layout.ac_con_widget);

        Intent intent_on = new Intent(context, AcConWidget.class);
        intent_on.setAction("AC");
        intent_on.putExtra("STAT", 1);
        PendingIntent pendingIntent_on = PendingIntent.getBroadcast(context, 0, intent_on,PendingIntent.FLAG_UPDATE_CURRENT);
        views.setOnClickPendingIntent(R.id.button_on, pendingIntent_on);

        Intent intent_off = new Intent(context, AcConWidget.class);
        intent_off.setAction("AC");
        intent_off.putExtra("STAT", 0);
        PendingIntent pendingIntent_off = PendingIntent.getBroadcast(context, 1, intent_off,PendingIntent.FLAG_UPDATE_CURRENT);
        views.setOnClickPendingIntent(R.id.button_off, pendingIntent_off);



        // Instruct the widget manager to update the widget
        appWidgetManager.updateAppWidget(appWidgetId, views);
    }

    @Override
    public void onUpdate(Context context, AppWidgetManager appWidgetManager, int[] appWidgetIds) {
        // There may be multiple widgets active, so update all of them
        for (int appWidgetId : appWidgetIds) {
            updateAppWidget(context, appWidgetManager, appWidgetId);
        }
    }

    @Override
    public void onEnabled(Context context) {
        // Enter relevant functionality for when the first widget is created
    }

    @Override
    public void onDisabled(Context context) {
        // Enter relevant functionality for when the last widget is disabled
    }


    @Override
    public void onReceive(final Context context, final Intent intent) {
        int i = intent.getIntExtra("STAT", 0);

        SharedPreferences sharedPreferences = context.getSharedPreferences("ADDRESS", Context.MODE_PRIVATE);
        String ip = sharedPreferences.getString("IP", "127.0.0.1");
        int port = Integer.parseInt(sharedPreferences.getString("PORT", "8080"));
        new UdpPacketSenderClass(i, ip, port);
        super.onReceive(context, intent);
    }
}


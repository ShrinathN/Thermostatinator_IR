package com.shrinath.tinyaccon;

import android.util.Log;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

import static android.content.ContentValues.TAG;

public class UdpPacketSenderClass {

    final static byte[] ON_SIGNAL = new byte[]{(byte) 0xb5, (byte) 0x8a, (byte) 0x3c, (byte) 0x9b, (byte) 0x64, (byte) 0x41, (byte) 0xbe, (byte) 0x0b, (byte) 0xf4, (byte) 0x10, (byte) 0xef};
    final static byte[] OFF_SIGNAL = new byte[]{(byte) 0xb5, (byte) 0x8a, (byte) 0x3c, (byte) 0x9b, (byte) 0x64, (byte) 0x41, (byte) 0xbe, (byte) 0x0b, (byte) 0xf4, (byte) 0x00, (byte) 0xff};

    public UdpPacketSenderClass(final int a, final String ip, final int port) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try{
                    Log.d(TAG, "run: " + a);
                    DatagramPacket datagramPacket = new DatagramPacket(a == 1 ? ON_SIGNAL : OFF_SIGNAL, 11);
                    DatagramSocket datagramSocket = new DatagramSocket();
                    datagramSocket.connect(InetAddress.getByName(ip), port);
                    datagramSocket.send(datagramPacket);
                    datagramSocket.close();
                }catch(Exception e) {
                    Log.d(TAG, "onReceive: " + e);
                }
            }
        }).start();
    }
}

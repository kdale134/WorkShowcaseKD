package com.example.cs360astudiointrokyledale;

/*
 * CS360-Mobile Architecture and Programming
 * Project Three
 * Kyle Dale
 * 10/15/2023
 * Professor DiMarzio
 * sendSMS.java
 */

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.telephony.SmsManager;
import android.widget.Toast;

public class sendSMS {

    // A static method to send an SMS
    public static void send(Context context, String phoneNumber, String message) {
        try {
            // Get the default SMS manager
            SmsManager smsManager = SmsManager.getDefault();

            // Create pending intents for tracking SMS sending and delivery status
            PendingIntent sentIntent = PendingIntent.getBroadcast(context, 0, new Intent("SMS_SENT"), PendingIntent.FLAG_UPDATE_CURRENT);
            PendingIntent deliveredIntent = PendingIntent.getBroadcast(context, 0, new Intent("SMS_DELIVERED"), PendingIntent.FLAG_UPDATE_CURRENT);

            // Send the SMS message
            smsManager.sendTextMessage(phoneNumber, null, message, sentIntent, deliveredIntent);
        } catch (Exception e) {
            // Handle any exceptions that may occur during SMS sending
            Toast.makeText(context, "SMS sending failed: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }
}

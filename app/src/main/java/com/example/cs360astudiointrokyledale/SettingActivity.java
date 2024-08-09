package com.example.cs360astudiointrokyledale;

/*
 * CS360-Mobile Architecture and Programming
 * Project Three
 * Kyle Dale
 * 10/15/2023
 * Professor DiMarzio
 * SettingActivity.java
 */



import android.Manifest;
import android.app.AlertDialog;
import android.app.PendingIntent;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.telephony.SmsManager;
import android.view.View;
import android.widget.Toast;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import android.widget.EditText;

public class SettingActivity extends AppCompatActivity{

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);
    }

    // Permission request code
    private static final int SMS_PERMISSION_REQUEST_CODE = 303;

    public void onLogoClick(View view) {
        Intent intent = new Intent(this, HomeActivity.class);
        startActivity(intent);
        finish(); // Close the current activity if needed
    }

    public void onSearchIconClick(View view) {
        // Handle the "Search" button click here
        Toast.makeText(this, "Search button clicked.", Toast.LENGTH_SHORT).show();
    }

    public void onAccountIconClick(View view) {
        Intent intent = new Intent(this, AccountActivity.class);
        startActivity(intent);
    }

    public void onAlertIconClick(View view) {
        // Check if the SMS permission is granted
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.SEND_SMS)
                != PackageManager.PERMISSION_GRANTED) {
            // Permission is not granted, request it from the user
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.SEND_SMS},
                    SMS_PERMISSION_REQUEST_CODE);
        } else {
            // Permission is already granted, show SMS dialog
            showSmsDialog();
        }
    }
    private void showSmsDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Send SMS");

        // Set up the layout for the dialog
        View view = getLayoutInflater().inflate(R.layout.sms_dialog, null);
        builder.setView(view);

        final EditText phoneNumberEditText = view.findViewById(R.id.phoneNumberEditText);

        builder.setPositiveButton("Send", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                // User clicked "Send," send SMS
                String phoneNumber = phoneNumberEditText.getText().toString();
                sendSMS(phoneNumber); // Call sendSMS method with the phone number
            }
        });

        builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                // User clicked "Cancel," do nothing
            }
        });

        builder.show();
    }

    private void sendSMS(String phoneNumber) { // Accept the phone number as an argument
        String message = "SMS Test";

        try {
            SmsManager smsManager = SmsManager.getDefault();
            PendingIntent sentIntent = PendingIntent.getBroadcast(this, 0, new Intent("SMS_SENT"), PendingIntent.FLAG_UPDATE_CURRENT);
            PendingIntent deliveredIntent = PendingIntent.getBroadcast(this, 0, new Intent("SMS_DELIVERED"), PendingIntent.FLAG_UPDATE_CURRENT);

            smsManager.sendTextMessage(phoneNumber, null, message, sentIntent, deliveredIntent);
        } catch (Exception e) {
            Toast.makeText(this, "SMS sending failed: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults); // Call super method

        if (requestCode == SMS_PERMISSION_REQUEST_CODE) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                // Permission granted, show SMS dialog
                showSmsDialog();
            } else {
                // Permission denied, show a message or handle it accordingly
                Toast.makeText(this, "SMS permission denied.", Toast.LENGTH_SHORT).show();
            }
        }
    }

    public void onGeneralButtonClick(View view) {
        // Handle the "General" button click here
        Toast.makeText(this, "General button clicked.", Toast.LENGTH_SHORT).show();
    }

    public void onNotificationsButtonClick(View view) {
        // Handle the "Notifications" button click here
        Toast.makeText(this, "Notifications button clicked.", Toast.LENGTH_SHORT).show();
    }

    public void onHelpButtonClick(View view) {
        // Handle the "Help" button click here
        Toast.makeText(this, "Help button clicked.", Toast.LENGTH_SHORT).show();
    }

    public void onAboutButtonClick(View view) {
        // Handle the "About" button click here
        Toast.makeText(this, "About button clicked.", Toast.LENGTH_SHORT).show();
    }

    public void onCenterIconClick(View view) {
        // Start the ItemEnterActivity
        Intent intent = new Intent(this, ItemEnterActivity.class);
        startActivity(intent);
    }

    public void onSettingsButtonClick(View view) {
        // Handle the "Settings" button click here
        Toast.makeText(this, "You are currently in settings!", Toast.LENGTH_SHORT).show();
    }

    public void onHomeButtonClick(View view) {
        // Start the HomeActivity
        Intent intent = new Intent(this, HomeActivity.class);
        startActivity(intent);
    }

}





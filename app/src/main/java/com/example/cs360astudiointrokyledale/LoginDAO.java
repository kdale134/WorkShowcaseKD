package com.example.cs360astudiointrokyledale;

/*
 * CS360-Mobile Architecture and Programming
 * Project Three
 * Kyle Dale
 * 10/15/2023
 * Professor DiMarzio
 * LoginDAO.java
 */

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteDatabase;

public class LoginDAO {
    private SQLiteDatabase database;
    private LoginDBHelper dbHelper;

    public LoginDAO(Context context) {
        dbHelper = new LoginDBHelper(context);
    }

    public void open() throws SQLException {
        database = dbHelper.getWritableDatabase();
    }

    public void close() {
        dbHelper.close();
    }

    // Method to add a new user account to the database
    public long addUser(String username, String password) {
        ContentValues values = new ContentValues();
        values.put(LoginDBHelper.COLUMN_USERNAME, username);
        values.put(LoginDBHelper.COLUMN_PASSWORD, password);

        // Insert the new user into the database
        return database.insert(LoginDBHelper.TABLE_NAME, null, values);
    }

    // Method to retrieve user information by username
    public User getUserByUsername(String username) {
        User user = null;
        Cursor cursor = database.query(
                LoginDBHelper.TABLE_NAME,
                null,
                LoginDBHelper.COLUMN_USERNAME + "=?",
                new String[]{username},
                null,
                null,
                null
        );

        if (cursor != null && cursor.moveToFirst()) {
            // Assuming you have a User class to represent user data
            user = new User();
            user.setId(cursor.getLong(cursor.getColumnIndex(LoginDBHelper.COLUMN_ID)));
            user.setUsername(cursor.getString(cursor.getColumnIndex(LoginDBHelper.COLUMN_USERNAME)));
            user.setPassword(cursor.getString(cursor.getColumnIndex(LoginDBHelper.COLUMN_PASSWORD)));

            cursor.close();
        }

        return user;
    }

    // You can add more methods here for authentication and other database operations as needed
}


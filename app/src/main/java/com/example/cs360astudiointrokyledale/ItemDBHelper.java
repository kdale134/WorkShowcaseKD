package com.example.cs360astudiointrokyledale;

/*
 * CS360-Mobile Architecture and Programming
 * Project Three
 * Kyle Dale
 * 10/15/2023
 * Professor DiMarzio
 * ItemDBHelper.java
 */

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class ItemDBHelper extends SQLiteOpenHelper {
    // Define database name and version
    private static String DATABASE_NAME = "ItemWarehouse.db";
    private static int DATABASE_VERSION = 1;

    // Define table name and column names
    public static final String TABLE_NAME = "items";
    public static final String COLUMN_ID = "_id";
    public static String COLUMN_NAME = "name";
    public static String COLUMN_QUANTITY = "quantity";
    // Add more columns as needed

    // Define SQL statement to create the items table
    private static final String TABLE_CREATE =
            "CREATE TABLE " + TABLE_NAME + " (" +
                    COLUMN_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                    COLUMN_NAME + " TEXT, " +
                    COLUMN_QUANTITY + " INTEGER);";
    // Add columns to the CREATE TABLE statement

    public ItemDBHelper(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        // Create the items table
        db.execSQL(TABLE_CREATE);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        // Handle database upgrades if needed
        // Implement this method to perform actions when the database version changes
    }
}

package com.example.cs360astudiointrokyledale;

/*
 * CS360-Mobile Architecture and Programming
 * Project Three
 * Kyle Dale
 * 10/15/2023
 * Professor DiMarzio
 * ItemDAO.java
 */

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteDatabase;
import java.util.ArrayList;
import java.util.List;

public class ItemDAO {
    private SQLiteDatabase database;
    private ItemDBHelper dbHelper;

    public ItemDAO(Context context) {
        dbHelper = new ItemDBHelper(context);
    }

    public void open() throws SQLException {
        database = dbHelper.getWritableDatabase();
    }

    public void close() {
        dbHelper.close();
    }

    // Insert a new item into the database
    public long insertItem(Item item) {
        ContentValues values = new ContentValues();
        values.put(ItemDBHelper.COLUMN_NAME, item.getName());
        values.put(ItemDBHelper.COLUMN_QUANTITY, item.getQuantity());

        return database.insert(ItemDBHelper.TABLE_NAME, null, values);
    }

    // Get a list of all items in the database
    public List<Item> getAllItems() {
        List<Item> itemList = new ArrayList<>();
        SQLiteDatabase db = dbHelper.getReadableDatabase();

        String[] projection = {
                ItemDBHelper.COLUMN_ID,
                ItemDBHelper.COLUMN_NAME,
                ItemDBHelper.COLUMN_QUANTITY
        };

        Cursor cursor = db.query(
                ItemDBHelper.TABLE_NAME,
                projection,
                null,
                null,
                null,
                null,
                null
        );

        if (cursor != null) {
            if (cursor.moveToFirst()) {
                do {
                    // Check if the cursor has the expected columns
                    if (cursor.getColumnIndex(ItemDBHelper.COLUMN_ID) >= 0 &&
                            cursor.getColumnIndex(ItemDBHelper.COLUMN_NAME) >= 0 &&
                            cursor.getColumnIndex(ItemDBHelper.COLUMN_QUANTITY) >= 0) {

                        long id = cursor.getLong(cursor.getColumnIndex(ItemDBHelper.COLUMN_ID));
                        String name = cursor.getString(cursor.getColumnIndex(ItemDBHelper.COLUMN_NAME));
                        int quantity = cursor.getInt(cursor.getColumnIndex(ItemDBHelper.COLUMN_QUANTITY));

                        Item item = new Item();
                        item.setId(id);
                        item.setName(name);
                        item.setQuantity(quantity);

                        itemList.add(item);
                    } else {
                        // Handle the case where some expected columns are missing
                    }
                } while (cursor.moveToNext());
            }
            cursor.close();
        }

        return itemList;
    }


    // Delete an item from the database
    public void deleteItem(long itemId) {
        database.delete(ItemDBHelper.TABLE_NAME, ItemDBHelper.COLUMN_ID + " = ?", new String[]{String.valueOf(itemId)});
    }
}

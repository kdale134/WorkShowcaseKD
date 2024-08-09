package com.example.cs360astudiointrokyledale;

/*
 * CS360-Mobile Architecture and Programming
 * Project Three
 * Kyle Dale
 * 10/15/2023
 * Professor DiMarzio
 * ItemEnterActivity.java
 */

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import android.content.Intent;

public class ItemEnterActivity extends AppCompatActivity {

    private EditText itemNameEditText;
    private EditText itemQuantityEditText;
    private Button submitItemButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_itementer);

        itemNameEditText = findViewById(R.id.editTextItemName);
        itemQuantityEditText = findViewById(R.id.editTextItemQuantity);
        submitItemButton = findViewById(R.id.buttonSubmitItem);

        submitItemButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Get the entered item name and quantity
                String itemName = itemNameEditText.getText().toString().trim();
                String itemQuantityStr = itemQuantityEditText.getText().toString().trim();

                // Check if item name and quantity are not empty
                if (!itemName.isEmpty() && !itemQuantityStr.isEmpty()) {
                    int itemQuantity = Integer.parseInt(itemQuantityStr);

                    // Create an Item object with the entered data
                    Item newItem = new Item();
                    newItem.setName(itemName);
                    newItem.setQuantity(itemQuantity);

                    // Initialize the ItemDAO and open the database connection
                    ItemDAO itemDAO = new ItemDAO(ItemEnterActivity.this);
                    itemDAO.open();

                    // Insert the new item into the database
                    long itemId = itemDAO.insertItem(newItem);

                    // Close the database connection
                    itemDAO.close();

                    if (itemId != -1) {
                        // Item insertion successful, show a success message
                        Toast.makeText(ItemEnterActivity.this, "Item added to database", Toast.LENGTH_SHORT).show();

                        // Navigate back to HomeActivity
                        Intent intent = new Intent(ItemEnterActivity.this, HomeActivity.class);
                        startActivity(intent);
                        finish(); // Close the current activity

                    } else {
                        // Item insertion failed, show an error message
                        Toast.makeText(ItemEnterActivity.this, "Failed to add item to database", Toast.LENGTH_SHORT).show();
                    }
                } else {
                    // Item name or quantity is empty, show an error message
                    Toast.makeText(ItemEnterActivity.this, "Please fill in all fields", Toast.LENGTH_SHORT).show();
                }
            }
        });

    }
}

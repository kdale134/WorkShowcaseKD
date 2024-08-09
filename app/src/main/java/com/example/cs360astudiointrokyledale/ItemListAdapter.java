package com.example.cs360astudiointrokyledale;

/*
 * CS360-Mobile Architecture and Programming
 * Project Three
 * Kyle Dale
 * 10/15/2023
 * Professor DiMarzio
 * ItemListAdapter.java
 */

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;
import java.util.List;

public class ItemListAdapter extends RecyclerView.Adapter<ItemListAdapter.ViewHolder> {

    private List<Item> itemList;
    private Context context;

    // Constructor to initialize the adapter with context and data (itemList)
    public ItemListAdapter(Context context, List<Item> itemList) {
        this.context = context;
        this.itemList = itemList;
    }

    // Create and return a new ViewHolder when needed
    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        // Inflate the item layout from XML and return a ViewHolder
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_list, parent, false);
        return new ViewHolder(view);
    }

    // Bind data to the ViewHolder
    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        // Get the item at the current position
        Item item = itemList.get(position);

        // Set the item's name and quantity in the TextViews of the ViewHolder
        holder.textViewItemName.setText(item.getName());
        holder.textViewItemQuantity.setText("Quantity: " + item.getQuantity());
    }

    // Return the total number of items in the data set
    @Override
    public int getItemCount() {
        return itemList.size();
    }

    // Inner ViewHolder class to hold references to views within each item
    public class ViewHolder extends RecyclerView.ViewHolder {
        public TextView textViewItemName;
        public TextView textViewItemQuantity;

        // Constructor to initialize the views within the ViewHolder
        public ViewHolder(@NonNull View itemView) {
            super(itemView);
            textViewItemName = itemView.findViewById(R.id.textViewItemName);
            textViewItemQuantity = itemView.findViewById(R.id.textViewItemQuantity);
        }
    }
}

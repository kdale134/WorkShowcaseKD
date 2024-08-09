package com.example.cs360astudiointrokyledale;

/*
 * CS360-Mobile Architecture and Programming
 * Project Three
 * Kyle Dale
 * 10/15/2023
 * Professor DiMarzio
 * MainActivity.java
 */

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.content.Intent;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    private TextView textGreeting;
    private EditText nameText;
    private EditText passText;
    private Button buttonLogin;
    private Button buttonForgotPass;
    private Button buttonNewUser;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Initialize your views
        nameText = findViewById(R.id.nameText);
        passText = findViewById(R.id.passwordText);
        buttonLogin = findViewById(R.id.buttonLogin);
        buttonForgotPass = findViewById(R.id.buttonForgotPass);
        buttonNewUser = findViewById(R.id.buttonNewUser);

        // Start the login button in a disabled state
        buttonLogin.setEnabled(false);

        // Set up a TextWatcher to listen for text changes in nameText and passText
        TextWatcher textWatcher = new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
                // Not used in this case
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                // Check if the text in nameText and passText is not empty
                boolean isNameEmpty = nameText.getText().toString().trim().isEmpty();
                boolean isPassEmpty = passText.getText().toString().trim().isEmpty();

                // Enable or disable the login button based on the text input
                buttonLogin.setEnabled(!isNameEmpty && !isPassEmpty);
            }

            @Override
            public void afterTextChanged(Editable s) {
                // Not used in this case
            }
        };

        nameText.addTextChangedListener(textWatcher);
        passText.addTextChangedListener(textWatcher);
    }

    public void onLoginButtonClick(View view) {
        // Get the entered username and password from EditText views
        String username = nameText.getText().toString().trim();
        String password = passText.getText().toString().trim();

        // Perform authentication (e.g., check if the user exists in the database)
        LoginDAO loginDAO = new LoginDAO(this);
        loginDAO.open();

        // Check if the user exists and the password is correct
        User user = loginDAO.getUserByUsername(username);
        if (user != null && user.getPassword().equals(password)) {
            // Login successful, transition to activity_home.xml
            // You can start a new activity here

            Intent intent = new Intent(this, HomeActivity.class);
            startActivity(intent);
            finish(); // Close the current activity if needed
        } else {
            Toast.makeText(this, "Login failed. Please check your credentials.", Toast.LENGTH_SHORT).show();
        }

        loginDAO.close();
    }


    // Create a function that handles the "Forgot Password?" button click
    public void onForgotPasswordButtonClick(View view) {
        // Handle the "Forgot Password?" button click here (currently does nothing)
        Toast.makeText(this, "Forgot password button clicked.", Toast.LENGTH_SHORT).show();
    }

// Create a function that handles the "New User?" button click
    public void onNewUserButtonClick(View view) {
        // Start the RegisterActivity
        Intent intent = new Intent(this, RegisterActivity.class);
        startActivity(intent);
    }

}

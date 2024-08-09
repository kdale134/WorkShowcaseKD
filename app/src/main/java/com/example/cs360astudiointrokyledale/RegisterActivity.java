package com.example.cs360astudiointrokyledale;

/*
 * CS360-Mobile Architecture and Programming
 * Project Three
 * Kyle Dale
 * 10/15/2023
 * Professor DiMarzio
 * RegisterActivity.java
 */

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;



public class RegisterActivity extends AppCompatActivity {

    private EditText usernameEditText;
    private EditText passwordEditText;
    private Button registerButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register);

        usernameEditText = findViewById(R.id.usernameEditText);
        passwordEditText = findViewById(R.id.passwordEditText);
        registerButton = findViewById(R.id.registerButton);

        registerButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Get the entered username and password
                String username = usernameEditText.getText().toString().trim();
                String password = passwordEditText.getText().toString().trim();

                // Perform user registration
                LoginDAO loginDAO = new LoginDAO(RegisterActivity.this);
                loginDAO.open();

                // Check if the username is already taken
                User existingUser = loginDAO.getUserByUsername(username);

                if (existingUser != null) {
                    // Username is already taken, show an error message
                    Toast.makeText(RegisterActivity.this, "Username is already taken", Toast.LENGTH_SHORT).show();
                } else {
                    // Username is available, add the new user to the database
                    long userId = loginDAO.addUser(username, password);

                    if (userId != -1) {
                        // Registration successful, you can display a success message
                        // and then redirect to the login screen
                        Intent intent = new Intent(RegisterActivity.this, MainActivity.class);
                        intent.putExtra("registrationSuccess", true);
                        startActivity(intent);
                        finish();
                    } else {
                        // Registration failed, show an error message
                        // (e.g., failed to add user to the database)
                        Toast.makeText(RegisterActivity.this, "Registration failed", Toast.LENGTH_SHORT).show();
                    }
                }

                loginDAO.close();
            }
        });
    }
}


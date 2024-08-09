/*
Kyle Dale
CS-320-Software Test Automation
Project One
4/7/ - 4/9/2023
Professor Jacks
*/

import java.util.ArrayList;
import java.util.List;

// Defines the TaskService class
public class TaskService {
    private List<Task> tasks = new ArrayList<>();
    // addTask method 
    public void addTask(Task task) {
        tasks.add(task);
    }
    // Delete task method
    public void deleteTask(String taskId) {
        tasks.removeIf(task -> task.getTaskId().equals(taskId));
    }
    // Method to update a Task name
    public void updateTaskName(String taskId, String newName) {
        tasks.stream()
             .filter(task -> task.getTaskId().equals(taskId))
             .findFirst()
             .ifPresent(task -> task.setName(newName));
    }
    // Method to update a Task description
    public void updateTaskDescription(String taskId, String newDescription) {
        tasks.stream()
             .filter(task -> task.getTaskId().equals(taskId))
             .findFirst()
             .ifPresent(task -> task.setDescription(newDescription));
    }
    // Getter method for Tasks
    public List<Task> getTasks() {
        return tasks;
    }
}


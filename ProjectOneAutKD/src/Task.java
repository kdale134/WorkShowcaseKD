/*
Kyle Dale
CS-320-Software Test Automation
Project One
4/7/ - 4/9/2023
Professor Jacks
*/

// Defines the 'Task' class
public class Task {
	// Defines various private variables in the 'Task' class
    private final String taskId;
    private String name;
    private String description;
    // Defines input specifications for each variable and provides error message if supplied incorrect input
    public Task(String taskId, String name, String description) {
        if (taskId == null || taskId.length() > 10 || name == null || name.length() > 20 ||
                description == null || description.length() > 50) {
            throw new IllegalArgumentException("Invalid task field value(s)");
        }
        this.taskId = taskId;
        this.name = name;
        this.description = description;
    }
    /*
    Defines various getter and setter methods needed in functionality
    */
    public String getTaskId() {
        return taskId;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        if (name == null || name.length() > 20) {
            throw new IllegalArgumentException("Invalid name field value");
        }
        this.name = name;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        if (description == null || description.length() > 50) {
            throw new IllegalArgumentException("Invalid description field value");
        }
        this.description = description;
    }
}


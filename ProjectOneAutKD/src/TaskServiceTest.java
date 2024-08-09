/*
Kyle Dale
CS-320-Software Test Automation
Project One
4/7/ - 4/9/2023
Professor Jacks
*/


import org.junit.Test;
import static org.junit.Assert.*;
// Defines the TaskServiceTest class
public class TaskServiceTest {
	// Tests a valid Task input
    @Test
    public void testAddTask() {
        TaskService taskService = new TaskService();
        Task task1 = new Task("12345", "Task 1", "This is task 1");
        Task task2 = new Task("67890", "Task 2", "This is task 2");

        taskService.addTask(task1);
        taskService.addTask(task2);

        assertTrue(taskService.getTasks().contains(task1));
        assertTrue(taskService.getTasks().contains(task2));
    }
    // Test valid deletion of a Task
    @Test
    public void testDeleteTask() {
        TaskService taskService = new TaskService();
        Task task1 = new Task("12345", "Task 1", "This is task 1");

        taskService.addTask(task1);
        taskService.deleteTask("12345");

        assertFalse(taskService.getTasks().contains(task1));
    }
    // Test valid update to a Task name
    @Test
    public void testUpdateTaskName() {
        TaskService taskService = new TaskService();
        Task task1 = new Task("12345", "Task 1", "This is task 1");

        taskService.addTask(task1);
        taskService.updateTaskName("12345", "New Task 1 Name");

        assertEquals("New Task 1 Name", task1.getName());
    }
    // Test valid update to a Task description
    @Test
    public void testUpdateTaskDescription() {
        TaskService taskService = new TaskService();
        Task task1 = new Task("12345", "Task 1", "This is task 1");

        taskService.addTask(task1);
        taskService.updateTaskDescription("12345", "New Task 1 Description");

        assertEquals("New Task 1 Description", task1.getDescription());
    }
}

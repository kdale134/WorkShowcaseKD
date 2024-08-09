/*
Kyle Dale
CS-320-Software Test Automation
Project One
4/7/ - 4/9/2023
Professor Jacks
*/

import static org.junit.Assert.*;
import org.junit.Test;

// Defines the TaskTest class
public class TaskTest {
    
	// Ensures valid input functions properly
	@Test
	public void testValidTask() {
	    Task task = new Task("12345", "Test Task", "This is a test task");
	    assertEquals("12345", task.getTaskId());
	    assertEquals("Test Task", task.getName());
	    assertEquals("This is a test task", task.getDescription());
	}

    // Test with null as ID input, expected error
    @Test(expected = IllegalArgumentException.class)
    public void testNullTaskId() {
        Task task = new Task(null, "Test Task", "This is a test task");
    }
    // Test with null as Task name, expected error
    @Test(expected = IllegalArgumentException.class)
    public void testNullName() {
        Task task = new Task("12345", null, "This is a test task");
    }
    // Test with null as Task description, expected error
    @Test(expected = IllegalArgumentException.class)
    public void testNullDescription() {
        Task task = new Task("12345", "Test Task", null);
    }
    // Test with illegal ID input
    @Test(expected = IllegalArgumentException.class)
    public void testLongTaskId() {
        Task task = new Task("12345678901", "Test Task", "This is a test task");
    }
    // Test with illegal name input
    @Test(expected = IllegalArgumentException.class)
    public void testLongName() {
        Task task = new Task("12345", "This is a really long name", "This is a test task");
    }
    // Test with illegal description input
    @Test(expected = IllegalArgumentException.class)
    public void testLongDescription() {
        Task task = new Task("12345", "Test Task", "This is a really long description that should not be allowed");
    }
}


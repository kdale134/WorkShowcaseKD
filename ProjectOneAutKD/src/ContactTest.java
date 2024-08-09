/*
Kyle Dale
CS-320-Software Test Automation
Project One
4/7/ - 4/9/2023
Professor Jacks
*/

// Imports necessary libraries
import org.junit.Test;
import static org.junit.Assert.*;

// Define the ContactTest class
public class ContactTest {
	// Tests the various'getters and setters' in Contact
    @Test
    public void testGettersAndSetters() {
        // Create a new Contact object
        Contact contact = new Contact("123", "John", "Doe", "1234567890", "123 Main St.");

        // Test the getters
        assertEquals("123", contact.getId());
        assertEquals("John", contact.getFirstName());
        assertEquals("Doe", contact.getLastName());
        assertEquals("1234567890", contact.getPhone());
        assertEquals("123 Main St.", contact.getAddress());

        // Test the setters
        contact.setFirstName("Jane");
        contact.setLastName("Smith");
        contact.setPhone("0987654321");
        contact.setAddress("456 Elm St.");
        assertEquals("Jane", contact.getFirstName());
        assertEquals("Smith", contact.getLastName());
        assertEquals("0987654321", contact.getPhone());
        assertEquals("456 Elm St.", contact.getAddress());
    }

    @Test
    public void testToString() {
        // Create a new Contact object
        Contact contact = new Contact("123", "John", "Doe", "1234567890", "123 Main St.");

        // Test the toString method
        String expected = "Contact{id='123', firstName='John', lastName='Doe', phone='1234567890', address='123 Main St.'}";
        assertEquals(expected, contact.toString());
    }
}

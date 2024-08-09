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

// Defines the ContactServiceTest class
public class ContactServiceTest {
	// Test conditions for the various requirements specified in the assignment
    @Test
    public void testAddContact() throws Exception {
        ContactService contactService = new ContactService();
        Contact contact = new Contact("1", "John", "Doe", "1234567890", "123 Main St");
        contactService.addContact(contact);
        assertTrue(contactService.getContacts().containsKey("1"));
    }
    
    @Test(expected = Exception.class)
    public void testAddExistingContact() throws Exception {
        ContactService contactService = new ContactService();
        Contact contact1 = new Contact("1", "John", "Doe", "1234567890", "123 Main St");
        Contact contact2 = new Contact("1", "Jane", "Smith", "0987654321", "456 Oak Ave");
        contactService.addContact(contact1);
        contactService.addContact(contact2);
    }
    
    @Test
    public void testDeleteContact() throws Exception {
        ContactService contactService = new ContactService();
        Contact contact = new Contact("1", "John", "Doe", "1234567890", "123 Main St");
        contactService.addContact(contact);
        contactService.deleteContact("1");
        assertFalse(contactService.getContacts().containsKey("1"));
    }
    
    @Test(expected = Exception.class)
    public void testDeleteNonexistentContact() throws Exception {
        ContactService contactService = new ContactService();
        contactService.deleteContact("1");
    }
    
    @Test
    public void testUpdateContact() throws Exception {
        ContactService contactService = new ContactService();
        Contact contact = new Contact("1", "John", "Doe", "1234567890", "123 Main St");
        contactService.addContact(contact);
        contactService.updateContact("1", "Jane", null, "0987654321", "456 Oak Ave");
        Contact updatedContact = contactService.getContacts().get("1");
        assertEquals(updatedContact.getFirstName(), "Jane");
        assertEquals(updatedContact.getLastName(), "Doe");
        assertEquals(updatedContact.getPhone(), "0987654321");
        assertEquals(updatedContact.getAddress(), "456 Oak Ave");
    }
    
    @Test(expected = Exception.class)
    public void testUpdateNonexistentContact() throws Exception {
        ContactService contactService = new ContactService();
        contactService.updateContact("1", "Jane", null, "0987654321", "456 Oak Ave");
    }
    
    @Test(expected = Exception.class)
    public void testInvalidFirstName() throws Exception {
        ContactService contactService = new ContactService();
        Contact contact = new Contact("1", "John", "Doe", "1234567890", "123 Main St");
        contactService.addContact(contact);
        contactService.updateContact("1", "ThisNameIsTooLong", null, null, null);
    }
    
    @Test(expected = Exception.class)
    public void testInvalidLastName() throws Exception {
        ContactService contactService = new ContactService();
        Contact contact = new Contact("1", "John", "Doe", "1234567890", "123 Main St");
        contactService.addContact(contact);
        contactService.updateContact("1", null, "ThisNameIsTooLong", null, null);
    }
    
    @Test(expected = Exception.class)
    public void testInvalidNumber() throws Exception {
        ContactService contactService = new ContactService();
        Contact contact = new Contact("1", "John", "Doe", "1234567890", "123 Main St");
        contactService.addContact(contact);
contactService.updateContact("1", null, null, "not_a_number", null);
}
}



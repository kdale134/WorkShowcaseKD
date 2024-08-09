/*
Kyle Dale
CS-320-Software Test Automation
Project One
4/7/ - 4/9/2023
Professor Jacks
*/

// Imports necessary libraries
import java.util.HashMap;
import java.util.Map;

// Defines the ContactService class
public class ContactService {
    private Map<String, Contact> contacts;

    public ContactService() {
        contacts = new HashMap<String, Contact>();
    }
    // Define various functions within the ContactService class
    public void addContact(Contact contact) throws Exception {
        if (contacts.containsKey(contact.getId())) {
            throw new Exception("Contact ID already exists");
        }
        contacts.put(contact.getId(), contact);
    }

    public void deleteContact(String contactId) throws Exception {
        if (!contacts.containsKey(contactId)) {
            throw new Exception("Contact ID does not exist");
        }
        contacts.remove(contactId);
    }
    // updateContact function, includes error handling based on specifications 
    public void updateContact(String contactId, String firstName, String lastName, String number, String address) throws Exception {
        if (!contacts.containsKey(contactId)) {
            throw new Exception("Contact ID does not exist");
        }
        Contact contact = contacts.get(contactId);
        if (firstName != null && firstName.length() > 10) {
            throw new Exception("First name cannot be longer than 10 characters");
        }
        if (lastName != null && lastName.length() > 10) {
            throw new Exception("Last name cannot be longer than 10 characters");
        }
        if (number != null && number.length() != 10) {
            throw new Exception("Number must be exactly 10 digits");
        }
        if (address != null && address.length() > 30) {
            throw new Exception("Address cannot be longer than 30 characters");
        }
        if (firstName != null) {
            contact.setFirstName(firstName);
        }
        if (lastName != null) {
            contact.setLastName(lastName);
        }
        if (number != null) {
            contact.setPhone(number);
        }
        if (address != null) {
            contact.setAddress(address);
        }
        contacts.put(contactId, contact);
    }
    // Returns getContacts() for JUnit test purposes
    public Map<String, Contact> getContacts() {
        return contacts;
    }
}

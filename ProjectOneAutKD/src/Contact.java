/*
Kyle Dale
CS-320-Software Test Automation
Project One
4/7/ - 4/9/2023
Professor Jacks
*/

// Defines the Contact class
public class Contact {
	// Declares necessary variables for function
    private String id;
    private String firstName;
    private String lastName;
    private String phone;
    private String address;
    // Defines the Contact object
    public Contact(String id, String firstName, String lastName, String phone, String address) {
    	// Various error handling methods to abide by requirements
    	if (firstName != null && firstName.length() > 10) {
            throw new IllegalArgumentException("First name cannot be longer than 10 characters");
        }
        if (lastName != null && lastName.length() > 10) {
            throw new IllegalArgumentException("Last name cannot be longer than 10 characters");
        }
        if (phone != null && phone.length() != 10) {
            throw new IllegalArgumentException("Number must be exactly 10 digits");
        }
        if (address != null && address.length() > 30) {
            throw new IllegalArgumentException("Address cannot be longer than 30 characters");
        }
        
        this.id = id;
        this.firstName = firstName;
        this.lastName = lastName;
        this.phone = phone;
        this.address = address;
    }
    // Define get methods for various variables in Contact class
    public String getId() {
        return id;
    }

    public String getFirstName() {
        return firstName;
    }

    public void setFirstName(String firstName) {
        this.firstName = firstName;
    }

    public String getLastName() {
        return lastName;
    }

    public void setLastName(String lastName) {
        this.lastName = lastName;
    }

    public String getPhone() {
        return phone;
    }

    public void setPhone(String phone) {
        this.phone = phone;
    }

    public String getAddress() {
        return address;
    }

    public void setAddress(String address) {
        this.address = address;
    }

    @Override
    public String toString() {
        return "Contact{" +
                "id='" + id + '\'' +
                ", firstName='" + firstName + '\'' +
                ", lastName='" + lastName + '\'' +
                ", phone='" + phone + '\'' +
                ", address='" + address + '\'' +
                '}';
    }
}

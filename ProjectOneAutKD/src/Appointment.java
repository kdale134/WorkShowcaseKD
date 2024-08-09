/*
Kyle Dale
CS-320-Software Test Automation
Project One
4/7/ - 4/9/2023
Professor Jacks
*/

// imports necessary date functionality
import java.util.Date;

// defines the Appointment class
public class Appointment {
	// define necessary private variables to ensure encapsulation
    private final String id;
    private String description;
    private final Date appointmentDate;
    // Defines the Appointment object with the id, date, and description parameters. 
    public Appointment(String id, Date appointmentDate, String description) {
       // if id length is null or greater than 10 throw error
    	if (id == null || id.length() > 10) {
            throw new IllegalArgumentException("Invalid appointment ID");
        }
    	// if appointment date is null or before current date throw an error
        if (appointmentDate == null || appointmentDate.before(new Date())) {
            throw new IllegalArgumentException("Invalid appointment date");
        }
        // if description is null or greater than 50 characters throw an error
        if (description == null || description.length() > 50) {
            throw new IllegalArgumentException("Invalid appointment description");
        }
        this.id = id;
        this.appointmentDate = appointmentDate;
        this.description = description;
    }
    // various getter methods to access functionality without breaking encapsulation
    public String getId() {
        return id;
    }
    
    public Date getAppointmentDate() {
        return appointmentDate;
    }
    
    public String getDescription() {
        return description;
    }
    
    public void setDescription(String description) {
        if (description == null || description.length() > 50) {
            throw new IllegalArgumentException("Invalid appointment description");
        }
        this.description = description;
    }
}

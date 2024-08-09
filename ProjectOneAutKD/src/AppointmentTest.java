/*
Kyle Dale
CS-320-Software Test Automation
Project One
4/7/ - 4/9/2023
Professor Jacks
*/

// imports necessary libraries for function
import static org.junit.Assert.*;
import org.junit.Test;
import java.util.Date;
// defines the 'AppointmentTest' class
public class AppointmentTest {

    @Test
    public void testValidAppointment() {
        // create a valid appointment
        Date appointmentDate = new Date(System.currentTimeMillis() + 3600000); // 1 hour from now
        Appointment appointment = new Appointment("12345", appointmentDate, "Valid appointment");

        // check that the appointment fields were set correctly
        assertEquals("12345", appointment.getId());
        assertEquals(appointmentDate, appointment.getAppointmentDate());
        assertEquals("Valid appointment", appointment.getDescription());
    }

    @Test(expected = IllegalArgumentException.class)
    public void testNullId() {
        // create an appointment with null id (should throw IllegalArgumentException)
        Date appointmentDate = new Date(System.currentTimeMillis() + 3600000); // 1 hour from now
        Appointment appointment = new Appointment(null, appointmentDate, "Invalid appointment");
    }

    @Test(expected = IllegalArgumentException.class)
    public void testInvalidId() {
        // create an appointment with invalid id (too long) (should throw IllegalArgumentException)
        Date appointmentDate = new Date(System.currentTimeMillis() + 3600000); // 1 hour from now
        Appointment appointment = new Appointment("12345678901", appointmentDate, "Invalid appointment");
    }

    @Test(expected = IllegalArgumentException.class)
    public void testNullAppointmentDate() {
        // create an appointment with null appointment date (should throw IllegalArgumentException)
        Appointment appointment = new Appointment("12345", null, "Invalid appointment");
    }

    @Test(expected = IllegalArgumentException.class)
    public void testPastAppointmentDate() {
        // create an appointment with past appointment date (should throw IllegalArgumentException)
        Date appointmentDate = new Date(System.currentTimeMillis() - 3600000); // 1 hour ago
        Appointment appointment = new Appointment("12345", appointmentDate, "Invalid appointment");
    }

    @Test(expected = IllegalArgumentException.class)
    public void testNullDescription() {
        // create an appointment with null description (should throw IllegalArgumentException)
        Date appointmentDate = new Date(System.currentTimeMillis() + 3600000); // 1 hour from now
        Appointment appointment = new Appointment("12345", appointmentDate, null);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testInvalidDescription() {
        // create an appointment with invalid description (too long) (should throw IllegalArgumentException)
        Date appointmentDate = new Date(System.currentTimeMillis() + 3600000); // 1 hour from now
        Appointment appointment = new Appointment("12345", appointmentDate, "This overly bloated description that will ultimately say or convey anything, is too long to be valid as an input for this program.");
    }
}

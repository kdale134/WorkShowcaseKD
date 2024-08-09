/*
Kyle Dale
CS-320-Software Test Automation
Project One
4/7/ - 4/9/2023
Professor Jacks
*/

// imports necessary libraries for functionality
import static org.junit.Assert.*;
import java.util.Date;
import org.junit.Before;
import org.junit.Test;
import java.util.List;

// defines the 'AppointmentServiceTest' class
public class AppointmentServiceTest {

    private AppointmentService appointmentService;
    // set base condition before each test
    @Before
    public void setUp() {
        appointmentService = new AppointmentService();
    }
    
    // test to determine proper functionality for adding an appointment
    @Test
    public void testAddAppointment() {
        Appointment appointment = new Appointment("ID001", new Date(), "Appointment 1");
        appointmentService.addAppointment(appointment);
        List<Appointment> appointments = appointmentService.getAppointments();
        assertTrue(appointments.contains(appointment));
    }

 // test to determine proper functionality for deleting an appointment
    @Test
    public void testDeleteAppointment() {
        Appointment appointment = new Appointment("ID001", new Date(), "Appointment 1");
        appointmentService.addAppointment(appointment);
        appointmentService.deleteAppointment("ID001");
        List<Appointment> appointments = appointmentService.getAppointments();
        assertFalse(appointments.contains(appointment));
    }

 // test to determine proper functionality when trying to add a null appointment
    @Test
    public void testAddNullAppointment() {
        appointmentService.addAppointment(null);
        assertTrue(appointmentService.getAppointments().isEmpty());
    }

    // test to determine proper functionality when trying to add a duplicate appointment
    @Test
    public void testAddDuplicateAppointment() {
        Appointment appointment1 = new Appointment("ID001", new Date(), "Appointment 1");
        Appointment appointment2 = new Appointment("ID001", new Date(), "Appointment 2");
        appointmentService.addAppointment(appointment1);
        appointmentService.addAppointment(appointment2);
        List<Appointment> appointments = appointmentService.getAppointments();
        assertEquals(1, appointments.size());
        assertEquals(appointment1, appointments.get(0));
    }

    // test to determine proper functionality when trying to delete a null appointment
    @Test
    public void testDeleteNullAppointment() {
        appointmentService.deleteAppointment(null);
        assertTrue(appointmentService.getAppointments().isEmpty());
    }
    // test to determine proper functionality when trying to delete a non-existent appointment
    @Test
    public void testDeleteNonexistentAppointment() {
        Appointment appointment = new Appointment("ID001", new Date(), "Appointment 1");
        appointmentService.addAppointment(appointment);
        appointmentService.deleteAppointment("ID002");
        List<Appointment> appointments = appointmentService.getAppointments();
        assertEquals(1, appointments.size());
        assertEquals(appointment, appointments.get(0));
    }

    // test to determine proper functionality when trying to add an appointment with a unique id
    @Test
    public void testAddAppointmentWithUniqueID() {
        Appointment appointment1 = new Appointment("ID001", new Date(), "Appointment 1");
        Appointment appointment2 = new Appointment("ID002", new Date(), "Appointment 2");
        appointmentService.addAppointment(appointment1);
        appointmentService.addAppointment(appointment2);
        List<Appointment> appointments = appointmentService.getAppointments();
        assertEquals(2, appointments.size());
        assertTrue(appointments.contains(appointment1));
        assertTrue(appointments.contains(appointment2));
    }

    // test to determine proper functionality when trying to delete an appointment by id
    @Test
    public void testDeleteAppointmentByID() {
        Appointment appointment1 = new Appointment("ID001", new Date(), "Appointment 1");
        Appointment appointment2 = new Appointment("ID002", new Date(), "Appointment 2");
        appointmentService.addAppointment(appointment1);
        appointmentService.addAppointment(appointment2);
        appointmentService.deleteAppointment("ID001");
        List<Appointment> appointments = appointmentService.getAppointments();
        assertEquals(1, appointments.size());
        assertFalse(appointments.contains(appointment1));
        assertTrue(appointments.contains(appointment2));
    }
}



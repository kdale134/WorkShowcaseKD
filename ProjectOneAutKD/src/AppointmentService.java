/*
Kyle Dale
CS-320-Software Test Automation
Project One
4/7/ - 4/9/2023
Professor Jacks
*/

// import necessary array functionality
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
// defines the 'AppointmentService' class
public class AppointmentService {
    private Map<String, Appointment> appointments;
    // defines a map of appointments
    public AppointmentService() {
        appointments = new HashMap<>();
    }
    // function for adding an appointment to the previous map
    public void addAppointment(Appointment appointment) {
        if (appointment != null && !appointments.containsKey(appointment.getId())) {
            appointments.put(appointment.getId(), appointment);
        }
    }
    // function to delete an appointment from the map
    public void deleteAppointment(String id) {
        if (id != null && appointments.containsKey(id)) {
            appointments.remove(id);
        }
    }
    // function to get the list of appointments 
    public List<Appointment> getAppointments() {
        return new ArrayList<>(appointments.values());
    }
}

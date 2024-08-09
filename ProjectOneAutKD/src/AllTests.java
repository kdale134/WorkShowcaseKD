/*
Kyle Dale
CS-320-Software Test Automation
Project One
4/7/ - 4/9/2023
Professor Jacks
*/

import org.junit.platform.suite.api.SelectClasses;
import org.junit.platform.suite.api.Suite;

// Defines testing suite to run all JUnit Tests at once
@Suite
@SelectClasses({ AppointmentServiceTest.class, AppointmentTest.class, ContactServiceTest.class, ContactTest.class,
		TaskServiceTest.class, TaskTest.class })
public class AllTests {

}

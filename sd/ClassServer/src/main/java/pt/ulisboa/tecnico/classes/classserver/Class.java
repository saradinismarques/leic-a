package pt.ulisboa.tecnico.classes.classserver;

import pt.ulisboa.tecnico.classes.classserver.exceptions.*;
import pt.ulisboa.tecnico.classes.contract.ClassesDefinitions.*;

import java.time.LocalTime;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.ResourceBundle;
import java.util.logging.Logger;

public class Class {
    private static final Logger LOGGER = Logger.getLogger(Class.class.getName());
    public ArrayList<Student> enrolledStudents = new ArrayList<>();
    public ArrayList<Student> discardedStudents = new ArrayList<>();

    public int capacity;
    public boolean openEnrollments;

    public boolean serverActivity = true;
    public boolean gossipActivity = true;

    final List<LocalTime> vectorClock = new ArrayList<LocalTime>();
    final List<String> vectorClockString = new ArrayList<String>();
    
    public void setUpVectorClock(){
        vectorClock.add(LocalTime.MIN);
        vectorClock.add(LocalTime.MIN);
        vectorClock.add(LocalTime.MIN);

        for ( LocalTime l : vectorClock )
            vectorClockString.add(l.toString());
            
    }


    public HashMap<LocalTime,String> updatesMap = new HashMap<>();

    public int getCapacity() throws InactiveServerException{
        return capacity;
    }

    public boolean getOpenEnrollments() throws InactiveServerException{
        return openEnrollments;
    }

    public ArrayList<Student> getEnrolledStudents() throws InactiveServerException{
        return enrolledStudents;
    }

    public ArrayList<Student> getDiscardedStudent() throws InactiveServerException{
        return discardedStudents;
    }

    public List<String> getVectorClock(){
        return vectorClockString;
    }

    public boolean getGossipActivity(){
        return gossipActivity;
    }
    

    // ---------------STUDENT------------------------
    public synchronized ClassState listClass(boolean debugFlag) throws InactiveServerException {
        
        if(!this.serverActivity) {
            throw new InactiveServerException("Inactive Server");
        }

        if (debugFlag)
            LOGGER.info("Class was listed");
        ClassState classState = ClassState.newBuilder().setCapacity(capacity).setOpenEnrollments(openEnrollments).addAllEnrolled(enrolledStudents).addAllDiscarded(discardedStudents).build();
        return classState;
    }

    public synchronized void enroll(Student student, boolean debugFlag) throws InactiveServerException, EnrollmentsAlreadyClosedException, StudentAlreadyEnrolledException, FullClassException {
        if(!this.serverActivity) {
            throw new InactiveServerException("Inactive Server");
        } else if (!openEnrollments) {
            throw new EnrollmentsAlreadyClosedException("Enrollments Already Closed");
        } else if (enrolledStudents.contains(student)) {
            throw new StudentAlreadyEnrolledException("Student Already Enrolled");
        } else if (capacity == enrolledStudents.size()) {
            throw new FullClassException("Full Class");
        } else {
            enrolledStudents.add(student);
        }

        LocalTime time = LocalTime.now();
        updatesMap.put(time,"ENROLL");

        if (debugFlag)
            LOGGER.info("Student: " + student.getStudentName() + " with studentId: " + student.getStudentId() + " enrolled in this Class");
    }

    // ---------------PROFESSOR------------------------
    public synchronized void openEnrollments(int capacity, boolean debugFlag) throws InactiveServerException, EnrollmentsAlreadyOpenedException, FullClassException {
        if(!this.serverActivity) {
            throw new InactiveServerException("Inactive Server");
        } else if (openEnrollments) {
            throw new EnrollmentsAlreadyOpenedException("Enrollments Already Opened");
        } else if(enrolledStudents.size() > capacity) {
            throw new FullClassException("Full Class");
        }
        this.capacity = capacity;
        this.openEnrollments = true;

        LocalTime time = LocalTime.now();
        updatesMap.put(time,"OPEN");

        if (debugFlag)
            LOGGER.info("Professor opened enrollments");
    }

    public synchronized void closeEnrollments(boolean debugFlag) throws InactiveServerException, EnrollmentsAlreadyClosedException{
        if(!this.serverActivity) {
            throw new InactiveServerException("Inactive Server");
        } else if (!openEnrollments) {
            throw new EnrollmentsAlreadyClosedException("Enrollments Already Closed");
        }
        this.openEnrollments = false;

        LocalTime time = LocalTime.now();
        updatesMap.put(time,"CLOSE");

        if (debugFlag)
            LOGGER.info("Professor closed enrollments");
    }

    public synchronized void cancelEnrollment(String studentId, boolean debugFlag) throws InactiveServerException, NonExistingStudentException {
        if(!this.serverActivity) {
            throw new InactiveServerException("Inactive Server");
        }
        boolean hasStudent = false;

        for (Student st :enrolledStudents) {
            if (st.getStudentId().equals(studentId)) {
                hasStudent = true;
                enrolledStudents.remove(st);
                discardedStudents.add(st);
                break;
            }
        }
        if (!hasStudent)
            throw new NonExistingStudentException("Non Existing Student");

        if (debugFlag)
            LOGGER.info("Professor canceled enrollment for student: " + studentId);

        LocalTime time = LocalTime.now();
        updatesMap.put(time,"CANCEL");
    }

    // -------------------ADMIN------------------------
    public synchronized void activate(boolean debugFlag){
        if (debugFlag)
            LOGGER.info("Admin activated server");
        this.serverActivity = true;
    }

    public synchronized void deactivate(boolean debugFlag){
        if (debugFlag)
            LOGGER.info("Admin deactivated server");
        this.serverActivity = false;
    }

    public ClassState dump(boolean debugFlag) throws InactiveServerException{
        if(!this.serverActivity) {
            throw new InactiveServerException("Inactive Server");
        }
        ClassState classState = ClassState.newBuilder().setCapacity(capacity).setOpenEnrollments(openEnrollments).addAllEnrolled(enrolledStudents).addAllDiscarded(discardedStudents).build();

        if (debugFlag)
            LOGGER.info("Admin asked to execute dump");
        return classState;
    }

    public synchronized void activateGossip(boolean debugFlag) throws InactiveServerException{
        if (debugFlag)
            LOGGER.info("Admin activated gossip");
        this.gossipActivity = true;
    }

    public synchronized void deactivateGossip(boolean debugFlag) throws InactiveServerException{
        if (debugFlag)
            LOGGER.info("Admin deactivated gossip");
        this.gossipActivity = false;
    }

    public synchronized void gossip(boolean debugFlag) throws InactiveServerException{
        if ( debugFlag)
            LOGGER.info("Admin gossip");
        if ( this.gossipActivity == false ){
            // nao pode fazer gossip
        }
        else{
        }
    }


    //------------------------SERVER-----------------------------
    public void propagateState(ClassState classState, Boolean debugFlag) throws InactiveServerException{
        if(!this.serverActivity) {
            throw new InactiveServerException("Inactive Server");
        }

        this.capacity = classState.getCapacity();
        this.openEnrollments = classState.getOpenEnrollments();
        this.enrolledStudents = new ArrayList<Student>(classState.getEnrolledList());
        this.discardedStudents = new ArrayList<Student>(classState.getDiscardedList());

        if (debugFlag)
            LOGGER.info("Propagated state");
        
    }

    public void openUpdate(int capacity, boolean openEnrollments) throws InactiveServerException{

        if(!this.serverActivity) {
            throw new InactiveServerException("Inactive Server");
        }

        this.capacity = capacity;
        this.openEnrollments = openEnrollments;

    }

    public void enrolledUpdate(List<Student> enrolled) throws InactiveServerException{

        ArrayList<Student> enrolledArrayList = new ArrayList<Student>(enrolled);
        if(!this.serverActivity) {
            throw new InactiveServerException("Inactive Server");
        }

        this.enrolledStudents = enrolledArrayList;
    }

    public void discardedUpdate(ArrayList<Student> discarded, ArrayList<Student> enrolled, boolean debugFlag) throws InactiveServerException{

        if(!this.serverActivity) {
            throw new InactiveServerException("Inactive Server");
        }
        this.enrolledStudents = enrolled;
        this.discardedStudents = discarded;

        if (debugFlag)
            LOGGER.info("Discarded students have been updated");
    }

    public List<String> updateVectorClock(int server_id, List<String> receivedVectorClockString) throws InactiveServerException{

        if(!this.serverActivity) {
            throw new InactiveServerException("Inactive Server");
        }

        List<LocalTime> receivedVectorClock = new ArrayList<>();
        for ( String l : receivedVectorClockString ){
            receivedVectorClock.add(LocalTime.parse(l));
        }
        
        List<String> updatesList = new ArrayList<>();

        LocalTime otherServerTime = receivedVectorClock.get(server_id);
        LocalTime myTime = vectorClock.get(server_id);




        if ( myTime.isAfter(otherServerTime) ){
            // TODO : alterar para mandar o proprio server
            for ( LocalTime key : updatesMap.keySet() ){
                if ( key.isAfter(otherServerTime) ){
                    String update = updatesMap.get(key);
                    updatesList.add(update);
                }
            }
        }



        return updatesList;
    }



}